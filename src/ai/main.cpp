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



using Boards = ext::darray1<Go::Board, size_t>;


void load_gnugo_dataset(Boards & boards) 
{
	size_t const DATASET_BOARDS = 611887;
	                            //597637
	size_t const DATASET_GAMES = 14250;

	//INFO: num of moves: 597637
	//INFO: num of games: 14250

	size_t nmoves_read = 0;

	boards.resize(DATASET_BOARDS);
	
	Go::SFG sfg;
		
	DString fname(255);

	// j - board
	// i - game	
	size_t j = 0;
	size_t i = 0;
	for (; i < DATASET_GAMES; ++i)
	{
		// number of moves in this game
		size_t nmoves = 0;
		
		print(fname.range(), "dataset/gnugo-v1/{}.sfg", Num(i,6,0,'0'));		
		//print("DEBUG: {}\n", fname.c_str());

		Go::Score2 result = 0;		
		
		sfg.open(fname.c_str());
		sfg.read_header(&result);			
		
		Go::Board * curr;
		Go::Board * next;

		curr = &boards.at(j);
		
		// new game
		curr->reset();
		curr->result = result;;

		Go::Action act;
		Go::Ply ply;
		char const* com;
		
		j += 1;
		while (sfg.has_command()) 
		{
			sfg.read_move(&act, &ply, &com);
			nmoves_read += 1;
			
			next = &boards.at(j);
			
			Go::move(*next, *curr, act, ply);
			next->result = result; // overwrite result
			
			curr->action = act;
			
			curr = next;			
			j += 1;
		}
		
		sfg.close();
	}
		
	print("INFO: number of boards loaded: {}\n", j);
	print("INFO: number of games loaded: {}\n", i);
	print("INFO: number of moves read: {}\n", nmoves_read);
	
}

/*
idea:
	optimize NN with respect to some value function L
	than optimize with respect to N=sum(abs(ps)) treating L as a constraint
	
	so
	dL/dps is best gradient, but we can use other gradients as in RMS for ex
	choose such gradient that also minimize N
	
	
	ex. new RMS
		p > 0 and dp < 0 => -1
		p > 0 and dp > 0 => +1/100
		p < 0 and dp < 0 => -1/100
		p < 0 and dp > 0 => +1
		
		maybe also bigger rise faster so: delta = 1/100 + p*0.1
		tzn. prefer gradients that will drive parameters to zero
	
	comparsion normalizing term
		normalizing term ie adding sum(ps**2) to L will drive ALL params towards zero - it is fighting with grad(L)
		new RMS will prefer zeroing params over increasing them but WONT fight with grad(L)
	
	original RMS for comparsion:
		dp < 0 => -1
		dp > 0 => +1
	
	this idea behaves kindof like lexicographical evaluation function in evo.alg ie. L = (a,b,c) and L1 > L2 defined by lexographical order
	
	
	frezze 10% of params each time making an update?
	
	
	---
	grad is 0
	net is:
		x0 * p0 + x0 * p1 -> y0
		y0 - 1 -> eee
		requirement x0 == y0
		p0,p1 = 0.1, 0.9
		error is zero grad is zero
		how to move this network to p0,p1 = (0, 1) ???
		maybe add sum(sqrt(params)) -> one big param is cheaper than 2 small ones
		let K = sum(sqrt(params))
		
		calc gradK under constraint gradL == 0
		
		d(sqrt(x))/dx = 1/(2*sqrt(x))
		d(p0,p1) = (0.32, 0.94)
		
		
		gradL = 0
		gradK != 0
		
		
		gradL is 0
		
		
		while learnig calc gradL and gradK and what?
		
		
		
	
	
	
*/

/*
template <class T>
struct Net
{
	T net;
	naiveMem mem;
	Random rand;

	void make_net(T & net, Mem & mem)
};*/



int main(int argc, char * argv[])
{
	ext::set_relative_working_dir("../..");

	using namespace naive;


	// 1. Load data
	Boards rep;
	load_gnugo_dataset(rep);


	// 2. Init model
	Random rand;	
	LincNet net;
	net.init(BATCH_SIZE, 81*4, 81);	
	randomize(net.par().v, rand);

	
	// 4. Predict and calc accuracy
	//auto & inn = net.get_input();
	//inn[{n,i}]


	return 0;
}
