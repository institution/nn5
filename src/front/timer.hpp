#pragma once

#include <thread>
#include "../front/front.hpp"

namespace front
{
	struct Timer
	{
		Tick t1{0};
		
		void set(Tick delay) {
			t1 = front::get_ticks() + delay;
		}

		Tick left() const {
			return t1 - get_ticks();
		}
		
		void wait()
		{
			auto l = left();		
			if (l > 0) {
				std::this_thread::sleep_for(std::chrono::milliseconds(l));	
			}
			// t1 = get_ticks();
		}

	};

}
