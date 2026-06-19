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

#include <immintrin.h>
#include "common.hpp"

#if defined(__GNUC__) || defined(__clang__)
    #define CPU_PAUSE() __builtin_ia32_pause()
#elif defined(_MSC_VER)
    #define CPU_PAUSE() _mm_pause()
#else
    #define CPU_PAUSE()
#endif

namespace proj {

	template <class derived_t>
	class Lock {
	public:

		Lock(): _isLocked() {}

        ~Lock() {
            getThreadID(true);
        }

		void lock() {
			try {
				static_cast<derived_t*>(this)->lockImpl(getThreadID());
				_isLocked = true;
			}
			catch(...) {
				std::cout << "An error has occurred.\n";
				unlock();
				exit(EXIT_FAILURE);
			}
		}

		void unlock() noexcept {
			if (_isLocked) {
				static_cast<derived_t*>(this)->unlockImpl(getThreadID());
				_isLocked = false;
			}
		}

	protected:

		static uint32_t getThreadID(bool reset = false) {
            static std::atomic<uint32_t> generator {0};

            if (reset) {
                generator.store(0);
                return 0;
            }

			thread_local int id = generator.fetch_add(1);
			return id;
		}

        uint64_t randomInRange(uint16_t min, uint16_t max) {
            std::mt19937 generator(std::random_device{}());
            std::uniform_int_distribution<uint64_t> distribution(min, max);
            return distribution(generator);
        }

        void hardWait(uint32_t duration) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(duration * 1us);
        }

        void softWait(uint32_t duration) {
            for (uint32_t i = 0; i < duration; ++i)
                CPU_PAUSE();
        }

        void backoff(uint32_t& attempts, uint32_t& duration, uint32_t max) {
            if (attempts++ < max) {
                softWait(duration);
                duration = std::min(duration << 1, max);
            }
            else {
                hardWait(this->randomInRange(max, max << 2));
                std::this_thread::yield();
            }
        }
        
	private:
		bool _isLocked;
	};
}