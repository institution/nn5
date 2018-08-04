#pragma once

#include <stdlib.h>
#include <cassert>
#include "format.hpp"

namespace ext{

	extern bool FAIL_THROWS;

	template <class... Args>
	[[noreturn]] 
	void fail(char const* fmt, Args... args) {
		print(stderr, fmt, args...);
		if (FAIL_THROWS) {
			throw std::exception();
		}
		else {
			assert(0);
			exit(-1); 			
		}
		//exit(-1); 
	}


	[[noreturn]] 
	inline void fail(char const* fmt) {
		// use printf so we can use this to report errors in print/string library
		std::fputs(fmt , stderr);
		if (FAIL_THROWS) {
			throw std::exception();
		}
		else {
			assert(0);
			exit(-1); 			
		}
		//exit(-1); 
	}

	

	
	[[noreturn]] 
	inline void fail() {
		//print(stderr, fmt, args...);
		if (FAIL_THROWS) {
			throw std::exception();
		}
		else {			
			assert(0);
			exit(-1); 
		}
		//exit(-1); 
	}
	
	
}
