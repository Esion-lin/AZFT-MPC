#include "tuple.h"
#include <stdio.h>
#include <time.h>
#include "omp.h"
Tuple::Tuple(Shape shape){
	this->shape = shape;
	data = new baseInt[shape.l*shape.w*shape.h];
}

Tuple::~Tuple(){
	delete data;
	data = NULL;
}
void Tuple::reshape(Shape shape, baseInt * input){
	delete data;
	data = NULL;
	this->shape = shape;
	data = new baseInt[shape.l*shape.w*shape.h];
	unserialize(input);
}
void Tuple::serialize(baseInt * output) const{
	/**/
	int length = shape.l*shape.w*shape.h;
	memcpy(output, data, shape.l*shape.w*shape.h*sizeof(baseInt));
}
void Tuple::unserialize(baseInt * input){
	/**/
	memcpy(this->data, input, shape.l*shape.w*shape.h*sizeof(baseInt));
}
Tuple Tuple::sub_tuple(unsigned int x, unsigned int y, unsigned int x_len, unsigned int y_len){
	struct Shape subshape={x_len,y_len,shape.h};
	Tuple subtuple(subshape);
	baseInt subdata[x_len*y_len*shape.h];
	for(int i = 0; i < shape.h; i++){
		for(int j = 0; j < y_len; j++){
			memcpy(subdata + j*x_len + i * x_len * y_len , data + (y + j) * shape.l + x + i * shape.l * shape.w, x_len*sizeof(baseInt));
		}
	}
	subtuple.unserialize(subdata);
	return subtuple;
}
baseInt Tuple::find_max(unsigned int x, unsigned int y, unsigned int x_len, unsigned int y_len, unsigned int hight){
	baseInt max_data = -100000; 
	int tmp = 0;
	for(int i = 0; i < y_len; i++){
		for(int j = 0; j < x_len; j++){
			tmp = (y+i) * shape.l + x + hight * shape.l * shape.w + j;
			if(data[tmp] > max_data){
				max_data = data[tmp];
			}
		}
	}
	return max_data;
}
baseInt Tuple::find_avg(unsigned int x, unsigned int y, unsigned int x_len, unsigned int y_len, unsigned int hight){
	baseInt avg_data = 0; 
	int tmp = 0;
	for(int i = 0; i < y_len; i++){
		for(int j = 0; j < x_len; j++){
			avg_data += data[(y+i) * shape.l + x + hight * shape.l * shape.w + j];
		}
	}
	return avg_data/(x_len*y_len);
}
Image::Image(std::vector<Tuple> subtuple){
	this->shape = {subtuple[0].shape};
	this->shape.h = subtuple.size();
	/*copy data*/
	this->data = new baseInt[subtuple[0].shape.l*subtuple[0].shape.w*subtuple.size()];
	for(int i = 0; i < subtuple.size(); i++){
		if(subtuple[i].shape != subtuple[0].shape || subtuple[i].shape.h != 1){
			shape = {0,0,0};
			delete data;
			data = NULL;
			break;
		}else{
			subtuple[i].serialize(data + i*shape.l*shape.w);
		}
	}

}
Tuple Image::convolution(Tuple tuple, bool pending, unsigned int stride){
	
	/*from left to right, from up to down*/
	if(tuple.shape.l != tuple.shape.w){
		char error[80];
    	sprintf(error, "error:illegal shape, [%d,%d] is not square.",tuple.shape.l, tuple.shape.w);
		throw error;
	}
	if(tuple.shape.h != shape.h){
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

	Tuple ans_tuple(ans_shape);
	baseInt ans_data[ans_shape.l * ans_shape.w * ans_shape.h];
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
				ans_data[itr ++ ] = tuple.sub_tuple(a,b,c,d) * sub_tuple(tmp_start_x >= 0 ? tmp_start_x:0 , start_y >= 0 ? start_y:0 , c, d);
			}else{
				ans_data[itr ++ ] = tuple * sub_tuple(tmp_start_x, start_y, c, d);
			}
			tmp_start_x += stride;
		}
		start_y += stride;
	}
	ans_tuple.unserialize(ans_data);
	return ans_tuple;

}
Image Image::pooling(Shape kenerl_shape, bool pending, unsigned int stride, int type){
	if(kenerl_shape.l != kenerl_shape.w){
		char error[80];
    	sprintf(error, "error:illegal shape, [%d,%d] is not square.",kenerl_shape.l, kenerl_shape.w);
		throw error;
	}
	Shape ans_shape = {0,0,shape.h};
	int start_x = 0;
	int start_y = 0;
	if(pending){
		start_x = - (kenerl_shape.l - 1)/2;
		start_y = - (kenerl_shape.l - 1)/2;
		ans_shape.l = (shape.l - 1) / stride + 1;
		ans_shape.w = (shape.w - 1) / stride + 1;
	}else{
		start_x = 0;
		start_y = 0;
		ans_shape.l = (shape.l - kenerl_shape.l ) / stride + 1;
		ans_shape.w = (shape.w - kenerl_shape.w ) / stride + 1;
	}
	Image ans_tuple(ans_shape);
	baseInt ans_data[ans_shape.l * ans_shape.w * ans_shape.h];
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
			if(tmp_start_x + kenerl_shape.l > shape.l){
				c = shape.l - tmp_start_x;
				now_pend = true;
			}
			if(start_y + kenerl_shape.w > shape.w){
				d = shape.w - start_y;
				now_pend = true;
			}
			if(now_pend){
				for(int i = 0; i < ans_shape.h; i ++){
					ans_data[itr+i*ans_shape.w*ans_shape.l] = (type == max_pooling?find_max(tmp_start_x >= 0 ? tmp_start_x:0 , start_y >= 0 ? start_y:0 , c, d, i):find_avg(tmp_start_x >= 0 ? tmp_start_x:0 , start_y >= 0 ? start_y:0 , c, d, i));
				}
			}else{
				for(int i = 0; i < ans_shape.h; i ++){
					ans_data[itr+i*ans_shape.w*ans_shape.l] = (type == max_pooling?find_max(tmp_start_x,start_y,c,d,i):find_avg(tmp_start_x,start_y,c,d,i));
				}
			}
			itr ++;
			tmp_start_x += stride;
			
		}
		start_y += stride;
	}
	ans_tuple.unserialize(ans_data);
	return ans_tuple;
}
Image Image::convolution(std::vector<Tuple> tuples, bool pending, unsigned int stride){
	/*tuples size -> result tuple.h*/
	/*from left to right, from up to down*/
	std::vector<Tuple> ans_tmp(tuples.size());
	#pragma omp parallel for
	for(int i = 0; i < tuples.size(); i++){
		ans_tmp[i] = convolution(tuples[i], pending, stride); 
	}
	Image result_img(ans_tmp);
	return result_img;
}
void Image::ReLU(bool simple, baseInt alpha){
	if(simple){
		for(int i = 0; i < shape.l*shape.w*shape.h; i++){
			data[i] = data[i] < 0 ? 0:data[i];
		}
	}else{
		for(int i = 0; i < shape.l*shape.w*shape.h; i++){
			data[i] = data[i] < 0 ? data[i]*alpha:data[i];
		}
	}
}
void Image::BN_for_test(baseInt mu, baseInt sigma, baseInt gamma, baseInt beta, baseInt epsilon){
	for(int i = 0; i < shape.l*shape.w*shape.h; i++){
		data[i] = gamma * (data[i] - mu) / sqrt(sigma + epsilon) + beta;
	}
}
void Image::add_other(Image res){
	for(int i = 0; i < shape.l*shape.w*shape.h; i++){
		data[i] = data[i] + res[i];
	}
}
void Image::FC(baseInt * output, baseInt * weight, int weight_width){
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
		for(int j = 0; j < shape.size(); j++){
			output[i] += data[j] * weight[j + i*shape.size()];
		}	
	}
	
}
void Image::FC(baseInt * output, unsigned char * weight, int weight_width){
	baseInt tmp;
	for(int i = 0; i < weight_width; i++){
		output[i] = 0;
		for(int j = 0; j < shape.size(); j++){
			memcpy(&tmp, weight + (j + i*shape.size())*sizeof(baseInt), sizeof(baseInt));
			printf(" tmp : %f ",tmp);
			output[i] += data[j] * tmp;
		}	
	}
}
void softmax(baseInt * input, baseInt * output, int input_len){
	baseInt sum = 0;
	for(int i = 0; i < input_len; i++){
		sum += exp(input[i]);
	}
	for(int i = 0; i < input_len; i++){
		output[i] = exp(input[i])/sum;
	}
}