/*
 *  ======== ti_drivers_config.c ========
 *  Configured TI-Drivers module definitions
 *
 *  DO NOT EDIT - This file is generated for the LP_EM_CC35X1
 *  by the SysConfig tool.
 */

#include <stddef.h>
#include <stdint.h>

#ifndef DeviceFamily_CC35XX
#define DeviceFamily_CC35XX
#endif

#include <ti/devices/DeviceFamily.h>

#include "ti_drivers_config.h"

/*
 *  =============================== ADC ===============================
 */

#include <ti/drivers/ADC.h>
#include <ti/drivers/adc/ADCWFF3.h>
#include <ti/drivers/GPIO.h>

#define CONFIG_ADC_COUNT 1

/*
 *  ======== ADCWFF3_objects ========
 */
ADCWFF3_Object ADCWFF3_objects[CONFIG_ADC_COUNT];

/*
 *  ======== ADCWFF3_hwAttrs ========
 */
const ADCWFF3_HWAttrs ADCWFF3_hwAttrs[CONFIG_ADC_COUNT] = {
    /* CONFIG_ADC_0 */
    {
        .adcInputDIO         = CONFIG_GPIO_ADC_0_CHANNEL,
        .internalChannel     = 5,
        .refSource           = ADCWFF3_VDDA_REFERENCE,
        .samplingDuration    = 20,
        .adcClkDivider       = ADCWFF3_CLKDIV_8,
        .sampleClkSrc        = ADCWFF3_SAMPLE_CLK_SOC_CLK,
        .fullScaleRange      = ADCWFF3_FULL_SCALE_RANGE_0V0_3V3
    },
};

/*
 *  ======== ADC_config ========
 */
const ADC_Config ADC_config[CONFIG_ADC_COUNT] = {
    /* CONFIG_ADC_0 */
    {
        .fxnTablePtr    = &ADCWFF3_fxnTable,
        .object         = &ADCWFF3_objects[CONFIG_ADC_0],
        .hwAttrs        = &ADCWFF3_hwAttrs[CONFIG_ADC_0]
    },
};

const uint_least8_t CONFIG_ADC_0_CONST = CONFIG_ADC_0;
const uint_least8_t ADC_count = CONFIG_ADC_COUNT;


/*
 *  =============================== Key Store ===============================
 */
#include <third_party/hsmddk/include/Integration/Adapter_PSA/incl/adapter_psa_key_management.h>

uint8_t volatileAllocBuffer[KEYSTORE_VOLATILE_MEMORY_POOL_SIZE];
const size_t volatileAllocBufferSizeBytes  = KEYSTORE_VOLATILE_MEMORY_POOL_SIZE;

const size_t MBEDTLS_KEY_VOLATILE_COUNT    = KEYSTORE_VOLATILE_SLOT_COUNT;
const size_t MBEDTLS_KEY_ASSET_STORE_COUNT = KEYSTORE_ASSET_STORE_SLOT_COUNT;
/* For cache slots */
const size_t MBEDTLS_KEY_PERSISTENT_COUNT  = KEYSTORE_PERSISTENT_SLOT_COUNT;
/* For Key Store flash space */
const size_t FLASH_KEY_PERSISTENT_COUNT    = KEYSTORE_PERSISTENT_NUM_KEYS;

psa_key_context_t gl_PSA_Key[KEYSTORE_TOTAL_SLOT_COUNT];


/*
 *  =============================== GPIO ===============================
 */

#include <ti/drivers/GPIO.h>

/* The range of pins available on this device */
const uint_least8_t GPIO_pinLowerBound = 0;
const uint_least8_t GPIO_pinUpperBound = 37;

/*
 *  ======== gpioPinConfigs ========
 *  Array of Pin configurations
 */
