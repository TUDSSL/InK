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
 /* memory.h
 *
 *  Created on: 14 Feb 2018
 *
 */

#ifndef MEMORY_H_
#define MEMORY_H_

/* defines non-volatile variable */
#ifdef __GNUC__
    #define __nv    __attribute__((section(".nv_vars")))
#else
    #define __nv __attribute__((section(".TI.persistent")))
#endif


void __dma_copy(unsigned int from, unsigned int to, unsigned short size);

#endif /* MEMORY_H_ */
