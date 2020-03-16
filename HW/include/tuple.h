#ifndef _TUPLE_H
#define _TUPLE_H
#include<vector>
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
	~Tuple();
	void serialize(unsigned char * output);
	void unserialize(unsigned char * input);
	unsigned char operator*(const Tuple & trp);
	const unsigned char& operator[] (const int index);
	/*h-Dimension cannot be subed*/
	Tuple sub_tuple(unsigned int x, unsigned int y, unsigned int x_len, unsigned int y_len);
private:
	unsigned char  * data;
};

class Image: public Tuple{
	unsigned int stride;
	bool pending;
	Shape kernel_shape;
public:
	Image(std::vector<Tuple> subtuple);
	void split(Shape shape, bool pending, unsigned int stride);
	Image convolution(std::vector<Tuple> tuples);
	Tuple convolution(Tuple tuple);
};

#endif