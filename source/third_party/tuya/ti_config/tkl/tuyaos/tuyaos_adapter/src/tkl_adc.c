// --- BEGIN: user defines and implements ---
#include "tkl_adc.h"
#include "tuya_error_code.h"
#include <ti/drivers/ADC.h>

// Dependency Injection Header
#include "tkl_board_config.h"

// TI SimpleLink ADCs are 12-bit hardware
#define TI_ADC_RESOLUTION_BITS  12
#define TI_ADC_MAX_RAW_VALUE    ((1 << TI_ADC_RESOLUTION_BITS) - 1) // 4095

/* Array to store handles for multiple ADC channels.
 * We use TKL_HW_MAX_ADC_PORTS defined in board_config.h 
 */
static ADC_Handle g_adc_handles[TKL_HW_MAX_ADC_PORTS] = {NULL};

/* Helper to check validity */
static bool is_valid_adc(TUYA_ADC_NUM_E port_num) {
    return (port_num < TKL_HW_MAX_ADC_PORTS);
}
// --- END: user defines and implements ---

OPERATE_RET tkl_adc_init(TUYA_ADC_NUM_E port_num, TUYA_ADC_BASE_CFG_T *cfg)
{
    // --- BEGIN: user implements ---
    if (!is_valid_adc(port_num)) {
        return OPRT_INVALID_PARM;
    }

    // [DYNAMIC LOOKUP] Get the real TI hardware index
    int16_t ti_index = tkl_hw_get_adc_index(port_num);

    if (ti_index < 0) {
        return OPRT_NOT_SUPPORTED; // Port not mapped by Application
    }

    // If already open, close it first
    if (g_adc_handles[port_num] != NULL) {
        ADC_close(g_adc_handles[port_num]);
        g_adc_handles[port_num] = NULL;
    }

    ADC_init();

    ADC_Params params;
    ADC_Params_init(&params);

    // Open using the MAPPED TI index
    g_adc_handles[port_num] = ADC_open(ti_index, &params);

    if (g_adc_handles[port_num] == NULL) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

OPERATE_RET tkl_adc_deinit(TUYA_ADC_NUM_E port_num)
{
    // --- BEGIN: user implements ---
    if (!is_valid_adc(port_num)) {
        return OPRT_INVALID_PARM;
    }

    if (g_adc_handles[port_num] != NULL) {
        ADC_close(g_adc_handles[port_num]);
        g_adc_handles[port_num] = NULL;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

uint8_t tkl_adc_width_get(TUYA_ADC_NUM_E port_num)
{
    // --- BEGIN: user implements ---
    return 12;
    // --- END: user implements ---
}

uint32_t tkl_adc_ref_voltage_get(TUYA_ADC_NUM_E port_num)
{
    // --- BEGIN: user implements ---
    // Dynamic VRef discovery works via the open handle
    if (is_valid_adc(port_num) && g_adc_handles[port_num] != NULL) {
        uint32_t vref_microvolts = ADC_convertToMicroVolts(g_adc_handles[port_num], TI_ADC_MAX_RAW_VALUE);
        return (vref_microvolts / 1000); 
    }
    return 3300;
    // --- END: user implements ---
}

int32_t tkl_adc_temperature_get(void)
{
    // --- BEGIN: user implements ---
    return OPRT_NOT_SUPPORTED;
    // --- END: user implements ---
}

OPERATE_RET tkl_adc_read_data(TUYA_ADC_NUM_E port_num, int32_t *buff, uint16_t len)
{
    // --- BEGIN: user implements ---
    if (!is_valid_adc(port_num) || g_adc_handles[port_num] == NULL || buff == NULL) {
        return OPRT_INVALID_PARM;
    }

    int32_t result = OPRT_OK;
    uint16_t rawValue;
    int_fast16_t res;

    for (uint16_t i = 0; i < len; i++) {
        res = ADC_convert(g_adc_handles[port_num], &rawValue);

        if (res == ADC_STATUS_SUCCESS) {
            buff[i] = (int32_t)rawValue;
        } else {
            result = OPRT_COM_ERROR;
            break; 
        }
    }
    return result;
    // --- END: user implements ---
}

OPERATE_RET tkl_adc_read_single_channel(TUYA_ADC_NUM_E port_num, uint8_t ch_id, int32_t *data)
{
    // --- BEGIN: user implements ---
    return tkl_adc_read_data(port_num, data, 1);
    // --- END: user implements ---
}

OPERATE_RET tkl_adc_read_voltage(TUYA_ADC_NUM_E port_num, int32_t *buff, uint16_t len)
{
    // --- BEGIN: user implements ---
    if (!is_valid_adc(port_num) || g_adc_handles[port_num] == NULL || buff == NULL) {
        return OPRT_INVALID_PARM;
    }

    uint16_t rawValue;
    int_fast16_t res;
    uint32_t microVolts;

    for (uint16_t i = 0; i < len; i++) {
        res = ADC_convert(g_adc_handles[port_num], &rawValue);

        if (res == ADC_STATUS_SUCCESS) {
            microVolts = ADC_convertToMicroVolts(g_adc_handles[port_num], rawValue);
            buff[i] = (int32_t)(microVolts / 1000);
        } else {
            return OPRT_COM_ERROR;
        }
    }
    return OPRT_OK;
    // --- END: user implements ---
}