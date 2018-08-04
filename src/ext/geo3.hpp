#pragma once

#include "aga3.hpp"


namespace ext {
	
	template <class T>
	using v3 = aga3::Mv1<T>;
	
	
	using v3s = v3<int16_t>;
	using v3i = v3<int32_t>;
	using v3f = v3<float>;
	
	template <class T>
	T sgn(T val) {
		return (T(0) < val) - (val < T(0));
	}

	
	inline float frac(float x) {
		return x - float(int(x));
	}

	template <class T>
	v3<T> vsgn(v3<T> v) {
		v3<T> r;
		for (auto i=0; i<3; ++i) {		
			r[i] = sgn(v[i]);		
		}
		return r;
	}
	
	template <class T>
	v3<T> vfrac(v3<T> v) {
		v3<T> r;
		for (auto i=0; i<3; ++i) {		
			r[i] = frac(v[i]);
		}
		return r;
	}


	template <class T>
	v3<T> vsub(v3<T> x, v3<T> y) {
		v3<T> r;
		for (int i=0; i<3; ++i) {		
			r[i] = x[i] - y[i];
		}
		return r;
	}
	
	template <class T>
	v3<T> vsub(v3<T> x, T y) {
		v3<T> r;
		for (int i=0; i<3; ++i) {		
			r[i] = x[i] - y;
		}
		return r;
	}
	
	template <class T>
	v3<T> vsub(T x, v3<T> y) {
		v3<T> r;
		for (int i=0; i<3; ++i) {		
			r[i] = x - y[i];
		}
		return r;
	}
	

	template <class T>
	v3<T> vmul(v3<T> x, v3<T> y) {
		v3<T> r;
		for (int i=0; i<3; ++i) {		
			r[i] = x[i] * y[i];
		}
		return r;
	}
	
	
	template <class T>
	v3<T> vmul(v3<T> x, T y) {
		v3<T> r;
		for (int i=0; i<3; ++i) {		
			r[i] = x[i] * y;
		}
		return r;
	}
	
	template <class T>
	v3<T> vmul(T x, v3<T> y) {
		v3<T> r;
		for (int i=0; i<3; ++i) {		
			r[i] = x * y[i];
		}
		return r;
	}

	template <class T>
	v3<T> vdiv(v3<T> x, v3<T> y) {
		v3<T> r;
		for (int i=0; i<3; ++i) {		
			r[i] = x[i] / y[i];
		}
		return r;
	}
	
	template <class T>
	v3<T> vdiv(T x, v3<T> y) {
		v3<T> r;
		for (int i=0; i<3; ++i) {		
			r[i] = x / y[i];
		}
		return r;
	}
	
	template <class T>
	v3<T> vdiv(v3<T> x, T y) {
		v3<T> r;
		for (int i=0; i<3; ++i) {		
			r[i] = x[i] / y;
		}
		return r;
	}
	
	
	
	template <class T>
	bool vle(T s, v3<T> y) {
		for (int i=0; i<3; ++i) {		
			if (s <= y[i]) 
				{continue;}
			else 
				{break;}
		}
		return true;
	}
	
	/// return index of min element
	template <class T>
	int imin(v3<T> t) 
	{	
		if (t[0] < t[1]) {
			// not t[1]
			if (t[0] < t[2]) {
				// not [t2]
				return 0;
			}
			else {
				//not t[0]
				return 2;
			}
		}
		else {
			// not t[0]
			if (t[1] < t[2]) {
				// not t[2]
				return 1;
			}
			else {
				// not t[1]
				return 2;
			}
		}		
	}



	
	template <class T>
	auto rotate_matrix4(aga3::Mv02<T> const& r) -> std::array<T, 16> {
		// Convert rotor3 to glMatrix4.
		// rotor -- multivector
		// return -- opengl (loadable to matrix) list
		// [dorst] chapter 7.10.4
		
		auto w = r[0];
		auto z = r[1];
		auto x = r[2];
		auto y = r[3];

		return std::array<T, 16>({
			T(1.-2.*y*y-2.*z*z), T(2.*x*y-2.*w*z),	T(2.*x*z+2.*w*y),	T(0.),
			T(2.*y*x+2.*w*z),	T(1.-2.*z*z-2.*x*x), T(2.*y*z-2.*w*x),	T(0.),
			T(2.*z*x-2.*w*y),	T(2.*z*y+2.*w*x),	T(1.-2.*x*x-2.*y*y), T(0.),
			T(0.),			   T(0.),			   T(0.),			   T(1.),
		});
	}

	template <class T>
	auto translate_matrix4(aga3::Mv1<T> const& r) -> std::array<T, 16> {
		auto x = r[0];
		auto y = r[1];
		auto z = r[2];
		
		return std::array<T, 16>({
			T(1), T(0), T(0), T(0),
			T(0), T(1), T(0), T(0),
			T(0), T(0), T(1), T(0),
			T(x), T(y), T(z), T(1),
		});
	}






	
}

