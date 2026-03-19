/*
 * tuya_mbedtls_port.c
 * REAL Implementation connecting mbedTLS to TI TRNG Hardware
 */
#include <string.h>
#include "mbedtls/entropy.h"
#include <ti/drivers/TRNG.h>
#include <ti/drivers/dpl/DebugP.h>

/* REQUIRED: This header defines CryptoKeyPlaintext_initKey */
#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
/* SysConfig is hiding the setting, so we define the stub manually */
#include "FreeRTOS.h"
#include "task.h"



int mbedtls_hardware_poll( void *data, unsigned char *output, size_t len, size_t *olen )
{
    TRNG_Handle handle;
    TRNG_Params params;
    int_fast16_t result;
    CryptoKey entropyKey; // Wrapper for the buffer

    /* 1. Initialize Driver */
    TRNG_init();
    TRNG_Params_init(&params);
    params.returnBehavior = TRNG_RETURN_BEHAVIOR_BLOCKING;

    /* 2. Open Driver */
    handle = TRNG_open(0, &params);
    if (!handle) {
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }

    /* 3. Initialize the CryptoKey wrapper 
     * This takes the raw 'output' buffer and wraps it into the 'entropyKey' struct.
     * We cast 'output' to (uint8_t*) to match the TI API signature.
     */
    CryptoKeyPlaintext_initKey(&entropyKey, (uint8_t*)output, len);

    /* 4. Generate Entropy 
     * Now we pass just 2 arguments: the handle and the key structure.
     */
    result = TRNG_generateEntropy(handle, &entropyKey);
    
    TRNG_close(handle);

    if (result != TRNG_STATUS_SUCCESS) {
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }

    *olen = len;
    return 0; // Success
}