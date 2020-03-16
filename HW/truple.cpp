#include "truple.h"
#include <stdio.h>
Truple::Truple(Shape shape){
	this.shape = shape;
	data = new unsigned char[shape.l*shape.w*shape.h];
}
Truple::~Truple(){
	delete data;
	data = = NULL;
}
void Truple::serialize(unsigned char * output, int &length){
	/**/
	length = shape.l*shape.w*shape.h;
	memcpy(output, this.data, shape.l*shape.w*shape.h);
}
void Truple::unserialize(unsigned char * input){
	/**/
	memcpy(this.data, input, shape.l*shape.w*shape.h);
}
Truple Truple::sub_truple(unsigned int x, unsigned int y, unsigned int x_len, unsigned int y_len){
	struct Shape subshape={x_len,y_len,shape.h};
	Truple subtruple(subshape);
	unsigned char subdata[x_len*y_len*shape.h];
	for(int i = 0; i < shape.h; i++){
		for(int j = 0; j < y_len; j++){
			memcpy(subdata + j*x_len + i * x_len * y_len, data + (y + j) * shape.l + x + i * shape.l * shape.w, x_len);
		}
	}
	subtruple.unserialize(subdata);
	return subtruple;
}
const unsigned char& Truple::operator[] (const unsigned int index){
	if(index >= shape.l*shape.w*shape.h){
		char error[80];
    	sprintf(error, "error:out of range, should less [%d] but [%d].", shape.l*shape.w*shape.h,index);
		throw error;
	}
	return data[index];
}
unsigned char Truple::operator*(const Truple & trp){
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
}
void Image::split(Shape shape, bool pending, unsigned int stride){
	this.stride = stride;
	this.pending = pending;
	this.kernel_shape = shape;
}
Truple Image::convolution(Truple truple){
	/*from left to right, from up to down*/
	if(pending){

	}else{

	}
}
Truple Image::convolution(std::vector<Truple> truples){
	/*truples size -> result truple.h*/
	/*from left to right, from up to down*/
	if(pending){

	}else{

	}
}