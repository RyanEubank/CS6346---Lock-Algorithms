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

#include "anderson_lock.hpp"
#include "backoff.hpp"

namespace proj {

	AndersonLock::AndersonLock(uint32_t count): _slots(count), _flags(), _next(0) {
        _flags.reserve(count);
        _flags.emplace_back(std::make_unique<std::atomic<bool>>(true));
        for (uint32_t i = 1; i < count; ++i)
            _flags.emplace_back(std::make_unique<std::atomic<bool>>(false));
    }

	void AndersonLock::lockImpl(uint32_t me) {
        Backoff b;

        _slots[me] = _next.fetch_add(1, std::memory_order_acq_rel);
        while (!_flags[_slots[me] % _flags.size()]->load(std::memory_order_acquire)) {
            b.yield();
        }
	}

	void AndersonLock::unlockImpl(uint32_t me) noexcept {
        _flags[_slots[me] % _flags.size()]->store(false, std::memory_order_release);
		_flags[(_slots[me] + 1) % _flags.size()]->store(true, std::memory_order_release);
	}
}