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


	// 2 char command + null term string
	struct SFGParser
	{
		SourceReader r;

		bool is_uppercase(char c) { return 65 <= c and c <= 90; }
		bool is_white(char c) { return c == '\n' or c == ' ' or c =='(' or c == ')' or c == ';'; }
		bool is_open_param(char c) { return c == '['; }
		bool is_close_param(char c) { return c == ']'; }

		bool eof() const { return r.eof(); }

		void open(char const* path);
		void close();
		void read_cmd(CharRng w);
		void read_arg(CharRng w);
		void accept_whites();
		void accept_instr(CharRng & w);
		void accept_param(CharRng & w);
	};

	struct SFG
	{

		void open(char const* path, Score2 * result);

		void has_next()
		void next(Action & act, Ply & ply, Rng<char> comment);



		void close();
	};

	Action sfg_arg_to_action(char const* arg);

	void sfg_load(SFGParser & sfg, char const* path);
	bool sfg_next(SFGParser & sfg, Board & next, Board & curr);



}