GPIO_PinConfig gpioPinConfigs[GPIO_NUMBER_OF_CONFIGS] = {
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO0 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO1 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO2 */
    /* Owned by CONFIG_ADC_0_CHANNEL as ADC_0_CHANNEL */
    GPIO_CFG_INPUT_INTERNAL | GPIO_CFG_IN_INT_NONE | GPIO_CFG_PULL_NONE_INTERNAL, /* CONFIG_GPIO_ADC_0_CHANNEL */
    /* Owned by CONFIG_UART2_1 as RX */
    GPIO_CFG_INPUT_INTERNAL | GPIO_CFG_IN_INT_NONE | GPIO_CFG_PULL_DOWN_INTERNAL, /* CONFIG_GPIO_UART2_1_RX */
    /* Owned by CONFIG_UART2_0 as TX */
    GPIO_CFG_OUTPUT_INTERNAL | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_HIGH, /* CONFIG_GPIO_UART2_0_TX */
    /* Owned by CONFIG_UART2_0 as RX */
    GPIO_CFG_INPUT_INTERNAL | GPIO_CFG_IN_INT_NONE | GPIO_CFG_PULL_DOWN_INTERNAL, /* CONFIG_GPIO_UART2_0_RX */
    GPIOWFF3_DO_NOT_CONFIG, /* Pin is not available on this device */
    GPIOWFF3_DO_NOT_CONFIG, /* Pin is not available on this device */
    GPIOWFF3_DO_NOT_CONFIG, /* Pin is not available on this device */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO10 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO11 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO12 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO13 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO14 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO15 */
    /* Owned by CONFIG_UART2_1 as TX */
    GPIO_CFG_OUTPUT_INTERNAL | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_HIGH, /* CONFIG_GPIO_UART2_1_TX */
    /* Owned by CONFIG_SPI_0 as SCLK */
    GPIO_CFG_OUTPUT_INTERNAL | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW, /* CONFIG_GPIO_SPI_0_SCLK */
    /* Owned by CONFIG_SPI_0 as POCI */
    GPIO_CFG_INPUT_INTERNAL | GPIO_CFG_IN_INT_NONE | GPIO_CFG_PULL_NONE_INTERNAL, /* CONFIG_GPIO_SPI_0_POCI */
    /* Owned by CONFIG_SPI_0 as PICO */
    GPIO_CFG_OUTPUT_INTERNAL | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW, /* CONFIG_GPIO_SPI_0_PICO */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO20 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO21 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO22 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO23 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO24 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO25 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO26 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO27 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO28 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO29 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO30 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO31 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO32 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO33 */
    GPIO_CFG_OUTPUT_INTERNAL | GPIO_CFG_OUT_STR_LOW | GPIO_CFG_OUT_LOW, /* CONFIG_GPIO_LED_0 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO35 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO36 */
    GPIOWFF3_DO_NOT_CONFIG, /* GPIO37 */
};

/*
 *  ======== gpioCallbackFunctions ========
 *  Array of callback function pointers
 *  Change at runtime with GPIO_setCallback()
 */
GPIO_CallbackFxn gpioCallbackFunctions[GPIO_NUMBER_OF_CONFIGS];

/*
 *  ======== gpioUserArgs ========
 *  Array of user argument pointers
 *  Change at runtime with GPIO_setUserArg()
 *  Get values with GPIO_getUserArg()
 */
void* gpioUserArgs[GPIO_NUMBER_OF_CONFIGS];

const uint_least8_t CONFIG_GPIO_ADC_0_CHANNEL_CONST = CONFIG_GPIO_ADC_0_CHANNEL;
const uint_least8_t CONFIG_GPIO_LED_0_CONST = CONFIG_GPIO_LED_0;
const uint_least8_t CONFIG_GPIO_SPI_0_SCLK_CONST = CONFIG_GPIO_SPI_0_SCLK;
const uint_least8_t CONFIG_GPIO_SPI_0_POCI_CONST = CONFIG_GPIO_SPI_0_POCI;
const uint_least8_t CONFIG_GPIO_SPI_0_PICO_CONST = CONFIG_GPIO_SPI_0_PICO;
const uint_least8_t CONFIG_GPIO_UART2_0_TX_CONST = CONFIG_GPIO_UART2_0_TX;
const uint_least8_t CONFIG_GPIO_UART2_0_RX_CONST = CONFIG_GPIO_UART2_0_RX;
const uint_least8_t CONFIG_GPIO_UART2_1_TX_CONST = CONFIG_GPIO_UART2_1_TX;
const uint_least8_t CONFIG_GPIO_UART2_1_RX_CONST = CONFIG_GPIO_UART2_1_RX;

/*
 *  ======== GPIO_config ========
 */
const GPIO_Config GPIO_config = {
    .configs = (GPIO_PinConfig *)gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *)gpioCallbackFunctions,
    .userArgs = gpioUserArgs,
    .intPriority = (~0)
};

/*
 *  =============== GPTimer ===============
 */

#include <ti/drivers/timer/GPTimerWFF3.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/GPIO.h>
#include <ti/devices/cc35xx/inc/hw_memmap.h>
#include <ti/devices/cc35xx/inc/hw_ints.h>

#define CONFIG_GPTIMER_COUNT 1

/*
 *  ======== GPTimerWFF3_objects ========
 */
GPTimerWFF3_Object GPTimerWFF3_objects[CONFIG_GPTIMER_COUNT];

