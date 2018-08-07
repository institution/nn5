#include "sfg-parser.hpp"

namespace Go {

	// parser ------------------------------------------------------

	void SFGParser::open(char const* path) {
		r.close();
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


	// sfg ---------------------------------------------------------

	void sfg_load(SFGParser & sfg, char const* path) 
	{
		sfg.open(path);

		FixedString<3> cmd;
		FixedString<32> arg;

		while(1) {
			sfg.read_cmd(cmd.range());
			sfg.read_arg(arg.range());

			if (cmd == "SZ") {
				if (arg != "9") {
					ext::fail("ERROR: sfg have diffrent board size\n");
				}
			}
			else if (cmd == "KM") {
				if (arg != "5.5") {
					ext::fail("ERROR: sfg have diffrent komi: {}\n", arg);
				}
			}
			else if (cmd == "HA") {
				if (arg != "0") {
					ext::fail("ERROR: sfg have diffrent handicap\n");
				}
			}
			else if (cmd != "RE") {
				// TODO: parse and set result
				//RE[W+10.5]
				print("MESSAGE: sfg result: {}\n", arg);
				break;
			}
		}
	}


	Go::Action sfg_arg_to_action(char const* arg)
	{
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


	bool sfg_next(SFGParser & sfg, Board & next, Board & curr)
	{
		FixedString<3> cmd;
		FixedString<512> arg;

		Go::Action act{Go::ActionNone};
		Go::Ply ply{Go::PlyNone};

		while (1)
		{
			if (sfg.eof()) {
				print("INFO: end of sfg file\n");
				return 0;
			}

			cmd.clear();
			arg.clear();
			sfg.read_cmd(cmd.range());
			sfg.read_arg(arg.range());

			if (cmd == "C") {
				// ignore comments for now
			}
			else if (cmd == "B") {
				ply = Go::PlyBlack;
				act = sfg_arg_to_action(arg.c_str());
				break;
			}
			else if (cmd == "W") {
				ply = Go::PlyWhite;
				act = sfg_arg_to_action(arg.c_str());
				break;
			}
			else if (cmd == "") {
				// no more actions
				//
				return 0;
			}
		}

		Err err = move(next, curr, act, ply);
		if (err) {
			// display error
			print("ERROR: {}\n", err);
		}
		return 1;
	}
	
}
