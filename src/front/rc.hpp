#pragma once

#include "../ext/ext.hpp"
#include <vector>
#include <sstream>
#include "cerror.hpp"

using ext::fail;

namespace front 
{

	inline bool is_quote_str(char c)     { return c == '"';}
	inline bool is_escape_char(char c)   { return c == '\\';}
	inline bool is_body_str(char c)      { return char(32) <= c and c <= char(127) and c != '\\' and c != '"';}
	inline bool is_open_sym(char c)      { return char(33) <= c and c <= char(127);}
	inline bool is_body_sym(char c)      { return char(33) <= c and c <= char(127);}
	inline bool is_open_comment(char c)  { return c == '#'; }
	inline bool is_white(char c)         { return c == '\t' or c == ' '; }
	inline bool is_endline(char c)       { return c == '\n'; }
	inline bool is_eof(char c)           { return c == EOF; }

	using ext::Path;


	using namespace ext;
	
	struct SrcInfo
	{
		Path path;
		FILE * fp;
		char c;      // assert c != EOF
		size_t mark;
		size_t line_beg;
		size_t line_no;		
	};

	struct SrcReader
	{		
		// assert fis.size > 0 or fis.back().c != EOF
		std::vector<SrcInfo> fis;
		
		void rc(Path const& path) 
		{
			SrcInfo fi;
			fi.path = path;
			fi.fp = file_open(path, "rb");
			fi.c = file_get(fi.fp);
			fi.mark = 0;
			fi.line_beg = 0;
			fi.line_no = 1;
			
			
				
			if (fi.c == EOF) {
				print("WARNING: rc {}: empty file\n", path);
				file_close(fi.fp);
			}
			else {									
				fis.push_back(fi);
			}
		}
		
		size_t tell() const 
		{
			return fis.empty() ? 0 : file_tell(fis.back().fp);
		}
		
		/*void seek(size_t offset)
		{
			if (fis.empty()) {
				ext::fail("ERROR: seek: no open file\n");
			}
			file_seek(fis.back().fp, offset);
		}*/
		
		char get() 
		{	
			if (fis.size() > 0) 
			{
				auto & fi = fis.back();
				
				char c0 = fis.back().c;
			
				fi.c = file_get(fi.fp);
				
				if (fi.c == EOF) {
					file_close(fi.fp);
					fis.pop_back();
				}
				
				return c0;
			}
			else {
				return EOF;
			}
		}
		
		bool eof() const { return fis.empty(); }		
		char peek() const { return fis.empty() ? EOF : fis.back().c; }
		
		void close() 
		{
			while (fis.size() > 0) 
			{
				auto & fi = fis.back();
				file_close(fi.fp);
				fis.pop_back();
			}
		}
		
		/*size_t & mark() { 
			if (fis.empty()) { 
				ext::fail("ERROR: mark\n");
			} 
			return fis.back().mark; 
		}*/
		char const* get_path() const 
		{
			if (fis.empty()) { 
				return "<empty>";
			}			
			return fis.back().path.c_str();
		}
		
		void set_mark() { 
			if (fis.empty()) { ext::fail("ERROR: set_mark\n"); } 
			
			fis.back().mark = file_tell(fis.back().fp) - 1; 
		}
		
		void unread() {
			if (fis.empty()) { ext::fail("ERROR: unread\n"); } 
			
			auto & fi = fis.back();
			file_seek(fi.fp, fi.mark); 
			fi.c = file_get(fi.fp);
		}
		
		size_t & line_beg() { 
			if (fis.empty()) { 
				ext::fail("ERROR: line_beg\n");
			} 
			
			return fis.back().line_beg; 
		}		
		
		size_t & line_no() { 
			if (fis.empty()) { 
				ext::fail("ERROR: line_no\n");
			} 			
			return fis.back().line_no; 
		}
	};

	using Reader = SrcReader;
	using Writer = std::stringstream;
		
	struct RC
	{
		Writer w;
		Reader r;
		
		std::string tmp;
		CError error;
		
		void rc(Path const& path);
		
		
		void accept_string();
		void accept_symbol();
		void accept_comment();
		void skip_whites();

		bool eof() const {
			return r.eof();
		}

		bool ok() const 
		{
			return not eof() and not error.has();
		}
		
		void end()
		{
			if (error.has()) {
				ext::fail(error.get().c_str());
			}			
		}

		void close()
		{
			r.close();
			end();
		}

		template <class... Args>
		void set_error(char const* fmt, Args... args)
		{
			if (not error.has()) 
			{
				auto err = error.error.range();
				print(err, "ERROR {}:{}:{}: ", r.get_path(), r.line_no(), int64_t(r.tell()) - r.line_beg() + 1);
				print(err, fmt, args...);
				print(err, "\n");				
			}
		}

		int to_int(std::string const& s);
		

		uint32_t read_uint32();
		int32_t read_int32();

		uint16_t read_uint16();
		int16_t read_int16();

		uint8_t read_uint8();
		int8_t read_int8();

		
		std::string const& read_string();
		std::string const& read_symbol();
		void read_string(char * ptr, size_t size);


		void unread() { r.unread(); }
		
		
		RC() = default;
		RC(Path const& path) { rc(path); }		
		~RC() { close(); }

	};
	
	using RCParser = RC;

	inline void read(RCParser & p, uint32_t & x) { x = p.read_uint32(); }
	inline void read(RCParser & p, int32_t & x) { x = p.read_int32(); }
	inline void read(RCParser & p, uint16_t & x) { x = p.read_uint16(); }
	inline void read(RCParser & p, int16_t & x) { x = p.read_int16(); }
	inline void read(RCParser & p, uint8_t & x) { x = p.read_uint8(); }
	inline void read(RCParser & p, int8_t & x) { x = p.read_int8(); }	
	inline void read(RCParser & p, std::string & x) { x = p.read_string(); }
	inline void read(RCParser & p, char * ptr, size_t size) { p.read_string(ptr, size); }

}
