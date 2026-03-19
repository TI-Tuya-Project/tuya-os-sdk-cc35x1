#include "tuya_cloud_types.h"
#include "tkl_mutex.h"
#include "tkl_semaphore.h"

// --- Bridge TAL (SDK) -> TKL (TI Driver) ---
/*
OPERATE_RET tal_mutex_create_init(TKL_MUTEX_HANDLE *p_mutex) { return tkl_mutex_create_init(p_mutex); }
OPERATE_RET tal_mutex_lock(TKL_MUTEX_HANDLE mutex)           { return tkl_mutex_lock(mutex); }
OPERATE_RET tal_mutex_unlock(TKL_MUTEX_HANDLE mutex)         { return tkl_mutex_unlock(mutex); }
OPERATE_RET tal_mutex_release(TKL_MUTEX_HANDLE mutex)        { return tkl_mutex_release(mutex); }

OPERATE_RET tal_semaphore_create_init(TKL_SEM_HANDLE *p_sem, uint32_t init_count, uint32_t max_count) {
    return tkl_semaphore_create_init(p_sem, init_count, max_count);
}
OPERATE_RET tal_semaphore_wait(TKL_SEM_HANDLE sem, uint32_t timeout) { return tkl_semaphore_wait(sem, timeout); }
OPERATE_RET tal_semaphore_post(TKL_SEM_HANDLE sem)                 { return tkl_semaphore_post(sem); }
OPERATE_RET tal_semaphore_release(TKL_SEM_HANDLE sem)              { return tkl_semaphore_release(sem); }
*/
// Hardware reset
/*void NVIC_SystemReset(void) {
    volatile unsigned long *AIRCR = (volatile unsigned long *)0xE000ED0C;
    *AIRCR = 0x05FA0004;
    // Electrical signals may take some times to propgate 
    // so we use a while 1
    // after the hardware is finished
    // electrical signals hit the core,and forcibly reset
    // the program counter.
    while(1);
}*/


