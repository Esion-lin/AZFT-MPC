#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h> 
#include <string.h>
#include <math.h>
#define biggest_kenel 7
#define max_pooling 0
#define avg_pooling 1
struct Shape{
	int l;
	int w;
	int h;
	int size;
};


struct Tuple{
	struct Shape shape;
	float* data;
};

struct Tuple sub_tuple(struct Tuple org, unsigned int x, unsigned int y, unsigned int x_len, unsigned int y_len);
float multiply(struct Tuple tp1, struct Tuple tp2);
float find_max(struct Tuple org, uint32_t x, uint32_t y, uint32_t x_len, uint32_t y_len, uint32_t hight);
float find_avg(struct Tuple org, uint32_t x, uint32_t y, uint32_t x_len, uint32_t y_len, uint32_t hight);
struct Tuple convolution(struct Tuple image, struct Tuple tuple, bool pending, uint32_t stride);
struct Tuple convolution_embed(struct Tuple image, struct Tuple* tuples, uint32_t tuples_size, bool pending, uint32_t stride);
struct Tuple pooling(struct Tuple tuple, struct Shape kenerl_shape, bool pending, uint32_t stride, uint32_t type);
void ReLU(struct Tuple* tuple, bool simple, float alpha);
void BN_for_test(struct Tuple* tuple, float mu, float sigma, float gamma, float beta, float epsilon);
struct Tuple add_other(struct Tuple tp1, struct Tuple tp2);
void FC_f(struct Tuple tuple, float* output, float* weight, uint32_t weight_width);
void FC(struct Tuple tuple, float* output, uint8_t* weight, uint32_t weight_width);
void softmax(float* input, float* output, uint32_t input_len);