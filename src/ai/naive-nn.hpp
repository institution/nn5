#pragma once

#include "ext/ext.hpp"

// go input:
// board is 9x9
// plane is 11x11
// plane 0: on-the-move player stones
// plane 1: other player stones
// plane 2: border (1 if outside board)
// plane 3: ones 

// inputv: 11x11x4 (9x9x4 board padded + 11x11 border)
// inputp: (3x3x4 -> 1) x128
// towerv00: 11x11x128 (9x9 padded)
// towerp00: (3x3x128 -> 1) x128
// towerv01: 11x11x128 (9x9 padded)
// towerp01: (3x3x128 -> 1) x128
// towerv02: 11x11x128 (9x9 padded)
// towerp02: (3x3x128 -> 1) x128
// towerv03: 11x11x128 (9x9 padded)
// towerp03: (3x3x128 -> 1) x128
// towerv04: 11x11x128 (9x9 padded)
// towerp04: (3x3x128 -> 1) x128
// towerv05: 11x11x128 (9x9 padded)
// towerp05: (3x3x128 -> 1) x128
// towerv06: 11x11x128 (9x9 padded)
// towerp06: (3x3x128 -> 1) x128
// towerv07: 11x11x128 (9x9 padded)
// towerp07: (3x3x128 -> 1) x128
// towerv08: 11x11x128 (9x9 padded)
// towerp08: (3x3x128 -> 1) x128
// towerv09: 11x11x128 (9x9 padded)
// towerp09: (3x3x128 -> 1) x128
// towerv10: 11x11x128 (9x9 padded)
// reshape
// fcvA:     11x11x128 flat
// fcpA:     11x11x128 flat -> 1024
// fcvB      1024
// fcpB:     1024 -> 81
// output:   81


#define For(x,X) for (size_t x=0; x<X; ++x)

using Float = float;

template <class T>
size_t kiB(size_t x) {
	return (x * sizeof(T)) >> 10;
}


namespace naive
{
	using namespace ext;

	// each layer manages self params and self output memory
	// net -- combination of layers, manages output arrays memory
	
	// terminology:
	//   net,layer,array -- same interface
	//   model -- wrapper around net, takes problem and applies to net
	
		
	template <size_t N>
	struct Dim {
		size_t dim_[N];
		size_t & operator[](size_t i) { assert(i < N); return dim_[i]; }
		size_t const& operator[](size_t i) const { assert(i < N); return dim_[i]; }	
		size_t size() const { return N; }
		size_t span() const { 
			auto i = N;
			size_t m = 1;
			while(i > 0) {
				--i;
				m *= dim_[i];
			}
			return m;
		}
	};
	
	
	template<size_t N>
	size_t get_pos1(Dim<N> & dim, Dim<N> const& pos) 
	{ 
		size_t pos1 = 0;
		auto i = N;
		size_t m = 1;
		while (i > 0)
		{
			--i;		
			pos1 += pos[i] * m;
			m *= dim[i];
		}
		return pos1;
	}
	
	// array stores dual x and dx 
	template <size_t N>
	struct Array
	{
		Dim<N> dim;
		
		Rng<Float> s;
		Rng<Float> d;
				
		Float & operator[](size_t i) { return s[i]; }
		Float const& operator[](size_t i) const { return s[i]; }
		
		Float & operator[](Dim<N> pos) { return s[get_pos1(dim, pos)]; }
		Float const& operator[](Dim<N> pos) const { return s[get_pos1(dim, pos)]; }
				
		size_t size() const { 
			return s.b - s.a;			
		}
	};
	
	
	template <size_t K, size_t N>
	Array<K> reshape(Array<N> x, Dim<K> dim) 
	{
		Array<K> y = x;
		y.dim = dim;
		
		auto k = x.size();
				
		if (x.size() != k) {
			ext::fail("ERROR: reshape: invalid size\n");
		}
		if (get_array_size(x.dim) != k) {
			ext::fail("ERROR: reshape: invalid size\n");
		}
		if (get_array_size(y.dim) != k) {
			ext::fail("ERROR: reshape: invalid size\n");
		}
		
		return y;
	}
	
	
	
