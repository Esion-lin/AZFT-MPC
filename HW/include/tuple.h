#ifndef _TUPLE_H
#define _TUPLE_H
#include <vector>
#include <string.h>
#include <stdio.h>
#include <math.h>
#define baseInt double
#define biggest_kenel 7
#define max_pooling 0
#define avg_pooling 1

struct Shape{
	int l;
	int w;
	int h;
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
			throw error;
		}
		return data[index];
	}
	baseInt operator*(Tuple trp){
		/**/
		if(trp.shape != this->shape){
			char error[80];
	    	sprintf(error, "error:shape not mapping, should have been [%d,%d,%d] but [%d,%d,%d].", this->shape.l,this->shape.w,this->shape.h,trp.shape.l,trp.shape.w,trp.shape.h);
			throw error;
		}
		baseInt ans = 0;
		for(int i = 0; i < shape.l*shape.w*shape.h; i++){
			ans = ans + data[i] * trp[i];
		}
		return ans;
	}
	
	/*h-Dimension cannot be subed*/
	Tuple sub_tuple(unsigned int x, unsigned int y, unsigned int x_len, unsigned int y_len);
	baseInt find_max(unsigned int x, unsigned int y, unsigned int x_len, unsigned int y_len, unsigned int hight);
	baseInt find_avg(unsigned int x, unsigned int y, unsigned int x_len, unsigned int y_len, unsigned int hight);
	void output_data(){
		int itr = 0;
		for(int i = 0; i < shape.h; i++){
			printf("{\n");
			for(int j = 0; j < shape.w; j++){
				printf("[");
				for(int k = 0; k < shape.l; k++){
					printf("%f,", data[itr++]);
				}
				printf("]\n");
			}
			printf("}\n");

		}
	}
protected:
	baseInt * data;
};

class Image: public Tuple{
public:
	Image(Shape shape):Tuple(shape){};
	Image(std::vector<Tuple> subtuple);
	Image convolution(std::vector<Tuple> tuples, bool pending, unsigned int stride);
	Tuple convolution(Tuple tuple,bool pending, unsigned int stride);
	void ReLU(bool simple = true, double alpha = 0);
	void BN_for_test(double mu, double sigma, double gamma, double beta, double epsilon = 0.0001);
	void add_other(Image res);
	Image pooling(Shape kenerl_shape, bool pending, unsigned int stride, int type);
	void FC(baseInt * output, baseInt * weight, int weight_width);


};
void softmax(baseInt * input, baseInt * output, int input_len);
#endif