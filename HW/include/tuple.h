#ifndef _TUPLE_H
#define _TUPLE_H
#include <vector>
#include <string.h>
#include <stdio.h>
#include <math.h>
#define baseInt float
#define biggest_kenel 7
#define max_pooling 0
#define avg_pooling 1

struct Shape{
	int l;
	int w;
	int h;
	int size(){
		return l*w*h;
	}
	bool operator!=(const Shape other) const{  
        return this->l != other.l || this->w != other.w;  
    }  
};
class Tuple{
public:
	Shape shape;
	Tuple(Shape shape);
	Tuple(const Tuple &other){
		this->shape = other.shape;
		data = new baseInt[shape.l*shape.w*shape.h];
		other.serialize(data);
	}
	Tuple(){
		data = new baseInt[biggest_kenel*biggest_kenel*biggest_kenel];
		shape = {biggest_kenel,biggest_kenel,biggest_kenel};
	}
	~Tuple();
	void reshape(Shape shape, baseInt * input);
	void serialize(baseInt * output) const;
	void unserialize(baseInt * input);
	baseInt& operator[] (const int index){
		if(index >= shape.l*shape.w*shape.h){
			char error[80];
	    	sprintf(error, "error:out of range, should less [%d] but [%d].", shape.l*shape.w*shape.h,index);
			printf(error);
			throw error;
		}
		return data[index];
	}
	baseInt operator*(Tuple trp){
		/**/
		if(trp.shape != this->shape){
			char error[80];
	    	sprintf(error, "error:shape not mapping, should have been [%d,%d,%d] but [%d,%d,%d].", this->shape.l,this->shape.w,this->shape.h,trp.shape.l,trp.shape.w,trp.shape.h);
			printf(error);
			throw error;
		}
		baseInt ans = 0;
		for(int i = 0; i < shape.l*shape.w*shape.h; i++){
			ans = ans + data[i] * trp[i];
		}
		return fmod(ans,2000);
	}
	void operator=(Tuple trp){
		/**/
		shape = trp.shape;
		delete data;
		data = new baseInt[trp.shape.size()];
		trp.serialize(data);
	}
	/*h-Dimension cannot be subed*/
	Tuple sub_tuple(uint32_t x, uint32_t y, uint32_t x_len, uint32_t y_len);
	baseInt find_max(uint32_t x, uint32_t y, uint32_t x_len, uint32_t y_len, uint32_t hight);
	baseInt find_avg(uint32_t x, uint32_t y, uint32_t x_len, uint32_t y_len, uint32_t hight);
	void output_data(){
		int itr = 0;

				for(int k = 0; k < shape.l; k++){
					printf("%f,", data[itr++]);
				}
				printf("]\n");

		
	}
protected:
	baseInt * data;
};

class Image: public Tuple{
public:
	Image(){};
	Image(Shape shape):Tuple(shape){};
	Image(const Image &other){
		this->shape = other.shape;
		data = new baseInt[shape.l*shape.w*shape.h];
		other.serialize(data);
	};
	Image(std::vector<Tuple> subtuple);
	Image convolution(std::vector<Tuple> tuples, bool pending, uint32_t stride);
	Tuple convolution(Tuple tuple,bool pending, uint32_t stride);
	void ReLU(bool simple = true, baseInt alpha = 0);
	void BN_for_test(baseInt mu, baseInt sigma, baseInt gamma, baseInt beta, baseInt epsilon);
	void add_other(Image res);
	Image pooling(Shape kenerl_shape, bool pending, uint32_t stride, int type);
	void FC(baseInt * output, baseInt * weight, int weight_width);
	void FC(baseInt * output, uint8_t * weight, int weight_width);

};
void softmax(baseInt * input, baseInt * output, int input_len);
#endif