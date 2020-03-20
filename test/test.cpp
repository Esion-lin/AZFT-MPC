/*
this file is just for testing code

*/

#include "tuple.h"
#include <time.h>
#include "crypto_pend.h"
void add_cov(unsigned char * structure, unsigned char * W, int size_of_kenerl, int& layer, Shape shape, int pending, int stride, int & offset_struct, int & offset_w, int target_layer){
	if(target_layer == layer){
		structure[0] = layer ++;	
	}else{
		structure[0] = target_layer;
		layer++;
	}
	structure[1] = layer;
	structure[2] = COV;
	structure[3] = size_of_kenerl/256; //kenerl size
	structure[4] = size_of_kenerl%256; //kenerl size
	structure[5] = pending; //pending
 	structure[6] = stride; //stride
	structure[7] = shape.l; //shape.l, w, h
	structure[8] = shape.w;
	structure[9] = shape.h / 256;
	structure[10] = shape.h % 256;
/*	printf("[%u %u]\n",structure[9],structure[10]);*/
	baseInt kernel_data[shape.size()];
	int itr = 0;
	for(int i = 0; i < shape.h; i++){
		for(int j = 0; j < shape.w; j++){
			for(int k = 0; k < shape.l; k++){
				kernel_data[itr ++] = i * j * 0.3 + k*0.005 + 0.007;
			}	
		}
	}
	for(int i = 0; i < 64; i++){
		memcpy(W + i * shape.size()*sizeof(baseInt), kernel_data, shape.size()*sizeof(baseInt));
	}
	offset_struct += SIZE_COV;
	offset_w += 64 * shape.size()*sizeof(baseInt);
}
void add_cov(unsigned char * structure, baseInt * W, int size_of_kenerl, int& layer, Shape shape, int pending, int stride, int & offset_struct, int & offset_w, int target_layer){
	if(target_layer == layer){
		structure[0] = layer ++;	
	}else{
		structure[0] = target_layer;
		layer++;
	}
	structure[1] = layer;
	structure[2] = COV;
	structure[3] = size_of_kenerl/256; //kenerl size
	structure[4] = size_of_kenerl%256; //kenerl size
	structure[5] = pending; //pending
 	structure[6] = stride; //stride
	structure[7] = shape.l; //shape.l, w, h
	structure[8] = shape.w;
	structure[9] = shape.h / 256;
	structure[10] = shape.h % 256;
/*	printf("[%u %u]\n",structure[9],structure[10]);*/
	baseInt kernel_data[shape.size()];
	int itr = 0;
	for(int i = 0; i < shape.h; i++){
		for(int j = 0; j < shape.w; j++){
			for(int k = 0; k < shape.l; k++){
				kernel_data[itr ++] = i * j * 0.3 + k*0.005 + 0.007;
			}	
		}
	}
	for(int i = 0; i < 64; i++){
		memcpy(W + i * shape.size(), kernel_data, shape.size()*sizeof(baseInt));
	}
	offset_struct += SIZE_COV;
	offset_w += 64 * shape.size();
}
void add_BN(unsigned char * structure, unsigned char * W, int& layer, int & offset_struct, int & offset_w){
	structure[0] = layer;
	structure[1] = layer;
	structure[2] = BN_ID;

	baseInt mu[5] = {5.8, 4.2, 2.5, 1.5, 0.00001};
	memcpy(W, mu, 5*sizeof(baseInt));
	offset_struct += SIZE_BN;
	offset_w += 5*sizeof(baseInt);
}
void add_BN(unsigned char * structure,  baseInt * W, int& layer, int & offset_struct, int & offset_w){
	structure[0] = layer;
	structure[1] = layer;
	structure[2] = BN_ID;

	baseInt mu[5] = {5.8, 4.2, 2.5, 1.5, 0.00001};
	memcpy(W, mu, 5*sizeof(baseInt));
	offset_struct += SIZE_BN;
	offset_w += 5;
}
void add_RELU(unsigned char * structure, int& layer, baseInt alpha, int & offset_struct){
	structure[0] = layer;
	structure[1] = layer;
	structure[2] = RELU;
	memcpy(structure + 3, &alpha, sizeof(baseInt));
	offset_struct += SIZE_RELU;
}
void add_SHORTCUT(unsigned char * structure, int& layer,int add_layer, int & offset_struct){
	structure[0] = layer;
	structure[1] = layer;
	structure[2] = SHORTCUT;
	structure[3] = add_layer;
	offset_struct += SIZE_SHORTCUT;
}
void add_POOLING(unsigned char * structure, int& layer, Shape shape, int pending, int stride, int pooling_type, int & offset_struct){
	structure[0] = layer ++;
	structure[1] = layer;
	structure[2] = POOLING;
	structure[3] = pooling_type;
	structure[4] = pending; //pending
	structure[5] = stride; //stride
	structure[6] = shape.l;
	structure[7] = shape.w;
	offset_struct += SIZE_POOLING;
}
void make_structure_input(truthtee_pend *tee){
	int struct_size = SIZE_COV + SIZE_BN + SIZE_RELU + SIZE_POOLING;
	int w_size = 7*7*3*64*sizeof(baseInt) + 5 * sizeof(baseInt);
	if(w_size % 32!=0){
		w_size += (32 - w_size % 32);
	}
	unsigned char structure[struct_size];
	unsigned char W[w_size];
	unsigned int W_len = w_size;
	unsigned char W_en[w_size];
	unsigned int W_en_len;
	baseInt kernel_data[7*7*3];
	int itr = 0;
	int layer = 0;
	/*add cov*/
	structure[itr ++] = layer ++;
	structure[itr ++] = layer;
	structure[itr ++] = COV;
	structure[itr ++] = 0; //kenerl size
	structure[itr ++] = 64; //kenerl size
	structure[itr ++] = 1; //pending
 	structure[itr ++] = 2; //stride
	structure[itr ++] = 7; //shape.l, w, h
	structure[itr ++] = 7;
	structure[itr ++] = 0;
	structure[itr ++] = 3;
	
	/*add BN*/
	structure[itr ++] = layer;
	structure[itr ++] = layer;
	structure[itr ++] = BN_ID;
	/*add RELU*/
	structure[itr ++] = layer;
	structure[itr ++] = layer;
	structure[itr ++] = RELU;
	baseInt alpha = 0;
	memcpy(structure + itr, &alpha, sizeof(baseInt));
	itr += sizeof(baseInt);
	/*add pooling*/
	structure[itr ++] = layer ++;
	structure[itr ++] = layer;
	structure[itr ++] = POOLING;
	structure[itr ++] = max_pooling;
	structure[itr ++] = 0; //pending
	structure[itr ++] = 2; //stride
	structure[itr ++] = 3;
	structure[itr ++] = 3;
	/*add weight cov*/
	itr = 0;
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 7; j++){
			for(int k = 0; k < 7; k++){
				kernel_data[itr ++] = i * j * 0.3 + k*0.005 + 0.007;
			}	
		}
	}
	for(int i = 0; i < 64; i++){
		memcpy(W + i * 7*7*3*sizeof(baseInt), kernel_data, 7*7*3*sizeof(baseInt));
	}
	baseInt mu[5] = {5.8, 4.2, 2.5, 1.5, 0.00001};
	memcpy(W + 64 * 7*7*3*sizeof(baseInt), mu, 5*sizeof(baseInt));

	tee->test_with_remote(W, W_len, W_en, W_en_len);
	baseInt arr[100];
	tee->block(W_en , W_en_len, structure, struct_size,arr);
}
void make_structure_cov1(truthtee_pend *tee, bool projection){
	int struct_size;
	int w_size;
	if(projection){
		struct_size = SIZE_COV * 4 + SIZE_BN * 4 + SIZE_RELU * 3 + SIZE_SHORTCUT;
		w_size = (1*1*64*64 + 5 + 3*3*64*64 + 5 + 1*1*64*256 + 5 + 1*1*64*256 + 5) * sizeof(baseInt);
	}else{
		struct_size = SIZE_COV * 3 + SIZE_BN * 3 + SIZE_RELU * 3 + SIZE_SHORTCUT;
		w_size = (1*1*256*64 + 5 + 3*3*64*64 + 5 + 1*1*64*256 + 5) * sizeof(baseInt);
	}
	if(w_size % 32!=0){
		w_size += (32 - w_size % 32);
	}
	unsigned char structure[struct_size];
	unsigned char W[w_size];
	unsigned int W_len = w_size;
	unsigned char W_en[w_size];
	unsigned int W_en_len;
	int layer = 0;
	int addition_layer = 0;
	int offset_struct = 0;
	int offset_w = 0;
	if(projection){
		add_cov(structure + offset_struct, W + offset_w, 64, layer, {1,1,64}, false, 1, offset_struct, offset_w, layer);	
	}else{
		add_cov(structure + offset_struct, W + offset_w, 64, layer, {1,1,256}, false, 1, offset_struct, offset_w, layer);
	}
	add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
	add_RELU(structure + offset_struct, layer, 0, offset_struct);
	add_cov(structure + offset_struct, W + offset_w, 64, layer, {3,3,64}, true, 1, offset_struct, offset_w, layer);
	add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
	add_RELU(structure + offset_struct, layer, 0, offset_struct);
	add_cov(structure + offset_struct, W + offset_w, 256, layer, {1,1,64}, false, 1, offset_struct, offset_w, layer);
	add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
	addition_layer = layer;
	if(projection){
		add_cov(structure + offset_struct, W + offset_w, 256, layer, {1,1,64}, false, 1, offset_struct, offset_w, 0);
		add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
		add_SHORTCUT(structure + offset_struct, layer, addition_layer, offset_struct);
	}else{
		add_SHORTCUT(structure + offset_struct, layer, 0, offset_struct);	
	}
	add_RELU(structure + offset_struct, layer, 0, offset_struct);
	tee->test_with_remote(W, W_len, W_en, W_en_len);
	baseInt arr[100];
	tee->block(W_en , W_en_len, structure, struct_size,arr);
}
void make_structure_cov2(truthtee_pend *tee, bool projection){
	int struct_size;
	int w_size;
	if(projection){
		struct_size = SIZE_COV * 4 + SIZE_BN * 4 + SIZE_RELU * 3 + SIZE_SHORTCUT;
		w_size = (1*1*256*128 + 5 + 3*3*128*128 + 5 + 1*1*128*512 + 5 + 1*1*256*512 + 5) * sizeof(baseInt);
	}else{
		struct_size = SIZE_COV * 3 + SIZE_BN * 3 + SIZE_RELU * 3 + SIZE_SHORTCUT;
		w_size = (1*1*512*128 + 5 + 3*3*128*128 + 5 + 1*1*128*512 + 5);
	}
	if(w_size % 32!=0){
		w_size += (32 - w_size % 32);
	}
	unsigned char structure[struct_size];
	baseInt W[w_size];
	unsigned int W_len = w_size;
	int layer = 0;
	int addition_layer = 0;
	int offset_struct = 0;
	int offset_w = 0;
	if(projection){
		add_cov(structure + offset_struct, W + offset_w, 128, layer, {1,1,256}, false, 2, offset_struct, offset_w, layer);	
	}else{
		add_cov(structure + offset_struct, W + offset_w, 128, layer, {1,1,512}, false, 1, offset_struct, offset_w, layer);
	}
	add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
	add_RELU(structure + offset_struct, layer, 0, offset_struct);
	add_cov(structure + offset_struct, W + offset_w, 128, layer, {3,3,128}, true, 1, offset_struct, offset_w, layer);
	add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
	add_RELU(structure + offset_struct, layer, 0, offset_struct);
	add_cov(structure + offset_struct, W + offset_w, 512, layer, {1,1,128}, false, 1, offset_struct, offset_w, layer);
	add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
	addition_layer = layer;
	if(projection){
		add_cov(structure + offset_struct, W + offset_w, 512, layer, {1,1,256}, false, 2, offset_struct, offset_w, 0);
		add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
		add_SHORTCUT(structure + offset_struct, layer, addition_layer, offset_struct);
	}else{
		add_SHORTCUT(structure + offset_struct, layer, 0, offset_struct);	
	}
	add_RELU(structure + offset_struct, layer, 0, offset_struct);
	baseInt arr[100];
	tee->block(W , W_len, structure, struct_size,arr);
}
void make_structure_cov3(truthtee_pend *tee, bool projection){
	int struct_size;
	int w_size;
	if(projection){
		struct_size = SIZE_COV * 4 + SIZE_BN * 4 + SIZE_RELU * 3 + SIZE_SHORTCUT;
		w_size = (1*1*256*128 + 5 + 3*3*128*128 + 5 + 1*1*128*512 + 5 + 1*1*256*512 + 5) * sizeof(baseInt);
	}else{
		struct_size = SIZE_COV * 3 + SIZE_BN * 3 + SIZE_RELU * 3 + SIZE_SHORTCUT;
		w_size = (1*1*512*128 + 5 + 3*3*128*128 + 5 + 1*1*128*512 + 5);
	}
	if(w_size % 32!=0){
		w_size += (32 - w_size % 32);
	}
	unsigned char structure[struct_size];
	baseInt W[w_size];
	unsigned int W_len = w_size;
	int layer = 0;
	int addition_layer = 0;
	int offset_struct = 0;
	int offset_w = 0;
	if(projection){
		add_cov(structure + offset_struct, W + offset_w, 128, layer, {1,1,256}, false, 2, offset_struct, offset_w, layer);	
	}else{
		add_cov(structure + offset_struct, W + offset_w, 128, layer, {1,1,512}, false, 1, offset_struct, offset_w, layer);
	}
	add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
	add_RELU(structure + offset_struct, layer, 0, offset_struct);
	add_cov(structure + offset_struct, W + offset_w, 128, layer, {3,3,128}, true, 1, offset_struct, offset_w, layer);
	add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
	add_RELU(structure + offset_struct, layer, 0, offset_struct);
	add_cov(structure + offset_struct, W + offset_w, 512, layer, {1,1,128}, false, 1, offset_struct, offset_w, layer);
	add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
	addition_layer = layer;
	if(projection){
		add_cov(structure + offset_struct, W + offset_w, 512, layer, {1,1,256}, false, 2, offset_struct, offset_w, 0);
		add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
		add_SHORTCUT(structure + offset_struct, layer, addition_layer, offset_struct);
	}else{
		add_SHORTCUT(structure + offset_struct, layer, 0, offset_struct);	
	}
	add_RELU(structure + offset_struct, layer, 0, offset_struct);
	baseInt arr[100];
	tee->block(W , W_len, structure, struct_size,arr);
}
void make_structure_cov4(){

}
void make_structure_output(){

}
int main(){
	//srand((unsigned)time(NULL));
	/*test 8*8*3 matric cov 3*3*3*/
	truthtee_pend *tee = new truthtee_pend();
	baseInt data[224*224*3];

	unsigned char data_of_img[224*224*3*sizeof(baseInt)];
	unsigned int data_of_img_len = 224*224*3*sizeof(baseInt);
	unsigned char data_of_img_en[224*224*3*sizeof(baseInt)];
	unsigned int data_of_img_en_len;
	
	int itr = 0;
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 224; j++){
			for(int k = 0; k < 224; k++){
				data[itr ++] = 0.1*k + 0.0386 + j * 0.8 + i * 0.003 + 0.007 * i * j * k;
			}
		}
	}
	memcpy(data_of_img,data,224*224*3*sizeof(baseInt));
	tee->test_with_remote(data_of_img, data_of_img_len, data_of_img_en, data_of_img_en_len);
	tee->data_input(data_of_img_en, data_of_img_en_len, 224,224,3);
	make_structure_input(tee);
	make_structure_cov1(tee,true);
	make_structure_cov1(tee,false);
	make_structure_cov1(tee,false);
	make_structure_cov2(tee, true);
	make_structure_cov2(tee, false);
	make_structure_cov2(tee, false);
	make_structure_cov2(tee, false);
	printf("now shape is [%d,%d,%d]",tee->data_image.shape.l,tee->data_image.shape.w,tee->data_image.shape.h);
	/*std::vector<Tuple> tuples;
	Tuple kernel({3,3,3});
	kernel.unserialize(kernel_data);
	tuples.push_back(kernel);
	tuples.push_back(kernel);
	kernel.output_data();
	Image ans = tee.image.convolution(tuples, true, 1);
	Image ans_1 = ans.pooling({3,3,1},false,1,avg_pooling); 
	ans.output_data();
	ans_1.output_data();*/
	return 0;
}