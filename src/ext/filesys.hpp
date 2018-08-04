#pragma once

#include <string>

namespace ext{

	using Path = std::string;
	

	inline Path parent_path(Path const& p) {
		auto i = p.rfind('/');
		if (i != std::string::npos) {
			return p.substr(0, i);
		}
		else {
			return p;
		}
	}
	
	inline Path canonical(Path const& p) {
		return p;
	}

	inline bool is_absolute(Path const& p) {
		return p.size() > 0 and p[0] == '/';
	}

	
	inline Path _join(Path const& a, Path const& b) {
		auto A = a.size();
		auto B = b.size();
		
		if (A == 0) {
			return b;
		}
		
		if (B == 0) {
			return a;
		}
		
		if (a[A-1] != '/' and b[B-1] != '/') {
			return a + std::string("/") + b;
		}
		
		if (a[A-1] == '/' and b[B-1] == '/') {
			return a + b.substr(1);
		}
		
		return a + b;
	}

	inline Path make_path(Path const& a, char const* b) 
	{
		return _join(a, Path(b));
	}


	/*inline Path operator/(char const* a, char const* b) {
		return _join(Path(a), Path(b));
	}*/

	inline Path operator/(char const* a, Path const& b) {
		return _join(Path(a), b);
	}

	inline Path operator/(Path const& a, char const* b) {
		return _join(a, Path(b));
	}

	inline Path operator/(Path const& a, Path const& b) {
		return _join(a, b);
	}
	
}
	
