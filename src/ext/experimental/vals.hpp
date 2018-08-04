#pragma once

#include <type_traits>

template <class I>
struct Vals{

	using KPV = typename I::value_type;
	using PV = typename std::tuple_element<1, KPV>::type;	
	using V = typename std::remove_pointer<PV>::type;
	
	struct Iter{
		I i;
		Iter(I i): i(i) {}
				
		void operator++() {
			++i;
		}
		
		V & operator*() {
			return (*i).second;
		}
		
		V const& operator*() const {
			return *(*i).second;
		}
		
		bool operator!=(Iter other) const {
			return i != other.i;
		}
	};
		
	Iter b,e;
	
	Vals(I && b, I && e): b(b), e(e) {}
	
	auto begin() {
		return b;
	}
	
	auto end() {
		return e;
	}
		
};

/*template <class T, class F>
void for_each(Vals<T> & xs, F f) {
	for (auto & x: xs) {
		F(x);
	}
}*/

template <class T>
auto list_values(T & t) {
	return Vals<typename T::iterator>(t.begin(), t.end());
}

template <class T>
auto list_values(T const& t) {
	return Vals<typename T::const_iterator>(t.begin(), t.end());
}


