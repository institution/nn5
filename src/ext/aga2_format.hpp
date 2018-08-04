#pragma once

#include "aga2.hpp"
#include "format.hpp"


namespace aga2
{
	using ext::print;

	// ostream
	template <class O, class R>
	void print1(O & o, Mv0<R> const& x) {
		print(o, "{}", x[0]);
	}

	template <class O, class R>
	void print1(O & o, Mv1<R> const& x) {
		print(o, "({} {})", x[0], x[1]);
	}

	template <class O, class R>
	void print1(O & o, Mv2<R> const& x) {
		print(o, "(mv2 {})", x[0]);
	}

	template <class O, class R>
	void print1(O & o, Mv02<R> const& x) {
		print(o, "(mv02 {} {})", x[0], x[1]);
	}
}
