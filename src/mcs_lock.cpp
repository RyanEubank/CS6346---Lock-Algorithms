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

#include "mcs_lock.hpp"

namespace proj {

	MCSLock::MCSLock(uint32_t count):  
        _tail(nullptr),
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

	void MCSLock::lockImpl(uint32_t me) {
        _preds[me] = _tail.exchange(_nodes[me]);
        if (_preds[me] != nullptr) {
            _nodes[me]->isLocked.store(true, std::memory_order_release);
            _preds[me]->next.store(_nodes[me], std::memory_order_release);
            while (_nodes[me]->isLocked.load(std::memory_order_acquire)) {
                CPU_PAUSE();
            }
        }
	}

	void MCSLock::unlockImpl(uint32_t me) noexcept {
        if (_nodes[me]->next.load(std::memory_order_acquire) == nullptr) {
            if (_tail.compare_exchange_strong(_nodes[me], nullptr))
                return;
            while (_nodes[me]->next.load(std::memory_order_relaxed) == nullptr) {
                CPU_PAUSE();
            }
        }
        _nodes[me]
            ->next.load(std::memory_order_acquire)
            ->isLocked.store(false, std::memory_order_release);
        _nodes[me]->next.store(nullptr, std::memory_order_seq_cst);
	}
}