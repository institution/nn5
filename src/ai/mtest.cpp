#include <unistd.h>


#include <iostream>
#include <fstream>
#include <array>
#include <eigen3/Eigen/Dense>
#include "../ext/ext.hpp"
#include "../ext/randm.hpp"




using Float = float;

using Eigen::Map;
using VectorXf = Eigen::VectorXf;
using MatrixXf = Eigen::MatrixXf;
using ArrayXf = Eigen::ArrayXf;

//template <class T>

#include "ext/ext.hpp"

using namespace ext;

/*
x,y,z - input



Node_2to1
	// x,y -> z; z unique for this link
	
	value z
	deriv dz/dy dz/dx
	
	calculate(Index x,y)
		input x,y
		output f(x,y)   hidden g(x,y)
		deriv dfdx dfdy
		


8x8	


memory

param 
input 
hidden 
output




node:
	ptr[X,Y,Z] + slice
	
	ptr 
		X, incX
		Y, incY
		Z, incZ
		
		
		
input [X,Y,Z]	

conv




neuron N->1

inn[N]: INN
val[i]
out[1]: OUT,VAL
wag[N]: PAR
bia[1]: PAR


for i in N
	val1[i] = inn[i] * wag[i]

for i in N
	val2[0] += val[i]
	
val3[0] = relu(val2[0])




for j in NUM_NEURON_HIDDEN_1
	row_wag = wag[j]
	row_val1 = val1[j]
	for i in N
		val1[i] = inn[i] * row_wag[i]

for j in NUM_NEURON_HIDDEN_1
	val2[j,0] = 0
	for i in N
		val2[j,0] += val[j,i]
	
for j in NUM_NEURON_HIDDEN_1
	val3[j,0] = relu(val2[j,0])
 


fc layer N->M memory
N + 2*N*M + 2*M

fc layer N->M opers
3 N M + M

*/




// go input:
// board is max 9x9
// plane is 11x11
// plane 0: on-the-move player stones
// plane 1: other player stones
// plane 2: border (1 if outside board)

// B - batch size
// input: 11x11x3 (9x9x2 board padded + 11x11 border)
// conv_f: (3x3x3 -> 1) x128
// tower0:  11x11x128 (9x9 padded)
// ker0: (3x3x128 -> 1) x128
// tower1:  11x11x128
// ker[1-8]: (3x3x128 -> 1) x128
//        11x11x128
// ker[9]: (3x3x128 -> 1) x128
//        11x11x128
// fc: 9x9x128 -> 1024
//        1024
// fc: 1024 -> 1
//        1


// s = 2*r+1
// conv() 
// W*H*D -> W*H*D*s

// deconv() 
// W*H*D*s -> W*H*D


// B - batch size
// input: 11x11x3 (9x9x2 board padded + 11x11 border)
// conv_f: (3x3x3 -> 1) x128
// tower0:  11x11x128 (9x9 padded)
// ker0: (3x3x128 -> 1) x128
// tower1:  11x11x128
// ker[1-8]: (3x3x128 -> 1) x128
//        11x11x128
// ker[9]: (3x3x128 -> 1) x128
//        11x11x128
// fc: 9x9x128 -> 1024
//        1024
// fc: 1024 -> 1
//        1




			
			
				






int main(int argc, char * argv[])
{
	chdir("/home/sakar/workspace/nn5-go");
	






	
	using Float = int8_t;
	
	size_t N = 1024;
	size_t M = 1024;
	size_t TEST_N = 1000;
	
	std::vector<Float> v;
	v.resize(N + N*M + N*M);
	Float * p = &v[0];
	
	for (size_t i = 0; i < v.size(); ++i)
	{
		v[i] = (int8_t)ext::uniform_f(-1, +1);
	}	
	
	if (0) {	/*	
		Map<VectorXf> inn{p, N};
		Map<MatrixXf> wag{p+N, M, N};
		Map<MatrixXf> val{p+N+M*N, M, N};
				
		for (size_t b = 0; b < TEST_N; ++b)
		{
		
			for (size_t i = 0; i < M; ++i)
			{
				val.col(i).array() = wag.col(i).array() * inn.array();
			}
		}	*/
	}
	
	if (1) {
		Float * inn = p;
		Float * wag = p + N;
		Float * val = p+N+M*N;
	
		for (size_t b = 0; b < TEST_N; ++b)
		{	
			Float * row_wag = wag;
			Float * row_val = val;
			for (size_t j = 0; j < M; ++j)
			{				
				for (size_t i = 0; i < N; i += 4)
				{
					row_val[i] = row_wag[i] * inn[i];	
					row_val[i+1] = row_wag[i+1] * inn[i+1];	
					row_val[i+2] = row_wag[i+2] * inn[i+2];	
					row_val[i+3] = row_wag[i+3] * inn[i+3];					
				}
				row_wag += N;
				row_val += N;
			}
		}	
	}

	


	return 0;
}
