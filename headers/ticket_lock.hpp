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

#include "lock.hpp"

namespace proj {

	class TicketLock : public Lock<TicketLock> {
	public:

		TicketLock(uint32_t thread_count);

	private:

		friend class Lock<TicketLock>;

		std::atomic<uint32_t> _number;
        uint32_t _turn;

		void lockImpl(uint32_t me);
		void unlockImpl(uint32_t me) noexcept;
	};
}