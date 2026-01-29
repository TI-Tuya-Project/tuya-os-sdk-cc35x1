/**
 * @file tkl_board.c
 * @brief Tuya Kernel Layer - Hardware Board Implementation
 */

#include "tkl_board_config.h"
#include <string.h>

// Global storage for the board configuration
// Initialize all maps to -1 (Invalid) by default
static TKL_BOARD_CONFIG_T g_board_config;
static int g_is_initialized = 0;

static void _init_defaults(void) {
    if (!g_is_initialized) {
        memset(&g_board_config, -1, sizeof(g_board_config));
        g_is_initialized = 1;
    }
}

void tkl_hw_board_init(const TKL_BOARD_CONFIG_T *config)
{
    if (config) {
        // Copy the provided config into our internal storage
        memcpy(&g_board_config, config, sizeof(TKL_BOARD_CONFIG_T));
        g_is_initialized = 1;
    } else {
        _init_defaults();
    }
}

// --- Getters ---

int16_t tkl_hw_get_adc_index(uint8_t port_num)
{
    _init_defaults();
    if (port_num >= TKL_HW_MAX_ADC_PORTS) return -1;
    return g_board_config.adc_map[port_num];
}

int16_t tkl_hw_get_gpio_index(uint8_t pin_id)
{
    _init_defaults();
    if (pin_id >= TKL_HW_MAX_GPIO_PINS) return -1;
    return g_board_config.gpio_map[pin_id];
}

int16_t tkl_hw_get_uart_index(uint8_t port_id)
{
    _init_defaults();
    if (port_id >= TKL_HW_MAX_UART_PORTS) return -1;
    return g_board_config.uart_map[port_id];
}

int16_t tkl_hw_get_pwm_index(uint8_t channel)
{
    _init_defaults();
    if (channel >= TKL_HW_MAX_PWM_CHANNELS) return -1;
    return g_board_config.pwm_map[channel];
}

int16_t tkl_hw_get_i2c_index(uint8_t port)
{
    _init_defaults();
    if (port >= TKL_HW_MAX_I2C_PORTS) return -1;
    return g_board_config.i2c_map[port];
}

int16_t tkl_hw_get_i2s_index(uint8_t port)
{
    _init_defaults();
    if (port >= TKL_HW_MAX_I2S_PORTS) return -1;
    return g_board_config.i2s_map[port];
}

int16_t tkl_hw_get_spi_index(uint8_t port)
{
    _init_defaults();
    if (port >= TKL_HW_MAX_SPI_PORTS) return -1;
    return g_board_config.spi_map[port];
}