/*
 *  ======== GPTimerWFF3_hwAttrs ========
 */
static const GPTimerWFF3_HWAttrs GPTimerWFF3_hwAttrs[CONFIG_GPTIMER_COUNT] = {
  {
    .baseAddr           = GPTIMER0_BASE,
    .intNum             = INT_GPTIMER_0_EVT_CPU_IRQ,
    .intPriority        = (~0),
    .powerID            = PowerWFF3_PERIPH_GPTIMER0,
    .channelConfig[0]   = {
        .pin     = GPIO_INVALID_INDEX,
        .pinMux  = GPIO_MUX_GPIO_INTERNAL,
        .nPin    = GPIO_INVALID_INDEX,
        .nPinMux = GPIO_MUX_GPIO_INTERNAL,
    },
    .channelConfig[1]   = {
        .pin     = GPIO_INVALID_INDEX,
        .pinMux  = GPIO_MUX_GPIO_INTERNAL,
        .nPin    = GPIO_INVALID_INDEX,
        .nPinMux = GPIO_MUX_GPIO_INTERNAL,
    },
    .channelConfig[2]   = {
        .pin     = GPIO_INVALID_INDEX,
        .pinMux  = GPIO_MUX_GPIO_INTERNAL,
        .nPin    = GPIO_INVALID_INDEX,
        .nPinMux = GPIO_MUX_GPIO_INTERNAL,
    },
    .channelConfig[3]   = {
        .pin     = GPIO_INVALID_INDEX,
        .pinMux  = GPIO_MUX_GPIO_INTERNAL,
        .nPin    = GPIO_INVALID_INDEX,
        .nPinMux = GPIO_MUX_GPIO_INTERNAL,
    },
  },
};

/*
 *  ======== GPTimer_config ========
 */
const GPTimerWFF3_Config GPTimerWFF3_config[CONFIG_GPTIMER_COUNT] = {
    {   /* CONFIG_GPTIMER_0 */
        .object      = &GPTimerWFF3_objects[CONFIG_GPTIMER_0],
        .hwAttrs     = &GPTimerWFF3_hwAttrs[CONFIG_GPTIMER_0]
    },
};

const uint_least8_t CONFIG_GPTIMER_0_CONST = CONFIG_GPTIMER_0;
const uint_least8_t GPTimerWFF3_count = CONFIG_GPTIMER_COUNT;

/*
 *  =============================== PWM ===============================
 */

#include <ti/drivers/PWM.h>
#include <ti/drivers/pwm/PWMTimerWFF3.h>

/* include driverlib definitions */
#include <ti/devices/cc35xx/inc/hw_ints.h>
#include <ti/devices/cc35xx/inc/hw_memmap.h>

#define CONFIG_PWM_COUNT 1

/*
 *  ======== PWMTimerWFF3_objects ========
 */
PWMTimerWFF3_Object PWMTimerWFF3_objects[CONFIG_PWM_COUNT];

/*
 *  ======== PWMTimerWFF3_hwAttrs ========
 */
const PWMTimerWFF3_HwAttrs PWMTimerWFF3_hwAttrs[CONFIG_PWM_COUNT] = {
    /* CONFIG_PWM_0 */
    {
        .gpTimerInstance = CONFIG_GPTIMER_0,
        .preScalerDivision = 1
    },
};

/*
 *  ======== PWM_config ========
 */
const PWM_Config PWM_config[CONFIG_PWM_COUNT] = {
    /* CONFIG_PWM_0 */
    {
        .fxnTablePtr = &PWMTimerWFF3_fxnTable,
        .object = &PWMTimerWFF3_objects[CONFIG_PWM_0],
        .hwAttrs = &PWMTimerWFF3_hwAttrs[CONFIG_PWM_0]
    },
};

const uint_least8_t CONFIG_PWM_0_CONST = CONFIG_PWM_0;
const uint_least8_t PWM_count = CONFIG_PWM_COUNT;

/*
 *  =============================== Power ===============================
 */
#include <ti/drivers/Power.h>

extern void PowerWFF3_sleepPolicy(void);
/*
 *  This structure defines the configuration for the Power Manager.
 */
const PowerWFF3_Config PowerWFF3_config = {
    .policyInitFxn             = NULL,
    .policyFxn                 = PowerWFF3_sleepPolicy,
};

/*
 *  =============================== RNG ===============================
 */

#include <ti/drivers/RNG.h>
#include <ti/drivers/rng/RNGXXF3HSM.h>

#define CONFIG_RNG_COUNT 0

#if defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=32
#else
__attribute__ ((aligned (32)))
#endif

