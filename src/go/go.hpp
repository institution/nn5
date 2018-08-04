#pragma once

#include "ext/ext.hpp"
#include "sfg-parser.hpp"

/**
go state (board)
board database
	bulk save/load
GO Game RULES (headless)
	SFG import/export



max 15x15 board
// simplified ko: cannot take same action 3 times in a row
black is -
white is +
black moves first
komi = +5.5
handicap = 0 (placed randomly)
resignation ends game and sets score to RULE_RESIGN_SCORE for the opponent
turn limit = RULE_TURN_LIMIT (even number preferred)
after turn limit is reached game ends and score is calculated
area scoring (stones + territory)
sucide allowed

**/


namespace Go {

	using namespace ext;

	// Types
	using Ply = uint8_t;
	using Action = uint8_t;
	using Vec = ext::v2<int8_t>;
	using Score2 = int16_t;
	using Turn = uint8_t;
	using Result = uint8_t;
	using Err = char const*;

	// Game Rules
	uint8_t const RULE_BOARD = 9;    // MAX 13
	Turn const RULE_TURN_LIMIT = 81*2+1;   // 2*board_size + 1(to give last move to white)
	Score2 const RULE_KOMI = 11;   // 5.5 * 2 for white
	uint16_t const RULE_HANDICAP = 0; // handicap placed randomly
	bool const RULE_SUICIDE = true;   // is suicide allowed

	// if set than making the same move 2 times in a row is forbidden
	// problem is this rule will forbid some moves that don't repeat board positions
	// also we have turn limit so loosing player is under pressure to make progress elsewhere
	// more complicated ko rule would need to store and compare with previous boards
	// maybe forbid repeating 3 times in a row??
	bool const RULE_KO_REPEAT = false;

	// amount of score set in case of resign
	// half the size of the board ~ eq of winning with 3/4 board occupied
	Score2 const RULE_RESIGN_SCORE = (RULE_BOARD * RULE_BOARD * 2) / 2;

	uint8_t const MAX_BOARD = 13;
	uint8_t const N = RULE_BOARD;

	// Enums
	Action const ActionNone   = 0;
	Action const ActionPass   = 1;
	Action const ActionResign = 2;


	Ply const PlyNone = 0b00;
	Ply const PlyBlack = 0b01;
	Ply const PlyWhite = 0b10;



	// vec<->action conversions
	inline Vec action_to_vec(Action p) { assert(3<=p); assert(p<N*N+3); return Vec((p-3) % N, (p-3) / N); }
	inline bool is_action_move(Action x) { assert(x < N*N + 3); return x >= 3; }
	inline Action action_move(Vec v) { return (v[0] + v[1] * N) + 3; }

	struct Plane {
		std::array<uint16_t, 13> p;

		uint16_t get(Vec v) const {
			return ((p.at(v[1]) >> v[0]) & 1);
		}

		void set(Vec v) {
			p.at(v[1]) |= (uint16_t(1) << v[0]);
		}

		void unset(Vec v) {
			p.at(v[1]) &= ~(uint16_t(1) << v[0]);
		}

		void clear() {
			for (size_t i = 0; i < p.size(); ++i) {
				p[i] = 0;
			}
		}
	};

	template <class T, uint16_t M>
	struct Stack{
		std::array<T,M> a;
		uint16_t n{0};

		void push(T t) {
			assert(n < M);
			a[n] = t;
			n += 1;
		}

		T pop() {
			assert(n > 0);
			n -= 1;
			return a[n];
		}

		T top() const {
			assert(n > 0);
			return a[n-1];
		}

		uint16_t size() const {
			return n;
		}
	};

	// game state
	struct Board
	{
		// 52
		Plane black;
		Plane white;

		// 12
		Ply player;
		Turn turn;
		Action ah[2];  // last 2 actions taken by players, ah[0] == action(t-1), ah[1] == action(t-2)

		Score2 result;  // game result
		Action action;  // action taken

		char pad[5];

		void reset();
		Ply get(Vec vec) const;
		Ply get_winner() const;

	};


	uint32_t const REP_SIZE = 500000;

	struct Rep
	{
		using Index = uint32_t;

		Index rep_size{0};
		Index rep_next{0};
		Board rep[REP_SIZE];

		Index next(Index i) {
			return (i + 1 == REP_SIZE) ? (0) : (i + 1);
		}

		Index prev(Index i) {
			return (i != 0) ? (i - 1) : (REP_SIZE - 1);
		}

		Board & get(Index i) {
			assert(i < REP_SIZE);
			return rep[i];
		}

		Board & insert() {
			Board & b = rep[rep_next];
			rep_next = next(rep_next);
			if (rep_size < rep_next) {
				rep_size = rep_next;
			}
			return b;
		}
	};




	void load_sfg(Rep & rep, char const* path);

	// force place stone, no game progression, for testing
	void just_place(Board & curr, Vec pos, Ply ply);

	Err move(Board & next, Board & curr, Action pos, Ply ply);

	bool at_liberty(Plane const& self, Plane const& other, Vec seed);
	void remove_group(Plane & self, Vec seed);



}





