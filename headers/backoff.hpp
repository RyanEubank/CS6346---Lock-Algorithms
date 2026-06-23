/*******************************************************************************   
CS4346 Multicore Programming Project

Copyright (C) 2026 Ryan Eubank

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*******************************************************************************/

#pragma once

#include <immintrin.h>
#include "common.hpp"

#if defined(_MSC_VER) 
    #define CPU_PAUSE() _mm_pause()
#elif defined(__i386__) || defined(__x86_64__) 
    #if defined(__clang__) 
        #define CPU_PAUSE() _mm_pause()
    #else 
        #define CPU_PAUSE() __builtin_ia32_pause()
    #endif 
#elif defined(__arm__) 
    #define CPU_PAUSE() __yield(); 
#else 
    #error "Unknown ISA/compiler"
#endif 

namespace proj {

	class Backoff {
    private:
        static constexpr uint32_t MAX_PAUSES = 64;
        uint32_t _numPauses = 1;

    public:

        void yield();
	};
}