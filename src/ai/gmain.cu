#include <vector>

using Float = float;

__global__ void gpu_mul(Float * val, Float * wag, Float * inn, size_t N, size_t M)
{
	auto val_ind = blockIdx.x + blockIdx.y * N;
	auto wag_ind = blockIdx.x;
	auto inn_ind = blockIdx.y;
	
	if (val_ind < N*N and wag_ind < N and inn_ind < N) 
	{
		val[blockIdx.x + blockIdx.y * N] = wag[blockIdx.x] * inn[blockIdx.y];
	}
}


void cpu_mul(Float * val, Float * wag, Float * inn, size_t N, size_t M)
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
	

int main() 
{
	size_t N = 10000;
	size_t M = 10000;
	
	
	std::vector<Float> v;
	v.resize(N + N*M + N*M);
	Float * p = &v[0];

	Float * inn = p;
	Float * wag = p + N;
	Float * val = p+N+M*N;
		
	for (size_t i = 0; i < v.size(); ++i)
	{
		//v[i] = (int8_t)randm::uniform_f(-1, +1);
	}	

	// cpu
	if (0) {
		cpu_mul(val, wag, inn, N, M);			
	}

	// gpu
	if (1) {
	
		Float * gp;
		cudaMalloc(&gp, N + N*M + N*M);
		
		Float * gpu_inn = gp;
		Float * gpu_wag = gp + N;
		Float * gpu_val = gp+N+M*N;
		
		cudaMemcpy(gp, p, v.size(), cudaMemcpyHostToDevice);
		
		dim3 threadsPerBlock(N,M,1);
		gpu_mul<<<1, threadsPerBlock>>>(gpu_val, gpu_wag, gpu_inn, N, M);
	
		
		
		cudaMemcpy(p, gp, v.size(), cudaMemcpyDeviceToHost);

		cudaFree(gp);
		
	}
	
	

	return 0;
}
