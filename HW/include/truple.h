#ifndef _TRUPLE_H
#define _TRUPLE_H
#include<vector>
struct Shape{
	int l;
	int w;
	int h;
};
class Truple{
public:
	Shape shape;
	truple(Shape shape);
	serialize(unsigned char * output, int &length);
	unserialize(unsigned char * input);
	unsigned char &operator*(const Truple & trp);
private:
	unsigned char  * data;
};
class Kernel:public Truple{

};
class Image: public Truple{
	unsigned int stride;
	bool pending;
	Shape kerneL_shape;
public:
	void split(Shape shape, bool pending, unsigned int stride);
	Truple convolution(std::vector<Truple> truples);
};

#endif