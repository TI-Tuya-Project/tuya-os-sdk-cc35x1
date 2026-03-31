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

#endif /* TUYA_TEST_H */