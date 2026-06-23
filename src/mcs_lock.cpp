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
#include "backoff.hpp"

namespace proj {

	MCSLock::MCSLock(uint32_t count):  _tail(nullptr), _nodes(count), _preds(count) {}

	void MCSLock::lockImpl(uint32_t me) {
        Backoff b;
        _preds[me] = _tail.exchange(&_nodes[me]);

        if (_preds[me] != nullptr) {
            _nodes[me].isLocked = true;
            _preds[me]->next = &_nodes[me];

            while (_nodes[me].isLocked) {
                b.yield();
            }
        }
	}

	void MCSLock::unlockImpl(uint32_t me) noexcept {
        Backoff b;

        if (_nodes[me].next == nullptr) {
            qnode* mynode = &_nodes[me];
            if (_tail.compare_exchange_weak(mynode, nullptr, std::memory_order_acq_rel))
                return;
            while (_nodes[me].next == nullptr) {
                b.yield();
            }
        }
        _nodes[me].next->isLocked = false;
        _nodes[me].next = nullptr;
	}
}