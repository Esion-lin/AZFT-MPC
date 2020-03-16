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
	Truple(Shape shape);
	~Truple();
	void serialize(unsigned char * output, int &length);
	void unserialize(unsigned char * input);
	unsigned char operator*(const Truple & trp);
	const unsigned char& operator[] (const int index);
	/*h-Dimension cannot be subed*/
	Truple sub_truple(unsigned int x, unsigned int y, unsigned int x_len, unsigned int y_len);
private:
	unsigned char  * data;
};

class Image: public Truple{
	unsigned int stride;
	bool pending;
	Shape kernel_shape;
public:
	void split(Shape shape, bool pending, unsigned int stride);
	Truple convolution(std::vector<Truple> truples);
	Truple convolution(Truple truple);
};

#endif