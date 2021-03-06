#include <string.h>
#include "component.h"
#include "model.h"

int main(){
	/*
	+ l1 l2 l3
	- l3 l2 l4
	== l4 l1 l5
	out -> l5
	cov org 225 225 3 lay1 64 true 2 3 3 3 false 0
	pooling lay1 ...
	ReLU lay2
	BN lay3
	.....
	FC -> lay7
	out -> lay7
	*/
	struct Code code;

	code.code_size = 10;
	code.pos_s = (uint8_t*)malloc(code.code_size*POS_LEN);
	uint8_t* Structure = (uint8_t*)malloc(INS_LEN*3 + COV_LEN + POOLING_LEN + RELU_LEN + BN_LEN + FC_LEN + SHORTCUT_LEN + OUT_LEN);
	
	code.now_pos = 0;
	uint32_t itr = 0;
	


	char label1[LABEL_LEN] = "l1";
	float l1 = 18;
	float l2 = 22;
	char label2[LABEL_LEN] = "l2";
	char label3[LABEL_LEN] = "l3";
	char label4[LABEL_LEN] = "l4";
	char label5[LABEL_LEN] = "l5";
	memcpy(code.pos_s,&itr,POS_LEN);
	Structure[itr++] = ADD_OP;
	memcpy(Structure + itr, label1, LABEL_LEN);
	itr += LABEL_LEN;
	Structure[itr++] = FLOAT_TP;
	memcpy(Structure + itr, label2, LABEL_LEN);
	itr += LABEL_LEN;
	Structure[itr++] = FLOAT_TP;
	memcpy(Structure + itr, label3, LABEL_LEN);
	itr += LABEL_LEN;
	Structure[itr++] = FLOAT_TP;

	memcpy(code.pos_s + POS_LEN,&itr,POS_LEN);
	Structure[itr++] = SUB_OP;
	memcpy(Structure + itr, label3, LABEL_LEN);
	itr += LABEL_LEN;
	Structure[itr++] = FLOAT_TP;
	memcpy(Structure + itr, label2, LABEL_LEN);
	itr += LABEL_LEN;
	Structure[itr++] = FLOAT_TP;
	memcpy(Structure + itr, label4, LABEL_LEN);
	itr += LABEL_LEN;
	Structure[itr++] = FLOAT_TP;

	memcpy(code.pos_s + POS_LEN*2,&itr,POS_LEN);
	Structure[itr++] = EQ_OP;
	memcpy(Structure + itr, label4, LABEL_LEN);
	itr += LABEL_LEN;

	Structure[itr++] = FLOAT_TP;
	memcpy(Structure + itr, label1, LABEL_LEN);
	itr += LABEL_LEN;
	Structure[itr++] = FLOAT_TP;
	memcpy(Structure + itr, label5, LABEL_LEN);
	itr += LABEL_LEN;

	Structure[itr++] = FLOAT_TP;

	printf("next add structure of cov\n");
	memcpy(code.pos_s + POS_LEN*3,&itr,POS_LEN);
	Structure[itr++] = COV;
	char cov_label[LABEL_LEN] = "org";
	memcpy(Structure + itr, cov_label, LABEL_LEN);
	itr += LABEL_LEN;
	Structure[itr++] = 225/256;
	Structure[itr++] = 225%256;
	Structure[itr++] = 225/256;
	Structure[itr++] = 225%256;
	Structure[itr++] = 3/256;
	Structure[itr++] = 3%256;
	char cov_label_out[LABEL_LEN] = "lay1";
	memcpy(Structure + itr, cov_label_out, LABEL_LEN);
	itr += LABEL_LEN;
	Structure[itr++] = 64/256;
	Structure[itr++] = 64%256;
	Structure[itr++] = 1;
	Structure[itr++] = 2;
	Structure[itr++] = 5/256;
	Structure[itr++] = 5%256;
	Structure[itr++] = 5/256;
	Structure[itr++] = 5%256;
	Structure[itr++] = 3/256;
	Structure[itr++] = 3%256;
	Structure[itr++] = 0;
	uint32_t wpos = 0;
	memcpy(Structure + itr, &wpos, POS_LEN);
	itr += POS_LEN;

	printf("next add structure of pooling\n");
	memcpy(code.pos_s + POS_LEN*4,&itr,POS_LEN);
	Structure[itr++] = POOLING;
	memcpy(Structure + itr, cov_label_out, LABEL_LEN);
	itr += LABEL_LEN;
	Structure[itr++] = 113/256;
	Structure[itr++] = 113%256;
	Structure[itr++] = 113/256;
	Structure[itr++] = 113%256;
	Structure[itr++] = 64/256;
	Structure[itr++] = 64%256;
	char pool_label_out[LABEL_LEN] = "lay2";
	memcpy(Structure + itr, pool_label_out, LABEL_LEN);
	itr += LABEL_LEN;
	Structure[itr++] = max_pooling;
	Structure[itr++] = 1;
	Structure[itr++] = 2;
	Structure[itr++] = 5/256;
	Structure[itr++] = 5%256;
	Structure[itr++] = 5/256;
	Structure[itr++] = 5%256;
	Structure[itr++] = 1;

	printf("next add structure of ReLU\n");
	memcpy(code.pos_s + POS_LEN*5,&itr,POS_LEN);
	Structure[itr++] = RELU;
	memcpy(Structure + itr, pool_label_out, LABEL_LEN);
	itr += LABEL_LEN;
	Structure[itr++] = 57/256;
	Structure[itr++] = 57%256;
	Structure[itr++] = 57/256;
	Structure[itr++] = 57%256;
	Structure[itr++] = 64/256;
	Structure[itr++] = 64%256;
	char ReLU_label_out[LABEL_LEN] = "lay3";
	memcpy(Structure + itr, ReLU_label_out, LABEL_LEN);
	itr += LABEL_LEN;
	float alpha = 0;
	memcpy(Structure + itr, &alpha, sizeof(float));
	itr += sizeof(float);
	Structure[itr++] = 0;



	printf("next add weight of cov\n");
	code.W_len = 5*5*3*64*sizeof(float) + 5*sizeof(float) + 57*57*64*10*sizeof(float);
	code.W = (uint8_t*)malloc(code.W_len);
	float arr[75];
	for(int i = 0; i < 75; i++){
		arr[i] = 0.5;
	}
	for(int i = 0; i < 64; i++){
		memcpy(code.W + i * 5*5*3*sizeof(float), arr, 5*5*3*sizeof(float));
	}	
	save_model<uint8_t>(code.W , 5*5*3*64*sizeof(float), "./kernels");
	printf("next add structure of BN\n");
	memcpy(code.pos_s + POS_LEN*6,&itr,POS_LEN);
	Structure[itr++] = BN_ID;
	memcpy(Structure + itr, ReLU_label_out, LABEL_LEN);
	itr += LABEL_LEN;
	Structure[itr++] = 57/256;
	Structure[itr++] = 57%256;
	Structure[itr++] = 57/256;
	Structure[itr++] = 57%256;
	Structure[itr++] = 64/256;
	Structure[itr++] = 64%256;
	char BN_label_out[LABEL_LEN] = "lay4";
	memcpy(Structure + itr, BN_label_out, LABEL_LEN);
	itr += LABEL_LEN;
	Structure[itr++] = 0;
	uint32_t bn_wpos = 5*5*3*64*sizeof(float);
	memcpy(Structure + itr, &bn_wpos, POS_LEN);
	itr += POS_LEN;

	printf("next add weight of BN\n");
	float w_of_bn[5] = {0.1234,77.123,55.34,87.123,76};
	memcpy(code.W + 64 * 5*5*3*sizeof(float), w_of_bn, 5*sizeof(float));
	save_model<uint8_t>(code.W + 64 * 5*5*3*sizeof(float) , 5*sizeof(float), "./bn");

	printf("next add structure of SHORTCUT\n");
	memcpy(code.pos_s + POS_LEN*7,&itr,POS_LEN);
	Structure[itr++] = SHORTCUT;
	memcpy(Structure + itr, pool_label_out, LABEL_LEN);
	itr += LABEL_LEN;
	Structure[itr++] = 57/256;
	Structure[itr++] = 57%256;
	Structure[itr++] = 57/256;
	Structure[itr++] = 57%256;
	Structure[itr++] = 64/256;
	Structure[itr++] = 64%256;
	memcpy(Structure + itr, BN_label_out, LABEL_LEN);
	itr += LABEL_LEN;
	Structure[itr++] = 57/256;
	Structure[itr++] = 57%256;
	Structure[itr++] = 57/256;
	Structure[itr++] = 57%256;
	Structure[itr++] = 64/256;
	Structure[itr++] = 64%256;
	char SHORT_label_out[LABEL_LEN] = "lay5";
	memcpy(Structure + itr, SHORT_label_out, LABEL_LEN);
	itr += LABEL_LEN;
	Structure[itr++] = 0;

	printf("next add structure of FC\n");
	memcpy(code.pos_s + POS_LEN*8,&itr,POS_LEN);
	Structure[itr++] = FC_ID;
	memcpy(Structure + itr, SHORT_label_out, LABEL_LEN);
	itr += LABEL_LEN;
	Structure[itr++] = 57/256;
	Structure[itr++] = 57%256;
	Structure[itr++] = 57/256;
	Structure[itr++] = 57%256;
	Structure[itr++] = 64/256;
	Structure[itr++] = 64%256;
	char FC_label_out[LABEL_LEN] = "lay6";
	memcpy(Structure + itr, FC_label_out, LABEL_LEN);
	itr += LABEL_LEN;
	uint32_t weight_len = 10;
	memcpy(Structure + itr, &weight_len, sizeof(uint32_t));
	itr+=sizeof(uint32_t);
	Structure[itr++] = 0;
	uint32_t fc_wpos = 5*5*3*64*sizeof(float) + 5*sizeof(float);
	memcpy(Structure + itr, &fc_wpos, POS_LEN);
	itr += POS_LEN;

	/*add out layer*/
	memcpy(code.pos_s + POS_LEN*9,&itr,POS_LEN);
	Structure[itr++] = OUT_OP;
	printf("add out %s\n", FC_label_out);
	memcpy(Structure + itr, FC_label_out, LABEL_LEN);
	itr += LABEL_LEN;



	printf("next add weight of FC\n");
	float* w_of_fc = (float*)malloc(57*57*64*10*sizeof(float));
	for(int i = 0; i < 57*57*64*10;i ++){
		w_of_fc[i] = 1;
	}
	memcpy(code.W + 64 * 5*5*3*sizeof(float) + 5*sizeof(float), w_of_fc, 57*57*64*10*sizeof(float));
	save_model<uint8_t>(code.W + 64 * 5*5*3*sizeof(float) + 5*sizeof(float) , 57*57*64*10*sizeof(float), "./fc");

	printf("code size [%u]\n",itr);
	code.S = Structure;
	code.S_len =  INS_LEN*3 + COV_LEN + POOLING_LEN + RELU_LEN + BN_LEN + FC_LEN + SHORTCUT_LEN + OUT_LEN;
	uint8_t* out_stream = (uint8_t*)malloc(code.S_len + code.W_len + code.code_size*sizeof(uint32_t) + 4*sizeof(uint32_t));
	uint32_t out_stream_len;
	code_deserialize(code, out_stream, &out_stream_len);
	printf("start store data.....%u\n",out_stream[0]);
	save_model<uint8_t>(out_stream,out_stream_len,"./code_data.code");
	free(out_stream);
	free(code.W);
	free(code.S);
	free(code.pos_s);
	free(w_of_fc);
	

	/*gen_data*/
	struct Data data;
	data.label_size = 0;
	data.item_capacity = 6;
	data.data_len = 0;
	data.data_capacity = 8*5 + 225*225*3*sizeof(float);
	data.label = (uint8_t*)malloc(data.item_capacity*LABEL_LEN);
	data.pos = (uint8_t*)malloc(data.item_capacity*POS_LEN);
	data.data = (uint8_t*)malloc(data.data_capacity);
	uint8_t ll1[4],ll2[4];
	uint32_t l1_len = 4,l2_len = 4;
	memcpy(ll1, &l1, 4);
	memcpy(ll2, &l2, 4);
	add_data(&data, (uint8_t*)label1, LABEL_LEN, ll1, l1_len);
	add_data(&data, (uint8_t*)label2, LABEL_LEN, ll2, l2_len);
	float* image_data = (float*)malloc(225*225*3*sizeof(float));
	for(int i = 0; i < 225*225*3; i++){
		image_data[i] = i/225/225;
	}

	uint32_t image_len = 225*225*3*sizeof(float);
	add_data(&data, (uint8_t*)cov_label, LABEL_LEN, (uint8_t*)image_data, image_len);
	uint8_t* label_stream = (uint8_t*)malloc(data.label_size*LABEL_LEN);
	uint32_t label_stream_len;
	uint8_t* data_stream = (uint8_t*)malloc(data.label_size*POS_LEN + data.data_len);
	uint32_t data_stream_len;
	deserialize(data, label_stream, &label_stream_len, data_stream, &data_stream_len);
	save_model<uint8_t>(label_stream,label_stream_len*LABEL_LEN,"./label_data.data");
	save_model<uint8_t>(data_stream,data_stream_len,"./data_data.data");
	
	free(data.label);
	free(data.pos);
	free(data.data);
	free(image_data);
	free(label_stream);
	free(data_stream);
}	