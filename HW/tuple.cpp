#include "tuple.h"
#include <stdio.h>
Tuple::Tuple(Shape shape){
	this.shape = shape;
	data = new unsigned char[shape.l*shape.w*shape.h];
}

Tuple::~Tuple(){
	delete data;
	data = NULL;
}
void Tuple::serialize(unsigned char * output){
	/**/
	int length = shape.l*shape.w*shape.h;
	memcpy(output, this.data, shape.l*shape.w*shape.h);
}
void Tuple::unserialize(unsigned char * input){
	/**/
	memcpy(this.data, input, shape.l*shape.w*shape.h);
}
Tuple Tuple::sub_tuple(unsigned int x, unsigned int y, unsigned int x_len, unsigned int y_len){
	struct Shape subshape={x_len,y_len,shape.h};
	Tuple subtuple(subshape);
	unsigned char subdata[x_len*y_len*shape.h];
	for(int i = 0; i < shape.h; i++){
		for(int j = 0; j < y_len; j++){
			memcpy(subdata + j*x_len + i * x_len * y_len, data + (y + j) * shape.l + x + i * shape.l * shape.w, x_len);
		}
	}
	subtuple.unserialize(subdata);
	return subtuple;
}
const unsigned char& Tuple::operator[] (const unsigned int index){
	if(index >= shape.l*shape.w*shape.h){
		char error[80];
    	sprintf(error, "error:out of range, should less [%d] but [%d].", shape.l*shape.w*shape.h,index);
		throw error;
	}
	return data[index];
}
unsigned char Tuple::operator*(const Tuple & trp){
	/**/
	if(trp.shape != this.shape){
		char error[80];
    	sprintf(error, "error:shape not mapping, should have been [%d,%d,%d] but [%d,%d,%d].", this.shape.l,this.shape.w,this.shape.h,trp.shape.l,trp.shape.w,trp.shape.h);
		throw error;
	}
	unsigned char ans = 0;
	for(int i = 0; i < shape.l*shape.w*shape.h; i++){
		ans =( ans + data[i] * trp[i] )% 256
	}
	return ans;
}
Image::Image(std::vector<Tuple> subtuple){
	this.shape = {subtuple[0].shape};
	this.shape.h = subtuple.size();
	/*copy data*/
	data = new unsigned char[subtuple[0].shape.l*subtuple[0].shape.w*subtuple.size()];
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
void Image::split(Shape shape, bool pending, unsigned int stride){
	this.stride = stride;
	this.pending = pending;
	this.kernel_shape = shape;
}
Tuple Image::convolution(Tuple tuple){
	/*from left to right, from up to down*/
	if(tuple.l != tuple.w){
		char error[80];
    	sprintf(error, "error:illegal shape, [%d,%d] is not square.",tuple.l, tuple.w);
		throw error;
	}
	Shape ans_shape = {0,0,1};
	if(pending){
		int start_x = - (tuple.l - 1)/2;
		int start_y = - (tuple.l - 1)/2;
		ans_shape.l = shape.l / stride;
		ans_shape.w = shape.w / stride;
	}else{
		int start_x = 0;
		int start_y = 0;
		ans_shape.l = (shape.l - (tuple.l - 1)/2) / stride;
		ans_shape.w = (shape.w - (tuple.w - 1)/2) / stride;
	}
	Tuple ans_tuple(ans_shape);
	unsigned char ans_data[ans_shape.l * ans_shape.w * ans_shape.h];
	unsigned int itr = 0;
	for(int i = 0; i < ans_shape.w; i++){
		int tmp_start_x = start_x;
		for(int j = 0; j < ans_shape.l; j++){
			/*
			judge if it need pending
			*/
			int a = 0,b = 0,c = tuple.l,d = tuple.w;
			bool now_pend = false;
			if(tmp_start_x < 0){
				a = 0 - tmp_start_x;
				c = tuple.l + tmp_start_x;
				now_pend = true;
			}
			if(start_y < 0){
				b = 0 - start_y;
				d = tuple.w + start_y;
				now_pend = true;
			}
			if(tmp_start_x + tuple.l > shape.l){
				c = shape.l - tmp_start_x;
				now_pend = true;
			}
			if(start_y + tuple.w > shape.w){
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

Image Image::convolution(std::vector<Tuple> tuples){
	/*tuples size -> result tuple.h*/
	/*from left to right, from up to down*/
	std::vector<Tuple> ans_tmp;
	for(int i = 0; i < tuples.size(); i++){
		ans_tmp.push_back(convolution(tuples[i])); 
	}
	Image result_img(ans_tmp);
	return result_img;
}