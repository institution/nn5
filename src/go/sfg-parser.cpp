#include "sfg-parser.hpp"


void SFGParser::open(char const* path) {
	r.open(path);
	accept_whites();
}


void SFGParser::close() {
	r.close();
}


void SFGParser::read_cmd(CharRng cmd) {
	accept_instr(cmd);
}

void SFGParser::read_arg(CharRng arg) {
	accept_param(arg);
	accept_whites();
}





void SFGParser::accept_whites() {
	// multiple whitespaces
	while (is_white(r.peek())) {
		r.get();
	}
}

void SFGParser::accept_instr(CharRng & w) {
	// one or two uppercase letters
	if (is_uppercase(r.peek())) {
		ext::put(w, r.get());
	}
	else {
		ext::fail("ERROR: accept_instr expected uppercase, got: {}\n", Char(r.peek()));
	}

	if (is_uppercase(r.peek())) {
		put(w, r.get());
	}

}

void SFGParser::accept_param(CharRng & w) {
	// [some text inside square brackets]
	if (not is_open_param(r.get())) { ext::fail(""); }
	while (not is_close_param(r.peek())) {
		put(w, r.get());
	}

	// get close_param char: ']'
	r.get();

	//put(w, '\0');
}
