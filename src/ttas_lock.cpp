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

#include "ttas_lock.hpp"
#include "backoff.hpp"

namespace proj {

	TTASLock::TTASLock(uint32_t count): 
        _flag(false), 
        _min_backoff(0), 
        _max_backoff(512) 
    {

    }

	void TTASLock::lockImpl(uint32_t me) {
        Backoff b;

        while (true) {
            if (!_flag.exchange(true, std::memory_order_acq_rel))
                break;

            while (_flag.load(std::memory_order_acquire)) 
                b.yield();
        }
	}

	void TTASLock::unlockImpl(uint32_t me) noexcept {
		_flag.store(false, std::memory_order_release);
	}
}