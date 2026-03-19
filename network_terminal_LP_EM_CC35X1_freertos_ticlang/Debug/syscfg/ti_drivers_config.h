/*
 *  ======== ti_drivers_config.h ========
 *  Configured TI-Drivers module declarations
 *
 *  The macros defines herein are intended for use by applications which
 *  directly include this header. These macros should NOT be hard coded or
 *  copied into library source code.
 *
 *  Symbols declared as const are intended for use with libraries.
 *  Library source code must extern the correct symbol--which is resolved
 *  when the application is linked.
 *
 *  DO NOT EDIT - This file is generated for the LP_EM_CC35X1
 *  by the SysConfig tool.
 */
#ifndef ti_drivers_config_h
#define ti_drivers_config_h

#define CONFIG_SYSCONFIG_PREVIEW

#define CONFIG_LP_EM_CC35X1
#ifndef DeviceFamily_CC35XX
#define DeviceFamily_CC35XX
#endif

#include <ti/devices/DeviceFamily.h>

#include <stdint.h>

/* support C++ sources */
#ifdef __cplusplus
extern "C" {
#endif


/*
 *  ======== ADC ========
 */

extern const uint_least8_t              CONFIG_ADC_0_CONST;
#define CONFIG_ADC_0                    0
#define CONFIG_TI_DRIVERS_ADC_COUNT     1



/*
 *  ======== Key Store ========
 */

#define KEYSTORE_VOLATILE_MEMORY_POOL_SIZE 2760

#define KEYSTORE_VOLATILE_SLOT_COUNT       5
#define KEYSTORE_ASSET_STORE_SLOT_COUNT    53
#define KEYSTORE_PERSISTENT_SLOT_COUNT     1
#define KEYSTORE_TOTAL_SLOT_COUNT          59

#define KEYSTORE_PERSISTENT_NUM_KEYS       45



/*
 *  ======== GPIO ========
 */
#include <ti/drivers/GPIO.h>

#define GPIO_NUMBER_OF_CONFIGS 38

/* Owned by CONFIG_ADC_0_CHANNEL as  */
extern const uint_least8_t CONFIG_GPIO_ADC_0_CHANNEL_CONST;
#define CONFIG_GPIO_ADC_0_CHANNEL 3

extern const uint_least8_t CONFIG_GPIO_LED_0_CONST;
#define CONFIG_GPIO_LED_0 34

/* Owned by CONFIG_SPI_0 as  */
extern const uint_least8_t CONFIG_GPIO_SPI_0_SCLK_CONST;
#define CONFIG_GPIO_SPI_0_SCLK 17

/* Owned by CONFIG_SPI_0 as  */
extern const uint_least8_t CONFIG_GPIO_SPI_0_POCI_CONST;
#define CONFIG_GPIO_SPI_0_POCI 18

/* Owned by CONFIG_SPI_0 as  */
extern const uint_least8_t CONFIG_GPIO_SPI_0_PICO_CONST;
#define CONFIG_GPIO_SPI_0_PICO 19

/* Owned by CONFIG_UART2_0 as  */
extern const uint_least8_t CONFIG_GPIO_UART2_0_TX_CONST;
#define CONFIG_GPIO_UART2_0_TX 5

/* Owned by CONFIG_UART2_0 as  */
extern const uint_least8_t CONFIG_GPIO_UART2_0_RX_CONST;
#define CONFIG_GPIO_UART2_0_RX 6

/* Owned by CONFIG_UART2_1 as  */
extern const uint_least8_t CONFIG_GPIO_UART2_1_TX_CONST;
#define CONFIG_GPIO_UART2_1_TX 16

/* Owned by CONFIG_UART2_1 as  */
extern const uint_least8_t CONFIG_GPIO_UART2_1_RX_CONST;
#define CONFIG_GPIO_UART2_1_RX 4



/* The range of pins available on this device */
extern const uint_least8_t GPIO_pinLowerBound;
extern const uint_least8_t GPIO_pinUpperBound;

/* LEDs are active high */
#define CONFIG_GPIO_LED_ON  (1)
#define CONFIG_GPIO_LED_OFF (0)

#define CONFIG_LED_ON  (CONFIG_GPIO_LED_ON)
#define CONFIG_LED_OFF (CONFIG_GPIO_LED_OFF)


/*
 *  ======== GPTimer ========
 */

/*
 *  CH3: Unassigned
 *  CH3N: Unassigned
 */
extern const uint_least8_t                  CONFIG_GPTIMER_0_CONST;
#define CONFIG_GPTIMER_0                    0
#define CONFIG_TI_DRIVERS_GPTIMER_COUNT     1


/*
 *  ======== PWM ========
 */

extern const uint_least8_t              CONFIG_PWM_0_CONST;
#define CONFIG_PWM_0                    0
#define CONFIG_TI_DRIVERS_PWM_COUNT     1


/*
 *  ======== RNG ========
 */

#define CONFIG_TI_DRIVERS_RNG_COUNT     0

#define RNG_POOL_BYTE_SIZE 



/*
 *  ======== SPI ========
 */

/*
 *  PICO: GPIO19
 *  POCI: GPIO18
 *  SCLK: GPIO17
 */
extern const uint_least8_t              CONFIG_SPI_0_CONST;
#define CONFIG_SPI_0                    0
#define CONFIG_TI_DRIVERS_SPI_COUNT     1


/*
 *  ======== TRNG ========
 */

extern const uint_least8_t              CONFIG_TRNG_0_CONST;
#define CONFIG_TRNG_0                   0
#define CONFIG_TI_DRIVERS_TRNG_COUNT    1


/*
 *  ======== UART2 ========
 */

/*
 *  TX: GPIO5
 *  RX: GPIO6
 *  XDS110 UART
 */
extern const uint_least8_t                  CONFIG_UART2_0_CONST;
#define CONFIG_UART2_0                      0
/*
 *  TX: GPIO16
 *  RX: GPIO4
 */
extern const uint_least8_t                  CONFIG_UART2_1_CONST;
#define CONFIG_UART2_1                      1
#define CONFIG_TI_DRIVERS_UART2_COUNT       2


/*
 *  ======== Board_init ========
 *  Perform all required TI-Drivers initialization
 *
 *  This function should be called once at a point before any use of
 *  TI-Drivers.
 */
extern void Board_init(void);

/*
 *  ======== Board_initGeneral ========
 *  (deprecated)
 *
 *  Board_initGeneral() is defined purely for backward compatibility.
 *
 *  All new code should use Board_init() to do any required TI-Drivers
 *  initialization _and_ use <Driver>_init() for only where specific drivers
 *  are explicitly referenced by the application.  <Driver>_init() functions
 *  are idempotent.
 */
#define Board_initGeneral Board_init

#ifdef __cplusplus
}
#endif

#endif /* include guard */
