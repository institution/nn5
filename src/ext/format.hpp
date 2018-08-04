#pragma once

#include <string>
#include <iostream>
#include <iomanip> // setprecision
#include <sstream> // stringstream
#include "fmt/format.h"
#include <type_traits>
#include <cstdio>

/*
no iostream dependency
[opt] no string dependency
*/

namespace ext {

	inline void fail_no_print(char const* msg) {
		std::fputs(msg , stderr);
		assert(0);
		exit(1);
	}





	inline std::string to_string(float f, int n)
	{
		std::stringstream stream;
		stream << std::fixed << std::setprecision(n) << f;
		std::string s = stream.str();
		return s;
	}


	inline void to_string(char * buf, size_t n, float f, int prec)
	{
		std::stringstream stream;
		stream << std::fixed << std::setprecision(prec) << f;
		std::string s = stream.str();
		for (size_t i = 0; i < n; ++i)
		{
			if (i >= s.size()) break;
			buf[i] = s.at(i);
		}
	}



	template <class T>
	struct Rng
	{
		T * a;
		T * b;
		Rng(T * a, T * b): a(a), b(b) {}
	};

	struct Char {
		char c;
		Char() = default;
		Char(char c): c(c) {}
	};

	template <int16_t N>
	struct FixedString
	{
		static_assert(N >= 1, "FixedString: N must be at least 1");

		char c[N];
		uint16_t n;

		FixedString() { clear(); }
		FixedString(char const* s) { set(s); }

		bool empty() const { return c[0] == '\0'; }
		void clear() { c[0] = '\0'; }
		char const* c_str() const { return c; }
		Rng<char> range() { return Rng<char>(c, c+N); }
		void set(char const* s) {
			if (strlen(s) + 1 < N) {
				strcpy(c, s);
			}
			else {
				fail_no_print("ERROR: FixedString set\n");
			}
		}
		bool operator==(char const* s) {
			return strcmp(c, s) == 0;
		}
		bool operator!=(char const* s) {
			return strcmp(c, s) != 0;
		}
	};




	using CharRng = Rng<char>;

	inline void put(Rng<char> & ss, char c)
	{
		if (ss.a + 1 == ss.b) {
			fail_no_print("ERROR: string too long\n");
		}

		*ss.a = c;
		++ss.a;
		*ss.a = '\0';
	}

	inline void put(FILE * fp, char c) {
		if (fputc(c, fp) == EOF) {
			fail_no_print("ERROR: while printing to FILE\n");
		}
	}


	template <class O>
	void print1(O & o, Char x) {
		put(o, x.c);
	}

	template <class O>
	void print1(O & o, char const* x) {
		while (*x != '\0') {
			put(o, *x);
			++x;
		}
	}


	template <class O, int16_t N>
	void print1(O & o, FixedString<N> x) {
		print1(o, x.c_str());
	}


	template <class O>
	void print1(O & o, std::string const& x) {
		print1(o, x.c_str());
	}


	template <class O, class T>
	void print1_inttype(O & o, T num)
	{
		unsigned int const N = 32;

		auto x = num;

		char buf[N];

		unsigned int i = 0;
		do {
			auto digit = x % 10;
			x = x / 10;

			if (i >= N) { fail_no_print("ERROR: while printing int: buffer overflow\n"); }
			buf[i] = char(digit + '0');
			++i;
		} while (x > 0);

		if (num < 0) {
			if (i >= N) { fail_no_print("ERROR: while printing int: buffer overflow\n"); }
			buf[i] = char('-');
			++i;
		}

		while (i > 0) {
			i--;
			put(o, buf[i]);
		}

	}

	template <class O> void print1(O & o, uint8_t x) { print1_inttype(o, x); }
	template <class O> void print1(O & o, uint16_t x) { print1_inttype(o, x); }
	template <class O> void print1(O & o, uint32_t x) { print1_inttype(o, x); }
	template <class O> void print1(O & o, uint64_t x) { print1_inttype(o, x); }

	template <class O> void print1(O & o, int8_t x) { print1_inttype(o, x); }
	template <class O> void print1(O & o, int16_t x) { print1_inttype(o, x); }
	template <class O> void print1(O & o, int32_t x) { print1_inttype(o, x); }
	template <class O> void print1(O & o, int64_t x) { print1_inttype(o, x); }


	template <class O>
	void print1(O & o, float x)
	{
		size_t const N = 32;

		char buf[N];

		int r = snprintf(buf, N, "%.2f", x);
		if (r < 0 or int(N) <= r) {
			fail_no_print("ERROR: while printing float: buffer overflow\n");
		}

		print1(o, buf);
	}







	template<class O, class T>
	void print(O & o, T x) {
		print1(o, x);
	}

	template<class O, class T>
	void print(O && o, T x) {
		print1(o, x);
	}

	template<class O, class Arg, class ... Args>
	void print(O & o, char const* fmt, Arg arg, Args ...args)
	{
		while (1)
		{
			if (*fmt == '\0') {
				fail_no_print("ERROR: unexpected \0 in format string; some arguments still left\n");
			}
			else if (*fmt == '{') {
				++fmt;
				if (*fmt == '{') {
					put(o, *fmt);
				}
				else {
					char const* a = fmt;
					while (*fmt != '}') {
						if (*fmt == '\0') {
							fail_no_print("ERROR: print: unmatched '{'\n");
						}
						++fmt;
					}
					char const* b = fmt;
					// [a, b) contain format args
					++fmt;
					break;
				}
			}
			else if (*fmt == '}') {
				++fmt;
				if (*fmt == '}') {
					put(o, *fmt);
				}
				else {
					fail_no_print("ERROR: single '}'\n");
				}
			}
			else {
				put(o, *fmt);
			}
			++fmt;
		}
		print1(o, arg);
		print(o, fmt, args...);
	}



	template<class Arg, class ... Args>
	void print(char const* fmt, Arg arg, Args ...args)
	{
		print(stdout, fmt, arg, args...);
	}

	inline void print(char const* fmt)
	{
		print(stdout, fmt);
	}



}
