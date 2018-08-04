#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include "../ext/ext.hpp"

namespace front {
		

	namespace Color {
		
		struct RGBA8;
		struct RGBAf;
		struct RGB8;
		struct RGBf;
		struct A8;
		struct Af;
		
		// RGBA8 ------------------------------------------------------------------
		struct RGBA8 {
			uint8_t r,g,b,a;
						
			RGBA8() = default;
			RGBA8(RGBA8 const&) = default;
			RGBA8(uint8_t r, uint8_t g, uint8_t b, uint8_t a): r(r), g(g), b(b), a(a) {}
		};

		
		template <class O>
		void print1(O & o, RGBA8 const& c) {
			print(o, "({} {} {} {})", uint(c.r), uint(c.g), uint(c.b), uint(c.a));			
		}

		inline bool operator!=(RGBA8 const& x, RGBA8 const& y) {
			return x.r != y.r or x.g != y.g or x.b != y.b or x.a != y.a;
		}

		inline bool operator==(RGBA8 const& x, RGBA8 const& y) {
			return x.r == y.r and x.g == y.g and x.b == y.b and x.a == y.a;
		}

		// RGBAf ------------------------------------------------------------------
		struct RGBAf {
			float_t r,g,b,a;
			
			RGBAf() = default;
			RGBAf(RGBAf const&) = default;
			RGBAf(float_t r, float_t g, float_t b, float_t a): r(r), g(g), b(b), a(a) {}			
		};
		
		template <class O>
		void print1(O & o, RGBAf const& c) {
			print(o, "({} {} {} {})", c.r, c.g, c.b, c.a);			
		}

		inline bool operator!=(RGBAf const& x, RGBAf const& y) {
			return x.r != y.r or x.g != y.g or x.b != y.b or x.a != y.a;
		}

		inline bool operator==(RGBAf const& x, RGBAf const& y) {
			return x.r == y.r and x.g == y.g and x.b == y.b and x.a == y.a;
		}

		// RGB8 ------------------------------------------------------------------
		struct RGB8 {
			uint8_t r,g,b;
						
			RGB8() = default;
			RGB8(RGB8 const&) = default;
			RGB8(uint8_t r, uint8_t g, uint8_t b): r(r), g(g), b(b) {}
		};

		template <class O>
		void print1(O & o, RGB8 const& c) {
			print(o, "({} {} {} 0)", c.r, c.g, c.b);			
		}
		
		// RGBf ------------------------------------------------------------------
		struct RGBf {
			float_t r,g,b;
						
			RGBf() = default;
			RGBf(RGBf const&) = default;
			RGBf(float_t r, float_t g, float_t b): r(r), g(g), b(b) {}
		};




		template <class O>
		void print1(O & o, RGBf const& c) {
			print(o, "({} {} {} 0)", c.r, c.g, c.b);
		}
		
		
		// R8 ------------------------------------------------------------------
		struct R8 {
			uint8_t r;
						
			R8() = default;
			R8(R8 const&) = default;
			R8(uint8_t r): r(r) {}
		};

		template <class O>
		void print1(O & o, R8 c) {
			print(o, "({} 0 0 0)", c.r);
		}
		
		
		// Rf ------------------------------------------------------------------
		struct Rf {
			float_t r;
						
			Rf() = default;
			Rf(Rf const&) = default;
			Rf(float_t r): r(r) {}
		};

		template <class O>
		void print1(O & o, Rf c) {
			print(o, "({} 0 0 0)", c.r);
		}
		
				
		// conversions -----------------------------------------------------------
		inline RGBAf make_RGBAf(RGBA8 c)
		{
			return RGBAf(
				float_t(c.r)/255.0f,
				float_t(c.g)/255.0f,
				float_t(c.b)/255.0f,
				float_t(c.a)/255.0f
			);
		}
		
		inline RGBA8 make_RGBA8(RGBAf c)
		{
			return RGBA8(
				c.r * 255.0f,
				c.g * 255.0f,
				c.b * 255.0f,
				c.a * 255.0f
			);
		}
		
		
	}

}
