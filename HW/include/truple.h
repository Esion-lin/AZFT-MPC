#ifndef _TRUPLE_H
#define _TRUPLE_H
struct Shape{
	int l;
	int w;
	int h;
};
class Truple{
public:
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
public:

};

#endif