// train net to predict go moves from gnugo dataset
// compare to baseline models
// baseline models:
//  random: choose random position from empty squares
//  linear: 1-layer linear model 81*4 -> 81, choose argmax
//  logreg: logistic regression
//  k-nearest: choose randomly from k-nearest
//  MCplayoutEval: playout each move n-times, choose best


#include <unistd.h>
#include <iostream>
#include <fstream>
#include <array>

#include "ext/ext.hpp"
#include "naive-nn.hpp"
#include "../go/sfg.hpp"


using namespace ext;

using Float = float;

// go input:
// board is max 9x9
// plane is 11x11
// plane 0: on-the-move player stones
// plane 1: other player stones
// plane 2: border (1 if outside board)
// plane 3: ones


int const BATCH_SIZE = 32;

template <class T>
size_t kiB(size_t x) {
	return (x * sizeof(T)) >> 10;
}

int main(int argc, char * argv[])
{
	ext::set_relative_working_dir("../..");

	using namespace naive;


	// 1. Load data
	{

		//INFO: num of moves: 597637
		//INFO: num of games: 14250

		size_t const REP_SIZE = 597637;
		ext::darray1<Go::Board, size_t> rep;
		rep.resize(REP_SIZE);
		size_t rep_pos = 0;


		Go::SFG sfg;
		Go::Board game;
		char const* fpat = "dataset/gnugo-v1/{}.sfg";

		// num of positions, num of games
		Float npos = 0;
		Float ngames = 0;

		for (size_t i = 0; i < 14250; ++i)
		{
			FString<7> num;
			DString fname(PATH_MAX);
			auto r = num.range();
			print1(r, i, 6, 0, '0');
			auto rr = fname.range();
			print(rr, fpat, num);

			print("DEBUG: {}\n", fname.c_str());

			Go::Score2 result = 0;
			
			
			sfg.open(fname.c_str());
			sfg.read_header(&result);			
			
			Go::Board * curr;
			Go::Board * next;

			curr = &rep.at(rep_pos);
			curr->reset();
			curr->result = result;;

			Go::Action act;
			Go::Ply ply;
			char const* com;
			
			while (sfg.has_command()) 
			{
				sfg.read_move(&act, &ply, &com);


				next = &rep.at(rep_pos);
				next->result = result;

				npos += 1;
			}
			ngames += 1;
			
			sfg.close();
		}
		print("INFO: num of moves: {}\n", npos);
		print("INFO: num of games: {}\n", ngames);

	}


	// 2. Init model

	Random rand;
	Mem mem;
	LinearNet net;

	// calc needed memory
	mem.set_unlimited();
	net.init(mem, BATCH_SIZE, 81*4, 81);
	size_t np, nv;
	mem.get_used_memory(np, nv, BATCH_SIZE);

	print(" LinearNet\n");
	print("batch size: {}\n", net.N);
	print("input size: {}\n", net.H0);
	print("output size: {}\n", net.H1);
	print("number of parameters: {}\n", np);
	print("store memory: {} kiB\n", kiB<Float>(np) );
	print("operational memory x 1: {} kiB\n", kiB<Float>(np * 2 + nv * 2             ) );
	print("operational memory x N: {} kiB\n", kiB<Float>(np * 2 + nv * 2 * BATCH_SIZE) );
	print("\n");

	// alloc mem
	mem.malloc(np, nv, BATCH_SIZE);
	net.init(mem, BATCH_SIZE, 81*4, 81);

	// random init
	{
		auto & x = mem.ps.s;
		For (i, x.size())
		{
			x[i] = rand.uniform_f(-1, 1);
		}
	}



	// 4. Predict and calc accuracy
	//auto & inn = net.get_input();
	//inn[{n,i}]


	return 0;
}
