/**
 * @file tkl_board_config.h
 * @brief Tuya Kernel Layer - Hardware Board Configuration (BSP)
 * @note This file defines the "Dependency Injection" structure.
 * The Application Layer must instantiate this, fill it with 
 * SysConfig-generated indices (ti_drivers_config.h), and pass it 
 * to tkl_hw_board_init().
 */

#ifndef TKL_BOARD_CONFIG_H
#define TKL_BOARD_CONFIG_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Define logical limits for the mapping tables
#define TKL_HW_MAX_ADC_PORTS   8
#define TKL_HW_MAX_GPIO_PINS   64
#define TKL_HW_MAX_UART_PORTS  4
#define TKL_HW_MAX_PWM_CHANNELS 8
#define TKL_HW_MAX_I2C_PORTS   2
#define TKL_HW_MAX_I2S_PORTS   1
#define TKL_HW_MAX_TIMER_PORTS  4

typedef struct {

    uint32_t timer_freq;

    // ADC Mapping: Tuya Port ID -> TI Driver Index (CONFIG_ADC_x)
    int16_t adc_map[TKL_HW_MAX_ADC_PORTS];

    // GPIO Mapping: Tuya Pin ID -> TI Driver Index (CONFIG_GPIO_x)
    int16_t gpio_map[TKL_HW_MAX_GPIO_PINS];

    // UART Mapping: Tuya Port ID -> TI Driver Index (CONFIG_UART_x)
    int16_t uart_map[TKL_HW_MAX_UART_PORTS];

    // PWM Mapping: Tuya Channel -> TI Driver Index (CONFIG_PWM_x)
    int16_t pwm_map[TKL_HW_MAX_PWM_CHANNELS];

    // I2C Mapping: Tuya Port -> TI Driver Index
    int16_t i2c_map[TKL_HW_MAX_I2C_PORTS];

    // I2S Mapping
    int16_t i2s_map[TKL_HW_MAX_I2S_PORTS];

    // Timer Mapping: Tuya Timer ID -> TI Driver Index (CONFIG_TIMER_x)
    int16_t timer_map[TKL_HW_MAX_TIMER_PORTS];

} TKL_BOARD_CONFIG_T;

/**
 * @brief Initialize the Board Configuration
 * @param config Pointer to the configuration structure filled by the App
 */
void tkl_hw_board_init(const TKL_BOARD_CONFIG_T *config);

// --- Getters used by TKL Drivers ---
uint32_t tkl_hw_get_board_timer_freq(void);
int16_t tkl_hw_get_adc_index(uint8_t port_num);
int16_t tkl_hw_get_gpio_index(uint8_t pin_id);
int16_t tkl_hw_get_uart_index(uint8_t port_id);
int16_t tkl_hw_get_pwm_index(uint8_t channel);
int16_t tkl_hw_get_i2c_index(uint8_t port);
int16_t tkl_hw_get_i2s_index(uint8_t port);
int16_t tkl_hw_get_timer_index(uint8_t port);

#ifdef __cplusplus
}
#endif

#endif // TKL_BOARD_CONFIG_H