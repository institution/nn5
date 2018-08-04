#define CATCH_CONFIG_MAIN
#include "../catch/catch.hpp"

#include "ext.hpp"

TEST_CASE( "darrayN", "" ) {
	
	SECTION("memory-layout") {
		
		ext::darrayN<int16_t, int32_t, 3> arr;
		
		arr.resize({3,2,2}, 0);
		
		REQUIRE(arr.size() == 12);
		
		for (size_t i = 0; i < arr.size(); ++i)
		{
			arr.at(i) = i;
		}
		
		REQUIRE(arr({0,0,0}) == 0);
		REQUIRE(arr({1,0,0}) == 1);
		REQUIRE(arr({2,0,0}) == 2);
		
		REQUIRE(arr({0,1,0}) == 3);
		REQUIRE(arr({1,1,0}) == 4);
		REQUIRE(arr({2,1,0}) == 5);
		
		
		REQUIRE(arr({0,0,1}) == 6);
		REQUIRE(arr({1,0,1}) == 7);
		REQUIRE(arr({2,0,1}) == 8);
		
		REQUIRE(arr({0,1,1}) == 9);
		REQUIRE(arr({1,1,1}) == 10);
		REQUIRE(arr({2,1,1}) == 11);
	}
	
	/*SECTION("int16") {
		std::stringstream s;

		int16_t x = 0x4063;
		write(s, x);
		int16_t y;
		read(s, y);
		
		REQUIRE(x == y);		
	}*/
}