const RNG_ReturnBehavior RNGXXF3HSM_returnBehavior = RNG_RETURN_BEHAVIOR_POLLING;

RNGXXF3HSM_Object RNGXXF3HSM_objects[CONFIG_RNG_COUNT];

/*
 *  ======== RNGXXF3HSMHWAttrs ========
 */
const RNGXXF3HSM_HWAttrs RNGXXF3HSM_hwAttrs[CONFIG_RNG_COUNT] = {
};

const RNG_Config RNG_config[CONFIG_RNG_COUNT] = {
};


const uint_least8_t RNG_count = CONFIG_RNG_COUNT;

/*
 *  =============================== SPI DMA ===============================
 */
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPIWFF3DMA.h>
#include <ti/drivers/dma/DMAWFF3.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_ints.h)

#define CONFIG_SPI_COUNT 1

/*
 *  ======== SPIWFF3DMA_objects ========
 */
SPIWFF3DMA_Object SPIWFF3DMA_objects[CONFIG_SPI_COUNT];

/*
 *  ======== SPIWFF3DMA_hwAttrs ========
 */
const SPIWFF3DMA_HWAttrs SPIWFF3DMA_hwAttrs[CONFIG_SPI_COUNT] = {
    {
        .baseAddr = SPI0_BASE,
        .intNum = INT_SP_SPI_0_EVT_REQ,
        .intPriority = (~0),
        .swiPriority = 0,
        .defaultTxBufValue = ~0,
        .rxDmaChannel = 11,
        .txDmaChannel = 10,
        .rxChannelEvtMux = DMAWFF3_PERIPH_SPI_0,
        .txChannelEvtMux = DMAWFF3_PERIPH_SPI_0,
        .minDmaTransferSize = 10,
        .picoPinMux   = 4,
        .pociPinMux   = 4,
        .sclkPinMux  = 4,
        .picoPin = CONFIG_GPIO_SPI_0_PICO,
        .pociPin = CONFIG_GPIO_SPI_0_POCI,
        .sclkPin  = CONFIG_GPIO_SPI_0_SCLK,
        .csnPin  = GPIO_INVALID_INDEX,
        .powerID = PowerWFF3_PERIPH_SPI0,
    },
};

/*
 *  ======== SPI_config ========
 */
const SPI_Config SPI_config[CONFIG_SPI_COUNT] = {
    /* CONFIG_SPI_0 */
    {
        .fxnTablePtr = &SPIWFF3DMA_fxnTable,
        .object = &SPIWFF3DMA_objects[CONFIG_SPI_0],
        .hwAttrs = &SPIWFF3DMA_hwAttrs[CONFIG_SPI_0]
    },
};

const uint_least8_t CONFIG_SPI_0_CONST = CONFIG_SPI_0;
const uint_least8_t SPI_count = CONFIG_SPI_COUNT;

/*
 *  =============================== TRNG ===============================
 */

#include <ti/drivers/TRNG.h>
#include <ti/drivers/trng/TRNGXXF3HSM.h>

#define CONFIG_TRNG_COUNT 1

TRNGXXF3HSM_Object trngXXF3HSMObjects[CONFIG_TRNG_COUNT];

/*
 *  ======== trngXXF3HSMHWAttrs ========
 */
static const TRNGXXF3HSM_HWAttrs trngXXF3HSMHWAttrs[CONFIG_TRNG_COUNT] = {
    {0},
};

const TRNG_Config TRNG_config[CONFIG_TRNG_COUNT] = {
    {   /* CONFIG_TRNG_0 */
        .object         = &trngXXF3HSMObjects[CONFIG_TRNG_0],
        .hwAttrs        = &trngXXF3HSMHWAttrs[CONFIG_TRNG_0]
    },
};

const uint_least8_t CONFIG_TRNG_0_CONST = CONFIG_TRNG_0;
const uint_least8_t TRNG_count = CONFIG_TRNG_COUNT;

/*
 *  =============================== UART2 ===============================
 */

#include <ti/drivers/UART2.h>
#include <ti/drivers/uart2/UART2WFF3.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/dma/DMAWFF3.h>
#include <ti/drivers/Power.h>
#include <ti/devices/cc35xx/inc/hw_memmap.h>
#include <ti/devices/cc35xx/inc/hw_ints.h>
#include <ti/devices/cc35xx/inc/hw_soc_aon.h>

#define CONFIG_UART2_COUNT 2

UART2WFF3_Object UART2WFF3_objects[CONFIG_UART2_COUNT];

