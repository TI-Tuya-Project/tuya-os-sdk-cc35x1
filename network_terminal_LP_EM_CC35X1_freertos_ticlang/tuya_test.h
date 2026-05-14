#ifndef TUYA_TEST_H
#define TUYA_TEST_H

#include <stdint.h>

/*===========================================================*/
/*                   TKL ADC                                 */
/*===========================================================*/
/*String constants for the CLI parser*/ 
extern const char tuyaAdcTestStr[];
extern const char tuyaAdcTestUsageStr[];
extern const char tuyaAdcTestDetailsStr[];

/* Function prototypes for the CLI table*/
int32_t cmdTuyaAdcTestCallback(void *arg);
int32_t printTuyaAdcTestUsage(void *arg);
/*===========================================================*/
/*                   TKL ADC                                 */
/*===========================================================*/


/*===========================================================*/
/*                   TKL FLASH                               */
/*===========================================================*/
/* String constants for the CLI parser */
extern const char tuyaFlashTestStr[];
extern const char tuyaFlashTestUsageStr[];
extern const char tuyaFlashTestDetailsStr[];

/* Function prototypes for the CLI table */
int32_t cmdTuyaFlashTestCallback(void *arg);
int32_t printTuyaFlashTestUsage(void *arg);
/*===========================================================*/
/*                   TKL FLASH                               */
/*===========================================================*/


/*===========================================================*/
/*                   TKL MEMORY                              */
/*===========================================================*/
/*String constants for the CLI parser*/ 
extern const char tuyaMemTestStr[];
extern const char tuyaMemTestUsageStr[];
extern const char tuyaMemTestDetailsStr[];

/* Function prototypes for the CLI table */
int32_t cmdTuyaMemTestCallback(void *arg);
int32_t printTuyaMemTestUsage(void *arg);
/*===========================================================*/
/*                   TKL MEMORY                              */
/*===========================================================*/


/*===========================================================*/
/* TKL FS (File System)                                      */
/*===========================================================*/
extern const char tuyaFsTestStr[];
extern const char tuyaFsTestUsageStr[];
extern const char tuyaFsTestDetailsStr[];

int32_t printTuyaFsTestUsage(void *arg);
int32_t cmdTuyaFsTestCallback(void *arg);
/*===========================================================*/
/* TKL FS (File System)                                      */
/*===========================================================*/





/*===========================================================*/
/* TKL GPIO                                */
/*===========================================================*/
/* String constants for the CLI parser */
extern const char tuyaGpioTestStr[];
extern const char tuyaGpioTestUsageStr[];
extern const char tuyaGpioTestDetailsStr[];

/* Function prototypes for the CLI table */
int32_t cmdTuyaGpioTestCallback(void *arg);
int32_t printTuyaGpioTestUsage(void *arg);
/*===========================================================*/
/* TKL GPIO                                */
/*===========================================================*/



/*===========================================================*/
/* TKL HASH                                                  */
/*===========================================================*/
/* String constants for the CLI parser */
extern const char tuyaHashTestStr[];
extern const char tuyaHashTestUsageStr[];
extern const char tuyaHashTestDetailsStr[];

/* Function prototypes for the CLI table */
int32_t cmdTuyaHashTestCallback(void *arg);
int32_t printTuyaHashTestUsage(void *arg);
/*===========================================================*/
/* TKL HASH                                                  */
/*===========================================================*/


/*===========================================================*/
/* TKL I2C                                                   */
/*===========================================================*/
/* String constants for the CLI parser */
extern const char tuyaI2cTestStr[];
extern const char tuyaI2cTestUsageStr[];
extern const char tuyaI2cTestDetailsStr[];

/* Function prototypes for the CLI table */
int32_t cmdTuyaI2cTestCallback(void *arg);
int32_t printTuyaI2cTestUsage(void *arg);
/*===========================================================*/
/* TKL I2C                                                   */
/*===========================================================*/



/*===========================================================*/
/* TKL MUTEX                                                 */
/*===========================================================*/
extern const char tuyaMutexTestStr[];
extern const char tuyaMutexTestUsageStr[];
extern const char tuyaMutexTestDetailsStr[];

int32_t cmdTuyaMutexTestCallback(void *arg);
int32_t printTuyaMutexTestUsage(void *arg);
/*===========================================================*/
/* TKL MUTEX                                                 */
/*===========================================================*/



/*===========================================================*/
/* TKL NETWORK                                               */
/*===========================================================*/
extern const char tuyaNetTestStr[];
extern const char tuyaNetTestUsageStr[];
extern const char tuyaNetTestDetailsStr[];

int32_t cmdTuyaNetTestCallback(void *arg);
int32_t printTuyaNetTestUsage(void *arg);
/*===========================================================*/



/*===========================================================*/
/* TKL OUTPUT (LOGGING)                                      */
/*===========================================================*/
extern const char tuyaOutputTestStr[];
extern const char tuyaOutputTestUsageStr[];
extern const char tuyaOutputTestDetailsStr[];

