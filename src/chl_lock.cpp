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

#include "chl_lock.hpp"
#include "backoff.hpp"

namespace proj {

	CHLLock::CHLLock(uint32_t count):  
        _tail(std::make_shared<qnode>()),
        _nodes(),
        _preds()
    {
        _nodes.reserve(count);
        _preds.reserve(count);
        for (uint32_t i = 0; i < count; ++i) {
            _nodes.emplace_back(std::make_shared<qnode>());
            _preds.emplace_back(std::make_shared<qnode>());
        }
    }

	void CHLLock::lockImpl(uint32_t me) {
        Backoff b;

        _nodes[me]->isLocked.store(true, std::memory_order_release);
        _preds[me] = _tail.exchange(_nodes[me], std::memory_order_acq_rel);

        while (_preds[me]->isLocked.load(std::memory_order_acquire)) {
            b.yield();
        }
	}

	void CHLLock::unlockImpl(uint32_t me) noexcept {
        _nodes[me]->isLocked.store(false, std::memory_order_release);
        _nodes[me] = _preds[me];
	}
}