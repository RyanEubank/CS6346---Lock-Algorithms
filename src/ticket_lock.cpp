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

#include "ticket_lock.hpp"
#include "backoff.hpp"

namespace proj {

	TicketLock::TicketLock(uint32_t count): _number(0), _turn(0) {}

	void TicketLock::lockImpl(uint32_t me) {
        Backoff b;
        uint32_t ticket = _number.fetch_add(1, std::memory_order_acq_rel);
        
        while (_turn != ticket) { 
            b.yield();
        }
	}

	void TicketLock::unlockImpl(uint32_t me) noexcept {
        ++_turn;
	}
}