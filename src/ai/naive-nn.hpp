#pragma once
#include <type_traits>
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
	
		
	/// Store N-dimensional position or area
	template <size_t N>
	struct Dim 
	{
		size_t dim_[N];
		
		// linear access
		size_t & operator[](size_t i) { assert(i < N); return dim_[i]; }
		size_t const& operator[](size_t i) const { assert(i < N); return dim_[i]; }	
		
		// dimensions
		size_t size() const { return N; }
		size_t span() const;
			
		// constructing
		template <class ... Args,
			class = std::enable_if_t<(sizeof...(Args) == N)>,
			class = std::enable_if_t<(... && std::is_convertible_v<Args, size_t>)>
		>
		Dim(Args ... args): dim_{args ...} {}
		Dim() = default;
	};
	
	/// Return linear size of adressed space for this dim
	template <size_t N>
	size_t Dim<N>::span() const 
	{ 
		auto i = N;
		size_t m = 1;
		while (i > 0) {
			--i;
			m *= dim_[i];
		}
		return m;
	}
	
	/// Return linear(1) position of posN in dimN
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
	
	template <size_t N>
	struct Mat
	{
		Rng<Float> rng;
		Dim<N> dim;		
		
		Mat() = default;		
		Mat(Rng<Float> const& rng, Dim<N> const& dim): rng(rng), dim(dim) {}
				
		void set(Float * a, Float * b) 
		{
			static_assert(N == 1);
			rng.a = a;
			rng.b = b;
			dim[0] = b - a;
		}
		
		void set(Rng<Float> const& src) 
		{
			static_assert(N == 1);
			rng.a = src.a;
			rng.b = src.b;
			dim[0] = src.b - src.a;
		}
		
		
		// linear
		size_t size() const { return rng.size(); }
		
		Float & operator[](size_t i) { return rng[i]; }
		Float const& operator[](size_t i) const { return rng[i]; }
	
	
		// layer
		Mat<N> & inn() { return *this; }
		Mat<N> & out() { return *this; }
		
		template <size_t M>
		Mat<M> reshaped(Dim<M> const& ndim) {
			if (ndim.span() != size()) {
				ext::fail("ERROR: reshaped: invalid dim\n");
			}
			return Mat<M>(rng, ndim);
		}
		
		// n-dim access
		template <class ... Args,
			class = std::enable_if_t<(sizeof...(Args) == N)>,
			class = std::enable_if_t<(... && std::is_convertible_v<Args, size_t>)>
		>
		Float & operator()(Args ... args) { return rng[get_pos1(dim, {args...})]; }
		
		template <class ... Args,
			class = std::enable_if_t<(sizeof...(Args) == N)>,
			class = std::enable_if_t<(... && std::is_convertible_v<Args, size_t>)>
		>
		Float const& operator()(Args ... args) const{ return rng[get_pos1(dim, {args...})]; }
		
		// n-dim access
		Float & operator()(Dim<N> const& pos) { return rng[get_pos1(dim, pos)]; }
		Float const& operator()(Dim<N> const& pos) const { return rng[get_pos1(dim, pos)]; }
	};
	
	/// N-dim memory view of dual mem areas
	template <size_t N>
	struct DualMat
	{
		Mat<N> v;
		Mat<N> d;
		
		Mat<N> & sole() { return v; }
		Mat<N> & dual() { return d; }

		DualMat<N> & inn() { return *this; }
		DualMat<N> & out() { return *this; }
		
		Dim<N> & dim() { return v.dim; }
		Dim<N> const& dim() const { return v.dim; }
		size_t dim(unsigned i) const { return v.dim[i]; }
		
		size_t size() const { return v.size(); }
		
		/*void set(Dim<N> dim_, Rng<Float> lin_, Rng<Float> dlin_) {
			dim = dim_;
			lin = lin_;
			dlin = dlin_;
		}*/
		
	};
	
	
	template <size_t K, size_t N>
	DualMat<K> reshape(DualMat<N> const& src, Dim<K> const& dim) 
	{
		if (dim.span() != src.size()) {
			ext::fail("ERROR: reshape: invalid dim\n");
		}
		
		// same memory, diffrent dim
		DualMat<K> dst = src;
		dst.dim = dim;
		return dst;
	}
	
	
	
	struct DualBuf
	{
		Buf<Float> a;
		Buf<Float> d;		
	};
	
	/*
	struct MemBuf: Buf
	{
		void reserve()
		Mem()
		~MemBuf()
	};*/
	
	struct Mem
	{		
		Float * ptr{nullptr};
		
		DualMat<1> free_param;
		DualMat<1> free_value;		
		DualMat<1> used_param;
		DualMat<1> used_value;
		
		
		Mem() {}
		~Mem() {
			print("INFO: releasing memory\n");
			delete [] ptr;
		}
	
		DualMat<1> & par() { return used_param; }
		DualMat<1> & val() { return used_value; }		
		DualMat<1> const& par() const { return used_param; }
		DualMat<1> const& val() const { return used_value; }
		
		void malloc(size_t np, size_t nv, size_t N);		
		
		Rng<Float> move_a(Mat<1> & m, size_t k) {
			Rng<Float> r;
			r.a = m.rng.a;
			m.rng.a += k;
			if (m.rng.a > m.rng.b) {
				ext::fail("ERROR: buffer overflow\n");
			}
			m.dim[0] -= k;
			r.b = m.rng.a;
			return r;
		}
		Rng<Float> move_b(Mat<1> & m, size_t k) {
			Rng<Float> r;
			r.a = m.rng.b;
			m.rng.b += k;			
			m.dim[0] += k;
			r.b = m.rng.b;
			return r;
		}
		
		
		template <size_t N>
		void assign_param(DualMat<N> & m, Dim<N> const& d) 
		{			
			//assert(m.v.rng.size() == 0);
			//assert(m.v.dim.span() == 0);
		
			//assert(m.d.rng.size() == 0);
			//assert(m.d.dim.span() == 0);
			
			auto span = d.span();
			
			m.v.rng = move_a(free_param.v, span);
			m.v.dim = d;
			move_b(used_param.v, span);
			
			m.d.rng = move_a(free_param.d, span);
			m.d.dim = d;
			move_b(used_param.d, span);
		}
		
		template <size_t N>
		void assign_value(DualMat<N> & m, Dim<N> const& d) 
		{
			//assert(m.v.rng.size() == 0);
			//assert(m.v.dim.span() == 0);
		
			//assert(m.d.rng.size() == 0);
			//assert(m.d.dim.span() == 0);
			
			auto span = d.span();
			
			m.v.rng = move_a(free_value.v, span);
			m.v.dim = d;
			move_b(used_value.v, span);
			
			m.d.rng = move_a(free_value.d, span);
			m.d.dim = d;
			move_b(used_value.d, span);			
		}
		
		
		
		
		void get_used_memory(size_t & np, size_t & nv, size_t N);
		
		void fake_malloc()
		{
			Float * max = reinterpret_cast<Float*>(1000000000);
			Float * min = reinterpret_cast<Float*>(0);
			 				
			free_param.v.set(min,max);
			free_param.d.set(min,max);
			free_value.v.set(min,max);
			free_value.d.set(min,max);
			
			used_param.v.set(free_param.v.rng.a, free_param.v.rng.a);
			used_param.d.set(free_param.d.rng.a, free_param.d.rng.a);
			used_value.v.set(free_value.v.rng.a, free_value.v.rng.a);
			used_value.d.set(free_value.d.rng.a, free_value.d.rng.a);	
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
	
	
	
	
			
	

	
	// Dual
	//  assign
	//  a/d
	//  reshape
	//  inn == out == self
			
	// Layer	
	//  init
	//  prop/backprop
	//  inn/out/...
	
	// Network -- layer with memory
	//  alloc
	//  prop/backprop
	//  inn/out/...
	
	// Standard endpoints
	// inn()
	// pat()
	// out()	
	// mse()
	
	
	
	
	struct ReLU 
	{
		// H -> H
		size_t N,H;
		
		DualMat<2> xs;
		DualMat<2> ys;
				
		void prop();
		void backprop();
		
		void init(Mem & mem, DualMat<2> const& xs_) {
			xs = xs_;
			N = xs.dim(0);
			H = xs.dim(1);			
			mem.assign_value(ys, {N,H});
		}		
	};
	
	
	
	struct Linc 
	{
		// fully connected linear combination layer
		// N -> M
	
		size_t N, H0, H1;
		
		DualMat<2> xs; // N,H0
		DualMat<2> ps; // H1,H0
		DualMat<2> ys; // N,H1
				
		DualMat<2> & inn() { return xs; }		
		DualMat<2> & par() { return ps; }		
		DualMat<2> const& out() const { return ys; }
					
				
		void prop();
		void backprop();				
		
		void init(Mem & mem, DualMat<2> const& xs_, size_t H1_) 
		{
			xs = xs_;			
			N = xs.dim(0);
			H0 = xs.dim(1);			
			H1 = H1_;			
			mem.assign_param(ps, {H1,H0});
			mem.assign_value(ys, { N,H1});
		}
			
	};
	
	
	
	
	
	
	struct Conv 
	{
		// filter layer
		
		// input    N  H W C0
		// kernel   C1 3 3 C0 
		// output   N  H W C1
		
		size_t N, H, W, C0, C1;
		
		DualMat<4> xs; // N H W C0
		DualMat<4> ps; // C1 3 3 C0
		DualMat<4> ys; // N H W C1
		
		DualMat<4> & inn() { return xs; }
		DualMat<4> & par() { return ps; }
		DualMat<4> & out() { return ys; }
			
		void init(Mem & mem, DualMat<4> const& xs_, size_t C1_) 
		{
			xs = xs_;						
			N = xs.dim(0);
			H = xs.dim(1);
			W = xs.dim(2);
			C0 = xs.dim(3);
			C1 = C1_;
			
			mem.assign_param(ps, {C1,3u,3u,C0});
			mem.assign_value(ys, { N,H,W,C1});
		}
		
		void prop();
		void backprop();
		
		void kermul(size_t n, size_t h0, size_t w0, size_t c1);
	};
	
	
	
	
	struct Mse
	{
		// N -> 1
		size_t N;
	
		DualMat<1> xs0;
		DualMat<1> xs1;		
		DualMat<1> ys;
				
		DualMat<1> & inn0() { return xs0; }
		DualMat<1> & inn1() { return xs1; }
		DualMat<1> const& out() const { return ys; }
				
		void init(Mem & mem, DualMat<1> const& xs0_, DualMat<1> const& xs1_) 
		{
			assert(xs0_.size() == xs1_.size());			
			xs0 = xs0_;
			xs1 = xs1_;
			N = xs0_.dim(0);
			mem.assign_value(ys, {1});
		}
		
		void prop();
		void backprop();		
	};
	


	template <class T>
	void randomize(T & ts, Random & rand)
	{
		For (i, ts.size()) { ts[i] = rand.uniform_f(-1, 1); }
	}

	struct XorNet {
		// N*2 -> N*2 -> N*1 -> MSE
		size_t N{4};

		DualMat<2> input;		
		Linc hidden;
		Linc output;
		DualMat<1> pattern;
		Mse mse;
		
		
	};
	
	
	struct LinearNet
	{
		// H0 -> H1
		size_t N, H0, H1;
		
		DualMat<2> inn_;   // H0
		DualMat<2> pat_; // H1
		Linc out_; // H1
		Mse mse_; // 1
		
		// also manage your own memory	
		Mem mem;

		
		
		DualMat<2> & inn() { return inn_; }		
		DualMat<2> & pat() { return pat_; }		
		DualMat<2> const& out() const { return out_.out(); }
		DualMat<1> const& mse() const { return mse_.out(); }
		DualMat<1> const& err() const { return mse_.out(); }
		DualMat<1> & par() { return mem.par(); }
		
		
		void alloc() {
			// mse <- M <- N
			mem.assign_value(inn_, {N,H0});
			mem.assign_value(pat_, {N,H1});
			out_.init(mem, inn_.out(), H1);
			mse_.init(mem, out_.out(), pat_.out());
		}
		
		void init(size_t N_, size_t H0_, size_t H1_) 
		{
			N = N_;
			H0 = H0_;
			H1 = H1_;
		
			// calc used mem
			mem.fake_malloc();
			alloc();
			
			auto np = mem.par().v.size();
			auto nv = mem.val().v.size();
						
			// alloc mem
			mem.malloc(np, nv, N);
			alloc();

			mse_.out().d.fill(1);
		
			if (1) {
				print("INFO:  LinearNet\n");
				print("INFO: batch size: {}\n", N);
				print("INFO: input size: {}\n", H0);
				print("INFO: output size: {}\n", H1);
				print("INFO: number of parameters: {}\n", np);
				print("INFO: store memory: {} kiB\n", kiB<Float>(np) );
				print("INFO: operational memory x 1: {} kiB\n", kiB<Float>(np * 2 + nv * 2             ) );
				print("INFO: operational memory x N: {} kiB\n", kiB<Float>(np * 2 + nv * 2 * N) );
				print("\n");
			}
		}
		
		
		//Rng<Float> & param() { return mem.ps.s; }	
		//Rng<Float> & dparam() { return mem.ps.d; }	
		
		void prop() {
			// clear ?
			out_.prop();
			mse_.prop();
		}
		
		void backprop() {			
			mse_.backprop();
			out_.backprop();					
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

	
	
	
		

} // ns


