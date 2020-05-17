#ifndef _COMPONENT_H_
#define _COMPONENT_H_
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
uint32_t double_item(struct Data* data);
uint32_t double_data(struct Data* data);
uint32_t index_of(struct Data data, uint8_t* name, uint32_t name_len);
uint32_t get_data(struct Data data, uint32_t index, uint32_t* ele_size, uint8_t* ele);
uint32_t put_data(struct Data *data, uint32_t index, uint32_t ele_size, uint8_t* ele);
uint32_t add_data(struct Data* data, uint8_t *name, uint32_t name_len, uint8_t* data_in, uint32_t data_len);
uint32_t deserialize(struct Data data, uint8_t* label, uint32_t* label_len, uint8_t* data_out, uint32_t* data_len);
uint32_t serialize(struct Data* data, uint8_t* label, uint32_t label_len, uint8_t* data_in, uint32_t data_len);
uint32_t code_deserialize(struct Code code, uint8_t* stream, uint32_t* stream_len);
struct Code code_serialize(uint8_t* stream, uint32_t stream_len);

#endif