	template <size_t N>
	Float & d(Array<N> & x, Dim<N> const& pos) { return x.d[get_pos1(x.dim, pos)]; }
	
	template <size_t N>
	Float const& d(Array<N> const& x, Dim<N> const& pos) { return x.d[get_pos1(x.dim, pos)]; }
	
	
	struct Mem
	{		
		Float * ptr{nullptr};
		
		Array<1> vs; // temporary values
		Array<1> ps; // parameters
		
		
		
		void malloc(size_t np, size_t nv, size_t N);
		
		Mem() {}
		~Mem() { delete [] ptr; }
		
		void get_used_memory(size_t & np, size_t & nv, size_t N);

		
		void set_unlimited() {
			Float * max = reinterpret_cast<Float*>(1000000000);
			vs.s = Rng<Float>(nullptr,max);
			vs.d = Rng<Float>(nullptr,max);
			ps.s = Rng<Float>(nullptr,max);
			ps.d = Rng<Float>(nullptr,max);	
			vs.dim[0] = 0;
			ps.dim[0] = 0;
		}
		
		/*
		void print() {
			ext::print("MemoryObject\n");
			ext::print("ptr = {}\n", ptr);
			ext::print("vs = {}\n", vs.s);
			ext::print("ps = {}\n", ps.s);
			ext::print("dvs = {}\n", vs.d);
			ext::print("dps = {}\n", ps.v);
		}*/
	};
	
	
	
	void alloc(Rng<Float> & dst, Rng<Float> & src, size_t n) 
	{		
		dst.a = src.a;
		src.a += n;
		if (src.a > src.b) {
			ext::fail("ERROR: range overflow\n");
		}
		dst.b = src.a;		
	}
			
	template <size_t N>
	void alloc(Array<N> & dst, Array<1> & src, Dim<N> const& dim) 
	{
		dst.dim = dim;
		alloc(dst.s, src.s, dim.span());
		alloc(dst.d, src.d, dim.span());
			
	}

	void Mem::get_used_memory(size_t & np, size_t & nv, size_t N)
	{
		nv = (reinterpret_cast<size_t>(vs.s.a)/sizeof(Float)) / N;  //  
		np = reinterpret_cast<size_t>(ps.s.a)/sizeof(Float);  // num of params
	}
	

	void Mem::malloc(size_t np, size_t nv, size_t N)
	{	
		// memory layout:
		// params[np]		
		// dparams[np]
		// values[nv]
		// dvalues[nv]
	
		ptr =  new Float[np*2 + nv*2*N];
	
		Float * p = ptr;
		Float * q = p + np;
		ps.s = Rng<Float>{p, q};
		
		p = q;
		q = p + np;		
		ps.d = Rng<Float>{p, q};
		
		p = q;
		q = p + nv*N;		
		vs.s = Rng<Float>{p, q};
		
		p = q;
		q = p + nv*N;		
		vs.d = Rng<Float>{p, q};
		
	}
	
	
	
	
	struct ReLU 
	{
		// H -> H
		size_t N,H;
		
		Array<2> xs;
		Array<2> ys;
		
		void prop();
		void backprop();
		
		void init(Mem & mem, Array<2> const& xs_) {
			this->xs = xs_;
			N = xs.dim[0];
			H = xs.dim[1];			
			alloc(ys, mem.vs, Dim<2>{N,H});
		}		
	};
	
	void ReLU::prop() 
	{
		For (n, N) {
			For (h, H) {
				auto x = xs[{n,h}];
				ys[{n,h}] = (x > 0) ? x : 0;
			}
		}
	}

	void ReLU::backprop() 
	{
		For (n, N) {
			For (h, H) {			
				auto dx = (xs[{n,h}] > 0) ? 1 : 0;
				d(xs,{n,h}) = d(ys,{n,h}) * dx;
			}
		}
	}
	
	
	struct Linc {
		// fully connected linear combination layer
		// N -> M
	
		size_t N, H0, H1;
		
		Array<2> xs; // N,H0
		Array<2> ps; // H1,H0
		Array<2> ys; // N,H1
				
		void prop();
		void backprop();				
		
