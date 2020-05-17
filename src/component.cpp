#include "component.h"
uint32_t double_item(struct Data* data){

	uint8_t* new_label = (uint8_t*)malloc((data->item_capacity)*2*LABEL_LEN);
	printf("check point 3\n");
	uint8_t* new_pos = (uint8_t*)malloc(data->item_capacity*2*POS_LEN); 
	
	memcpy(new_label, data->label, (data->label_size)*LABEL_LEN);
	memcpy(new_pos, data->pos, (data->label_size)*POS_LEN);

	data->item_capacity = 2*data->item_capacity;
	free(data->label);
	free(data->pos);

	data->pos = new_pos;
	data->label = new_label;
	return 0;
}
uint32_t double_data(struct Data* data){
	uint8_t* new_data = (uint8_t*)malloc(data->data_capacity*2);
	memcpy(new_data, data->data, data->data_len);
	data->data_capacity = 2*data->data_capacity;
	free(data->data);
	data->data = new_data;
	return 0;
}
//query Data using label name, return index of label name.
uint32_t index_of(struct Data data, uint8_t* name, uint32_t name_len){
	uint8_t* label_tmp = data.label;
	uint32_t ans = data.label_size;
	for(int i = 0; i < data.label_size; i ++){
		if(memcmp(name, label_tmp + (i*LABEL_LEN), LABEL_LEN) == 0){
			ans = i; break;
		}
	}

	label_tmp = NULL;
	return ans;
	
}
//Use subscripts to access data, return stream of data
uint32_t get_data(struct Data data, uint32_t index, uint32_t* ele_size, uint8_t* ele){
	if(index >= data.label_size) return index;
	uint32_t pos;
	uint32_t data_size;
	memcpy(&pos, data.pos + (index * POS_LEN), POS_LEN);
	memcpy(&data_size, data.data + pos, sizeof(uint32_t));

	if(data_size > *ele_size){
		printf("error:cannot store full data\n");
		return data_size;
	}
	*ele_size = data_size;
	memcpy(ele, data.data + pos + sizeof(uint32_t), *ele_size);

	return 0;
}
uint32_t put_data(struct Data *data, uint32_t index, uint32_t ele_size, uint8_t* ele){
	if(index >= data->label_size) return index;
	uint32_t pos;
	uint32_t data_size;
	memcpy(&pos, data->pos + (index * POS_LEN), POS_LEN);
	memcpy(&data_size, data->data + pos, sizeof(uint32_t));
	if(ele_size > data_size){
		return data_size;
	}
	memcpy(data->data + pos, &ele_size, sizeof(uint32_t));
	memcpy(data->data + pos + sizeof(uint32_t), ele, ele_size);
	return 0;

}
/*TODO: Add boundary check*/
uint32_t add_data(struct Data* data, uint8_t *name, uint32_t name_len, uint8_t* data_in, uint32_t data_len){
	uint32_t idx = index_of(*data, name, name_len);
	if(idx == data->label_size){
		if(data->label_size + 1 > data->item_capacity){
			printf("expend data label\n");
			double_item(data);
		}
		printf("check point 0\n");
		memcpy(data->label + data->label_size * LABEL_LEN, name, name_len);
		memcpy(data->pos + data->label_size * POS_LEN, &data->data_len, POS_LEN);
		data->label_size += 1;
		printf("check point 1\n");
		if(data->data_len + sizeof(uint32_t) + data_len > data->data_capacity){
			printf("expend data\n");
			double_data(data);
		}
		memcpy(data->data + data->data_len, &data_len, sizeof(uint32_t));
		memcpy(data->data + data->data_len + sizeof(uint32_t), data_in, data_len);
		data->data_len += (data_len + sizeof(uint32_t));
	}else{
		return put_data(data, idx, data_len, data_in);
	}
	return 0;
}
/*TODO: Add boundary check*/
uint32_t deserialize(struct Data data, uint8_t* label, uint32_t* label_len, uint8_t* data_out, uint32_t* data_len){
	
	memcpy(label, data.label, data.label_size*LABEL_LEN);
	memcpy(data_out, data.pos, data.label_size*POS_LEN);
	memcpy(data_out + data.label_size*POS_LEN, data.data, data.data_len);
	*label_len = data.label_size;
	*data_len = data.label_size*POS_LEN + data.data_len;
	return 0;
}
/*TODO: Add boundary check*/
struct Data serialize(uint8_t* label, uint32_t label_len, uint8_t* data_in, uint32_t data_len){
	struct Data data;
	data.label_size = label_len;
	data.item_capacity = label_len;
	data.label = (uint8_t*) malloc(label_len*LABEL_LEN);
	data.pos = (uint8_t*) malloc(label_len*POS_LEN);
	memcpy(data.label, label, label_len*LABEL_LEN);
	memcpy(data.pos, data_in, label_len*POS_LEN);
	data.data_len = data_len - label_len*POS_LEN;
	data.data_capacity = data.data_len;
	data.data = (uint8_t*) malloc(data.data_len);
	memcpy(data.data, data_in + label_len*POS_LEN, data.data_len);
	return data;
}
uint32_t code_deserialize(struct Code code, uint8_t* stream, uint32_t* stream_len){
	uint32_t itr = 0;
	memcpy(stream + itr, &code.code_size, sizeof(uint32_t));
	itr += sizeof(uint32_t);
	memcpy(stream + itr, code.pos_s, code.code_size*sizeof(uint32_t));
	itr += code.code_size*sizeof(uint32_t);
	memcpy(stream + itr, &code.now_pos, sizeof(uint32_t));
	itr += sizeof(uint32_t);
	memcpy(stream + itr, &code.S_len, sizeof(uint32_t));
	itr += sizeof(uint32_t);
	memcpy(stream + itr, &code.W_len, sizeof(uint32_t));
	itr += sizeof(uint32_t);
	memcpy(stream + itr, code.S, code.S_len);
	itr += code.S_len;
	memcpy(stream + itr, code.W, code.W_len);
	itr += code.W_len;
	*stream_len = itr;
	return 0;
}
struct Code code_serialize(uint8_t* stream, uint32_t stream_len){
	struct Code code;
	uint32_t itr = 0;
	memcpy(&code.code_size, stream+itr, sizeof(uint32_t));
	itr += sizeof(uint32_t);
	code.pos_s = (uint8_t*) malloc(code.code_size*sizeof(uint32_t));
	memcpy(code.pos_s, stream + itr, code.code_size*sizeof(uint32_t));
	itr += code.code_size*sizeof(uint32_t);
	memcpy(&code.now_pos, stream + itr, sizeof(uint32_t));
	itr += sizeof(uint32_t);
	memcpy(&code.S_len, stream + itr, sizeof(uint32_t));
	itr += sizeof(uint32_t);
	memcpy(&code.W_len, stream + itr, sizeof(uint32_t));
	itr += sizeof(uint32_t);
	code.S = (uint8_t*) malloc(code.S_len);
	memcpy(code.S, stream + itr, code.S_len);
	itr += code.S_len;
	code.W = (uint8_t*) malloc(code.W_len);
	memcpy(code.W, stream + itr, code.W_len);
	itr += code.W_len;
	return code;
}
