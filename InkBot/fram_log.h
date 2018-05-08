#ifndef _FRAMLOGMODE_H_
#define _FRAMLOGMODE_H_

#include <stdint.h>
/* defines non-volatile variable */
#ifdef __GNUC__
    #define __nv    __attribute__((section(".nv_vars")))
#else
    #define __nv __attribute__((section(".TI.persistent")))
#endif
#define LOGGER_SIZE 250

extern __nv float angle_data[LOGGER_SIZE];
extern __nv uint8_t state_data[LOGGER_SIZE];
extern __nv float prox_data[LOGGER_SIZE];
extern __nv uint8_t index_data[LOGGER_SIZE];

void framLog(float arg1, uint8_t arg2, float arg3);

#endif /*_FRAMLOGMODE_H_ */
