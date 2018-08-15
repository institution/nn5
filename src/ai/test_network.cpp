#define CATCH_CONFIG_MAIN
#include "../catch/catch.hpp"

#include "naive-nn.hpp"

// small value to disturb parameters
Float const EPS = 0.000001;


template <class Net>
Float stochastic_dparam(Net & nn, Float & para, Float & dpara) 
{
	// nn -- network, must be ready to run
	// return -- err(para + eps)  - err(para - eps) / 2eps
		
	auto orig_para = para;
	
	para = orig_para - EPS;
	nn.prop();
	auto e0 = nn.get_error();
			
	para = orig_para + EPS;
	nn.prop();
	auto e1 = nn.get_error();
	para = orig_para;
		
	return (e1 - e0) / (2.0 * EPS);
}

TEST_CASE("linear_layer_gradient", "") 
{
	// 1 init linear layer params to random
	// 2 use net to calc gradient for random input
	// 3 calc gradient by disturbing input
	// 4 compare gradients
	
	Random rand;	
	LinearNet net;	
	net.init(1, 4, 2);	// batch, input, output
	randomize(net.param(), rand);

	auto & inn = net.input();
	
	
	inn[{0,i}]
	
	
	
}



VectorXf get_random_label(int32_t M, bool normalized)
{
	VectorXf lab(M);
	if (normalized)
	{
		// normalized (sum to one) label; required by softmax loss
		randomize(lab, 0, 1);
		//print("lab1 = %||\n", lab);
		lab /= lab.sum();		
		//print("lab2 = %||\n", lab);
	}
	else {
		// random label
		randomize(lab,-1,1);
	}
	return lab;
}


template <class Net>
Float analytical_gradient_on(Net & nn, Float & para, Float & dpara) 
{	
	nn.run_one(1,false,2);
	return dpara;		
}


	


TEST_CASE( "DatasetXOR", "" ) 
{
	DatasetXOR ds;
	VectorXf z(1);
	ds.unpack_label(z, 0);
	REQUIRE(z[0] == 0.1);
	ds.unpack_label(z, 1);
	REQUIRE(z[0] == 0.9);
	ds.unpack_label(z, 2);
	REQUIRE(z[0] == 0.9);
	ds.unpack_label(z, 3);
	REQUIRE(z[0] == 0.1);

}



TEST_CASE( "Wags", "" ) 
{
	Wags w;
	VectorXf par(2), dpar(2);
	w.assign(par.data(), dpar.data(), 1,1);
	
	par(0) = 0;
	par(1) = 1;
	REQUIRE(w.wags(0) == 0);
	REQUIRE(w.bias(0) == 1);
	w.wags(0) = 2;
	w.bias(0) = 3;
	REQUIRE(par(0) == 2);
	REQUIRE(par(1) == 3);
	
	dpar(0) = 0;
	dpar(1) = 1;
	REQUIRE(w.dwags(0) == 0);
	REQUIRE(w.dbias(0) == 1);
	w.dwags(0) = 2;
	w.dbias(0) = 3;
	REQUIRE(dpar(0) == 2);
	REQUIRE(dpar(1) == 3);
	
	
}

TEST_CASE( "test_net1_multiple_runs_same_result", "" ) 
{
	
	auto N = 1;

	Net1 nn;
	nn.resize(1,1,1);
	nn.randomize(-1,1);
	
	
	VectorXf inn;
	inn.resize(N);
	randomize(inn,-1,1);
	
	nn.get_input() = inn;
	nn.get_label() = get_random_label(1, 1);
	
	
	nn.run_one(1,false,2);
		
	auto out0 = nn.get_output();
	auto lay0dw0 = nn.yx[0].dwags;
	auto lay0db0 = nn.yx[0].dbias;
	auto lay1dw0 = nn.yx[1].dwags;
	auto lay1db0 = nn.yx[1].dbias;
	
	nn.run_one(1,false,2);
	
	
	auto out1 = nn.get_output();
	auto lay0dw1 = nn.yx[0].dwags;
	auto lay0db1 = nn.yx[0].dbias;
	auto lay1dw1 = nn.yx[1].dwags;
	auto lay1db1 = nn.yx[1].dbias;
	
	REQUIRE(out0 == out1);
	REQUIRE(lay0dw0 == lay0dw1);
	REQUIRE(lay0db0 == lay0db1);
	REQUIRE(lay1dw0 == lay1dw1);
	REQUIRE(lay1db0 == lay1db1);
	
	
		
}



TEST_CASE( "test_net1_gradient", "" ) 
{
	bool const v = 0; // verbose
	
	Float tolerance = 0.001;
	
	auto N = 10;
	auto H = 5;
	auto M = 10;

	Net1 nn;
	
	
	nn.resize(N,H,M);
	
	VectorXf inn, lab;
	inn.resize(N);	
	lab.resize(M);
	
	
	for (int32_t k = 0; k < 100; ++k)
	{
		
		nn.randomize(-1,1);
		
		randomize(inn,-1,1);
		lab = get_random_label(M, 1);	
		
		
		/*inn[0] = 0.2;
		lab[0] = 0.3;
		nn.yx[0].wags(0,0) = 0.5;
		nn.yx[0].bias[0] = 0.1;
		nn.yx[1].wags(0,0) = 0.8;
		nn.yx[1].bias[0] = -0.2;
		*/
		
		nn.get_input() = inn;
		nn.get_label() = lab;
		
		if (v) print("Input: %||\n", inn);
		if (v) print("Label: %||\n", lab);
		nn.run_one(1,v,2);
		if (v) print("Output: %||\n", nn.get_output());
		
		
		for (int j=0; j < nn.yx[0].wags.rows(); ++j) {
			for (int i=0; i < nn.yx[0].wags.cols(); ++i) {
				
				auto & p = nn.yx[0].wags(j,i);
				auto & dp = nn.yx[0].dwags(j,i);
					
				
				auto sg = stochastic_gradient_on(nn,p,dp);
				auto ag = analytical_gradient_on(nn,p,dp);
					
				if (v) print("Layer 0 -> 1; Parameter [%||,%||]; true_grad = %||; calc_grad = %||\n", j, i, sg, ag);
				
				CHECK(sg == Approx(ag).epsilon(tolerance));
			}
		}
		
	
	}
		
}



