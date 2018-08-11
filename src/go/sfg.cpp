#include "sfg.hpp"

namespace Go {

	// sfg ------------------------------------------------------

	void SFG::open(char const* path) {
		r.close();
		r.open(path);
		accept_whites();
	}


	void SFG::close() {
		r.close();
	}



	void SFG::read_header(Score2 * result) 
	{
		accept_whites();
		if (r.get() != '(') { fail("ERROR: expected '('\n"); }
		accept_whites();
		if (r.get() != ';') { fail("ERROR: expected ';'\n"); }
		accept_whites();
				
		while(r.peek() != ';') 
		{
			read_command();
			
			if (cmd == "SZ") {
				if (arg != "9") {
					ext::fail("ERROR: sfg have diffrent board size\n");
				}
			}
			else if (cmd == "KM") {
				if (arg != "5.5") {
					ext::fail("ERROR: sfg have diffrent komi: {}\n", arg.c_str());
				}
			}
			else if (cmd == "HA") {
				if (arg != "0") {
					ext::fail("ERROR: sfg have diffrent handicap\n");
				}
			}
			else if (cmd == "RE") {
				
				if (result != nullptr) {
					*result = sfg_parse_score(arg.c_str());					
				}
					
				//RE[W+10.5]
				print("DEBUG: sfg result: {}\n", arg.c_str());				
			}
		}
		
		r.get();
		accept_whites();
	}
	
	bool SFG::has_command() {
		return is_open() and r.peek() != EOF;
	}
	
	void SFG::read_command() {
		accept_instr(cmd.range());
		accept_param(arg.range());
		accept_whites();
	}
	
	void SFG::read_move(Action * act, Ply * ply, char const** com) 
	{
		*ply = PlyNone;
		*act = ActionNone;
		if (com != nullptr) {
			*com = "";
		}
		
		while (r.peek() != ';' and r.peek() != ')')
		{		
			read_command();
			
			if (cmd == "B") {
				*ply = PlyBlack;
				*act = sfg_parse_action(arg.c_str());
			}
			else if (cmd == "W") {
				*ply = PlyWhite;
				*act = sfg_parse_action(arg.c_str());
			}
			else if (cmd == "C") {
				if (com != nullptr) {
					*com = arg.c_str();
				}
			}
			else {
				fail("ERROR: expected command B, W or C\n");
			}			
		}
		
		r.get();
		accept_whites();
	}



	void SFG::accept_whites() {
		// multiple whitespaces
		while (is_white(r.peek())) {
			r.get();
		}
	}

	void SFG::accept_instr(Rng<char> w) {
		// one or two uppercase letters
		if (is_uppercase(r.peek())) {
			ext::put(w, r.get());
		}
		else {
			fail("ERROR: accept_instr expected uppercase, got: {}\n", Char(r.peek()));
		}

		if (is_uppercase(r.peek())) {
			put(w, r.get());
		}
		
		put(w, '\0');
		
		accept_whites();
	}

	void SFG::accept_param(Rng<char> w) {
		// [some text inside square brackets]
		
		if (not is_open_param(r.get())) { ext::fail(""); }
		while (not is_close_param(r.peek())) {
			put(w, r.get());
		}
		put(w, '\0');
		
		// get close_param char: ']'
		r.get();

		//put(w, '\0');
		accept_whites();
	}


	

	
	
	Score2 sfg_parse_score(char const* s) {
		// B+12.5
		
		if (strlen(s) < 5) {
			fail("ERROR: cannot parse score\n");
		}
		
		Score2 r = parse_int(&s[2]) * 2 + 1;
		
		if (s[0] == 'B') {
			r = -r;
		}
		else if (s[0] == 'W') {
		
		}
		else {
			fail("ERROR: cannot parse score: expected B or W\n");
		}
		
		return r;
	}
	
	
	Go::Action sfg_parse_action(char const* arg)
	{
		print("DEBUG: sfg_parse_action: \"{}\"\n", arg);
		if (strlen(arg) == 2) {
			return Go::action_move(Go::Vec(arg[0] - 'a', arg[1] - 'a'));
		}
		else if (strlen(arg) == 0) {
			return Go::ActionPass;
		}
		else {
			ext::fail("ERROR: invalid sfg action\n");
		}
	}



	
}