static unsigned char uart2RxRingBuffer0[32];
/* TX ring buffer allocated to be used for nonblocking mode */
static unsigned char uart2TxRingBuffer0[32];
static unsigned char uart2RxRingBuffer1[32];
/* TX ring buffer allocated to be used for nonblocking mode */
static unsigned char uart2TxRingBuffer1[32];

static const UART2WFF3_HWAttrs UART2WFF3_hwAttrs[CONFIG_UART2_COUNT] = {
  {
    .baseAddr           = UARTLIN1_BASE,
    .intNum             = INT_SP_UART_1_INT_REQ,
    .intPriority        = (~0),
    .rxPin              = CONFIG_GPIO_UART2_0_RX,
    .txPin              = CONFIG_GPIO_UART2_0_TX,
    .ctsPin             = GPIO_INVALID_INDEX,
    .rtsPin             = GPIO_INVALID_INDEX,
    .flowControl        = UART2_FLOWCTRL_NONE,
    .rxBufPtr           = uart2RxRingBuffer0,
    .rxBufSize          = sizeof(uart2RxRingBuffer0),
    .txBufPtr           = uart2TxRingBuffer0,
    .txBufSize          = sizeof(uart2TxRingBuffer0),
    .txPinMux           = 5,
    .rxPinMux           = 5,
    .ctsPinMux          = GPIO_MUX_GPIO_INTERNAL,
    .rtsPinMux          = GPIO_MUX_GPIO_INTERNAL,
    .rxDmaChannel       = 3,
    .txDmaChannel       = 2,
    .codingScheme       = UART2WFF3_CODING_UART,
    .concatenateFIFO    = false,
    .irLPClkDivider     = 26,
    .powerID            = PowerWFF3_PERIPH_UARTLIN1
  },
  {
    .baseAddr           = UARTLIN2_BASE,
    .intNum             = INT_SP_UART_2_INT_REQ,
    .intPriority        = (~0),
    .rxPin              = CONFIG_GPIO_UART2_1_RX,
    .txPin              = CONFIG_GPIO_UART2_1_TX,
    .ctsPin             = GPIO_INVALID_INDEX,
    .rtsPin             = GPIO_INVALID_INDEX,
    .flowControl        = UART2_FLOWCTRL_NONE,
    .rxBufPtr           = uart2RxRingBuffer1,
    .rxBufSize          = sizeof(uart2RxRingBuffer1),
    .txBufPtr           = uart2TxRingBuffer1,
    .txBufSize          = sizeof(uart2TxRingBuffer1),
    .txPinMux           = 30,
    .rxPinMux           = 30,
    .ctsPinMux          = GPIO_MUX_GPIO_INTERNAL,
    .rtsPinMux          = GPIO_MUX_GPIO_INTERNAL,
    .rxDmaChannel       = 0,
    .txDmaChannel       = 1,
    .codingScheme       = UART2WFF3_CODING_UART,
    .concatenateFIFO    = false,
    .irLPClkDivider     = 26,
    .powerID            = PowerWFF3_PERIPH_UARTLIN2
  },
};

const UART2_Config UART2_config[CONFIG_UART2_COUNT] = {
    {   /* CONFIG_UART2_0 */
        .object      = &UART2WFF3_objects[CONFIG_UART2_0],
        .hwAttrs     = &UART2WFF3_hwAttrs[CONFIG_UART2_0]
    },
    {   /* CONFIG_UART2_1 */
        .object      = &UART2WFF3_objects[CONFIG_UART2_1],
        .hwAttrs     = &UART2WFF3_hwAttrs[CONFIG_UART2_1]
    },
};

const uint_least8_t CONFIG_UART2_0_CONST = CONFIG_UART2_0;
const uint_least8_t CONFIG_UART2_1_CONST = CONFIG_UART2_1;
const uint_least8_t UART2_count = CONFIG_UART2_COUNT;


#include <stdbool.h>

#include <ti/drivers/GPIO.h>



#include <ti/drivers/Board.h>

/*
 *  ======== Board_initHook ========
 *  Perform any board-specific initialization needed at startup.  This
 *  function is declared weak to allow applications to override it if needed.
 */
void __attribute__((weak)) Board_initHook(void)
{
}

/*
 *  ======== Board_init ========
 *  Perform any initialization needed before using any board APIs
 */
void Board_init(void)
{
    /* ==== /ti/drivers/Power initialization ==== */
    Power_init();

    /* ==== /ti/drivers/GPIO initialization ==== */
    /* Setup GPIO module and default-initialise pins */
    GPIO_init();

    /* ==== /ti/drivers/DMA initialization ==== */
    DMAWFF3_init();


    Board_initHook();
}

