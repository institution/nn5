#include "go.hpp"

namespace Go {

	void load_sfg(Rep & rep, char const* path)
	{
		Board * curr = &rep.insert();
		curr->reset();

		Score2 result = 0;

		SFGParser sfg;
		sfg.open(path);

		FixedString<3> cmd;
		auto p = cmd.c_str();
		sfg.read_cmd(cmd.range());


		FixedString<16> arg;

		while (p)
		{
			if (cmd == "SZ") {
				assert(strcmp(p+3, "9") == 0);
			}
			else if (strcmp(p, "KM") == 0) {
				assert(strcmp(p+3, "5.5") == 0);
			}
			else if (strcmp(p, "HA") == 0) {
				assert(strcmp(p+3, "0") == 0);
			}
			else if (strcmp(p, "RE")) {
				// "B+3.5"
				char who = p[3];
				int hscore = atoi(&p[5]);
				result = (who == 'B' ? -1 : +1) * hscore * 2 + 1;

				curr->result = result;
			}
			else if (strcmp(p, "B") == 0 or strcmp(p, "W") == 0)
			{
				auto ply = (p[0] == 'B' ? PlyBlack : PlyWhite);
				Action act = ActionPass;

				if (p[3] != '\0') {
					// not pass
					act = action_move(Vec(p[3] - 'a', p[4] - 'a'));
				}

				Board * next = &rep.insert();

				move(*next, *curr, act, ply);
				next->result = result;

				curr = next;
			}

			sfg.read_cmd(cmd.range());
		}




	}

	Score2 get_ply_sign(Ply ply) {
		if (ply == PlyBlack) return -1;
		if (ply == PlyWhite) return +1;
		return 0;
	}

	void Board::reset()
	{
		black.clear();
		white.clear();

		player = PlyBlack;
		turn = 0;
		ah[0] = ActionNone;
		ah[1] = ActionNone;

		result = 0;
		action = ActionNone;
	}

	Ply Board::get(Vec vec) const {
		if (black.get(vec)) { return PlyBlack; }
		if (white.get(vec)) { return PlyWhite; }
		return PlyNone;
	}

	Ply Board::get_winner() const {
		if (result < 0) return PlyBlack;
		if (result > 0) return PlyWhite;
		return PlyNone;
	}



	struct CountAreaImpl
	{
		/// area = territory + onboard stones

		Plane const& black;
		Plane const& white;

		Plane terr;  // mark counted territory
		Score2 black_score;
		Score2 white_score;
		Score2 gray_score;  // no ones land

		// fragment
		Stack<Vec, N*N> boundary;
		bool terr_black;
		bool terr_white;
		Score2 terr_score;

		CountAreaImpl(Plane const& black, Plane const& white): black(black), white(white) {}

		void visit(Vec v) {
			if (black.get(v)) {
				terr_black = true;
			}
			else if (white.get(v)) {
				terr_white = true;
			}
			else if (not terr.get(v)) {
				terr.set(v);
				terr_score += 2;
				boundary.push(v);
			}
		}

		void count_fragment(Vec v)
		{
			assert(boundary.size() == 0);
			terr_black = false;
			terr_white = false;
			terr_score = 0;

			visit(v);

			while (boundary.size() > 0)
			{
				auto v = boundary.pop();

				// only empty squares extends territory
				assert(white.get(v) == 0 and black.get(v) == 0);

				// look at neighs
				if (v[0] < N - 1) { visit({v[0] + 1, v[1]}); }
				if (v[1] < N - 1) { visit({v[0], v[1] + 1}); }
				if (v[0] > 0) { visit({v[0] - 1, v[1]}); }
				if (v[1] > 0) { visit({v[0], v[1] - 1}); }
			}

			// calc terr
			if (terr_black xor terr_white) {
				if (terr_black) { black_score += terr_score; }
				if (terr_white) { white_score += terr_score; }
			}
			else {
				gray_score += terr_score;
			}
		}

		Score2 call()
		{
			terr.clear();
			black_score = 0;
			white_score = 0;
			gray_score = 0;

			for (uint8_t j = 0; j < N; ++j)
			for (uint8_t i = 0; i < N; ++i)
			{
				Vec v = {i,j};
				if (black.get(v)) {
					black_score += 2;
				}
				else if (white.get(v)) {
					white_score += 2;
				}
				else if (not terr.get(v)) {
					count_fragment(v);
				}
			}

			print("INFO: white_score black_score gray_score = {} {} {}\n", white_score, black_score, gray_score);
			print("INFO: white_score + black_score + gray_score = {}\n", white_score + black_score + gray_score);
			print("INFO: N*N*2 = {}\n", N*N*2);
			assert(white_score + black_score + gray_score == N*N*2);

			return white_score - black_score;
		}


	};


	Score2 count_area(Plane const& black, Plane const& white) {
		CountAreaImpl fn(black, white);
		return fn.call();
	}



	void push_if_not_set(Stack<Vec, N*N> & boundary, Plane const& visited, Vec v) {
		if (not visited.get(v)) {
			boundary.push(v);
		}
	}

