#pragma once

#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <array>

#include "../ext/ext.hpp"
#include "source-reader.hpp"
#include "go.hpp"

using namespace ext;

namespace Go {

	Action sfg_parse_action(char const* s);
	Score2 sfg_parse_score(char const* s);
	
	struct SFG
	{
		SourceReader r;
		FString<4> cmd;
		DString arg{509};
		
		bool is_uppercase(char c) { return 65 <= c and c <= 90; }
		bool is_white(char c) { return c == '\n' or c == ' '; }
		bool is_open_param(char c) { return c == '['; }
		bool is_close_param(char c) { return c == ']'; }

		bool eof() const { return r.eof(); }

		void open(char const* path);
		bool is_open() { return r.is_open(); }
		void close();
		
		void mark() { r.mark(); }
		void unread() { r.unread(); }
		
		void accept_whites();
		void accept_instr(Rng<char> w);
		void accept_param(Rng<char> w);
		
		// ----------------		
		
		void read_header(Score2 * result);
		bool has_command();
		void read_command();
		void read_move(Action * act, Ply * ply, char const** com);
	};

	


}
