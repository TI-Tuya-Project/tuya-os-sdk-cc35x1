/**
 * @file example_tuya_ap_provision.c
 * @brief Tuya AP provisioning example for the TI CC35xx port.
 *
 * Starts the full Tuya AP-mode network configuration flow so the device
 * appears in the Tuya / Smart Life phone app:
 *
 *   1. Device starts as a Wi-Fi AP (SSID "SmartLife-XXYY", open, no password).
 *   2. Device broadcasts its identity over UDP every 1 s on port 6667.
 *   3. The Tuya app on the phone connects to that AP, discovers the broadcast,
 *      opens a TLS-PSK TCP connection and pushes the home-router credentials
 *      plus a cloud activation token.
 *   4. on_netcfg_finish() is called with the received SSID / password / token.
 *   5. The device stops the AP, joins the home router as a STA, then connects
 *      to Tuya cloud and completes activation.
 *
 * Credentials to fill in:
 *   TUYA_PRODUCT_KEY  - Product ID from the Tuya IoT Platform
 *   TUYA_UUID         - Device UUID (from the platform or chip license)
 *   TUYA_AUTHKEY      - Device auth-key matching the UUID
 */

#ifndef PROJECT_NAME
    #define PROJECT_NAME "TI_Tuya_Port"
#endif
#ifndef PROJECT_VERSION
    #define PROJECT_VERSION "1.0.0"
#endif
#ifndef PLATFORM_CHIP
    #define PLATFORM_CHIP "CC35XX"
#endif

/* ------------------------------------------------------------------ */
/* TODO: fill these in from the Tuya IoT Platform before flashing      */
/* ------------------------------------------------------------------ */
#define TUYA_PRODUCT_KEY "gon0wbdhnesxtkxo"
#define TUYA_UUID        "uuid1372d8812acd2534"
#define TUYA_AUTHKEY     "hKtjIfqvTav2ibTG3lcDKsa5Q0c2UGAg"
#define TUYA_SW_VERSION  "1.0.0"

/* 8-digit pincode — must match what is printed in the QR code.
 * The Tuya app scans this QR code and uses the pin to derive the TLS-PSK
 * via PBKDF2(pin, uuid) instead of the default fixed PSK on port 6668.   */
#define TUYA_PINCODE     "12345678"
/* ------------------------------------------------------------------ */

#include <stdio.h>
#include "tuya_cloud_types.h"
#include "uart_term.h"

#include "tal_api.h"
#include "tal_wifi.h"
#include "tkl_wifi.h"

#include "netcfg.h"
#include "ap_netcfg.h"
#include "tuya_iot.h"


#if defined(ENABLE_LIBLWIP) && (ENABLE_LIBLWIP == 1)
#include "lwip_init.h"
#endif

static tuya_iot_client_t s_tuya_client;
static volatile bool s_provisioned = false;

/* ------------------------------------------------------------------ */
/* QR code output (disabled — uncomment when qrencode_print is linked) */
/* ------------------------------------------------------------------ */

// static void qr_uart_puts(const char *str)
// {
//     UART_PRINT("%s", str);
// }

// static void print_provisioning_qrcode(void)
// {
//     char qr_payload[128];
//     snprintf(qr_payload, sizeof(qr_payload),
//              "{\"p\":\"%s\",\"u\":\"%s\",\"h\":\"%s\"}",
//              TUYA_PRODUCT_KEY, TUYA_UUID, TUYA_PINCODE);
//
//     UART_PRINT("\n\r[PROV] Scan this QR code with the Tuya / Smart Life app:\n\r");
//     UART_PRINT("[PROV] Payload: %s\n\r", qr_payload);
//     UART_PRINT("\n\r");
//
//     /* invert=1 so dark modules print as filled blocks on a light terminal */
//     qrcode_string_output(qr_payload, qr_uart_puts, 1);
//
//     UART_PRINT("\n\r[PROV] In the app: Add Device -> ... -> AP Mode -> Scan QR code\n\r");
// }



