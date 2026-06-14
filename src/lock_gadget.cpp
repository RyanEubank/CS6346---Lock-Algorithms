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

#include "lock_gadget.hpp"

namespace proj {

	LockGadget::LockGadget(uint32_t thread_count):
		_flags(thread_count),
		_victim(std::make_unique<std::atomic<uint32_t>>(0))
	{
		for (uint32_t i = 0; i < thread_count; ++i)
			_flags[i] = std::make_unique<std::atomic<bool>>(false);
	}

	void LockGadget::acquire(uint32_t id) {
		_flags[id]->store(true, std::memory_order_seq_cst);
		_victim->store(id, std::memory_order_seq_cst);
		
		bool contended = true;
		uint32_t victim = 0;
		bool flag = false;

		while (contended) {
			contended = false;
			victim = _victim->load(std::memory_order_seq_cst);

			for (uint32_t i = 0; i < _flags.size(); ++i) {
				if (i == id)
					continue;

				flag = _flags[i]->load(std::memory_order_seq_cst);

				if (flag && victim == id) {
					contended = true;
					break;
				}
			}
		}
	}

	void LockGadget::release(uint32_t id) noexcept {
		_flags[id]->store(false, std::memory_order_seq_cst);
	}

}