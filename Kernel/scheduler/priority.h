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
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/*
 * priority.h
 *
 *  Created on: 14 Feb 2018
 *
 */

#ifndef PRIORITY_H_
#define PRIORITY_H_

#define NUM_PRIORITY_LEVEL 8
#define NUM_PRIORITIES 64

typedef struct {
    volatile uint8_t _readyTable[NUM_PRIORITY_LEVEL];
    volatile uint8_t _readyLevel;
}priority_t;

// priority queue operations
void __priority_init(priority_t *p);
void __priority_insert(uint8_t priority,priority_t *p);
void __priority_remove(uint8_t priority,priority_t *p);
uint8_t __priority_highest(priority_t *p);

#endif /* PRIORITY_H_ */
