#include <emmintrin.h>
#include <stdio.h>
#include <stdlib.h>
#define KERNX 3 //this is the x-size of the kernel. It will always be odd.
#define KERNY 3 //this is the y-size of the kernel. It will always be odd.
int conv2D(float* in, float* out, int data_size_X, int data_size_Y,
	   float* kernel)
{
    int kern_cent_X = (KERNX-1)/2;
    int kern_cent_Y = (KERNY-1)/2;

    int min_padding = 3+ (KERNY -1)/2;
    int bufferedOutSize = (min_padding*2+data_size_X)*(min_padding*2+data_size_Y);
    float bufferedOut[bufferedOutSize];



    for(int z = 0; z < bufferedOutSize; z++){
	bufferedOut[z] = 0;
    }
    int current_loc = min_padding+(min_padding*(2*min_padding+data_size_X)); // zeros on top of matrix

    for(int i = 0; i < data_size_Y; i++){
	for(int u = 0; u < data_size_X; u++){
	    bufferedOut[u +current_loc] = in[i*data_size_X + u];
	}
	current_loc = current_loc +  2*min_padding + data_size_X;
    }
    
    // main convolution loop
    __m128 vecs[KERNX * KERNY];
    for(int z = 0; z < KERNX*KERNY; z++)
	vecs[z] = _mm_set_ps1(kernel[z]);

    for(int y = 0; y < data_size_Y; y++){ // the y coordinate of theoutput location we're focusing on
	for(int x = 0; x < data_size_X/4*4; x+=4){ // the x coordinate of the output location we're focusing on
	    __m128 sum = _mm_setzero_ps();

	    
	    for(int a = -kern_cent_X; a <= kern_cent_X; a++){ // kernel unflipped x coordinate
		for(int b = -kern_cent_Y; b <= kern_cent_Y; b++){ // kernel unflipped y coordinate

		    
		    sum = _mm_add_ps(
				     sum, _mm_mul_ps(
						vecs[(kern_cent_X-a)+(kern_cent_Y-b)*KERNX],
						_mm_loadu_ps((bufferedOut + (x + a + min_padding) + (y + b + min_padding) * (data_size_X + 2*min_padding))
							     ))
				     );

		}
	    }

	    
	    _mm_storeu_ps(out + x+y * data_size_X, sum);
	 	    
	}
    }

    
    if (data_size_X % 4 != 0){
	for (int y = 0; y < data_size_Y; y++){ 
	    for (int x = (data_size_X / 4) * 4; x < data_size_X; x++){ 	
		for(int i = -kern_cent_X; i <= kern_cent_X; i++){ // kernel unflipped x coordinate
		    for(int j = -kern_cent_Y; j <= kern_cent_Y; j++){ // kernel unflipped y coordinate
			// only do the operation if not out of bounds
			
			if(x+i>-1 && x+i<data_size_X && y+j>-1 && y+j<data_size_Y){
			    
			    //Note that the kernel is flipped
			                                                    out[x+y*data_size_X] +=
										kernel[(kern_cent_X-i)+(kern_cent_Y-j)*KERNX] * in[(x+i) + (y+j)*data_size_X];
			}
		    }
		}
	    }
	}
    }    
    return 1;
}
