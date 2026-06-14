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

#include "bakery_lock.hpp"

namespace proj {

	BakeryLock::BakeryLock(uint32_t count): _flags(count), _tickets(count) {
		for (uint32_t i = 0; i < count; ++i) {
			_flags[i] = std::make_unique<std::atomic<bool>>(false);
			_tickets[i] = std::make_unique<std::atomic<uint32_t>>(0);
		}
	}

	uint32_t getMaxTicket(
		const std::vector<std::unique_ptr<std::atomic<uint32_t>>>& tickets
	) {
		uint32_t max = 0;

		for (const auto& ptr : tickets) {
			uint32_t ticket = ptr->load(std::memory_order_relaxed);
			max = std::max(max, ticket);
		}

		return max;
	}

	void BakeryLock::lockImpl(uint32_t me) {
		uint32_t max = 0;
		uint32_t ticket = 0;

		_flags[me]->store(true, std::memory_order_seq_cst);

		for (const auto& ptr : _tickets) {
			uint32_t ticket = ptr->load(std::memory_order_relaxed);
			max = std::max(max, ticket);
		}
		ticket = 1 + max;
		
		_flags[me]->store(false, std::memory_order_seq_cst);
	
		for (uint32_t i = 0; i < _flags.size(); ++i) {
			if (i == me)
				continue;

			while (_flags[i]->load(std::memory_order_relaxed)) {}

			uint32_t other_ticket = _tickets[i]->load(std::memory_order_seq_cst);
			if (other_ticket == 0)
				continue;

			while (other_ticket < ticket || (other_ticket == ticket && i < me)) {
				other_ticket = _tickets[i]->load(std::memory_order_seq_cst);
			}
		}
	}

	void BakeryLock::unlockImpl(uint32_t me) noexcept {
		_tickets[me]->store(me, std::memory_order_seq_cst);
	}
}