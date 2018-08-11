#pragma once

#include "base.hpp"

namespace front {
	using ext::FString;
	using ext::print;

	/// This will store FIRST error that was set
	struct CError
	{
		//std::string error;
		FString<512> error;

		template <class... Args>
		void set(char const* fmt, Args... args)
		{
			if (error.empty())
			{
				print(error.range(), fmt, args...);
			}
		}

		FString<512> const& get() const { return error; }
		bool has() const { return not error.empty(); }
		void clear() { error.clear(); }

		operator bool() const { return has(); }
	};
}