int32_t cmdTuyaOutputTestCallback(void *arg);
int32_t printTuyaOutputTestUsage(void *arg);
/*===========================================================*/
/* TKL OUTPUT (LOGGING)                                      */
/*===========================================================*/



/*===========================================================*/
/* TKL PWM                                                   */
/*===========================================================*/
extern const char tuyaPwmTestStr[];
extern const char tuyaPwmTestUsageStr[];
extern const char tuyaPwmTestDetailsStr[];

int32_t cmdTuyaPwmTestCallback(void *arg);
int32_t printTuyaPwmTestUsage(void *arg);
/*===========================================================*/
/* TKL PWM                                                   */
/*===========================================================*/



/*===========================================================*/
/* TKL QUEUE                                                 */
/*===========================================================*/
extern const char tuyaQueueTestStr[];
extern const char tuyaQueueTestUsageStr[];
extern const char tuyaQueueTestDetailsStr[];

int32_t cmdTuyaQueueTestCallback(void *arg);
int32_t printTuyaQueueTestUsage(void *arg);
/*===========================================================*/
/* TKL QUEUE                                                 */
/*===========================================================*/



/*===========================================================*/
/* TKL RTC                                                   */
/*===========================================================*/
extern const char tuyaRtcTestStr[];
extern const char tuyaRtcTestUsageStr[];
extern const char tuyaRtcTestDetailsStr[];

int32_t cmdTuyaRtcTestCallback(void *arg);
int32_t printTuyaRtcTestUsage(void *arg);

/*===========================================================*/
/* TKL RTC                                                   */
/*===========================================================*/

/*===========================================================*/
/* TKL SEMAPHORE                                             */
/*===========================================================*/
extern const char tuyaSemTestStr[];
extern const char tuyaSemTestUsageStr[];
extern const char tuyaSemTestDetailsStr[];

int32_t cmdTuyaSemTestCallback(void *arg);
int32_t printTuyaSemTestUsage(void *arg);
/*===========================================================*/
/* TKL SEMAPHORE                                             */
/*===========================================================*/



/*===========================================================*/
/* TKL SPI                                                   */
/*===========================================================*/
extern const char tuyaSpiTestStr[];
extern const char tuyaSpiTestUsageStr[];
extern const char tuyaSpiTestDetailsStr[];

int32_t cmdTuyaSpiTestCallback(void *arg);
int32_t printTuyaSpiTestUsage(void *arg);
/*===========================================================*/
/* TKL SPI                                                   */
/*===========================================================*/



/*===========================================================*/
/* TKL THREAD TESTS                                          */
/*===========================================================*/

extern const char tuyaThreadTestStr[];
extern const char tuyaThreadTestUsageStr[];
extern const char tuyaThreadTestDetailsStr[];

int32_t printTuyaThreadTestUsage(void *arg);
int32_t cmdTuyaThreadTestCallback(void *arg);

/*===========================================================*/
/* TKL THREAD TESTS                                          */
/*===========================================================*/


/*===========================================================*/
/* TKL TIMER TESTS                                           */
/*===========================================================*/
extern const char tuyaTimerTestStr[];
extern const char tuyaTimerTestUsageStr[];
extern const char tuyaTimerTestDetailsStr[];

int32_t printTuyaTimerTestUsage(void *arg);
int32_t cmdTuyaTimerTestCallback(void *arg);

/*===========================================================*/
/* TKL TIMER TESTS                                           */
/*===========================================================*/

/*===========================================================*/
/* TKL UART TESTS                                            */
/*===========================================================*/
extern const char tuyaUartTestStr[];
extern const char tuyaUartTestUsageStr[];
extern const char tuyaUartTestDetailsStr[];

int32_t printTuyaUartTestUsage(void *arg);
int32_t cmdTuyaUartTestCallback(void *arg);
/*===========================================================*/
/* TKL UART TESTS                                            */
/*===========================================================*/



/*===========================================================*/
/* TKL WAKEUP TESTS                                          */
/*===========================================================*/
extern const char tuyaWakeupTestStr[];
extern const char tuyaWakeupTestUsageStr[];
extern const char tuyaWakeupTestDetailsStr[];

int32_t printTuyaWakeupTestUsage(void *arg);
int32_t cmdTuyaWakeupTestCallback(void *arg);
/*===========================================================*/
/* TKL WAKEUP TESTS                                          */
/*===========================================================*/


/*===========================================================*/
/* TKL WATCHDOG TESTS                                        */
/*===========================================================*/
extern const char tuyaWdgTestStr[];
extern const char tuyaWdgTestUsageStr[];
extern const char tuyaWdgTestDetailsStr[];

/*===========================================================*/
/* TKL WATCHDOG TESTS                                        */
/*===========================================================*/


int32_t printTuyaWdgTestUsage(void *arg);
int32_t cmdTuyaWdgTestCallback(void *arg);

#endif /* TUYA_TEST_H */