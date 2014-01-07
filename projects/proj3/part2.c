#include <emmintrin.h>
#include <omp.h>
#define KERNX 3 //this is the x-size of the kernel. It will always be odd.
#define KERNY 3 //this is the y-size of the kernel. It will always be odd.
int conv2D(float* in, float* out, int data_size_X, int data_size_Y,
	   float* kernel)
{
    int kern_cent_X = (KERNX-1)/2;
    int kern_cent_Y = (KERNY-1)/2;

    int min_padding =  (KERNY -1)/2;
    int bufferedOutSize = (min_padding*2+data_size_X)*(min_padding*2+data_size_Y);
    float bufferedOut[bufferedOutSize];
    /*
#pragma omp parallel for
    for(int z = 0; z < bufferedOutSize; z++){
        bufferedOut[z] = 0;
    }
    int current_loc = min_padding+(min_padding*(2*min_padding+data_size_X)); 
    for(int i = 0; i < data_size_Y; i++){
        for(int u = 0; u < data_size_X; u++){
            bufferedOut[u +current_loc] = in[i*data_size_X + u];
        }
        current_loc = current_loc +  2*min_padding + data_size_X;
    }
    
    int x_padding = data_size_X + 2;
    int y_padding = data_size_Y + 2;
    memset(bufferedOut,0, x_padding * y_padding * sizeof(float));
#pragma omp parallel for
    for (int k = 1; k < y_padding - 1; k++){
	omp_set_num_threads(16);
	memcpy(&bufferedOut[k * x_padding + 1], &in[(k-1) * data_size_X], sizeof(float) * data_size_X);
    }
    */
    int x_padding = data_size_X + 2; int y_padding = data_size_Y + 2;
#pragma omp parallel for firstprivate(x_padding, y_padding, kern_cent_X)
    for(int k = 0; k < x_padding * kern_cent_X; k++){
	bufferedOut[k] = 0;
	bufferedOut[(x_padding * y_padding) - (k+1)] = 0;
    }

#pragma omp parallel for firstprivate(data_size_Y, x_padding, data_size_X)
    for (int t = 0; t < data_size_Y; t++){
	bufferedOut[(t+1) * x_padding] = 0;
	memcpy(bufferedOut + 1 + ((t+1) * (x_padding)), in + (t*data_size_X), data_size_X* sizeof(float));
	bufferedOut[((t+1) * x_padding) + x_padding -1] = 0;
    }
 
    // main convolution loop
    __m128 vecs[KERNX * KERNY];
    for(int z = 0; z < KERNX*KERNY; z++)
        vecs[z] = _mm_set_ps1(kernel[z]);
#pragma omp parallel num_threads(16) firstprivate(data_size_X, data_size_Y, kern_cent_Y, kern_cent_X,vecs,min_padding)
    {
	int thread = omp_get_thread_num();
	//#pragma omp for
		for(int y = thread * (data_size_Y / 16); y < (thread == 15 ? data_size_Y : (data_size_Y / 16) * (thread + 1)); y++){ // the y coordinate of theoutput location we're focusing on
		    //#pragma omp parallel for schedule(static, data_size_Y / 16) num_threads(16)
		    //for (int y = 0; y < data_size_Y; y++){
	    for(int x = 0; x < data_size_X/4*4; x+=4){ // the x coordinate of the output location we're focusing on
		__m128 sum = _mm_setzero_ps();
        sum = _mm_add_ps(sum, _mm_mul_ps( vecs[(kern_cent_X-(-kern_cent_X))+(kern_cent_Y-(-kern_cent_Y))*KERNX], _mm_loadu_ps((bufferedOut + (x + (-kern_cent_X) + min_padding) + (y + (-kern_cent_Y) + min_padding) * (data_size_X + 2*min_padding)))));
        sum = _mm_add_ps(sum, _mm_mul_ps( vecs[(kern_cent_X-(-kern_cent_X+1))+(kern_cent_Y-(-kern_cent_Y))*KERNX], _mm_loadu_ps((bufferedOut + (x + (-kern_cent_X+1) + min_padding) + (y + (-kern_cent_Y) + min_padding) * (data_size_X + 2*min_padding)))));
        sum = _mm_add_ps(sum, _mm_mul_ps( vecs[(kern_cent_X-(-kern_cent_X+2))+(kern_cent_Y-(-kern_cent_Y))*KERNX], _mm_loadu_ps((bufferedOut + (x + (-kern_cent_X+2) + min_padding) + (y + (-kern_cent_Y) + min_padding) * (data_size_X + 2*min_padding)))));
        sum = _mm_add_ps(sum, _mm_mul_ps( vecs[(kern_cent_X-(-kern_cent_X))+(kern_cent_Y-(-kern_cent_Y+1))*KERNX], _mm_loadu_ps((bufferedOut + (x + (-kern_cent_X) + min_padding) + (y + (-kern_cent_Y+1) + min_padding) * (data_size_X + 2*min_padding)))));
        sum = _mm_add_ps(sum, _mm_mul_ps( vecs[(kern_cent_X-(-kern_cent_X+1))+(kern_cent_Y-(-kern_cent_Y+1))*KERNX], _mm_loadu_ps((bufferedOut + (x + (-kern_cent_X+1) + min_padding) + (y + (-kern_cent_Y+1) + min_padding) * (data_size_X + 2*min_padding)))));
        sum = _mm_add_ps(sum, _mm_mul_ps( vecs[(kern_cent_X-(-kern_cent_X+2))+(kern_cent_Y-(-kern_cent_Y+1))*KERNX], _mm_loadu_ps((bufferedOut + (x + (-kern_cent_X+2) + min_padding) + (y + (-kern_cent_Y+1) + min_padding) * (data_size_X + 2*min_padding)))));
        sum = _mm_add_ps(sum, _mm_mul_ps( vecs[(kern_cent_X-(-kern_cent_X))+(kern_cent_Y-(-kern_cent_Y+2))*KERNX], _mm_loadu_ps((bufferedOut + (x + (-kern_cent_X) + min_padding) + (y + (-kern_cent_Y+2) + min_padding) * (data_size_X + 2*min_padding)))));
        sum = _mm_add_ps(sum, _mm_mul_ps( vecs[(kern_cent_X-(-kern_cent_X+1))+(kern_cent_Y-(-kern_cent_Y+2))*KERNX], _mm_loadu_ps((bufferedOut + (x + (-kern_cent_X+1) + min_padding) + (y + (-kern_cent_Y+2) + min_padding) * (data_size_X + 2*min_padding)))));
        sum = _mm_add_ps(sum, _mm_mul_ps( vecs[(kern_cent_X-(-kern_cent_X+2))+(kern_cent_Y-(-kern_cent_Y+2))*KERNX], _mm_loadu_ps((bufferedOut + (x + (-kern_cent_X+2) + min_padding) + (y + (-kern_cent_Y+2) + min_padding) * (data_size_X + 2*min_padding)))));

		_mm_storeu_ps(out + x+y * data_size_X, sum);
	    }
	
	if (data_size_X % 4 != 0){
	    for (int y = 0; y < data_size_Y; y++){ //
		for (int x = (data_size_X / 4) * 4; x < data_size_X; x++){ //might have to be + 4 but im not sure, we'll see
		    for(int i = -kern_cent_X; i <= kern_cent_X; i++){ // kernel unflipped x coordinate
			for(int j = -kern_cent_Y; j <= kern_cent_Y; j++){ // kernel unflipped y coordinate
			    // only do the operation if not out of bounds
			    if(x+i>-1 && x+i<data_size_X && y+j>-1 && y+j<data_size_Y){
				//Note that the kernel is flipped
				out[x+y*data_size_X] += kernel[(kern_cent_X-i)+(kern_cent_Y-j)*KERNX] * in[(x+i) + (y+j)*data_size_X];
			    }
			}
		    }
		}
	    }
	}
    }
    }
    return 1;
}
