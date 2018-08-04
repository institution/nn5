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


template <class T>
inline void randomize(T & x, Float a, Float b) 
{
	for (int32_t i = 0; i < x.size(); ++i)
	{
		x(i) = ext::uniform_f(a, b);
	}	
}






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


6
6
6


3x3x128
6x128

(3x3x128 -> 1) x128


3x3x32 x32x4

3x3x64

128





000 black
010 white
100 border
111 ones

feat: white near border    
000
010 -> 1
101 -> 1
 
 0
 1
 1 -> 2
 
 ReLu_layer
 
 bias_layer - no bias, add full 1 layer to input
  x -> x + b
 





using Rngf = Rng<float>

deconv()

conv(out, r, inn, W, H, C, N)
	assert out.size() == W*H*C*N*((2r+1)**2)
	
	K = 2*r + 1
	
	for j K
		for i K
			inn



conv(W-2, H-2)


fw_conv_layer(input)
	// K -- conv features
	// B -- batch size
	//
	
    //H: height (spatial dimension)
    //W: width (spatial dimension)
    //C: channel (depth)
    //N: batch

	
	
	for Kf
		for w,h in 1:W-1, 1:H-1
			for b
				output_conv(w,h,f)[b] = AF(sum(conv_ker[f] vmul input_conv(w,h)[b]))
				output[w:w+Kw, h:h+Kh, 0:F][b] = conv_ker[0:Kw,0:Kh,0:D][f] vmul input[w:w+Kw, h:h+Kh, 0:D][b]
				
				
				
backprop
	
	F(x) = y	
	
	
	G F x
	
	dG/dF * dF/dx 
	
	dF/dx_i
	dG/df_j
	
	
	dG(F(x))/d(F(x))
	
	
	dG/dF
	
	dF/dx
	
// test on 1000000 network

//print("{}\n", sizeof(float));



void 

convolute_layer(W-2, H-2)
	// input   C0*W*H*N
	// kernel  C0*Kw*Kh*C1
	// output  C1*W*H*N
	
	
	// h0,w0 -- top-left corner
	
	Float * p = inn;
	Float * o = out;
	
	for (h0 = 0; h0 < H-2; ++h0) {
		
		for (w0 = 1; w0 < W - 2; ++w0) {
		
			assert(p == h0 * W*C + w0 * C)
		
			auto p0 = p;
			auto p1 = p0 + W*C;
			auto p2 = p1 + W*C;
		
			write(o, p0, 3*C);			
			write(o, p1, 3*C);
			write(o, p2, 3*C);
						
			p += C;
		}
		p += W*C;
	}

}		
				
			
			
				






int main(int argc, char * argv[])
{
	chdir("/home/sakar/workspace/mul-mat-speed");
	






	
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
