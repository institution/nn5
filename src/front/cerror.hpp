#pragma once

#include "base.hpp"

namespace front {
	using ext::FixedString;
	using ext::print;

	/// This will store FIRST error that was set
	struct CError
	{
		//std::string error;
		FixedString<512> error;

		template <class... Args>
		void set(char const* fmt, Args... args)
		{
			if (error.empty())
			{
				print(error.range(), fmt, args...);
			}
		}

		FixedString<512> const& get() const { return error; }
		bool has() const { return not error.empty(); }
		void clear() { error.clear(); }

		operator bool() const { return has(); }
	};
}
