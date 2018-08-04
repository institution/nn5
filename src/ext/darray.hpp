#pragma once

#include <vector>
#include <array>
#include "aga2.hpp"


namespace ext {


	template <class U>
	using v2 = aga2::Mv1<U>;

	//template <class U, class N>
	//using vN = std::array<U,N>;



	template <class T, class U>
	struct darray1 {
		std::vector<T> p;

		darray1() = default;
		darray1(U dim) { resize(dim); }
		darray1(U dim, T const& def) { resize(dim, def); }

		size_t size() const { return p.size(); }
		size_t byte_size() const { return p.size() * sizeof(T); }

		void resize(U dim) {
			p.resize(dim);
		}

		void resize(U dim, T const& def) {
			p.resize(dim, def);
		}

		U get_dim() const { return p.size(); }

		U get_pos(T const& t) const
		{
			size_t delta = &t - &p[0];
			assert(delta < size());
			return U(delta);
		}

		auto begin() { return p.begin(); }
		auto end() { return p.end(); }
		auto begin() const { return p.begin(); }
		auto end() const { return p.end(); }

		T const& operator[](size_t i) const { return p[i]; }
		T & operator[](size_t i) { return p[i]; }

		T const& at(size_t i) const { return p.at(i); }
		T & at(size_t i) { return p.at(i); }

		void fill(T const& f) {
			for (size_t i = 0; i < size(); ++i)
			{
				at(i) = f;
			}
		}

		T const& operator()(U pos) const {
			assert(U(0) <= pos);
			assert(pos < get_dim());
			return p.at(pos);
		}

		T & operator()(U pos) {
			assert(U(0) <= pos);
			assert(pos < get_dim());
			return p.at(pos);
		}

		void clear() { p.clear(); }
	};


	template <class T, class U>
	std::ostream & operator<<(std::ostream & o, darray1<T,U> const& a)
	{

		for (U i = 0; i < a.size(); ++i)
		{
			o << a(i) << ' ';
		}
		o << "\n";

		return o;
	}









	template <class T, class U>
	struct darray2 {
		std::vector<T> p;
		v2<U> dim;

		darray2() = default;
		darray2(v2<U> dim) { resize(dim); }
		darray2(v2<U> dim, T const& def) { resize(dim, def); }

		void resize(v2<U> dim) {
			p.resize(dim[0] * dim[1]);
			this->dim = dim;
		}

		void resize(v2<U> dim, T const& def) {
			p.resize(dim[0] * dim[1], def);
			this->dim = dim;
		}

		v2<U> get_dim() const { return dim; }

		v2<U> to_pos(size_t delta) const
		{
			assert(delta < size());
			U y = delta / dim[0];
			U x = delta % dim[0];
			return {x,y};
		}

		size_t to_ind(v2<U> pos) const
		{
			assert(0 <= pos[0]);
			assert(0 <= pos[1]);
			assert(pos[0] < dim[0]);
			assert(pos[1] < dim[1]);
			return pos[0] + pos[1] * dim[0];
		}


		v2<U> get_pos(T const& t) const
		{
			auto delta = &t - &p[0];
			return to_pos(delta);
		}



		auto begin() { return p.begin(); }
		auto end() { return p.end(); }
		auto begin() const { return p.begin(); }
		auto end() const { return p.end(); }

		size_t size() const { return p.size(); }
		size_t byte_size() const { return p.size() * sizeof(T); }

		T const& operator[](size_t i) const { return p[i]; }
		T & operator[](size_t i) { return p[i]; }

		T const& at(size_t i) const { return p.at(i); }
		T & at(size_t i) { return p.at(i); }

		void fill(T const& val) {
			for (size_t i = 0; i < size(); ++i)
			{
				at(i) = val;
			}
		}

		T const& operator()(v2<U> pos) const
		{
			return p.at(to_ind(pos));
		}

		T & operator()(v2<U> pos)
		{
			return p.at(to_ind(pos));
		}



		std::ostream & print(std::ostream & o) const
		{
			for (U j = 0; j < dim[1]; ++j) {
				for (U i = 0; i < dim[0]; ++i)
				{
					ext::print("%| 6.3d| ", (*this)({i,j}));
				}
				o << "\n";
			}
			return o;
		}

		void clear() {
			dim[0] = v2<U>(0,0);
			p.clear();
		}

		bool has(v2<U> pos) const {
			return
				0 <= pos[0] and pos[0] < dim[0] and
				0 <= pos[1] and pos[1] < dim[1];
		}
	};


	template <class T, class U>
	std::ostream & operator<<(std::ostream & o, darray2<T,U> const& a)
	{
		for (U j = 0; j < a.dim[1]; ++j) {
			for (U i = 0; i < a.dim[0]; ++i)
			{
				o << a({i,j}) << ' ';
			}
			o << "\n";
		}
		return o;
	}


	// data type, index type, dimensions
	template <class T, class U, int N>
	struct darrayN
	{
		using Idx = std::array<U, N>;

		std::vector<T> p;
		Idx dim;


		darrayN() = default;
		darrayN(Idx dim) { resize(dim); }
		darrayN(Idx dim, T const& def) { resize(dim, def); }

		size_t get_size(Idx dim) const {
			size_t s = 1;
			for (size_t i = 0; i < dim.size(); ++i)
			{
				s *= dim[i];
			}
			return s;
		}


		void resize(Idx dim)
		{
			p.resize(get_size(dim));
			this->dim = dim;
		}

		void resize(Idx dim, T const& def)
		{
			p.resize(get_size(dim), def);
			this->dim = dim;
		}


		Idx get_dim() const { return dim; }


		auto begin() { return p.begin(); }
		auto end() { return p.end(); }
		auto begin() const { return p.begin(); }
		auto end() const { return p.end(); }


		size_t size() const { return p.size(); }

		T const& operator[](size_t i) const { return p[i]; }
		T & operator[](size_t i) { return p[i]; }

		T const& at(size_t i) const { return p.at(i); }
		T & at(size_t i) { return p.at(i); }


		void fill(T const& def) {
			for (auto i = p.begin(); i != p.end(); ++i)
			{
				*i = def;
			}
		}


		T const& operator()(Idx pos) const
		{
			size_t off = pos[0];
			auto d = U(1);

			for (size_t i = 1; i < pos.size(); ++i)
			{
				assert(0 <= pos[i]);
				assert(pos[i] < dim[i]);

				d *= dim[i-1];
				off += pos[i] * d;
			}
			return p.at(off);
		}

		T & operator()(v2<U> pos)
		{

			size_t off = pos[0];
			auto d = U(1);

			for (size_t i = 1; i < pos.size(); ++i)
			{
				assert(0 <= pos[i]);
				assert(pos[i] < dim[i]);

				d *= dim[i-1];
				off += pos[i] * d;
			}
			return p.at(off);

		}
	};


	/*template <class T, class U>
	std::ostream & operator<<(std::ostream & o, darray2<T,U> const& a)
	{
		for (U j = 0; j < a.dim[1]; ++j) {
			for (U i = 0; i < a.dim[0]; ++i)
			{
				o << a({i,j}) << ' ';
			}
			o << "\n";
		}
		return o;
	}*/






}
