#include "tuple_c.h"


struct Tuple sub_tuple(struct Tuple org, uint32_t x, uint32_t y, uint32_t x_len, uint32_t y_len){
	struct Shape subshape={x_len,y_len,org.shape.h,x_len*y_len*org.shape.h};
	struct Tuple subtuple;
	subtuple.shape = subshape;
	subtuple.data = (float*) malloc(x_len*y_len*org.shape.h*sizeof(float));
	for(int i = 0; i < org.shape.h; i++){
		for(int j = 0; j < y_len; j++){
			memcpy(subtuple.data + j*x_len + i * x_len * y_len , org.data + (y + j) * org.shape.l + x + i *org.shape.l * org.shape.w, x_len*sizeof(float));
		}
	}
	return subtuple;
}
float multiply(struct Tuple tp1, struct Tuple tp2){
	float ans = 0;
	if(tp1.shape.l == tp2.shape.l && tp1.shape.w == tp2.shape.w && tp1.shape.h == tp2.shape.h){
		
		for(int i = 0; i < tp1.shape.l*tp1.shape.w*tp1.shape.h; i++){
			ans += tp1.data[i] * tp2.data[i];
		}
	}else{
		printf("error,  shape is not match\n");
	}
	return ans;
}
float find_max(struct Tuple org, uint32_t x, uint32_t y, uint32_t x_len, uint32_t y_len, uint32_t hight){
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
float find_avg(struct Tuple org, uint32_t x, uint32_t y, uint32_t x_len, uint32_t y_len, uint32_t hight){
	float avg_data = 0; 
	int tmp = 0;
	for(int i = 0; i < y_len; i++){
		for(int j = 0; j < x_len; j++){
			avg_data += org.data[(y+i) * org.shape.l + x + hight * org.shape.l * org.shape.w + j];
		}
	}
	return avg_data/(x_len*y_len);
}
struct Tuple convolution(struct Tuple image, struct Tuple tuple, bool pending, uint32_t stride){
	struct Tuple ans_tuple;
	if(tuple.shape.l != tuple.shape.w){
    	printf("error:illegal shape, [%d,%d] is not square.\n",tuple.shape.l, tuple.shape.w);
    	return  ans_tuple;
		
	}
	
	if(tuple.shape.h != image.shape.h){
    	printf("error:illegal cov, third dimension should be identical while [., ., %d] is not mapping [., ., %d].\n",tuple.shape.h, image.shape.h);
		return  ans_tuple;
	}
	struct Shape ans_shape = {0,0,1};
	int start_x = 0;
	int start_y = 0;
	if(pending){
		start_x = - (tuple.shape.l - 1)/2;
		start_y = - (tuple.shape.l - 1)/2;
		ans_shape.l = (image.shape.l - 1) / stride + 1;
		ans_shape.w = (image.shape.w - 1) / stride + 1;
	}else{
		start_x = 0;
		start_y = 0;
		ans_shape.l = (image.shape.l - tuple.shape.l ) / stride + 1;
		ans_shape.w = (image.shape.w - tuple.shape.w ) / stride + 1;
	}

	ans_shape.size = ans_shape.l*ans_shape.w*ans_shape.h;
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
			if(tmp_start_x + tuple.shape.l > image.shape.l){
				c = image.shape.l - tmp_start_x;
				now_pend = true;
			}
			if(start_y + tuple.shape.w > image.shape.w){
				d = image.shape.w - start_y;
				now_pend = true;
			}
			if(now_pend){
				ans_tuple.data[itr ++ ] = multiply(sub_tuple(tuple,a,b,c,d), sub_tuple(image, tmp_start_x >= 0 ? tmp_start_x:0 , start_y >= 0 ? start_y:0 , c, d));
			}else{
				ans_tuple.data[itr ++ ] = multiply(tuple, sub_tuple(image, tmp_start_x, start_y, c, d));
			}
			tmp_start_x += stride;
		}
		start_y += stride;
	}
	return ans_tuple;
}
struct Tuple convolution_embed(struct Tuple image, struct Tuple* tuples, uint32_t tuples_size, bool pending, uint32_t stride){
	struct Tuple* ans_tmp = (struct Tuple*)malloc(tuples_size * sizeof(struct Tuple));
	struct Tuple ans;
	//#pragma omp parallel for
	for(int i = 0; i < tuples_size; i++){
		ans_tmp[i] = convolution(image, tuples[i], pending, stride); 
	}
	struct Shape ans_shape = {ans_tmp[0].shape.l,ans_tmp[0].shape.w,tuples_size,ans_tmp[0].shape.l*ans_tmp[0].shape.w*tuples_size};
	ans.shape = ans_shape;
	ans.data = (float*) malloc(ans.shape.w*ans.shape.l*ans.shape.h*sizeof(float));
	uint32_t sq_size = ans_tmp[0].shape.l*ans_tmp[0].shape.w;
	for(int i = 0; i < tuples_size; i ++){
		memcpy(ans.data + i * sq_size, ans_tmp[i].data, sq_size);
		free(ans_tmp[i].data);
	}
	free(ans_tmp);
	return ans;
}
struct Tuple pooling(struct Tuple tuple, struct Shape kenerl_shape, bool pending, uint32_t stride, uint32_t type){
	if(kenerl_shape.l != kenerl_shape.w){
    	printf("error:illegal shape, [%d,%d] is not square.",kenerl_shape.l, kenerl_shape.w);
	}
	struct Shape ans_shape = {0,0,tuple.shape.h};
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
	struct Tuple ans_tuple;
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
void ReLU(struct Tuple* tuple, bool simple, float alpha){
	if(simple){
		for(int i = 0; i < tuple->shape.l*tuple->shape.w*tuple->shape.h; i++){
			tuple->data[i] = tuple->data[i] < 0 ? 0:tuple->data[i];
		}
	}else{
		for(int i = 0; i < tuple->shape.l*tuple->shape.w*tuple->shape.h; i++){
			tuple->data[i] = tuple->data[i] < 0 ? tuple->data[i]*alpha:tuple->data[i];
		}
	}
}
void BN_for_test(struct Tuple *tuple, float mu, float sigma, float gamma, float beta, float epsilon){
	for(int i = 0; i < tuple->shape.l*tuple->shape.w*tuple->shape.h; i++){
		tuple->data[i] = gamma * (tuple->data[i] - mu) / sqrt(sigma + epsilon) + beta;
	}
}
struct Tuple add_other(struct Tuple tp1, struct Tuple tp2){
	struct Tuple tuple;
	tuple.shape = tp1.shape;
	tuple.data = (float*) malloc(tp1.shape.l*tp1.shape.w*tp1.shape.h*sizeof(float));
	for(int i = 0; i < tp1.shape.l*tp1.shape.w*tp1.shape.h; i++){
		tuple.data[i] = tp1.data[i] + tp2.data[i];
	}
	return tuple;
}
void FC_f(struct Tuple tuple, float* output, float* weight, uint32_t weight_width){
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
		for(int j = 0; j < tuple.shape.size; j++){
			output[i] += tuple.data[j] * weight[j + i*tuple.shape.size];
		}	
	}
	
}
void FC(struct Tuple tuple, float* output, uint8_t* weight, uint32_t weight_width){
	float tmp;
	for(int i = 0; i < weight_width; i++){
		output[i] = 0;
		for(int j = 0; j < tuple.shape.size; j++){
			memcpy(&tmp, weight + (j + i*tuple.shape.size)*sizeof(float), sizeof(float));
			printf(" tmp : %f ",tmp);
			output[i] += tuple.data[j] * tmp;
		}	
	}
}
void softmax(float* input, float* output, uint32_t input_len){
	float sum = 0;
	for(int i = 0; i < input_len; i++){
		sum += exp(input[i]);
	}
	for(int i = 0; i < input_len; i++){
		output[i] = exp(input[i])/sum;
	}
}
// int test(){
// 	/*for test*/
// 	struct Tuple test_tp;
// 	struct Shape test_tp_shape = {225,225,3, 225*225*3};
// 	struct Tuple test_kernal;
// 	struct Shape test_kernal_shape = {5,5,3,5*5*3};
// 	test_tp.shape = test_tp_shape;
// 	test_kernal.shape = test_kernal_shape;

// 	test_tp.data = (float*)malloc(test_tp.shape.size*sizeof(float));
// 	test_kernal.data = (float*)malloc(test_kernal.shape.size*sizeof(float));
// 	for(int i = 0; i < test_tp.shape.size; i++){
// 		test_tp.data[i] = i/225 * pow(-1,i);
// 	};
// 	for(int i = 0; i < test_kernal.shape.size; i++){
// 		test_kernal.data[i] = 1;
// 	};
// 	struct Tuple* kernels = (struct Tuple*)malloc(1000*sizeof(struct Tuple));
// 	for(int i = 0; i < 100; i++){
// 		kernels[i] = test_kernal;
// 	}
// 	struct Tuple ans = convolution(test_tp, test_kernal, true, 2);

// 	printf("ans's shape {%d,%d,%d}\n", ans.shape.l, ans.shape.w, ans.shape.h);
// 	for(int i = 0; i < 113; i++){
// 		printf("%f ", ans.data[i]);
// 	}
// 	printf("\n");
// 	free(test_tp.data);
// 	free(test_kernal.data);
// 	free(ans.data);
// 	return 0;
// }