#pragma once
#include "format.hpp"
#include "aga2.hpp"

namespace ext {

	template <class U>
	using v2 = aga2::Mv1<U>;


	template <class T>
	T limit(T val, T min, T max) {
		if (val < min) return min;
		if (val > max) return max;
		return val;
	}


	template <class T>
	bool in_bound(T val, T pos, T end) {
		return pos <= val and val < end;
	}


	template <class U>
	bool in_bounds(v2<U> val, v2<U> pos, v2<U> end) {
		return
			in_bound(val[0], pos[0], end[0])
			and
			in_bound(val[1], pos[1], end[1]);
	}

	template <class T>
	struct aabb2 {
		v2<T> pos;
		v2<T> end;

		aabb2() = default;

		aabb2(v2<T> pos, v2<T> end):
			pos(pos), end(end)
		{}

		aabb2(T x0, T y0, T x1, T y1):
			pos(x0,y0), end(x1,y1)
		{}

		aabb2(aabb2 const& o):
			pos(o.pos), end(o.end)
		{}

		v2<T> dim() const {
			return end - pos;
		}

		void dim(v2<T> d) {
			end = pos + d;
		}

		
		template <class U>
		explicit aabb2(aabb2<U> other): aabb2(v2<T>(other.pos), v2<T>(other.end)) {
			
		}
		
	};
	

	template <class T>
	std::ostream & operator<<(std::ostream & o, aabb2<T> const& a)
	{
		o << a.pos[0] << " " << a.pos[1] << " " << a.end[0] << " " << a.end[1];
		return o;
	}


	template <class T>
	struct b2 {

		v2<T> pos, dim;

		b2() = default;

		b2(v2<T> pos, v2<T> dim):
			pos(pos), dim(dim)
		{}

		b2(T x, T y, T w, T h):
			pos(x,y), dim(w,h)
		{}

		b2(b2 const& o):
			pos(o.pos), dim(o.dim)
		{}

		v2<T> end() const {
			return pos + dim;
		}

		void end(v2<T> e) const {
			dim = e - pos;
		}
		
		template <class U>
		explicit b2(b2<U> other): b2(v2<T>(other.pos), v2<T>(other.dim)) {
			
		}

	};



	template <class T>
	b2<T> operator*(b2<T> const& b, T s) {
		return b2<T>(b.pos * s, b.dim * s);
	}


	template <class T>
	v2<T> vmul(v2<T> a, v2<T> b) {
		return v2<T>(a[0]*b[0], a[1]*b[1]);
	}

	template <class T>
	v2<T> vdiv(v2<T> a, v2<T> b) {
		return v2<T>(a[0]/b[0], a[1]/b[1]);
	}
	
	template <class T>
	v2<T> vdiv0(v2<T> a, v2<T> b) {
		return v2<T>(
			((b[0] != T(0)) ? (a[0]/b[0]) : (T(0))),
			((b[1] != T(0)) ? (a[1]/b[1]) : (T(0)))
		);
	}
	
	using v2s = v2<int16_t>;
	using b2s = b2<int16_t>;
	using v2f = v2<float>;
	
	inline v2s vmulsf(v2s s, v2f f) { return v2s(vmul(v2f(s), f)); }
	
	inline v2s calc_align(
		b2s box, v2s dim,
		v2f const& align = v2f(0.5, 0.5)
	) {
		return box.pos + v2s(vmul(v2f(box.dim - dim), align));
	}
	
	

	
	//template <class T>
	//v2<T> calc_align(b2<T> par, v2<T> dim, v2<float> align = v2<float>(0.5, 0.5)) {
	//	return v2<T>(v2<float>(par.pos) + vmul(v2<float>(par.dim - dim), align));
	//}

	template <class T>
	bool overlap(b2<T> b, v2<T> pos) {
		auto b_end = b.end();
		return
			b.pos[0] <= pos[0] and
			b.pos[1] <= pos[1] and
			pos[0] < b_end[0] and
			pos[1] < b_end[1];
	}

	template <class T>
	inline std::ostream & operator<<(std::ostream & o, b2<T> const& b) {
		o << format("({} {})", b.pos, b.dim);
		return o;
	}
	
	
	template <class T>
	b2<T> shrink(b2<T> box, T th) {
		return {
			box.pos + v2s(th,th), 
			box.dim - v2s(th,th) - v2s(th,th)
		};		
	}
	
	template <class T>
	b2<T> shrink(b2<T> box, v2<T> th) {
		return {
			box.pos + th, 
			box.dim - th - th
		};		
	}
	
	template <class T>
	b2<T> translate(b2<T> box, T th) {
		return {
			box.pos + v2s(th,th), 
			box.dim
		};		
	}
	
	
	/*template <class T>
	b2<T> intersect(b2<T> a, b2<T> b) {
		b2<T> c;
		
		
		auto b_end = b.end();
		return
			b.pos[0] <= pos[0] and
			b.pos[1] <= pos[1] and
			pos[0] <= b_end[0] and
			pos[1] <= b_end[1];
	}*/

}
