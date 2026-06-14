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

#include <chrono>
#include <thread>

#include "common.hpp"
#include "basic_filter_lock.hpp"
#include "iterated_filter_lock.hpp"
#include "bakery_lock.hpp"
#include "tournament_tree_lock.hpp"

// function declarations...
template <class T> void increment_counter(T& mtx, uint32_t& counter, uint32_t increments);
template <class T> void testLock();
void watch_counter(uint32_t& counter, uint32_t expected);

int main(int32_t argc, char** argv) {
	//testLock<proj::BasicFilterLock>();
	//testLock<proj::IteratedFilterLock>();
	//testLock<proj::TournamentTreeLock>();
	testLock<proj::BakeryLock>();
}

template <class T>
void increment_counter(T& mtx, uint32_t& counter, uint32_t increments) {
	for (uint32_t i = 0; i < increments; ++i) {
		mtx.lock();
		counter++;
		mtx.unlock();
	}
}

void watch_counter(uint32_t& counter, uint32_t expected) {
	while (counter < expected) {
		std::cout << "Value of counter is: " << counter << "\n";
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}

template <class T>
void testLock() {
	uint32_t counter = 0;
	uint32_t num_threads = 10;
	uint32_t num_increments = 100000;
	uint32_t expected_value = num_threads * num_increments;

	std::vector<std::thread> workers;
	workers.reserve(num_threads);

	T mutex(num_threads);

	for (uint32_t i = 0; i < num_threads; ++i) {
		workers.emplace_back(
			increment_counter<T>,
			std::reference_wrapper(mutex),
			std::reference_wrapper(counter),
			num_increments
		);
	}

	workers.emplace_back(
		watch_counter, 
		std::reference_wrapper(counter), 
		expected_value
	);

	for (auto& thread : workers)
		thread.join();
	
	std::cout << "The final count is: " << counter << std::endl;
}