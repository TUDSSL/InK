// This file is part of InK.
// 
// author = "Kasım Sinan Yıldırım " 
// maintainer = "Kasım Sinan Yıldırım "
// email = "sinanyil81 [at] gmail.com" 
//  
// copyright = "Copyright 2018 Delft University of Technology" 
// license = "LGPL" 
// version = "3.0" 
// status = "Production"
//
// 
// InK is free software: you ca	n redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>./*
 /* channel.h
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
