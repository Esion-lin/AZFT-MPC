#include "tuple_c.h"
#include <stdio.h>

Tuple sub_tuple(Truple org, uint32_t x, uint32_t y, uint32_t x_len, uint32_t y_len){
	struct Shape subshape={x_len,y_len,org.shape.h};
	Tuple subtuple;
	subtuple.shape = subshape;
	subtuple.data = (float*) malloc(x_len*y_len*org.shape.h*sizeof(float));
	for(int i = 0; i < org.shape.h; i++){
		for(int j = 0; j < y_len; j++){
			memcpy(subtuple.data + j*x_len + i * x_len * y_len , org.data + (y + j) * org.shape.l + x + i *org.shape.l * org.shape.w, x_len*sizeof(float));
		}
	}
	return subtuple;
}
float multiply(Truple tp1, Truple tp2){
	float ans = 0;
	if(tp1.shape.l == tp2.shape.l && tp1.shape.w == tp2.shape.w &&. tp1.shape.h == tp2.shape.h){
		
		for(int i = 0; i < tp1.shape.l*tp1.shape.w*tp1.shape.h; i++){
			ans += tp1.data[i] * tp2.data[i];
		}
	}else{
		printf("error,  shape is not match\n");
	}
	return ans;
}
float find_max(Truple org, uint32_t x, uint32_t y, uint32_t x_len, uint32_t y_len, uint32_t hight){
	float max_data = -100000; 
	int tmp = 0;
	for(int i = 0; i < y_len; i++){
		for(int j = 0; j < x_len; j++){
			tmp = (y+i) * org.shape.l + x + hight * org.shape.l * org.shape.w + j;
			if(org.data[tmp] > max_data){
				max_data = org.data[tmp];
			}
		}
	}
	return max_data;
}
float find_avg(Truple org, uint32_t x, uint32_t y, uint32_t x_len, uint32_t y_len, uint32_t hight){
	float avg_data = 0; 
	int tmp = 0;
	for(int i = 0; i < y_len; i++){
		for(int j = 0; j < x_len; j++){
			avg_data += org.data[(y+i) * org.shape.l + x + hight * org.shape.l * org.shape.w + j];
		}
	}
	return avg_data/(x_len*y_len);
}
Tuple convolution(Tuple image, Tuple tuple, bool pending, uint32_t stride){
	if(tuple.shape.l != tuple.shape.w){
		char error[80];
    	sprintf(error, "error:illegal shape, [%d,%d] is not square.",tuple.shape.l, tuple.shape.w);
		throw error;
	}
	if(tuple.shape.h != image.shape.h){
		char error[80];
    	sprintf(error, "error:illegal cov, third dimension should be identical while [., ., %d] is not mapping [., ., %d].",tuple.shape.h, shape.h);
		throw error;
	}
	Shape ans_shape = {0,0,1};
	int start_x = 0;
	int start_y = 0;
	if(pending){
		start_x = - (tuple.shape.l - 1)/2;
		start_y = - (tuple.shape.l - 1)/2;
		ans_shape.l = (shape.l - 1) / stride + 1;
		ans_shape.w = (shape.w - 1) / stride + 1;
	}else{
		start_x = 0;
		start_y = 0;
		ans_shape.l = (shape.l - tuple.shape.l ) / stride + 1;
		ans_shape.w = (shape.w - tuple.shape.w ) / stride + 1;
	}

	Tuple ans_tuple;
	ans_tuple.shape = ans_shape;
	ans_tuple.data = (float*) malloc(ans_shape.l * ans_shape.w * ans_shape.h*sizeof(float));
	unsigned int itr = 0;
	for(int i = 0; i < ans_shape.w; i++){
		int tmp_start_x = start_x;
		for(int j = 0; j < ans_shape.l; j++){
			/*
			judge if it need pending
			*/
			int a = 0,b = 0,c = tuple.shape.l,d = tuple.shape.w;
			bool now_pend = false;
			if(tmp_start_x < 0){
				a = 0 - tmp_start_x;
				c = tuple.shape.l + tmp_start_x;
				now_pend = true;
			}
			if(start_y < 0){
				b = 0 - start_y;
				d = tuple.shape.w + start_y;
				now_pend = true;

			}
			if(tmp_start_x + tuple.shape.l > shape.l){
				c = shape.l - tmp_start_x;
				now_pend = true;
			}
			if(start_y + tuple.shape.w > shape.w){
				d = shape.w - start_y;
				now_pend = true;
			}
			if(now_pend){
				ans_tuple.data[itr ++ ] = sub_tuple(tuple,a,b,c,d) * sub_tuple(image, tmp_start_x >= 0 ? tmp_start_x:0 , start_y >= 0 ? start_y:0 , c, d);
			}else{
				ans_tuple.data[itr ++ ] = multiply(tuple, sub_tuple(image, tmp_start_x, start_y, c, d));
			}
			tmp_start_x += stride;
		}
		start_y += stride;
	}
	return ans_tuple;
}
Tuple convolution(Tuple image, Tuple* tuples, uint32_t tuples_size, bool pending, uint32_t stride){
	Tuple* ans_tmp = (Tuple*)malloc(tuples_size * sizeof(Tuple));
	Tuple ans;
	//#pragma omp parallel for
	for(int i = 0; i < tuples_size; i++){
		ans_tmp[i] = convolution(image, tuples[i], pending, stride); 
	}
	ans.shape = {ans_tmp[0].shape.l,ans_tmp[0].shape.w,tuples_size}
	ans.data = (float*) malloc(ans.shape.w*ans.shape.l*ans.shape.h*sizeof(float));
	uint32_t sq_size = ans_tmp[0].shape.l*ans_tmp[0].shape.w;
	for(int i = 0; i < tuples_size; i ++){
		memcpy(ans.data + i * sq_size, ans_tmp[i].data, sq_size);
		free(ans_tmp[i].data);
	}
	free(ans_tmp);
	return ans;
}
Tuple pooling(Tuple tuple, Shape kenerl_shape, bool pending, uint32_t stride, uint32_t type){
	if(kenerl_shape.l != kenerl_shape.w){
		char error[80];
    	sprintf(error, "error:illegal shape, [%d,%d] is not square.",kenerl_shape.l, kenerl_shape.w);
		throw error;
	}
	Shape ans_shape = {0,0,tuple.shape.h};
	int start_x = 0;
	int start_y = 0;
	if(pending){
		start_x = - (kenerl_shape.l - 1)/2;
		start_y = - (kenerl_shape.l - 1)/2;
		ans_shape.l = (tuple.shape.l - 1) / stride + 1;
		ans_shape.w = (tuple.shape.w - 1) / stride + 1;
	}else{
		start_x = 0;
		start_y = 0;
		ans_shape.l = (tuple.shape.l - kenerl_shape.l ) / stride + 1;
		ans_shape.w = (tuple.shape.w - kenerl_shape.w ) / stride + 1;
	}
	Tuple ans_tuple;
	ans_tuple.shape = ans_shape;
	ans_tuple.data = (float*) malloc(ans_shape.l * ans_shape.w * ans_shape.h*sizeof(float));
	unsigned int itr = 0;
	for(int j = 0; j < ans_shape.w; j ++){
		int tmp_start_x = start_x;
		for(int k = 0; k < ans_shape.l; k ++){
			int a = 0,b = 0,c = kenerl_shape.l,d = kenerl_shape.w;
			bool now_pend = false;
			if(tmp_start_x < 0){
				a = 0 - tmp_start_x;
				c = kenerl_shape.l + tmp_start_x;
				now_pend = true;
			}
			if(start_y < 0){
				b = 0 - start_y;
				d = kenerl_shape.w + start_y;
				now_pend = true;

			}
			if(tmp_start_x + kenerl_shape.l > tuple.shape.l){
				c = tuple.shape.l - tmp_start_x;
				now_pend = true;
			}
			if(start_y + kenerl_shape.w > tuple.shape.w){
				d = tuple.shape.w - start_y;
				now_pend = true;
			}
			if(now_pend){
				for(int i = 0; i < ans_shape.h; i ++){
					ans_tuple.data[itr+i*ans_shape.w*ans_shape.l] = (type == max_pooling?find_max(tuple, tmp_start_x >= 0 ? tmp_start_x:0 , start_y >= 0 ? start_y:0 , c, d, i):find_avg(tuple, tmp_start_x >= 0 ? tmp_start_x:0 , start_y >= 0 ? start_y:0 , c, d, i));
				}
			}else{
				for(int i = 0; i < ans_shape.h; i ++){
					ans_tuple.data[itr+i*ans_shape.w*ans_shape.l] = (type == max_pooling?find_max(tuple, tmp_start_x,start_y,c,d,i):find_avg(tuple, tmp_start_x,start_y,c,d,i));
				}
			}
			itr ++;
			tmp_start_x += stride;
			
		}
		start_y += stride;
	}
	return ans_tuple;
}
void ReLU(Tuple &tuple, bool simple, float alpha){
	if(simple){
		for(int i = 0; i < tuple.shape.l*tuple.shape.w*tuple.shape.h; i++){
			tuple.data[i] = tuple.data[i] < 0 ? 0:tuple.data[i];
		}
	}else{
		for(int i = 0; i < tuple.shape.l*tuple.shape.w*tuple.shape.h; i++){
			tuple.data[i] = tuple.data[i] < 0 ? tuple.data[i]*alpha:tuple.data[i];
		}
	}
}
void BN_for_test(Tuple &tuple, float mu, float sigma, float gamma, float beta, float epsilon){
	for(int i = 0; i < tuple.shape.l*tuple.shape.w*tuple.shape.h; i++){
		tuple.data[i] = gamma * (tuple.data[i] - mu) / sqrt(sigma + epsilon) + beta;
	}
}
Tuple add_other(Tuple tp1, Tuple tp2){
	Tuple tuple;
	tuple.shape = tp1.shape;
	tuple.data = (float*) malloc(tp1.shape.l*tp1.shape.w*tp1.shape.h*sizeof(float));
	for(int i = 0; i < tp1.shape.l*tp1.shape.w*tp1.shape.h; i++){
		tuple.data[i] = tp1.data[i] + tp2.data[i];
	}
	return tuple;
}
void FC(Tuple tuple, float* output, float* weight, uint32_t weight_width){
	/*
	[n_1,.....,n_datalen]
		.				|
		.				|
		.				|weight_width 
		.				|
		.				|
	[n_1,.....,n_datalen]
	*/
	for(int i = 0; i < weight_width; i++){
		output[i] = 0;
		for(int j = 0; j < tuple.shape.size(); j++){
			output[i] += tuple.data[j] * weight[j + i*tuple.shape.size()];
		}	
	}
	
}
void FC(Tuple tuple, float* output, uint8_t* weight, uint32_t weight_width){
	float tmp;
	for(int i = 0; i < weight_width; i++){
		output[i] = 0;
		for(int j = 0; j < tuple.shape.size(); j++){
			memcpy(&tmp, weight + (j + i*tuple.shape.size())*sizeof(float), sizeof(float));
			printf(" tmp : %f ",tmp);
			output[i] += tuple.data[j] * tmp;
		}	
	}
}
void softmax(float* input, float* output, uint32_t input_len){
	baseInt sum = 0;
	for(int i = 0; i < input_len; i++){
		sum += exp(input[i]);
	}
	for(int i = 0; i < input_len; i++){
		output[i] = exp(input[i])/sum;
	}
}