/* ------------------------------------------------------------------ */
/* Tuya cloud event handler                                            */
/* ------------------------------------------------------------------ */
static void on_tuya_event(tuya_iot_client_t *client, tuya_event_msg_t *event)
{
    switch (event->id) {
    case TUYA_EVENT_BIND_START:
        UART_PRINT("\n\r[TUYA] Waiting for app to bind device...\n\r");
        break;
    case TUYA_EVENT_ACTIVATE_SUCCESSED:
        UART_PRINT("\n\r[TUYA] Device activated successfully!\n\r");
        break;
    case TUYA_EVENT_MQTT_CONNECTED:
        UART_PRINT("\n\r[TUYA] MQTT connected - device is online.\n\r");
        break;
    case TUYA_EVENT_MQTT_DISCONNECT:
        UART_PRINT("\n\r[TUYA] MQTT disconnected.\n\r");
        break;
    case TUYA_EVENT_RESET:
        UART_PRINT("\n\r[TUYA] Factory reset requested.\n\r");
        break;
    default:
        UART_PRINT("\n\r[TUYA] Event id: %d\n\r", (int)event->id);
        break;
    }
}

/* ------------------------------------------------------------------ */
/* Called by ap_netcfg when the phone delivers ssid/passwd/token       */
/* ------------------------------------------------------------------ */
static int on_netcfg_finish(int type, netcfg_info_t *info)
{
    if (info == NULL) {
        UART_PRINT("\n\r[PROV] ERROR: netcfg_finish called with NULL info\n\r");
        return OPRT_INVALID_PARM;
    }

    UART_PRINT("\n\r[PROV] Credentials received from app:\n\r");
    UART_PRINT("  SSID  : %s\n\r", info->ssid);
    UART_PRINT("  Token : %.*s\n\r", (int)info->t_len, info->token);

    netcfg_stop(NETCFG_TUYA_WIFI_AP);

    OPERATE_RET rt = tal_wifi_station_connect((int8_t *)info->ssid,
                                              info->p_len ? (int8_t *)info->passwd : NULL);
    if (rt != OPRT_OK) {
        UART_PRINT("\n\r[PROV] ERROR: station_connect failed: %d\n\r", (int)rt);
        return rt;
    }

    UART_PRINT("\n\r[PROV] Connecting to router '%s' ...\n\r", info->ssid);
    s_provisioned = true;
    return OPRT_OK;
}

/* ------------------------------------------------------------------ */
/* Kick off the AP-mode provisioning engine                            */
/* ------------------------------------------------------------------ */

static OPERATE_RET start_ap_provisioning(void)
{
    OPERATE_RET rt;

    UART_PRINT("\n\r[PROV] Initialising netcfg module...\n\r");
    rt = netcfg_init();
    if (rt != OPRT_OK) {
        UART_PRINT("\n\r[PROV] ERROR: netcfg_init failed: %d\n\r", (int)rt);
        return rt;
    }
    UART_PRINT("\n\r[PROV] netcfg_init OK\n\r");

    netcfg_args_t args = {
        .type    = NETCFG_TUYA_WIFI_AP,
        .uuid    = TUYA_UUID,
        .pincode = NULL,   /* NULL = UDP broadcast mode, no QR needed for testing */
    };

    UART_PRINT("\n\r[PROV] Initialising AP netcfg handler (uuid=%s)...\n\r", TUYA_UUID);
    rt = ap_netcfg_init(&args);
    if (rt != OPRT_OK) {
        UART_PRINT("\n\r[PROV] ERROR: ap_netcfg_init failed: %d\n\r", (int)rt);
        return rt;
    }
    UART_PRINT("\n\r[PROV] ap_netcfg_init OK\n\r");

    UART_PRINT("\n\r[PROV] Starting AP provisioning...\n\r");
    rt = netcfg_start(NETCFG_TUYA_WIFI_AP, on_netcfg_finish, NULL);
    if (rt != OPRT_OK) {
        UART_PRINT("\n\r[PROV] ERROR: netcfg_start failed: %d\n\r", (int)rt);
        return rt;
    }

    return OPRT_OK;
}