		void init(Mem & mem, Array<2> const& xs_, size_t H1_) {
			this->xs = xs_;
			H1 = H1_;
			N = xs.dim[0];
			H0 = xs.dim[1];			
			alloc(ps, mem.ps, Dim<2>{H1,H0});
			alloc(ys, mem.vs, Dim<2>{N,H1});
		}
			
	};
	
	void Linc::prop() 
	{
		// H1 <- H0
		
		For (n, N) {
			For (h1, H1) {
				Float y = 0;
				For (h0, H0) {
					y += ps[{h1,h0}] * xs[{n,h0}];
				}
				ys[{n,h1}] = y;
				//ys(n,j) = dot(ps(j), xs(n));
			}
		}
		//b.vals = (ba.wags * a.vals + ba.bias).array() - Float(2);	
		//set(b.difs, 1);
	}


	void Linc::backprop() 
	{
		// dL/dy[j] == dy[j]
		// dy[j]/dx[i] == p[j,i]
		For (n, N) {
			For (h0, H0) {
				Float dx = 0;
				For (h1, H1) {
					dx += d(ys,{n,h1}) * ps[{h1,h0}];
					
					d(ps,{h1,h0}) = d(ys,{n,h1}) * xs[{n,h0}];
				}
				// dL/dx[i]
				d(xs, {n,h0}) = dx;
			}
		}
			
		//a.difs.array() *= (ba.wags.transpose() * b.difs).array();	
		//ba.dwags.array() *= (b.difs * a.vals.transpose()).array();
		//ba.dbias.array() *= b.difs.array();		
		
	}
	
	
	
	
	struct Conv 
	{
		// filter layer
		
		// input    N  H W C0
		// kernel   C1 3 3 C0 
		// output   N  H W C1
		
		size_t N, H, W, C0, C1;
		
		Array<4> xs; // N H W C0
		Array<4> ps; // C1 3 3 C0
		Array<4> ys; // N H W C1
			
			
		void init(Mem & mem, Array<4> const& xs_, size_t C1_) 
		{
			xs = xs_;
			C1 = C1_;
			
			N = xs.dim[0];
			H = xs.dim[1];
			W = xs.dim[2];
			C0 = xs.dim[3];			
			
			alloc(ps, mem.ps, {C1,3,3,C0});
			alloc(ys, mem.vs, {N,H,W,C1});
		}
		
		
		
		void prop();
		void backprop();
		
		void kermul(size_t n, size_t h0, size_t w0, size_t c1);
	};
	
	void Conv::prop() 
	{
		For(n,N) {				
			// h0,w0 -- left top corner
			For (h0, H - 2) {
				For(w0, W - 2) {
					For (c1, C1) {
						kermul(n,h0,w0,c1);
					}			
				}
			}
			
		}
	}
	
	
	
	void Conv::kermul(size_t n, size_t h0, size_t w0, size_t c1)
	{
		// h0,w0 -- left top corner
		Float y = 0;
		// over kernel
		For (h, H) {
			For (w, W) {
				For (c0, C0) {
					y += ps[{c1, h, w, c0}] * xs[{n,h0+h,w0+w,c0}];
				}
			}
		}			
		ys[{n,h0,w0,c1}] = y;
	
	}
	/*
		
	void Conv::backprop(h0,w0) {		
		y = 0
		for j 0 3
			for i 0 3
				for c 0 C0
					y += kernel(j,i,c) * inn(h0+j, w0+i, c)
							
					//dkernel(j,i,c) =+ inn(h0+j, w0+i, c)       // dy/dk
					
					//dinn(h0+j, w0+i, c) =+ kernel(j,i,c)
					
					
			
		out[h0,w0,c1] = y
		
		//dout[h0,w0,c1] = 1
	}
	
	void backprop_apply_at(h0,w0) {		
		// y = out[h0,w0,c1]
		auto ddy = dout[h0,w0,c1]    // dL/dy
				
		y = 0
		for j 0 3
			for i 0 3
				for c 0 C0
					// y += kernel(j,i,c) * inn(h0+j, w0+i, c)
		
					auto x = inn(h0+j, w0+i, c)
					k = kernel(j,i,c)
					// dy/dk == x					
					// ddk := dL/dk
					auto & ddk = dkernel(j,i,c) 
					ddk += ddy * x
					
					auto & ddx = dinn(h0+j, w0+i, c);
					ddx =+ ddy * k
	
	}
	
	*/
	
	
	
