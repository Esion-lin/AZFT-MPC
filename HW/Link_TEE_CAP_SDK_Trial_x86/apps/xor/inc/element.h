#ifndef _ELEMENT_H_
#define _ELEMENT_H_
/*
S & W: 
Binary\Arithmatic\Logic s:{type:1,label1:LABEL_LEN, type:1, label2:LABEL_LEN, type:1, label3:LABEL_LEN, type:1}
cov s:{
	type:1
	in_label:LABEL_LEN,shape:
	out_label:LABEL_LEN
	size_of_kenerl:2
	is_pending:1
	stride:1
	shape_of_kennerl:l,w,h = 2+2+2 = 6
	ex_for_tee:1
	data_pos:POS_LEN
}	
w:{
	array:l*w*h*size_of_kenerl*sizeof(float)
}

pooling s:{
	type:1
	in_label:LABEL_LEN
	out_label:LABEL_LEN
	pooling_type:1
	is_pending:1
	stride:1
	shape_of_kennerl:l,w = 4
	ex_for_tee:1
}
ReLU s:{
	type:1
	in_label:LABEL_LEN
	out_label:LABEL_LEN
	alpha:sizeof(float)
}

BN s:{
	type:1
	in_label:LABEL_LEN
	out_label:LABEL_LEN
	ex_for_tee:1
	data_pos:POS_LEN
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
	in_label:LABEL_LEN
	out_label:LABEL_LEN
	weight_width:sizeof(int);
	ex_for_tee:1
	data_pos:POS_LEN
}
w: {
	array:weight_width*last_layer_output_size
}
SHORTCUT s:{
	type:1
	in_label1:LABEL_LEN
	in_label2:LABEL_LEN
	out_label:LABEL_LEN
	ex_for_tee:1
}
*/
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

//defined op
#define COV 0 
#define POOLING 1 
#define RELU 2 
#define BN_ID 3 
#define FC_ID 4
#define SHORTCUT 5

//length of S
#define SIZE_COV 11 
#define SIZE_POOLING 8 
#define SIZE_RELU (3 + sizeof(baseInt))
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
#define INT 	1
#define FLOAT 	2
#define INT_ARRAY	3
#define FLOAT_ARRAY 4
#define DATA_SIZE 	sizeof(uint32_t)
#define ARRAY_SIZE 	sizeof(uint32_t)

#define LABEL_LEN 5
#define POS_LEN 	sizeof(uint32_t)

struct Data{
	uint8_t * label;
	uint32_t label_size;
	uint8_t * pos;
	uint8_t * data;
	uint32_t data_len;

};
/*make sure the code is fixed*/
struct Code{
	
	uint32_t code_size; 
	uint8_t* pos_s;
	uint8_t* S;
	uint32_t S_pos;
	uint32_t S_len;
	uint8_t* W;
	uint32_t W_pos;
	uint32_t W_len;

};
//query Data using label name, return index of label name.
uint32_t index_of(Data data, char* name, uint32_t name_len){
	uint8_t * name_tmp = (uint8_t*)melloc(LABEL_LEN);
	memcpy(name_tmp, name, name_len);
	for(int i = name_len; i < LABEL_LEN; i++){
		name_tmp[i] = '\0';
	}
	uint8_t* label_tmp = data.label;
	uint32_t ans = data.label_size;
	for(int i = 0; i < data.label_size; i ++){
		if(memcmp(name_tmp, label_tmp + (i*LABEL_LEN), LABEL_LEN) == 0){
			ans = i; break;
		}
	}
	free(name_tmp);name_tmp = NULL;label_tmp = NULL;
	return ans;
	
}
//Use subscripts to access data, return stream of data
uint32_t get_data(Data data, uint32_t index, uint32_t &ele_size, uint8_t* ele){
	if(index >= data.label_size) return index+1;
	uint32_t pos;
	memcpy(&pos, data.pos + (index * POS_LEN), POS_LEN);
	memcpy(&ele_size, data.data + pos, sizeof(uint32_t));
	memcpy(ele, data.data + pos + sizeof(uint32_t), ele_size);
	return 0;
}
/*TODO: Add boundary check*/
uint32_t add_data(Data &data, char *name, uint32_t name_len, uint8_t * data_in, uint32_t data_len){
	memcpy(data.label + data.label_size * LABEL_LEN, name, name_len);

	for(int i = data.label_size * LABEL_LEN + name_len; i < (data.label_size + 1)* LABEL_LEN; i ++){
		data.label[i] = '\0';
	}
	memcpy(data.pos + data.label_size * POS_LEN, &data.data_len, POS_LEN);
	data.label_size += 1;
	memcpy(data.data + data.data_len, &data_len, sizeof(uint32_t));
	memcpy(data.data + data.data_len + sizeof(uint32_t), data_in, data_len);
	data.data_len += (data_len + sizeof(uint32_t));
	return 0;
}
/*TODO: Add boundary check*/
uint32_t deserialize(Data data, uint8_t* label, uint32_t &label_len, uint8_t* data_out, uint32_t &data_len){
	memcpy(label, data.label, data.label_size*LABEL_LEN);
	memcpy(data_out, data.pos, data.label_size*POS_LEN);
	memcpy(data_out + data.label_size*POS_LEN, data.data, data.data_len);
	label_len = data.label_size;
	data_len = data.label_size*POS_LEN + data.data_len;
	return 0;
}
/*TODO: Add boundary check*/
uint32_t serialize(Data &data, uint8_t *label, uint32_t label_len, uint8_t *data_in, uint32_t data_len){
	data.label_size = label_len;
	memcpy(data.label, label, label_len*LABEL_LEN);
	memcpy(data.pos, data_in, label_len*POS_LEN);
	data.data_len = data_len - label_len*POS_LEN;
	memcpy(data.data, data_in + label_len*POS_LEN, data.data_len);
	return 0;
}
uint32_t run_code(Data &data, Code &code){

}
#endif