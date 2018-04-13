/*
 * channel.h
 *
 *  Created on: 21 Feb 2018
 *
 */

#ifndef CHANNEL_CHANNEL_H_
#define CHANNEL_CHANNEL_H_

// should be used at the source channel
#define __CREATE_CHANNEL(src,dst,size)  \
        __nv uint8_t __CHAN_##src##_##dst[size] __attribute__((aligned(2)));    \
        __nv uint32_t __CHAN_##src##_##dst##timestamp

// should be used by the dst channel
#define __USE_CHANNEL(src,dst)  \
        extern uint8_t __CHAN_##src##_##dst[];  \
        extern uint32_t __CHAN_##src##_##dst##timestamp

#define __GET_CHANNEL_DATAPTR(src,dst)  \
        __CHAN_##src##_##dst

#define __SET_CHANNEL_TIMESTAMP(src,dst,x) __CHAN_##src##_##dst##timestamp = x
#define __GET_CHANNEL_TIMESTAMP(src,dst) __CHAN_##src##_##dst##timestamp


#endif /* CHANNEL_CHANNEL_H_ */