	struct Mse
	{
		size_t N,H;
	
		Array<2> xs0;
		Array<2> xs1;		
		Array<1> ys;
				
		void init(Mem & mem, Array<2> const& xs0_, Array<2> const& xs1_) {
			assert(xs0_.size() == xs1_.size());
			N = xs0_.dim[0];
			H = xs0_.dim[1];
						
			xs0 = xs0_;
			xs1 = xs1_;
			alloc(ys, mem.vs, {N});
		}	
		
		void prop();
		void backprop()	;
	};
	
	void Mse::prop() 
	{
		Float y = 0;		
		For(i, N) {
			auto x = xs0[i] - xs1[i];		
			y += x*x;
		}
		ys[0] = y;
	}
	
	void Mse::backprop() 
	{	
		For (n, N) {
			auto dy = d(ys,{n});
			assert(dy == 1);   // usually :)
			For (h, H) {
				auto x = xs0[{n,h}] - xs1[{n,h}];
				d(xs0,{n,h}) = dy * 2*x * (+1);
				d(xs1,{n,h}) = dy * 2*x * (-1);
			}
		}	
	}

	template <class T>
	void randomize(T & ts, Random & rand)
	{
		For (i, ts.size()) { ts[i] = rand.uniform_f(-1, 1); }
	}

	
	// Array
	// Layer = Array + input & output reference
	// Layer = multiple layers combined
	// Net = Layer + memory managment

	struct LinearNet
	{
		size_t N, H0, H1;
		
		Array<2> xs;   // H0
		Array<2> pattern; // H1
		Linc output; // H1
		Mse mse;
		
		// also manage your own memory	
		Mem mem;
		
		void alloc() {
			// mse <- M <- N
			alloc(xs, mem.vs, {N,H0});
			alloc(pattern, mem.vs, {N,H1});
			output.init(mem, xs, H1);
			mse.init(mem, output.ys, pattern);
		}
		
		void init(size_t N_, size_t H0_, size_t H1_) 
		{
			N = N_;
			H0 = H0_;
			H1 = H1_;
		
			// calc used mem
			mem.set_unlimited();
			this->alloc(mem);			
			size_t np, nv;
			mem.get_used_memory(np, nv, N);
			
			// alloc mem
			mem.malloc(np, nv, BATCH_SIZE);
			this->alloc(mem);
		
			if (1) {
				print("INFO:  LinearNet\n");
				print("INFO: batch size: {}\n", net.N);
				print("INFO: input size: {}\n", net.H0);
				print("INFO: output size: {}\n", net.H1);
				print("INFO: number of parameters: {}\n", np);
				print("INFO: store memory: {} kiB\n", kiB<Float>(np) );
				print("INFO: operational memory x 1: {} kiB\n", kiB<Float>(np * 2 + nv * 2             ) );
				print("INFO: operational memory x N: {} kiB\n", kiB<Float>(np * 2 + nv * 2 * BATCH_SIZE) );
				print("\n");
			}
		}

		
		Array<1> & param() { return mem.ps.s; }	
		Array<2> & input() { return xs; }		
		Array<2> & pattern() { return pattern; }		
		Array<2> const& output() const { return output.ys; }
		Array<1> const& error() const { return mse.ys; }
		
		
		void prop() {
			// clear ?
			output.prop();
			mse.prop();
		}
		
		void backprop() {			
			mse.backprop();
			output.backprop();			
		}
		
		
		
	};
	
	

	template <class T>
	void randomize(T & x, Random & rand, Float a, Float b) 
	{
		For (i, x.size())
		{
			x[i] = rand.uniform_f(a, b);
		}	
	}

	
	template <class T>
	void measure_net(T & net) {
		
		
	
	}
	
	
	void net_run() {
	
		
		
		/*
		prop();
		d(mse.ys,0) = 1;
		backprop();			
		*/
	}
	
	void net_learn() {
	
	}
	
	void net_init() {
	
	}
	
	
	struct Env
	{
		ext::Random rand;
		
	};
	
	
	
	
		

} // ns