	/// defined only if self.get(seed) == 1
	bool at_liberty(Plane const& self, Plane const& other, Vec seed)
	{
		assert(self.get(seed) == 1);

		Plane visited;
		visited.clear();
		Stack<Vec, N*N> boundary;

		boundary.push(seed);

		while (boundary.size() > 0)
		{
			auto v = boundary.pop();
			if (not visited.get(v))
			{
				// visit
				visited.set(v);

				if (self.get(v)) {
					// connection

					// add neighs
					if (v[0] < N - 1) {
						push_if_not_set(boundary, visited, {v[0] + 1, v[1]});
					}
					if (v[1] < N - 1) {
						push_if_not_set(boundary, visited, {v[0], v[1] + 1});
					}
					if (v[0] > 0) {
						push_if_not_set(boundary, visited, {v[0] - 1, v[1]});
					}
					if (v[1] > 0) {
						push_if_not_set(boundary, visited, {v[0], v[1] - 1});
					}
				}
				else if (other.get(v)) {
					// no liberty here
				}
				else {
					// found a liberty
					return true;
				}
			}
		}
		return false;
	}

	void push_and_unset_if_set(Stack<Vec, N*N> & boundary, Plane & plane, Vec v) {
		if (plane.get(v)) {
			boundary.push(v);
			plane.unset(v);
		}
	}

	// defined only if self.get(seed) == 1
	void remove_group(Plane & self, Vec seed)
	{
		assert(self.get(seed) == 1);

		//print("DEBUG: remove groups at seed = {}\n", seed);

		//Plane visited;
		//visited.clear();

		Stack<Vec, N*N> boundary;


		push_and_unset_if_set(boundary, self, seed);
		assert(boundary.size() > 0);

		while (boundary.size() > 0)
		{
			auto v = boundary.pop();

			//print("DEBUG: v = {}\n", v);

			// add neighs
			if (v[0] < N - 1) {
				push_and_unset_if_set(boundary, self, {v[0] + 1, v[1]});
			}
			if (v[1] < N - 1) {
				push_and_unset_if_set(boundary, self, {v[0], v[1] + 1});
			}
			if (v[0] > 0) {
				push_and_unset_if_set(boundary, self, {v[0] - 1, v[1]});
			}
			if (v[1] > 0) {
				push_and_unset_if_set(boundary, self, {v[0], v[1] - 1});
			}

		}
	}

	void remove_dead_stones(Plane & self, Plane const& other, Vec seed)
	{
		if (self.get(seed)) {
			if (not at_liberty(self, other, seed)) {
				print("DEBUG: no liberty at {}\n", seed);
				remove_group(self, seed);
			}
		}
	}



	void place_stone(Plane & self, Plane & other, Vec v)
	{
		self.set(v);

		Score2 s = 0;

		if (v[0] < N - 1) { remove_dead_stones(other, self, {v[0] + 1, v[1]}); }
		if (v[1] < N - 1) { remove_dead_stones(other, self, {v[0], v[1] + 1}); }
		if (v[0] > 0) { remove_dead_stones(other, self, {v[0] - 1, v[1]}); }
		if (v[1] > 0) { remove_dead_stones(other, self, {v[0], v[1] - 1}); }

		// sucide if any
		remove_dead_stones(self, other, v);
	}

	// place stone on board do not check for liberty or change other game state
	// overwrite existing stone of field
	// for testing
	void just_place(Board & b, Vec v, Ply ply)
	{
		if (ply == PlyBlack) {
			if (b.white.get(v)) {
				b.white.unset(v);
			}
			b.black.set(v);
		}
		else if (ply == PlyWhite) {
			if (b.black.get(v)) {
				b.black.unset(v);
			}
			b.white.set(v);
		}
		else {
			ext::fail("ERROR: wtf?\n");
		}
	}

	/// Execute action if valid. Report invalid action.
	Err move(Board & next, Board & curr, Action act, Ply ply)
	{
		/// next and curr may be the same object!


		//print("aaa\n");

		// p is active player
		if (ply == PlyNone or ply != curr.player) {
			ext::fail("Critical: invalid player\n");
		}

		Vec vec;

		// move
		if (is_action_move(act))
		{
			vec = action_to_vec(act);

			if (RULE_KO_REPEAT) {
				// cannot repeat move
				if (act == curr.ah[1]) { return "cannot repeat move"; }
			}

			// dest must be empty
			if (curr.get(vec) != PlyNone) { return "position is occupied"; }
		}

		// update curr
		curr.action = act;

		// init next
		next.black = curr.black;
		next.white = curr.white;

		assert(curr.player != PlyNone);
		next.player = (curr.player % 2) + 1;
		next.turn = curr.turn + 1;

		next.ah[1] = curr.ah[0];
		next.ah[0] = act;

		next.result = 0;
		next.action = ActionNone;


		if (is_action_move(act) or act == ActionPass) {
			// move or pass

			if (is_action_move(act))
			{
				// move
				if (ply == PlyBlack) {
					place_stone(next.black, next.white, vec);
				}
				else {
					place_stone(next.white, next.black, vec);
				}
			}

			// turn limit reached
			// pass pass
			if (next.turn == RULE_TURN_LIMIT or (next.ah[0] == ActionPass and next.ah[1] == ActionPass)) {

				// end game
				next.player = PlyNone;
				next.result = count_area(next.black, next.white) + RULE_KOMI;
			}

		}
		else if (act == ActionResign) {
			// resign
			next.player = PlyNone;
			// cause komi white gets more points
			// for 9x9 board and +5.5 komi black gets 75.5 and white 86.5
			next.result = -get_ply_sign(ply) * RULE_RESIGN_SCORE + RULE_KOMI;
		}
		else {
			ext::fail("ERROR: invalid action\n");
		}

		return nullptr;
	}

}
