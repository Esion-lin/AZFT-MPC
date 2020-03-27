/*
this file is just for testing code

*/
#include <stdlib.h> 
#include <stdio.h> 
#include "tuple.h"
#include <time.h>
#include "crypto_pend.h"
#include "model.h"
baseInt randf() { 
    return (baseInt)(rand()/(baseInt)RAND_MAX); 
} 
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
	
	for(int i = 0; i < size_of_kenerl; i++){
		int itr = 0;
		for(int i = 0; i < shape.h; i++){
			for(int j = 0; j < shape.w; j++){
				for(int k = 0; k < shape.l; k++){
					if(k%2==0)
						kernel_data[itr ++] = randf();
					else{
						kernel_data[itr ++] = - randf();
					}
				}	
			}
		}
		memcpy(W + i * shape.size()*sizeof(baseInt), kernel_data, shape.size()*sizeof(baseInt));
	}
	offset_struct += SIZE_COV;
	offset_w += size_of_kenerl * shape.size()*sizeof(baseInt);
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
	
	for(int i = 0; i < size_of_kenerl; i++){
		int itr = 0;
		for(int i = 0; i < shape.h; i++){
			for(int j = 0; j < shape.w; j++){
				for(int k = 0; k < shape.l; k++){
					if(k%2==0)
						kernel_data[itr ++] = randf();
					else{
						kernel_data[itr ++] = - randf();
					}
				}	
			}
		}
		memcpy(W + i * shape.size(), kernel_data, shape.size()*sizeof(baseInt));
	}
	offset_struct += SIZE_COV;
	offset_w += size_of_kenerl * shape.size();
}
void add_BN(unsigned char * structure, unsigned char * W, int& layer, int & offset_struct, int & offset_w){
	structure[0] = layer;
	structure[1] = layer;
	structure[2] = BN_ID;

	baseInt mu[5] = {5.8, 4.2, 2.5, 0, 0.00001};
	memcpy(W, mu, 5*sizeof(baseInt));
	offset_struct += SIZE_BN;
	offset_w += 5*sizeof(baseInt);
}
void add_BN(unsigned char * structure,  baseInt * W, int& layer, int & offset_struct, int & offset_w){
	structure[0] = layer;
	structure[1] = layer;
	structure[2] = BN_ID;

	baseInt mu[5] = {5.8, 4.2, 2.5, 0, 0.00001};
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
void add_FC(unsigned char * structure, baseInt * W, int& layer, int last_layer_len, int width_of_output, int & offset_struct, int & offset_w){
	structure[0] = layer;
	structure[1] = layer;
	structure[2] = FC_ID;
	memcpy(structure + 3, &width_of_output, sizeof(int));
	for(int i = 0; i < last_layer_len*width_of_output; i ++){
		if(randf()<0.95){
			W[i] = 0;
		}else
			W[i] = randf()/1000;
	}
	offset_struct += SIZE_FC;
	offset_w += (last_layer_len*width_of_output);

}
void make_structure_input(truthtee_pend *tee){
	int struct_size = SIZE_COV + SIZE_BN + SIZE_RELU + SIZE_POOLING;
	int w_size = 7*7*3*64*sizeof(baseInt) + 5 * sizeof(baseInt);
	if(w_size % 32!=0){
		w_size += (32 - w_size % 32);
	}
	unsigned char structure[struct_size];
	unsigned char W[w_size];
	baseInt W_base[w_size/sizeof(baseInt)];
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
	
	for(int i = 0; i < 64; i++){
		itr = 0;
		for(int i = 0; i < 3; i++){
			for(int j = 0; j < 7; j++){
				for(int k = 0; k < 7; k++){
					if(k%2==0)
						kernel_data[itr ++] = randf()/2;
					else{
						kernel_data[itr ++] = -randf()/2;
					}
				}	
			}
		}
		memcpy(W_base + i * 7*7*3, kernel_data, 7*7*3*sizeof(baseInt));
		memcpy(W + i * 7*7*3*sizeof(baseInt), kernel_data, 7*7*3*sizeof(baseInt));
	}
	baseInt mu[5] = {5.8, 4.2, 2.5, 1.5, 0.00001};
	memcpy(W + 64 * 7*7*3*sizeof(baseInt), mu, 5*sizeof(baseInt));
	memcpy(W_base + 64 * 7*7*3, mu, 5*sizeof(baseInt));
	tee->test_with_remote(W, W_len, W_en, W_en_len);
	baseInt arr[100];
	tee->block(W_en , W_en_len, structure, struct_size,arr);
	save_model<unsigned char>(structure, struct_size, "./save_data/input_struct.mdl");
	save_model<baseInt>(W_base, w_size/sizeof(baseInt), "./save_data/input_W.mdl");
}
void make_structure_cov1(truthtee_pend *tee, bool projection, int round){
	int struct_size;
	int w_size;
	printf("save_model %d\n", round);
	if(projection){
		struct_size = SIZE_COV * 4 + SIZE_BN * 4 + SIZE_RELU * 3 + SIZE_SHORTCUT;
		printf("struct_size %d %d %d %d %d\n", SIZE_COV, SIZE_BN, SIZE_RELU, SIZE_SHORTCUT, struct_size);
		w_size = (1*1*64*64 + 5 + 3*3*64*64 + 5 + 1*1*64*256 + 5 + 1*1*64*256 + 5);
	}else{
		struct_size = SIZE_COV * 3 + SIZE_BN * 3 + SIZE_RELU * 3 + SIZE_SHORTCUT;
		printf("struct_size %d %d %d %d %d\n", SIZE_COV, SIZE_BN, SIZE_RELU, SIZE_SHORTCUT, struct_size);
		w_size = (1*1*256*64 + 5 + 3*3*64*64 + 5 + 1*1*64*256 + 5);
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
	baseInt arr[100];
	save_model<unsigned char>(structure, struct_size, "./save_data/cov1_struct" + std::to_string(round) + ".mdl");
	save_model<baseInt>(W, w_size, "./save_data/cov1_W" + std::to_string(round) + ".mdl");
	//tee->block(weight , w_size, structuresss, struct_size,arr);
	printf("save_model %d\n", round);
	
}
void make_structure_cov2(truthtee_pend *tee, bool projection, int round){
	printf("save_model %d\n", round);
	int struct_size;
	int w_size;
	if(projection){
		struct_size = SIZE_COV * 4 + SIZE_BN * 4 + SIZE_RELU * 3 + SIZE_SHORTCUT;
		w_size = (1*1*256*128 + 5 + 3*3*128*128 + 5 + 1*1*128*512 + 5 + 1*1*256*512 + 5);
	}else{
		struct_size = SIZE_COV * 3 + SIZE_BN * 3 + SIZE_RELU * 3 + SIZE_SHORTCUT;
		w_size = (1*1*512*128 + 5 + 3*3*128*128 + 5 + 1*1*128*512 + 5);
	}
	if(w_size % 32!=0){
		w_size += (32 - w_size % 32);
	}
	printf("struct_size %d\n", struct_size);
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
	//tee->block(W , W_len, structure, struct_size,arr);
	printf("save_model %d\n", round);
	save_model<unsigned char>(structure, struct_size, "./save_data/cov2_struct" + std::to_string(round) + ".mdl");
	save_model<baseInt>(W, w_size, "./save_data/cov2_W" + std::to_string(round) + ".mdl");
}
void make_structure_cov3(truthtee_pend *tee, bool projection, int round){
	printf("save_model %d\n", round);
	int struct_size;
	int w_size;
	if(projection){
		struct_size = SIZE_COV * 4 + SIZE_BN * 4 + SIZE_RELU * 3 + SIZE_SHORTCUT;
		w_size = (1*1*512*256 + 5 + 3*3*256*256 + 5 + 1*1*256*1024 + 5 + 1*1*512*1024 + 5);
	}else{
		struct_size = SIZE_COV * 3 + SIZE_BN * 3 + SIZE_RELU * 3 + SIZE_SHORTCUT;
		w_size = (1*1*1024*256 + 5 + 3*3*256*256 + 5 + 1*1*256*1024 + 5);
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
		add_cov(structure + offset_struct, W + offset_w, 256, layer, {1,1,512}, false, 2, offset_struct, offset_w, layer);	
	}else{
		add_cov(structure + offset_struct, W + offset_w, 256, layer, {1,1,1024}, false, 1, offset_struct, offset_w, layer);
	}
	add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
	add_RELU(structure + offset_struct, layer, 0, offset_struct);
	add_cov(structure + offset_struct, W + offset_w, 256, layer, {3,3,256}, true, 1, offset_struct, offset_w, layer);
	add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
	add_RELU(structure + offset_struct, layer, 0, offset_struct);
	add_cov(structure + offset_struct, W + offset_w, 1024, layer, {1,1,256}, false, 1, offset_struct, offset_w, layer);
	add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
	addition_layer = layer;
	if(projection){
		add_cov(structure + offset_struct, W + offset_w, 1024, layer, {1,1,512}, false, 2, offset_struct, offset_w, 0);
		add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
		add_SHORTCUT(structure + offset_struct, layer, addition_layer, offset_struct);
	}else{
		add_SHORTCUT(structure + offset_struct, layer, 0, offset_struct);	
	}
	add_RELU(structure + offset_struct, layer, 0, offset_struct);
	baseInt arr[100];
	//tee->block(W , W_len, structure, struct_size,arr);

	printf("save_model %d\n", round);
	save_model<unsigned char>(structure, struct_size, "./save_data/cov"+ std::to_string(3+round / 10)+"_struct" + std::to_string(round%10) + ".mdl");
	save_model<baseInt>(W, w_size, "./save_data/cov"+ std::to_string(3+round / 10)+"_W" + std::to_string(round%10) + ".mdl");
}
void make_structure_cov4(truthtee_pend *tee, bool projection, int round){
	int struct_size;
	int w_size;
	if(projection){
		struct_size = SIZE_COV * 4 + SIZE_BN * 4 + SIZE_RELU * 3 + SIZE_SHORTCUT;
		w_size = (1*1*1024*512 + 5 + 3*3*512*512 + 5 + 1*1*512*2048 + 5 + 1*1*1024*2048 + 5);
	}else{
		struct_size = SIZE_COV * 3 + SIZE_BN * 3 + SIZE_RELU * 3 + SIZE_SHORTCUT;
		w_size = (1*1*2048*512 + 5 + 3*3*512*512 + 5 + 1*1*512*2048 + 5);
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
		add_cov(structure + offset_struct, W + offset_w, 512, layer, {1,1,1024}, false, 2, offset_struct, offset_w, layer);	
	}else{
		add_cov(structure + offset_struct, W + offset_w, 512, layer, {1,1,2048}, false, 1, offset_struct, offset_w, layer);
	}
	add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
	add_RELU(structure + offset_struct, layer, 0, offset_struct);
	add_cov(structure + offset_struct, W + offset_w, 512, layer, {3,3,512}, true, 1, offset_struct, offset_w, layer);
	add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
	add_RELU(structure + offset_struct, layer, 0, offset_struct);
	add_cov(structure + offset_struct, W + offset_w, 2048, layer, {1,1,512}, false, 1, offset_struct, offset_w, layer);
	add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
	addition_layer = layer;
	if(projection){
		add_cov(structure + offset_struct, W + offset_w, 2048, layer, {1,1,1024}, false, 2, offset_struct, offset_w, 0);
		add_BN(structure + offset_struct, W + offset_w, layer, offset_struct, offset_w);
		add_SHORTCUT(structure + offset_struct, layer, addition_layer, offset_struct);
	}else{
		add_SHORTCUT(structure + offset_struct, layer, 0, offset_struct);	
	}
	add_RELU(structure + offset_struct, layer, 0, offset_struct);
	baseInt arr[100];
	//tee->block(W , W_len, structure, struct_size,arr);
	save_model<unsigned char>(structure, struct_size, "./save_data/cov4_struct" + std::to_string(round) + ".mdl");
	save_model<baseInt>(W, w_size, "./save_data/cov4_W" + std::to_string(round) + ".mdl");
}
void make_structure_output(truthtee_pend *tee, int input_size){
	int struct_size;
	int w_size;
	struct_size = SIZE_POOLING + SIZE_FC;
	w_size = 1024*1000;
	unsigned char structure[struct_size];
	baseInt W[w_size];
	unsigned int W_len = w_size;
	int layer = 0;
	int addition_layer = 0;
	int offset_struct = 0;
	int offset_w = 0;
	add_POOLING(structure + offset_struct, layer, {input_size,input_size,1}, 0, 2, avg_pooling, offset_struct);
	add_FC(structure + offset_struct, W + offset_w, layer, 1024, 1000, offset_struct, offset_w);
	baseInt arr[1000];
	//tee->block(W , W_len, structure, struct_size,arr);
	save_model<unsigned char>(structure, struct_size, "./save_data/output_struct.mdl");
	save_model<baseInt>(W, w_size, "./save_data/output_W.mdl");
	/*for(int i = 0; i < 1000; i++){
		printf("%f ", arr[i]);
	}*/
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
				data[itr ++] = randf() ;
			}
		}
	}
	save_model<baseInt>(data, 224*224*3, "./save_data/data.mdl");
	memcpy(data_of_img,data,224*224*3*sizeof(baseInt));
	tee->test_with_remote(data_of_img, data_of_img_len, data_of_img_en, data_of_img_en_len);
	tee->data_input(data_of_img_en, data_of_img_en_len, 224,224,3);
	
	make_structure_input(tee);
	printf("cov1\n");
	make_structure_cov1(tee,true, 0);
	make_structure_cov1(tee,false, 1);
	make_structure_cov1(tee,false, 2);
	
	printf("cov2\n");
	make_structure_cov2(tee, true, 0);
	for(int i = 0; i < 3; i++){
		make_structure_cov2(tee, false, i+1);
	}
	printf("cov3\n");
	make_structure_cov3(tee, true,0);
	for(int i = 0; i < 22; i++){
		make_structure_cov3(tee, false, i + 1);	
	}

	printf("output\n");
	make_structure_output(tee, 14);
	//tee->data_image.output_data();
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