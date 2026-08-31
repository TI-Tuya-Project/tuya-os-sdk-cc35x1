/**
 * @file tkl_wifi.c
 * @brief Wi-Fi driver implementation for TI CC35xx (SimpleLink)
 * @version Final + RSSI + Status
 * @note Implements Scan, Connect, Disconnect, MAC, RSSI, and Connection Status.
 */

#include "tkl_wifi.h"
#include "tuya_error_code.h"
#include "tkl_memory.h"
#include "tkl_semaphore.h"
#include "tkl_output.h"
#include <string.h>
#include <stdio.h>
#include "uart_term.h"
#include "tkl_system.h"

/* LwIP includes for IP address handling. */
#include <lwip/netif.h>
#include <lwip/ip_addr.h>
#include "network_lwip.h"

/* TI Wi-Fi host driver header. */
#include "C:/ti/simplelink_wifi_sdk_9_21_00_15/source/ti/drivers/net/wifi/wifi_host_driver/inc_adapt/wlan_if.h"

/* Adapter constants. */
#ifndef WLAN_MAX_SCAN_COUNT
#define WLAN_MAX_SCAN_COUNT 20U
#endif

#define SCAN_SEMAPHORE_INITIAL_COUNT 0U
#define SCAN_SEMAPHORE_MAX_COUNT 1U

/* Delays preserve the existing hardware/network-stack sequencing. */
#define WIFI_SCAN_TIMEOUT_MS 3000U
#define WIFI_ROLE_TRANSITION_DELAY_MS 500U
#define AP_NETWORK_INTERFACE_DELAY_MS 100U

/* The TI AP implementation accepts at most four associated stations. */
#define AP_MAX_STATIONS 4U

/* TI expects a two-letter country code followed by the indoor-operation flag. */
static const uint8_t s_wifi_country_domain[] = {'E', 'U', 'I'};

/* Global variables. */
static WIFI_EVENT_CB g_wifi_event_cb = NULL;
static TKL_SEM_HANDLE g_scan_sem = NULL;
static AP_IF_S *g_scan_results_ptr = NULL;
static uint32_t g_scan_count = 0;

/* Track current connection status locally. */
static WF_STATION_STAT_E g_wifi_status = WSS_IDLE;

/* Helper utilities. */

/**
 * @brief Convert TI Security Bitmap to Tuya Auth Mode
 */
static WF_AP_AUTH_MODE_E _ti_sec_to_tuya(uint16_t security_info)
{
    uint8_t sec_bitmap = WLAN_SCAN_RESULT_SEC_TYPE_BITMAP(security_info);

    switch (sec_bitmap) {
        case WLAN_SEC_TYPE_OPEN:      return WAAM_OPEN;
        case WLAN_SEC_TYPE_WPA_WPA2:  return WAAM_WPA2_PSK;
        case WLAN_SEC_TYPE_WPA2_PLUS: return WAAM_WPA2_PSK;
        case WLAN_SEC_TYPE_WPA3:      return WAAM_WPA_WPA3_SAE;
        default:                      return WAAM_WPA2_PSK;
    }
}

