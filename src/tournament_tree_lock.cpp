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

#include "tournament_tree_lock.hpp"

namespace proj {

    struct lock_info {
        uint32_t tree_index;
        uint32_t lock_index;
    };

    std::vector<lock_info> pathFromLeafToRoot(uint32_t tree_size, uint32_t leaf) {
        uint32_t index = tree_size + leaf;
        std::vector<lock_info> path;

        path.reserve(tree_size);

        while (index > 0) {
            uint32_t previous = index;
            index = ((index - 1) >> 1);
            lock_info info { index, (previous == (index << 1) + 1) ? 0u : 1u };
            path.push_back(info);
        }

        return path;
    }

	TournamentTreeLock::TournamentTreeLock(uint32_t count): _tree() {
		uint32_t num_nodes = count - 1;
		_tree.reserve(num_nodes);
		for (uint32_t i = 0; i < num_nodes; ++i) 
			_tree.emplace_back(LockGadget(2));
	}

	void TournamentTreeLock::lockImpl(uint32_t me) {
        std::vector<lock_info> indices = pathFromLeafToRoot(_tree.size(), me);
        for (lock_info info : indices) 
            _tree[info.tree_index].acquire(info.lock_index);
	}

	void TournamentTreeLock::unlockImpl(uint32_t me) noexcept {
        std::vector<lock_info> indices = pathFromLeafToRoot(_tree.size(), me);
		std::for_each(indices.rbegin(), indices.rend(), [this](lock_info& info) {
			this->_tree[info.tree_index].release(info.lock_index);
		});
	}
}