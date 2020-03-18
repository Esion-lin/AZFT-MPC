/*
this file is just for testing code

*/

#include "tuple.h"
#include <time.h>

int main(){
	//srand((unsigned)time(NULL));
	/*test 8*8*3 matric cov 3*3*3*/
	baseInt data[8*8*3];
	baseInt kernel_data[3*3*3];
	Shape shape = {8,8,3};
	Shape kernel_shape = {3,3,3};
	int itr = 0;
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 8; j++){
			for(int k = 0; k < 8; k++){
				data[itr ++] = 0.1*k;
			}
		}
	}
	Image image(shape);
	image.unserialize(data);
	image.output_data();
	itr = 0;
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			for(int k = 0; k < 3; k++){
				kernel_data[itr ++] = 0.5;
			}
		}
	}

	std::vector<Tuple> tuples;
	Tuple kernel(kernel_shape);
	kernel.unserialize(kernel_data);
	tuples.push_back(kernel);
	tuples.push_back(kernel);
	kernel.output_data();
	Image ans = image.convolution(tuples, true, 1);
	Image ans_1 = ans.pooling({3,3,1},false,1,avg_pooling); 
	ans.output_data();
	ans_1.output_data();
	return 0;
}