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

	uint32_t calcNodes(uint32_t leaves) {
		return leaves;
	}

	TournamentTreeLock::TournamentTreeLock(uint32_t count): _tree() {
		uint32_t num_nodes = calcNodes(count);
		_tree.reserve(num_nodes);
		for (uint32_t i = 0; i < num_nodes; ++i) 
			_tree.emplace_back(LockGadget(2));
	}

	void TournamentTreeLock::lockImpl(uint32_t me) {
		std::cout << "Tournament Tree Lock not yet implemented!" << std::endl;
		exit(0);
	}

	void TournamentTreeLock::unlockImpl(uint32_t me) noexcept {
		std::cout << "Tournament Tree Lock not yet implemented!" << std::endl;
		exit(0);	
	}
}