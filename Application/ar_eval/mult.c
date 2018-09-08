// This file is part of InK.
// 
// author = "dpatoukas " 
// maintainer = "dpatoukas "
// email = "dpatoukas@gmail.com" 
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

#include <stdint.h>

// NOTE: This is not implemented as a built-in in libmspbuiltins because Clang
// doesn't issue calls to operations where result type is different from
// argument type. But this case is more efficient and easier to implement than
// a full 32-bit multiplication.

#ifndef __clang__
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

__attribute__((naked))
uint32_t mult16(uint16_t a, uint16_t b)
{
    // NOTE: We have to hardcode addresses, because we ca neither access
    // definitions from the msp430.h header inside the assembly, nor
    // parametrize the assembly snippet since function is naked. A non-naked
    // function with parametrized snipped turned out to be horribly inefficient
    // (the result construction needs a shift: 8 instructions...).

    // NOTE: The registers are dependent on compiler calling convention.

    // Load OP1 and OP2 into the hardware multiplier and read result.

#if defined(__clang__)
    __asm__ volatile (
        "MOV R15, &0x04C0\n"
        "MOV R14, &0x04C8\n"
        "MOV &0x04CA, R14\n"
        "MOV &0x04CC, R15\n"
        "RET\n"
    );
#elif defined(__GNUC__)
    __asm__ volatile (
        "MOV R12, &0x04C0\n"
        "MOV R13, &0x04C8\n"
        "MOV &0x04CA, R12\n"
        "MOV &0x04CC, R13\n"
        "RET\n"
    );
#else
    return (uint32_t)a * (uint32_t)b;
#endif
}
