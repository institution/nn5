#include "naive-nn.hpp"

namespace naive {

	void Mem::malloc(size_t np, size_t nv, size_t N) {	
		// memory layout:
		// params[np]		
		// dparams[np]
		// values[nv]
		// dvalues[nv]
	
		ptr =  new Float[np*2 + nv*2*N];
	
		Float * p = ptr;
		Float * q = p + np;
		Rng<Float> par{p,q};
				
		p = q;
		q = p + np;		
		Rng<Float> dpar{p,q};
		
		p = q;
		q = p + nv*N;		
		Rng<Float> val{p,q};
		
		p = q;
		q = p + nv*N;		
		Rng<Float> dval{p,q};
				
		
		
		free_param.v.set(par);
		free_param.d.set(dpar);
		free_value.v.set(val);
		free_value.d.set(dval);
		
		used_param.v.set({free_param.v.rng.a, free_param.v.rng.a});
		used_param.d.set({free_param.d.rng.a, free_param.d.rng.a});
		used_value.v.set({free_value.v.rng.a, free_value.v.rng.a});
		used_value.d.set({free_value.d.rng.a, free_value.d.rng.a});
	}
		
	void Mse::prop() {		
		
		Float y = 0;
		For(i, N) {
			auto x = xs0.v(i, h) - xs1.v(i, h);		
			y += (1.0f/N) * x * x;
		}
		ys.v(0) = y;
			
	}
	void Mse::backprop() {
		auto dy = ys.d(0);		
		assert(dy == 1);   // usually :)
		For (n, N) {
			auto x = xs0.v(n) - xs1.v(n);
			xs0.d(n) = dy * 2*x * (+1);
			xs1.d(n) = dy * 2*x * (-1);
		}
	}


	void Conv::prop() {
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
	void Conv::kermul(size_t n, size_t h0, size_t w0, size_t c1) {
		// h0,w0 -- left top corner
		Float y = 0;
		// over kernel
		For (h, H) {
			For (w, W) {
				For (c0, C0) {
					y += ps.v(c1, h, w, c0) * xs.v(n,h0+h,w0+w,c0);
				}
			}
		}			
		ys.v(n,h0,w0,c1) = y;
	
	}

	/* void Conv::backprop(h0,w0) {		
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

	void Linc::prop() {
		// H1 <- H0
		
		For (n, N) {
			For (h1, H1) {
				Float y = 0;
				For (h0, H0) {
					y += ps.v(h1,h0) * xs.v(n,h0);
				}
				ys.v(n,h1) = y;
				//ys(n,j) = dot(ps(j), xs(n));
			}
		}
		//b.vals = (ba.wags * a.vals + ba.bias)rray() - Float(2);	
		//set(b.difs, 1);
	}
	void Linc::backprop() {
		// dL/dy[j] == dy[j]
		// dy[j]/dx[i] == p[j,i]
		For (n, N) {
			For (h0, H0) {
				Float dx = 0;
				For (h1, H1) {
					dx += ys.d(n,h1) * ps.v(h1,h0);
					ps.d(h1,h0) = ys.d(n,h1) * xs.v(n,h0);
				}
				// dL/dx[i]
				xs.d(n,h0) = dx;
			}
		}
			
		//a.difsrray() *= (ba.wags.transpose() * b.difs)rray();	
		//ba.dwagsrray() *= (b.difs * a.vals.transpose())rray();
		//ba.dbiasrray() *= b.difsrray();		
		
	}

	void ReLU::prop() {
		For (n, N) {
			For (h, H) {
				auto x = xs.v(n,h);
				ys.v(n,h) = (x > 0) ? x : 0;
			}
		}
	}
	void ReLU::backprop() {
		For (n, N) {
			For (h, H) {			
				auto dx = (xs.v(n,h) > 0) ? 1 : 0;
				xs.d(n,h) = ys.d(n,h) * dx;
			}
		}
	}

	
}