/* TI WLAN event handler. */
void TiWlanEventHandler(WlanEvent_t *pWlanEvent)
{
    if (pWlanEvent == NULL) return;

    switch (pWlanEvent->Id)
    {
        case WLAN_EVENT_CONNECT:
            /* Update status: connected. */
            g_wifi_status = WSS_CONN_SUCCESS;
            if (g_wifi_event_cb) {
                g_wifi_event_cb(WFE_CONNECTED, NULL);
            }
            break;

        case WLAN_EVENT_DISCONNECT:
            /* Update status: disconnected. */
            g_wifi_status = WSS_IDLE;
            if (g_wifi_event_cb) {
                g_wifi_event_cb(WFE_DISCONNECTED, NULL);
            }
            break;

        case WLAN_EVENT_SCAN_RESULT:
        {
            if (g_scan_sem != NULL) {
                WlanEventScanResult_t *scan_data = &pWlanEvent->Data.ScanResult;
                uint32_t count = scan_data->NetworkListResultLen;

                if (count > WLAN_MAX_SCAN_COUNT) count = WLAN_MAX_SCAN_COUNT;

                g_scan_results_ptr = (AP_IF_S *)tkl_system_malloc(sizeof(AP_IF_S) * count);

                if (g_scan_results_ptr != NULL) {
                    memset(g_scan_results_ptr, 0, sizeof(AP_IF_S) * count);

                    for (uint32_t i = 0; i < count; i++) {
                        WlanNetworkEntry_t *entry = &scan_data->NetworkListResult[i];

                        g_scan_results_ptr[i].s_len = entry->SsidLen;
                        if (g_scan_results_ptr[i].s_len > WIFI_SSID_LEN) {
                            g_scan_results_ptr[i].s_len = WIFI_SSID_LEN;
                        }

                        memcpy(g_scan_results_ptr[i].ssid, entry->Ssid, g_scan_results_ptr[i].s_len);
                        g_scan_results_ptr[i].ssid[g_scan_results_ptr[i].s_len] = '\0';

                        memcpy(g_scan_results_ptr[i].bssid, entry->Bssid, MAC_ADDR_LEN);
                        g_scan_results_ptr[i].rssi = entry->Rssi;
                        g_scan_results_ptr[i].channel = entry->Channel;
                        g_scan_results_ptr[i].security = _ti_sec_to_tuya(entry->SecurityInfo);
                    }
                    g_scan_count = count;
                } else {
                    g_scan_count = 0;
                }

                tkl_semaphore_post(g_scan_sem);
            }
            break;
        }

        default:
            break;
    }
}

/* Core TKL Wi-Fi functions. */

OPERATE_RET tkl_wifi_init(WIFI_EVENT_CB cb)
{
    int ret;
    g_wifi_event_cb = cb;
    ret = tkl_semaphore_create_init(&g_scan_sem,
                                    SCAN_SEMAPHORE_INITIAL_COUNT,
                                    SCAN_SEMAPHORE_MAX_COUNT);
    if (ret != OPRT_OK) {
        return OPRT_COM_ERROR;
    }

    ret = Wlan_Start(TiWlanEventHandler);
    if (ret != 0) {
        return OPRT_COM_ERROR;
    }
    tkl_system_sleep(WIFI_ROLE_TRANSITION_DELAY_MS);
    RoleUpStaCmd_t staParams = {0};
    memcpy(staParams.countryDomain,
           s_wifi_country_domain,
           sizeof(staParams.countryDomain));
    staParams.wpsDisabled = TRUE;
    staParams.p2pDeviceEnabled = FALSE;

    ret = Wlan_RoleUp(WLAN_ROLE_STA, &staParams, WLAN_WAIT_FOREVER);
    if (ret != 0) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
}

OPERATE_RET tkl_wifi_scan_ap(const int8_t *ssid, AP_IF_S **ap_ary, uint32_t *num)
{
    int ret;
    g_scan_count = 0;
    g_scan_results_ptr = NULL;

    ret = Wlan_Scan(WLAN_ROLE_STA, NULL, WLAN_MAX_SCAN_COUNT);
    if (ret != 0) return OPRT_COM_ERROR;

    ret = tkl_semaphore_wait(g_scan_sem, WIFI_SCAN_TIMEOUT_MS);
    if (ret != OPRT_OK) return OPRT_TIMEOUT;

    if (g_scan_results_ptr != NULL) {
        *ap_ary = g_scan_results_ptr;
        *num = g_scan_count;
        return OPRT_OK;
    }

    *num = 0;
    return OPRT_OK;
}

OPERATE_RET tkl_wifi_release_ap(AP_IF_S *ap)
{
    if (ap != NULL) {
        tkl_system_free(ap);
    }
    return OPRT_OK;
}

