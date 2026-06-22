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
#include <numeric>
#include <thread>

#include "common.hpp"
#include "basic_filter_lock.hpp"
#include "iterated_filter_lock.hpp"
#include "bakery_lock.hpp"
#include "tournament_tree_lock.hpp"
#include "tas_lock.hpp"
#include "ttas_lock.hpp"
#include "ticket_lock.hpp"
#include "anderson_lock.hpp"
#include "chl_lock.hpp"
#include "mcs_lock.hpp"

#define RESET   "\033[0m"
#define BLUE   "\033[34m"
#define GREEN   "\033[32m"
#define RED   "\033[31m"

using namespace std::chrono_literals;

// function declarations...
template <class T> void doWork(T& mtx, uint64_t& counter, uint64_t increments);
template <class T> void startThreads(uint64_t num_threads, uint64_t workload, uint64_t& counter);
template <class T> uint64_t timeWorkload(uint64_t num_threads, uint64_t workload);
template <class T> void testLock(uint64_t runs, uint64_t num_threads, uint64_t workload);
void displayCounter(uint64_t& counter, uint64_t max, std::chrono::milliseconds duration);

int main(int32_t argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: main.exe <threads> <workload> <runs>\n";
        exit(EXIT_FAILURE);
    }

    uint64_t numThreads = std::strtol(argv[1], nullptr, 10);
    uint64_t workloadPerThread = std::strtol(argv[2], nullptr, 10);
    uint64_t runs = std::strtol(argv[3], nullptr, 10);

    testLock<proj::BasicFilterLock>(runs, numThreads, workloadPerThread);
    testLock<proj::IteratedFilterLock>(runs, numThreads, workloadPerThread);
    testLock<proj::TournamentTreeLock>(runs, numThreads, workloadPerThread);
    testLock<proj::BakeryLock>(runs, numThreads, workloadPerThread);
    testLock<proj::TASLock>(runs, numThreads, workloadPerThread);
    testLock<proj::TTASLock>(runs, numThreads, workloadPerThread);
    testLock<proj::TicketLock>(runs, numThreads, workloadPerThread);
    testLock<proj::AndersonLock>(runs, numThreads, workloadPerThread);
    testLock<proj::CHLLock>(runs, numThreads, workloadPerThread);
    testLock<proj::MCSLock>(runs, numThreads, workloadPerThread);

    return EXIT_SUCCESS;
}

template <class T>
void testLock(uint64_t runs, uint64_t num_threads, uint64_t workload) {
    std::vector<uint64_t> results;

    std::cout << BLUE << "[*] - Starting throughput test...\n" << RESET
            << "    \\__ Lock: " << typeid(T).name() << "\n"
            << "    \\__ Runs: " << runs << "\n"
            << "    \\__ Number of threads: " << num_threads << "\n"
            << "    \\__ Operations per thread: " << workload << "\n";

    for (uint64_t i = 0; i < runs; ++i) {
        uint64_t elapsed = timeWorkload<T>(num_threads, workload);

        if (elapsed == 0)
            return;
        
        double totalOperations = static_cast<double>(num_threads * workload);
        double throughput = totalOperations / static_cast<double>(elapsed);
        results.push_back(throughput);

        std::cout << GREEN << "[+] - Run " << i + 1 << " finished!\n" << RESET
            << "    \\__ Time elapsed (milliseconds): " << elapsed << "\n"
            << "    \\__ Throughput: " << throughput << " operations per millisecond.\n";
    }

    double average = std::reduce(results.begin(), results.end(), 0.0) / results.size();
    std::cout << "[+] - Average throughput per run: " << average 
              << " operations per millisecond\n" << std::endl;
}

template <class T>
void doWork(T& mtx, uint64_t& counter, uint64_t workload) {
	for (uint64_t i = 0; i < workload; ++i) {
		mtx.lock();
		++counter;
		mtx.unlock();
	}
}

void displayCounter(uint64_t& counter, uint64_t max, std::chrono::milliseconds duration) {
    std::cout << "[*] - DEBUG INFO\n";

	while (counter < max) {
        std::cout << "\r\033[2K    \\__ Counter value: " << counter << "   " << std::flush;
        std::this_thread::sleep_for(duration);
    }

    std::cout << "\r\033[2K    \\__ Counter value: " << counter << "   \n";
}

template <class T>
void startThreads(uint64_t num_threads, uint64_t workload, uint64_t& counter) {
    std::vector<std::thread> workers;
	workers.reserve(num_threads + 1);

	T mutex(num_threads);

#ifdef DEBUG
    workers.emplace_back(displayCounter, std::ref(counter), workload * num_threads, 1500ms);
#endif

	for (uint64_t i = 0; i < num_threads; ++i) 
		workers.emplace_back(doWork<T>, std::ref(mutex), std::ref(counter), workload);
    
	for (std::thread& thread : workers) 
		thread.join();
}

template <class T>
uint64_t timeWorkload(uint64_t num_threads, uint64_t workload) {
    uint64_t counter = 0;
    uint64_t numTasks = num_threads * workload;

    auto start = std::chrono::steady_clock::now();
    startThreads<T>(num_threads, workload, counter);
    auto elapsed = std::chrono::steady_clock::now() - start;

    if (counter != numTasks) {
        std::cerr << RED << "[-] - Mutual exclusion failed. Expected: " 
                  << numTasks << " Actual: " << counter << std::endl; 
        return 0;
    }

    return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
}