/* ------------------------------------------------------------------ */
/* Application entry point                                             */
/* ------------------------------------------------------------------ */
void user_main(void)
{
    OPERATE_RET rt;

    UART_PRINT("\n\r========================================\n\r");
    UART_PRINT("  TI Tuya Port - AP Provisioning Example\n\r");
    UART_PRINT("  Project : %s  v%s\n\r", PROJECT_NAME, PROJECT_VERSION);
    UART_PRINT("  Chip    : %s\n\r", PLATFORM_CHIP);
    UART_PRINT("  Compiled: %s\n\r", __DATE__);
    UART_PRINT("========================================\n\r");

    tal_kv_init(&(tal_kv_cfg_t){
        .seed = "vmlkasdh93dlvlcy",
        .key  = "dflfuap134ddlduq",
    });
    tal_sw_timer_init();
    tal_workq_init();

#if defined(ENABLE_LIBLWIP) && (ENABLE_LIBLWIP == 1)
    TUYA_LwIP_Init();
#endif

    UART_PRINT("\n\r[MAIN] Starting Wi-Fi driver...\n\r");
    rt = tal_wifi_init(tkl_wifi_default_event_cb);
    if (rt != OPRT_OK) {
        UART_PRINT("\n\r[MAIN] ERROR: tal_wifi_init failed: %d\n\r", (int)rt);
        goto __EXIT;
    }

    /* Work mode is set by ap_netcfg internally — do not call set_work_mode here */

    UART_PRINT("\n\r[MAIN] Initialising Tuya IoT client...\n\r");
    UART_PRINT("  product_key : %s\n\r", TUYA_PRODUCT_KEY);
    UART_PRINT("  uuid        : %s\n\r", TUYA_UUID);

    tuya_iot_config_t iot_cfg = {
        .productkey    = TUYA_PRODUCT_KEY,
        .uuid          = TUYA_UUID,
        .authkey       = TUYA_AUTHKEY,
        .software_ver  = TUYA_SW_VERSION,
        .event_handler = on_tuya_event,
    };
    rt = tuya_iot_init(&s_tuya_client, &iot_cfg);
    if (rt != OPRT_OK) {
        UART_PRINT("\n\r[MAIN] ERROR: tuya_iot_init failed: %d\n\r", (int)rt);
        goto __EXIT;
    }
    UART_PRINT("\n\r[MAIN] tuya_iot_init OK\n\r");

    rt = start_ap_provisioning();
    if (rt != OPRT_OK) {
        goto __EXIT;
    }

    bool cloud_started = false;

    for (;;) {
        if (!s_provisioned) {
            tal_system_sleep(200);
            continue;
        }

        if (!cloud_started) {
            WF_STATION_STAT_E sta_stat = WSS_IDLE;
            tal_wifi_station_get_status(&sta_stat);
            UART_PRINT("\n\r[MAIN] Waiting for IP... sta_stat=%d\n\r", (int)sta_stat);
            if (sta_stat != WSS_GOT_IP) {
                tal_system_sleep(500);
                continue;
            }

            UART_PRINT("\n\r[MAIN] Got IP - starting Tuya cloud connection...\n\r");
            rt = tuya_iot_start(&s_tuya_client);
            if (rt != OPRT_OK) {
                UART_PRINT("\n\r[MAIN] ERROR: tuya_iot_start failed: %d\n\r", (int)rt);
                goto __EXIT;
            }
            UART_PRINT("\n\r[MAIN] tuya_iot_start OK\n\r");
            cloud_started = true;
        }

        tuya_iot_yield(&s_tuya_client);
    }

__EXIT:
    UART_PRINT("\n\r[MAIN] Fatal error - halting.\n\r");
}

/* ------------------------------------------------------------------ */
/* RTOS thread wrappers                                                */
/* ------------------------------------------------------------------ */
#if OPERATING_SYSTEM == SYSTEM_LINUX

void main(int argc, char *argv[])
{
    user_main();
    while (1) {
        tal_system_sleep(500);
    }
}

#else

static THREAD_HANDLE ty_app_thread = NULL;

static void tuya_app_thread(void *arg)
{
    user_main();
    tal_thread_delete(ty_app_thread);
    ty_app_thread = NULL;
}

void tuya_app_main(void)
{
    THREAD_CFG_T thrd_param = {0};
    thrd_param.stackDepth = 1024 * 6;
    
    /* CHANGE THIS: Increase priority so it can process events faster */
    /* Usually, 0 is the lowest. 3 is a good 'middle' ground. */
    thrd_param.priority   = 3; 
    
    thrd_param.thrdname   = "tuya_app_main";
    
    tal_thread_create_and_start(&ty_app_thread, NULL, NULL, tuya_app_thread, NULL, &thrd_param);
}

#endif