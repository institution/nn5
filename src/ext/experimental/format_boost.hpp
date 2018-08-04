#pragma once

#include <string>
#include <iostream>
#include <boost/format.hpp>
#include <iomanip> // setprecision
#include <sstream> // stringstream



inline
std::string format(boost::format pattern) {
	return pattern.str();
}

template <class U, class ...T> inline
std::string format(boost::format pattern, U&& u, T&&... rest) {
	return format(pattern % u, rest...);
}

template <class ...T> inline
std::string format(std::string const& pattern, T&&... rest) {
	return format(boost::format(pattern), rest...);
}

template <class ...T> inline
std::string format(char const* pattern, T&&... rest) {
	return format(boost::format(pattern), rest...);
}


template <class ... Args>
void print(std::string const& s, Args ... args) {
	std::cout << format(s, args...);
}

template <class ... Args>
void print(char const* s, Args&& ... args) {
	std::cout << format(s, std::forward<Args>(args)...);
}

template <class ... Args>
void print(std::ostream & o, Args ... args) {
	o << format(args...);
}



inline std::string to_string(float f, int n)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(n) << f;
	std::string s = stream.str();
	return s;
}



