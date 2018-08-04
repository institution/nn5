
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

namespace naive
{
	// each layer manages self params and self output memory
	
	// array views
	struct ArrayMap4 {
	
		ArrayMap3 at(int32_t)		
	};
	
	struct View4 {
		int32_t pos[4];
		int32_t len[4];
		
		int32_t stride[4]
		Float *f;	
		
		
		template <int N>
		inc(Float * f)
	};
	
	
	
	struct Mem
	{		
		Array1 vs;
		Array1 ps;			
	};
	
	
	
	struct Array1
	{
		Rng<Float> s;
		Rng<Float> d;
		
		Float & operator[](size_t i) { return s[i]; }
		Float const& operator[](size_t i) const { return s[i]; }
	};
	
	Float & d(Layer & x, size_t i) { return x.d[i]; }
	Float const& d(Layer const& x, size_t i) { return x.d[i]; }
	
	void alloc(Array1 & out, Array1 & inn, size_t n) {
		out.s.alloc(inn.s, n);
		out.d.alloc(inn.d, n);		
	}
	
	
	struct Layer 
	{
		virtual void prop();
		virtual void backprop();		
	};
	
	
	struct ReLU {
		
		// N -> N
		
		Array1 xs;
		Array1 ys;		
		
		void prop();
		void backprop();
		
		void init(Mem & mem, Array1 const& xs, size_t n) {			
			this->xs = xs;
			ys.grab(mem.values, n);
		}
		
	};
	
	void ReLU::prop() 
	{
		for (int i = 0; i < xs.size() ; ++i)
		{	     
			auto x = xs[i];
			ys[i] = (x > 0) ? x : 0;
		}
	}

	void ReLU::backprop() 
	{
		for (int i = 0; i < xs.size() ; ++i)
		{	
			auto dx = (xs[i] > 0) ? 1 : 0;
			d(xs,i) = d(ys,i) * dx;
		}
	}
	
	
	struct Linc {
		// fully connected linear combination layer
		// H -> W
	
		size_t N, H0, H1;
		
		Array2 xs; // N,H0
		Array2 ps; // H1,H0
		Array2 ys; // N,H1	
				
		void prop();
		void backprop();
				
		
		void init(Mem & mem, Array1 const& xs, size_t n) {
			this->xs = xs;
			alloc(ys, mem.vs, N*H1);
			alloc(ps, mem.ps, H1*H0);
		}
			
	};
	
	void Linc::prop() 
	{
		// H1 <- H0
		
		for (size_t n = 0; n < N; ++n)
		{
			for (size_t j = 0; j < H1; ++j)
			{
				Float y = 0;
				for (size_t i = 0; i < H0; ++i)
				{
					y += ps(j,i) * xs(n,i);
				}
				ys(n,j) = y;
				
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
		for (size_t n = 0; n < N; ++n)
		{
			for (size_t i = 0; i < H0; ++i)
			{
				Float dx = 0;
				for (size_t j = 0; j < H1; ++j)
				{
					dx += d(ys,j) * ps(j,i);
					
					d(ps,j,i) = d(ys,j) * xs(i);
				}
				// dL/dx[i]
				dxs[i] = dx;
			}
		}
		//a.difs.array() *= (ba.wags.transpose() * b.difs).array();	
		//ba.dwags.array() *= (b.difs * a.vals.transpose()).array();
		//ba.dbias.array() *= b.difs.array();		
	}
	
	
	
	
	struct Conv {
		// filter layer
		
		// input    N  H W C0
		// kernel   C1 3 3 C0 
		// output   N  H W C1
		
		size_t N, H, W, C0, C1;
		
		Array4 xs; // N H W C0
		Array4 ps; // C1 3 3 C0
		Array4 ys; // N H W C1
			
			
		void init(Mem & mem, Array4 const& xs, size_t C1) 
		{
			this->xs = xs;
			this->C1 = C1;
			
			N = xs.dim[0];
			H = xs.dim[1];
			W = xs.dim[2];
			C0 = xs.dim[3];			
			
			ps.alloc(mem.ps, Dim4(C1,3,3,C0));
			ys.alloc(mem.vs, Dim4(N,H,W,C1));			
		}
		
	};
	
	void Conv::prop() 
	{
		for (size_t n = 0; n < N; ++n)
		{				
			// h,w -- left top corner
			for (size_t h = 0; h < H - 2; ++h) 
			{
				for (size_t w = 0; w < W - 2; ++w) 
				{
					for (size_t c1 = 0; c1 < C1; ++c1)
					{
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
					y += ps(c1, h, w, c0) * xs(n,h0+h,w0+w,c0)
				}
			}
		}			
		ys(n,h0,w0,c1) = y;
	
	}
		
		
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
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	MSE
	
	struct MSE 
	{
		size_t N;
	
		Rng<Float> xs0;
		Rng<Float> xs1;
		Rng<Float> ys;
		
		Rng<Float> xs0;
		Rng<Float> xs1;
		Rng<Float> ys;
				
		size_t needmem() { return 1; }
		
		void init(Rng<Float> & mem, Lay & inp) {
			out[0] = mem[0];
			mem.inc(1);
			out[1] = mem[0];
		}
		
		void init() {				
			
		}
	};

	struct Classic
	{

		Array1 input
		Linc hidden
		Linc output

		void init(size_t M, size_t H, size_t N) 
		{
			// mse <- M <- H <- N
			
			
			add<Linc>(H)
			add<ReLU>() 
			add<Linc>(M)
			add<Mse>()
			
			
			
			input.init(mem, N);
			hidden.init(mem, H, input)
			output.init(mem, M, hidden)
			mse.init(mem, output, pattern)
			
			//mse.get_output()
			
			
			
			
		}
		
		
		void prop
		void backprop
		
	};
	
	
	void do_network() {
		// operations:
		// init
		// prop
		// backprop
		
		pattern = Array(N)
		input = Array(N)
		output= Array(M)
		
		linc(H, ..., input)
		relu(...)
		linc(...,)
		
		a = Linc(H, input)
		b = ReLU(a) 
		output = Linc(M, b)
		d = Mse(output, pattern)
		
		
		
			
		input.init(mem, N);
		hidden.init(mem, H, input)
		output.init(mem, M, hidden)
		mse.init(mem, output, pattern)
	
	}




	
	struct Sum {
			
		Rng<Float> xs0;
		Rng<Float> xs1;		
		Rng<Float> ys;	
			
		Rng<Float> dxs0;
		Rng<Float> dxs1;
		Rng<Float> dys;
		
		void prop();
		void backprop();
		
	};
	
	void Sum::prop() 
	{
		for (int i = 0; i < xs.size() ; ++i)
		{	     
			ys[i] = xs0[i] + xs1[i];
		}
	}

	void Sum::backprop() 
	{
		for (int i = 0; i < xs.size() ; ++i)
		{	
			//dxs[i] = dys[i];
			// TODO
		}	
	}
	
	
			
		

}
