#pragma once


namespace ext {


	struct dstring {
		char const* ptr;

		operator char* () {
			return ptr;
		}

		dstring(size_t len): ptr(new char[len]) {
			
		}

		~dstring() {
			
		}
	};


}
