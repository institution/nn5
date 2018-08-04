#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include "ext/ext.hpp"

#include "go.hpp"
#include "sfg-parser.hpp"

char const* WDIR = "/home/sakar/workspace/nn5-go";

using namespace Go;

TEST_CASE( "Board", "" ) {

	Board a;
	Board b;
	a.reset();

	SECTION("2 x pass -> end game & calc score") {
		Board g;
		g.reset();
		move(g, g, ActionPass, PlyBlack);
		move(g, g, ActionPass, PlyWhite);
		REQUIRE(g.player == PlyNone);   // regress
		REQUIRE(g.result == RULE_KOMI); // regress
	}

	SECTION("mini game") {
		REQUIRE(a.player == PlyBlack);

		REQUIRE(move(b, a, action_move({1,0}), PlyBlack) == nullptr);

		REQUIRE(a.action == action_move({1,0}));
		REQUIRE(b.player == PlyWhite);

		REQUIRE(move(a, b, action_move({0,0}), PlyWhite) == nullptr);

		REQUIRE(a.ah[1] == action_move({1,0}));
		REQUIRE(a.ah[0] == action_move({0,0}));
		REQUIRE(b.action == action_move({0,0}));
		REQUIRE(a.player == PlyBlack);

		REQUIRE(move(b, a, action_move({0,1}), PlyBlack) == nullptr);

		REQUIRE(b.player == PlyWhite);
		REQUIRE(b.get({0,0}) == PlyNone);
		REQUIRE(b.get({1,0}) == PlyBlack);
		REQUIRE(b.get({0,1}) == PlyBlack);
		REQUIRE(b.turn == 3);

		// sucide
		REQUIRE(move(a, b, action_move({0,0}), PlyWhite) == nullptr);

		REQUIRE(a.get({0,0}) == PlyNone);
		REQUIRE(a.turn == 4);

	}
}


TEST_CASE( "at_liberty", "" ) {
	Board b;
	just_place(b,{1,0},PlyBlack);
	just_place(b,{0,0},PlyWhite);
	just_place(b,{1,1},PlyBlack);

	REQUIRE(at_liberty(b.black, b.white, {1,0}) == 1);
	REQUIRE(at_liberty(b.black, b.white, {1,1}) == 1);
	REQUIRE(at_liberty(b.white, b.black, {0,0}) == 1);
}



TEST_CASE( "remove_group of 2 stones", "" ) {
	Board b;
	just_place(b,{0,0},PlyWhite);
	just_place(b,{1,0},PlyWhite);

	just_place(b,{0,1},PlyBlack);
	just_place(b,{1,1},PlyBlack);

	just_place(b,{2,0},PlyBlack);

	REQUIRE(at_liberty(b.white, b.black, {1,0}) == 0);

	remove_group(b.white, {1,0});

	REQUIRE(b.white.get({1,0}) == 0);
	REQUIRE(b.white.get({0,0}) == 0);



}

TEST_CASE( "SFGParser", "" ) {
	/*
	chdir(WDIR);

	SECTION("read something") {

		SFGParser f;
		f.open("test/a.sfg");

		char const* p = f.read();
		while (p != nullptr) {
			//strcmp(p, )
			p = f.read();
		}

		REQUIRE(1 == 1);

	}
	*/
}
