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

	class MCSLock : public Lock<MCSLock> {
	public:

		MCSLock(uint32_t thread_count);

	private:

		friend class Lock<MCSLock>;

        struct qnode {
            std::atomic<bool> isLocked{false};
            std::atomic<std::shared_ptr<qnode>> next{nullptr};
        };

        std::atomic<std::shared_ptr<qnode>> _tail;
        std::vector<std::shared_ptr<qnode>> _nodes;
        std::vector<std::shared_ptr<qnode>> _preds;

		void lockImpl(uint32_t me);
		void unlockImpl(uint32_t me) noexcept;
	};
}