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

#include "iterated_filter_lock.hpp"

namespace proj {

	IteratedFilterLock::IteratedFilterLock(uint32_t count): 
		_level(count), 
		_victim(count) 
	{
		for (uint32_t i = 0; i < count; ++i) {
			_level[i] = std::make_unique<std::atomic<uint32_t>>(0);
			_victim[i] = std::make_unique<std::atomic<uint32_t>>(0);
		}
	}

	void IteratedFilterLock::lockImpl(uint32_t me) {
		for (uint32_t i = 1; i < _victim.size(); ++i) {
			_level[me]->store(i, std::memory_order_seq_cst);
			_victim[i]->store(me, std::memory_order_seq_cst);

			bool contended = true;
			uint32_t victim = 0;
			uint32_t level = 0;

			while (contended) {
				contended = false;
				victim = _victim[i]->load(std::memory_order_seq_cst);

				for (uint32_t j = 0; j < _level.size(); ++j) {
					if (j == me)
						continue;

					level = _level[j]->load(std::memory_order_seq_cst);

					if (level >= i && victim == me) {
						contended = true;
						break;
					}
				}
			}
		}
	}

	void IteratedFilterLock::unlockImpl(uint32_t me) noexcept {
		_level[me]->store(0, std::memory_order_seq_cst);
	}
}