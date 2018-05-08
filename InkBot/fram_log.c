#include "fram_log.h"

/*
 * FRAM Array reserved to store data memory for Temperature and Voltage
 * Stores up to 500 Angle + 500 Proximity + 1 index data
 */
__nv float angle_data[LOGGER_SIZE];
__nv uint8_t state_data[LOGGER_SIZE];
__nv float prox_data[LOGGER_SIZE];
__nv uint8_t index_data[LOGGER_SIZE];

__nv uint16_t log_index = 0;

__nv uint16_t idx = 0;

void framLog(float arg1, uint8_t arg2, float arg3)
{

    //circular logger 
    index_data[log_index]   = idx++; //global index
    angle_data[log_index]   = arg1;
    state_data[log_index]   = arg2;
    prox_data[log_index]    = arg3;

    log_index = (log_index + 1) % LOGGER_SIZE;
}
