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

#include "basic_filter_lock.hpp"

namespace proj {

	BasicFilterLock::BasicFilterLock(uint32_t count): _gadgets() {
		uint32_t num_gadgets = count - 1;
		_gadgets.reserve(num_gadgets);
		for (uint32_t i = 0; i < num_gadgets; ++i) 
			_gadgets.emplace_back(count);
	}

	void BasicFilterLock::lockImpl(uint32_t me) {
		for (LockGadget& gadget : _gadgets) 
			gadget.acquire(me);
	}

	void BasicFilterLock::unlockImpl(uint32_t me) noexcept {
		std::for_each(_gadgets.rbegin(), _gadgets.rend(), [=](auto& gadget) {
			gadget.release(me);
		});
	}
}