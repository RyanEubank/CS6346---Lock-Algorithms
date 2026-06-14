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

#include "common.hpp"

namespace proj {

	template <class derived_t>
	class Lock {
	public:

		Lock(): _isLocked() {}

		void lock() {
			try {
				static_cast<derived_t*>(this)->lockImpl(getThreadID());
				_isLocked = true;
			}
			catch(...) {
				std::cout << "An error has occurred.\n";
				unlock();
				exit(EXIT_FAILURE);
			}
		}

		void unlock() noexcept {
			if (_isLocked) {
				static_cast<derived_t*>(this)->unlockImpl(getThreadID());
				_isLocked = false;
			}
		}

	protected:

		uint32_t getThreadID() {
			static std::atomic<uint32_t> generator {0};
			static thread_local const int id = generator++;
			return id;
		}

	private:
		bool _isLocked;
	};
}