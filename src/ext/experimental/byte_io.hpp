#pragma once

#include <fstream>

namespace byte_io {

	/*
	uint32_t const current_version = 1;
	
	
	using Byte = uint8_t;
	
	template<typename A>
	Byte read_byte(A & ar) {
		Byte b;
		ar.read((char *)&b, sizeof(Byte));
		return b;
	}

	template<typename A>
	void write_byte(A & ar, Byte const& t) {		
		ar.write((char const*)&t, sizeof(Byte));
	}
	
	
	#include <boost/endian/conversion.hpp>
	
	using Endian = int8_t;
	Endian const
		EndianNone = 0,
		EndianLittle = 1,
		EndianBig = 2;
		
	Endian get_endian() {
		return EndianBig;
	}
		
		
	template <class T>
	T switch_endian(T x) 
	{
		T y;
		for (int8_t i = 0; i < sizeof(x); ++i) {
		
			x & (0xFF << i)
		}
	}
	
		
	namespace le {	
	
		// 0x0102 -> 0x02 0x01
	
		template<class T, typename A>
		void read(A & ar, T & r) {
			T t;
			r = 0;
			unsigned i = 0;
			
			// read size, int has diffrent size on diffrent platforms
			auto size = read_byte(ar);  
			
			while (i < size*8) {
				t = read_byte(ar);
				r = r | (t << i);
				i += 8;
			}
		}

		template<class T, typename A>
		void write(A & ar, T const& t) {		
			unsigned i = 0;
			
			// write size, int has diffrent size on diffrent platforms
			write_byte(ar, uint8_t(sizeof(T)));
			
			while (i < sizeof(T)*8) {
				write_byte(ar, (t >> i) & 0xFF);
				i += 8;
			}
			
		}
		
	}
	
	namespace be {	
	
		// 0x0102 -> 0x01 0x02
	
		template<class T, typename A>
		void read(A & ar, T & result) {
			T tmp;
			result = 0;
			unsigned i = 0;
			
			
			
			// size of the object
			auto size = read_byte(ar);  
			
			while (i < size*8) {
				
				tmp = T(read_byte(ar));
				
				r = r | (tmp << i);
				i += 8;
			}
		}

		template<class T, typename A>
		void write(A & ar, T const& t) {		
			unsigned i = 0;
			
			// write size, int has diffrent size on diffrent platforms
			write_byte(ar, uint8_t(sizeof(T)));
			
			while (i < sizeof(T)*8) {
				write_byte(ar, (t >> i) & 0xFF);
				i += 8;
			}
			
		}
		
	}
	
	
	
		
	
		
	template<typename A>
	void read(A & ar, std::string & t) {
		std::string::size_type n;
		read(ar, n);
		t.resize(n);
		for (std::string::size_type i=0; i<n; ++i) {
			read(ar, t[i]);
		}
	}

	template<typename A>
	void write(A & ar, std::string const& t) {		
		write(ar, t.size());
		for (auto c: t) {
			write(ar, c);
		}
	}

	

	template <class T, class A>
	void write(A & ar, aga2::Mv1<T> const& c)
	{
		write(ar, c[0]);
		write(ar, c[1]);		
	}


	template <class T, class A>
	void read(A & ar, aga2::Mv1<T> & c)
	{		
		read(ar, c[0]);
		read(ar, c[1]);
	}


	


	template<typename T, typename A>
	T read(A & ar) {
		T tmp;		
		read(ar, tmp);
		return tmp;
	}

	




	inline void write_file(std::string const& fname, Env & env) {
		std::ofstream f(fname, std::ios::binary);
		write(f, env);
	}
    
    inline void read_file(std::string const& fname, Env & env) {
		std::ifstream f(fname, std::ios::binary);
		read(f, env);
	}
	*/
}