OPERATE_RET tkl_wifi_station_connect(const int8_t *ssid, const int8_t *passwd)
{
    int16_t ret = 0;
    char sec_type = WLAN_SEC_TYPE_OPEN;
    char *password_ptr = NULL;
    int password_len = 0;

    /* Track connection state locally before the driver callback arrives. */
    g_wifi_status = WSS_CONNECTING;

    if (ssid == NULL) return OPRT_INVALID_PARM;

    if (passwd == NULL || strlen((char*)passwd) == 0) {
        sec_type = WLAN_SEC_TYPE_OPEN;
    } else {
        sec_type = WLAN_SEC_TYPE_WPA_WPA2;
        password_ptr = (char *)passwd;
        password_len = strlen((char*)passwd);
    }

    ret = Wlan_Connect((signed char *)ssid, strlen((char*)ssid),
                       NULL,
                       sec_type,
                       password_ptr,
                       password_len,
                       0);

    if (ret != 0) {
        g_wifi_status = WSS_CONN_FAIL;
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
}

OPERATE_RET tkl_wifi_station_disconnect(void)
{
    Wlan_Disconnect(WLAN_ROLE_STA, NULL);
    return OPRT_OK;
}

OPERATE_RET tkl_wifi_get_mac(const WF_IF_E wf, NW_MAC_S *mac)
{
    WlanMacAddress_t macParam = {0};
    int ret;

    if (mac == NULL) return OPRT_INVALID_PARM;

    /* Retrieve the station MAC used to form Tuya's SmartLife AP name. */
    macParam.roleType = WLAN_ROLE_STA;
    ret = Wlan_Get(WLAN_GET_MACADDRESS, &macParam);

    if (ret != 0 ||
        (macParam.pMacAddress[0] == 0 &&
         macParam.pMacAddress[MAC_ADDR_LEN - 1U] == 0)) {
        return OPRT_COM_ERROR;
    }

    memcpy(mac->mac, macParam.pMacAddress, MAC_ADDR_LEN);
    return OPRT_OK;
}

OPERATE_RET tkl_wifi_get_work_mode(WF_WK_MD_E *mode)
{
    *mode = WWM_STATION;
    return OPRT_OK;
}

OPERATE_RET tkl_wifi_set_work_mode(const WF_WK_MD_E mode)
{
    if (mode == WWM_STATION) return OPRT_OK;
    return OPRT_NOT_SUPPORTED;
}

/* --- Added Functionality: Status --- */
OPERATE_RET tkl_wifi_station_get_status(WF_STATION_STAT_E *stat)
{
    if (stat == NULL) return OPRT_INVALID_PARM;
    *stat = g_wifi_status;
    return OPRT_OK;
}

/* --- Added Functionality: RSSI --- */
OPERATE_RET tkl_wifi_station_get_conn_ap_rssi(int8_t *rssi)
{
    int ret;
    WlanBeaconRssi_t beaconRssi;

    if (rssi == NULL) return OPRT_INVALID_PARM;

    /* Only valid if connected. */
    if (g_wifi_status != WSS_CONN_SUCCESS) {
        return OPRT_COM_ERROR;
    }

    beaconRssi.role_id = WLAN_ROLE_STA;
    ret = Wlan_Get(WLAN_GET_RSSI, &beaconRssi);

    if (ret == 0) {
        /* Use average data RSSI. */
        *rssi = beaconRssi.rssi_data;
        return OPRT_OK;
    }

    return OPRT_COM_ERROR;
}

/* Soft AP support. */
OPERATE_RET tkl_wifi_start_ap(const WF_AP_CFG_IF_S *cfg)
{
    int ret;
    RoleUpApCmd_t apParams = {0};
    static char safe_ssid[WIFI_SSID_LEN + 1U];

    if (cfg == NULL || cfg->s_len > WIFI_SSID_LEN) {
        return OPRT_INVALID_PARM;
    }

    /* Switch from station mode before configuring the provisioning AP. */
    ret = Wlan_RoleDown(WLAN_ROLE_STA, WLAN_WAIT_FOREVER);
    if (ret != 0) {
        return OPRT_COM_ERROR;
    }
    tkl_system_sleep(WIFI_ROLE_TRANSITION_DELAY_MS);

    /* TI requires a null-terminated SSID, while Tuya provides an explicit length. */
    memset(safe_ssid, 0, sizeof(safe_ssid));
    memcpy(safe_ssid, cfg->ssid, cfg->s_len);
    apParams.ssid = (uint8_t *)safe_ssid;

    apParams.channel = cfg->chan;

    /* Use the same regulatory domain as the station role. */
    memcpy(apParams.countryDomain,
           s_wifi_country_domain,
           sizeof(apParams.countryDomain));

    apParams.sta_limit = AP_MAX_STATIONS;
    apParams.wpsDisabled = TRUE;
    apParams.p2pDeviceEnabled = FALSE;

    apParams.secParams.Type = WLAN_SEC_TYPE_OPEN;

    ret = Wlan_RoleUp(WLAN_ROLE_AP, &apParams, WLAN_WAIT_FOREVER);
    if (ret != 0) {
        return OPRT_COM_ERROR;
    }

    /* Attach the AP interface before starting its DHCP server. */
    network_stack_add_if_ap();
    tkl_system_sleep(AP_NETWORK_INTERFACE_DELAY_MS);

    if (network_stack_set_dhcp_server_if_ap(1) != 0) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
}

OPERATE_RET tkl_wifi_stop_ap(void)
{
    int ret = Wlan_RoleDown(WLAN_ROLE_AP, WLAN_WAIT_FOREVER);
    return (ret == 0) ? OPRT_OK : OPRT_COM_ERROR;
}


/* Default Wi-Fi event bridge used by higher-level Tuya code. */
void tkl_wifi_default_event_cb(WF_EVENT_E event, void *arg)
{

    switch (event) {
        case WFE_CONNECTED:
            UART_PRINT("\n\r[TKL WIFI] Status: Connected to Router\n\r");
            break;
        case WFE_CONNECT_FAILED:
            UART_PRINT("\n\r[TKL WIFI] Status: Connection to the Router Failed \n\r");
            break;
        case WFE_DISCONNECTED:
            UART_PRINT("\n\r[TKL WIFI] Status: Disconnected from the Router \n\r");
            break;
        default:
            UART_PRINT("\n\r[TKL WIFI] Status: Unhandled Event ID: %d\n\r", (int)event);
            break;
    }
}

/* Read the current interface IP configuration. */
OPERATE_RET tkl_wifi_get_ip(const WF_IF_E wf, NW_IP_S *ip)
{
    if (ip == NULL) {
        return OPRT_INVALID_PARM;
    }

    WlanRole_e role   = (wf == WF_AP) ? WLAN_ROLE_AP : WLAN_ROLE_STA;
    uint32_t raw_ip   = 0u;
    uint32_t raw_mask = 0u;
    uint32_t raw_gw   = 0u;

    int8_t ret = network_stack_get_if_ip(role, &raw_ip, &raw_mask, &raw_gw, NULL);
    if (ret < 0) {
        return OPRT_COM_ERROR;
    }

    ip4_addr_t addr;

    addr.addr = raw_ip;
    strncpy(ip->ip,   ip4addr_ntoa(&addr), sizeof(ip->ip)   - 1);
    addr.addr = raw_mask;
    strncpy(ip->mask, ip4addr_ntoa(&addr), sizeof(ip->mask) - 1);
    addr.addr = raw_gw;
    strncpy(ip->gw,   ip4addr_ntoa(&addr), sizeof(ip->gw)   - 1);

    return OPRT_OK;
}
