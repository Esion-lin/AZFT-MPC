#ifndef _ELEMENT_H_
#define _ELEMENT_H_
#define INS_LEN 19
#define OUT_LEN 6
#define COV_LEN 32
#define POOLING_LEN 25
#define RELU_LEN 22
#define BN_LEN 22
#define FC_LEN 26
#define SHORTCUT_LEN 29
/*
S & W: 

Binary\Arithmatic\Logic s:{type:1,label1:LABEL_LEN, type:1, label2:LABEL_LEN, type:1, label3:LABEL_LEN, type:1}

bratch s:{type:1,label1:LABEL_LEN, goto:sizeof(int)}
cov s:{
	type:1
	in_label:LABEL_LEN,shape:2*2*2
	out_label:LABEL_LEN
	size_of_kenerl:2
	is_pending:1
	stride:1
	shape_of_kennerl:l,w,h = 2+2+2 = 6
	ex_for_tee:1
	W_pos:POS_LEN
}	
w:{
	array:l*w*h*size_of_kenerl*sizeof(float)
}

pooling s:{
	type:1
	in_label:LABEL_LEN,shape:2*2*2
	out_label:LABEL_LEN
	pooling_type:1
	is_pending:1
	stride:1
	shape_of_kennerl:l,w = 4
	ex_for_tee:1
}
ReLU s:{
	type:1
	in_label:LABEL_LEN,shape:2*2*2
	out_label:LABEL_LEN
	alpha:sizeof(float)
	ex_for_tee:1
}

BN s:{
	type:1
	in_label:LABEL_LEN,shape:2*2*2
	out_label:LABEL_LEN
	ex_for_tee:1
	W_pos:POS_LEN
}
w: {
	mu:sizeof(float)
	sigma:sizeof(float)
	gammma:sizeof(float)
	beta:sizeof(float)
	epsilon:sizeof(float)
}
FC s:{
	type:1
	in_label:LABEL_LEN,shape:2*2*2
	out_label:LABEL_LEN
	weight_width:sizeof(int);
	ex_for_tee:1
	W_pos:POS_LEN
}
w: {
	array:weight_width*last_layer_output_size
}
SHORTCUT s:{
	type:1
	in_label1:LABEL_LEN,shape:2*2*2
	in_label2:LABEL_LEN,shape:2*2*2
	out_label:LABEL_LEN
	ex_for_tee:1
}
*/
#define OUT_OP 0
//Binary
#define AND_OP 1
#define NOT_OP 2
#define OR_OP 3
#define NAND_OP 4
#define SHF_LE 5
#define SHF_RI 6


//Arithmatic
#define ADD_OP 11
#define SUB_OP 12
#define MUL_OP 13
#define DIV_OP 14
#define POW_OP 15
//Logic
#define GREAT_OP 21
#define GE_OP 22
#define LESS_OP 23
#define LE_OP 24
#define EQ_OP 25

#define GOTO 99

//defined op
#define COV 100 
#define POOLING 101 
#define RELU 102 
#define BN_ID 103 
#define FC_ID 104
#define SHORTCUT 105

//length of S
#define SIZE_COV 11 
#define SIZE_POOLING 8 
#define SIZE_RELU (3 + sizeof(float))
#define SIZE_BN 3
#define SIZE_FC (3 + sizeof(int))
#define SIZE_SHORTCUT 4

/*
fixed data(without edition)
data{
	label: 5
	pos: 4
	size: 4
}
content{
	content:array_size*sizeof(int)
}
*/
#define BOOL_TP		0
#define INT_TP 		1
#define FLOAT_TP	2

#define DATA_SIZE 	sizeof(uint32_t)
#define ARRAY_SIZE 	sizeof(uint32_t)

#define LABEL_LEN 5
#define POS_LEN 	sizeof(uint32_t)

#define convert(des, src)	\
		memcpy(&des, src, 4)

#include "tuple_c.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
struct Data{
	/*size-fixed labels
	_ _ _ _ _ _ _ _ _ _ _ _ _ 
	^         ^         ^
	label[1]  label[2]  label[3]
	*/
	uint8_t * label;
	uint32_t label_size;
	uint32_t item_capacity;	
	/*size-fixed poss
	_ _ _ _ _ _ _ _ _ _ _ _ _ 
	^       ^       ^
	pos[1]  pos[2]  pos[3]
	*/
	uint8_t * pos;
	/*size-variable data
	_ _ _ _ _ _ _ _ _ _ _ _ _ 
	^       	^         ^
	*pos[1]     *pos[2]   *pos[3]
	
	*/
	uint8_t * data;
	uint32_t data_len;
	uint32_t data_capacity;

};
/*make sure the code is fixed*/
struct Code{
	
	uint32_t code_size; 
	/*4 bits, store the position of cmd[i] in S*/
	uint8_t* pos_s;
	/*now cmd index*/
	uint32_t now_pos;
	uint8_t* S;
	uint32_t S_len;
	uint8_t* W;
	uint32_t W_len;

};
struct Link_list{
	uint8_t label[LABEL_LEN];
	uint8_t* data;
	uint32_t data_len;
	struct Link_list* next;
};
uint32_t double_item(struct Data* data);
uint32_t double_data(struct Data* data);
uint32_t index_of(struct Data data, uint8_t* name, uint32_t name_len);
uint32_t get_data(struct Data data, uint32_t index, uint32_t* ele_size, uint8_t* ele);
uint32_t put_data(struct Data *data, uint32_t index, uint32_t ele_size, uint8_t* ele);
uint32_t add_data(struct Data* data, uint8_t *name, uint32_t name_len, uint8_t* data_in, uint32_t data_len);
uint32_t deserialize(struct Data data, uint8_t* label, uint32_t* label_len, uint8_t* data_out, uint32_t* data_len);
struct Data serialize(uint8_t* label, uint32_t label_len, uint8_t* data_in, uint32_t data_len);
uint32_t code_deserialize(struct Code code, uint8_t* stream, uint32_t* stream_len);
struct Code code_serialize(uint8_t* stream, uint32_t stream_len);
uint32_t run_code(struct Data* data, struct Code* code, struct Tuple* last_tp, uint8_t* last_label);
int test();
#endif