struct Shape{
	int l;
	int w;
	int h;
	int size;
};


struct Truple{
	Shape shape;
	float* data;
}

Tuple sub_tuple(Truple org, unsigned int x, unsigned int y, unsigned int x_len, unsigned int y_len);
float multiply(Truple tp1, Truple tp2);
float find_max(Truple org, uint32_t x, uint32_t y, uint32_t x_len, uint32_t y_len, uint32_t hight);
float find_avg(Truple org, uint32_t x, uint32_t y, uint32_t x_len, uint32_t y_len, uint32_t hight);
Tuple convolution(Tuple image, Tuple tuple, bool pending, uint32_t stride);
Tuple convolution(Tuple image, Tuple* tuples, uint32_t tuples_size, bool pending, uint32_t stride);
Tuple pooling(Tuple tuple, Shape kenerl_shape, bool pending, uint32_t stride, uint32_t type);
void ReLU(Tuple &tuple, bool simple, float alpha);
void BN_for_test(Tuple &tuple, float mu, float sigma, float gamma, float beta, float epsilon);
Tuple add_other(Tuple tp1, Tuple tp2);
void FC(Tuple tuple, float* output, float* weight, uint32_t weight_width);
void FC(Tuple tuple, float* output, uint8_t* weight, uint32_t weight_width);
void softmax(float* input, float* output, uint32_t input_len);