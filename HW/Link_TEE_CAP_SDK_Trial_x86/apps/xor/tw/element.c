#include "element.h"
uint32_t double_item(struct Data* data){

	uint8_t* new_label = (uint8_t*)malloc((data->item_capacity)*2*LABEL_LEN);
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
			//printf("expend data label\n");
			double_item(data);
		}
		memcpy(data->label + data->label_size * LABEL_LEN, name, name_len);
		memcpy(data->pos + data->label_size * POS_LEN, &data->data_len, POS_LEN);
		data->label_size += 1;
		if(data->data_len + sizeof(uint32_t) + data_len > data->data_capacity){
			//printf("expend data\n");
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
	stream = (uint8_t*)malloc(code.S_len + code.W_len + code.code_size*sizeof(uint32_t) + 4*sizeof(uint32_t));
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

/*decode protocol*/
/*TODO: Add boundary check*/
uint32_t run_code(struct Data* data, struct Code* code, struct Tuple* last_tp, uint8_t* last_label){
	/*optimise store*/
	/*query nower idex of S*/
	if(code->now_pos >= code->code_size){
		printf("Error. Try to run the code has ended\n");
		return 300;
	}
	uint32_t now_pos;
	memcpy(&now_pos, code->pos_s + (POS_LEN*code->now_pos), POS_LEN);
	code->now_pos ++;
	//printf("now_pos is %u, S[%d]type is %u\n",code->now_pos,now_pos,code->S[now_pos]);
	if(code->S[now_pos] < 99){
		/*recover data of label1\label2\label3*/
		uint8_t label1[LABEL_LEN];
		uint32_t type1;
		uint8_t label2[LABEL_LEN];
		uint32_t type2;
		uint8_t label3[LABEL_LEN];
		uint32_t type3;
		uint8_t data1[sizeof(uint32_t)];
		uint32_t data1_len = sizeof(uint32_t);
		uint8_t data2[sizeof(uint32_t)];
		uint32_t data2_len = sizeof(uint32_t);
		
		memcpy(label1, code->S + now_pos + 1, LABEL_LEN);
		type1 = code->S[now_pos + 1 + LABEL_LEN];
		memcpy(label2, code->S + now_pos + 2 + LABEL_LEN, LABEL_LEN);
		type2 = code->S[now_pos + 2 + 2*LABEL_LEN];
		memcpy(label3, code->S + now_pos + 3 + 2 * LABEL_LEN, LABEL_LEN);
		type3 = code->S[now_pos + 3 + 3*LABEL_LEN];
		if(0 != get_data(*data, index_of(*data, label1, LABEL_LEN), &data1_len, data1)){
			printf("error[label1]: cannot get right data in Binary/Arithmatic/Logic cmd.\n");
			return 100;
		}
		if(0 != get_data(*data, index_of(*data, label2, LABEL_LEN), &data2_len, data2)){
			printf("error[label2]: cannot get right data in Binary/Arithmatic/Logic cmd.\n");
			return 100;
		}
		// if(type3 < (type2>type1?type2:type1)){
		// 	printf("error[structure]: cannot convert bigger size data to smaller size data.\n");
		// 	return 101;
		// }
		if(code->S[now_pos] < 10){
			//binary
			uint8_t data3[sizeof(uint32_t)];
			uint32_t data3_len = sizeof(uint32_t);
			uint32_t tar,src1,src2;
			convert(src1,data1);
			convert(src2,data2);
			switch(code->S[now_pos]){
				case AND_OP:{
					tar = src1 & src2;
				}
				break;
				case NOT_OP:{
					tar = ~src1;
				}
				break;
				case OR_OP:{
					tar = src1 | src2;
				}
				break;
				case NAND_OP:{
					tar = src1 ^ src2;
				}
				break;
				case SHF_LE:{
					tar = src1 << src2;
				}
				break;
				case SHF_RI:{
					tar = src1 >> src2;
				}
				break;

			}
			memcpy(data3, &tar, sizeof(uint32_t));
			add_data(data, label3, LABEL_LEN, data3, sizeof(uint32_t));

		}else if(code->S[now_pos] < 20){
			//Arithmaic
			uint8_t data3[sizeof(uint32_t)];
			uint32_t data3_len = sizeof(uint32_t);
			float tar,src1,src2;
			convert(src1,data1);
			convert(src2,data2);
			switch(code->S[now_pos]){
				
				case ADD_OP:{
					tar = src1 + src2;
				}
				break;
				case SUB_OP:{
					tar = src1 - src2;
				}
				break;
				case MUL_OP:{
					tar = src1 * src2;
				}
				break;
				case DIV_OP:{
					tar = src1 / src2;
				}
				break;
				case POW_OP:{
					tar = pow(src1, src2);
				}
				break;
			}
			memcpy(data3, &tar, sizeof(float));
			add_data(data, label3, LABEL_LEN, data3, sizeof(float));
		}else{
			//Logic

			uint8_t data3[1];
			uint32_t data3_len = 1;
			float src1,src2;bool tar;
			convert(src1,data1);
			convert(src2,data2);
			switch(code->S[now_pos]){
				case GREAT_OP:{
					tar = src1 > src2;
				}
				break;
				case GE_OP:{
					tar = src1 >= src2;
				}
				break;
				case LESS_OP:{
					tar = src1 < src2;
				}
				break;
				case LE_OP:{
					tar = src1 <= src2;
				}
				break;
				case EQ_OP:{
					tar = src1 == src2;
				}
				break;
			}
			data3[0] = tar;
			printf("check mac successfully. get output: %d\n",tar);
			add_data(data, label3, LABEL_LEN, data3, 1);
		}
		
	}
	else if(code->S[now_pos] == 99){
		printf("do goto\n");
		uint8_t label[LABEL_LEN];
		uint8_t label_data[1];
		uint32_t label_data_len = 1;
		uint32_t tar_pos;
		memcpy(label, code->S + now_pos + 1, LABEL_LEN);
		if(0 != get_data(*data, index_of(*data, label, LABEL_LEN), &label_data_len, label_data)){
			printf("error[label]: cannot get right data in Binary/Arithmatic/Logic cmd.\n");
			return 100;
		}
		if(label_data[0] == 1){
			memcpy(&tar_pos, code->S + now_pos + 1 + LABEL_LEN, sizeof(uint32_t));
			code->now_pos = tar_pos;
		}
		
	}
	else{
		switch(code->S[now_pos]){
			case COV:{
				printf("do cov\n");
				/*recover the structure*/
				uint32_t step = now_pos + 1;
				uint8_t label[LABEL_LEN];
				struct Shape data_shape;
				uint8_t out_label[LABEL_LEN];
				uint32_t kernel_size;
				bool is_pending;
				uint32_t stride;
				struct Shape kernel_shape;
				uint8_t ex_for_tee;
				uint32_t W_pos;
				memcpy(label, code->S + step, LABEL_LEN);
				step += LABEL_LEN;
				data_shape.l = code->S[step]*256 + code->S[step+1];
				step += 2;
				data_shape.w = code->S[step]*256 + code->S[step+1];
				step += 2;
				data_shape.h = code->S[step]*256 + code->S[step+1];
				step += 2;
				data_shape.size = data_shape.l * data_shape.w * data_shape.h;
				memcpy(out_label, code->S + step, LABEL_LEN);
				step += LABEL_LEN;
				kernel_size = code->S[step++]*256 + code->S[step++]; 
				is_pending = (code->S[step++] == 1);
				stride = code->S[step++];
				kernel_shape.l = code->S[step]*256 + code->S[step+1];
				step += 2;
				kernel_shape.w = code->S[step]*256 + code->S[step+1];
				step += 2;
				kernel_shape.h = code->S[step]*256 + code->S[step+1];
				step += 2;
				kernel_shape.size = kernel_shape.l * kernel_shape.w * kernel_shape.h;
				ex_for_tee = code->S[step++];
				memcpy(&W_pos, code->S + step, POS_LEN);
				step += POS_LEN;
				/*recover the data*/
				struct Tuple image;
				if(memcmp(last_label, label, LABEL_LEN) == 0){
					image = *last_tp;
				}else{
					image.shape = data_shape;
					image.data = (float*)malloc(data_shape.size * sizeof(float));

					uint32_t check_len = data_shape.size * sizeof(float);
					uint8_t* check_data = (uint8_t*)malloc(check_len);
					if(0!=get_data(*data, index_of(*data, label, LABEL_LEN), &check_len, check_data)){
						printf("error[label]: cannot get right data in cov cmd.\n");
						free(check_data);
						free(image.data);
						return 100;
					}
					if(check_len != data_shape.size * sizeof(float)){
						printf("data error.shape not match\n");
						free(check_data);
						free(image.data);
						return 200;
					}
					memcpy(image.data,check_data,check_len);
					free(check_data);
				}
				
				/*recover the weight*/
				struct Tuple* kernels = (struct Tuple*)malloc(sizeof(struct Tuple)*kernel_size);
				for(int i = 0; i < kernel_size; i++){
					kernels[i].shape = kernel_shape;
					kernels[i].data = (float*)malloc(kernel_shape.size*sizeof(float));
					memcpy(kernels[i].data, code->W + W_pos + i*kernel_shape.size*sizeof(float), kernel_shape.size*sizeof(float));
				}
				struct Tuple ans = convolution_embed(image, kernels, kernel_size, is_pending, stride);
				if(image.data != last_tp->data && last_tp->data != NULL){
					free(last_tp->data);
					last_tp->data = NULL;
				}
				/**/
				if(ex_for_tee != 1){
					
					last_tp->shape = ans.shape;
					last_tp->data = ans.data;
					memcpy(last_label, out_label, LABEL_LEN); 
				}else{

					add_data(data, out_label, LABEL_LEN, (uint8_t*)ans.data, ans.shape.l*ans.shape.w*ans.shape.h*sizeof(float));
					free(ans.data);
					last_tp->data = NULL;	
				}
				/*free image and kernels*/
				free(image.data);image.data = NULL;
				for(int i = 0; i < kernel_size; i++){
					free(kernels[i].data); 
				}
				free(kernels);
				kernels = NULL;

			}
			break;
			case POOLING:{
				printf("do pooling\n");
				uint32_t step = now_pos + 1;
				uint8_t label[LABEL_LEN];
				struct Shape data_shape;
				uint8_t out_label[LABEL_LEN];
				uint32_t pooling_type;
				bool is_pending;
				uint32_t stride;
				struct Shape kernel_shape;
				uint8_t ex_for_tee;
				memcpy(label, code->S + step, LABEL_LEN);
				step += LABEL_LEN;
				data_shape.l = code->S[step++]*256 + code->S[step++];
				data_shape.w = code->S[step++]*256 + code->S[step++];
				data_shape.h = code->S[step++]*256 + code->S[step++];
				data_shape.size = data_shape.l * data_shape.w * data_shape.h;
				//printf("[%u, %u, %u]",data_shape.l , data_shape.w , data_shape.h);
				memcpy(out_label, code->S + step, LABEL_LEN);
				step += LABEL_LEN;
				pooling_type = code->S[step++];
				is_pending = (code->S[step++] == 1);
				stride = code->S[step++];
				kernel_shape.l = code->S[step++]*256 + code->S[step++];
				kernel_shape.w = code->S[step++]*256 + code->S[step++];
				kernel_shape.h = 1;
				kernel_shape.size = kernel_shape.l * kernel_shape.w * kernel_shape.h;
				ex_for_tee = code->S[step++];

				/*recover the data*/
				struct Tuple image;
				if(memcmp(last_label, label, LABEL_LEN) == 0){
					image = *last_tp;
				}else{
					image.shape = data_shape;
					image.data = (float*)malloc(data_shape.size * sizeof(float));

					uint32_t check_len = data_shape.size * sizeof(float);
					
					uint8_t* check_data = (uint8_t*)malloc(check_len);
					if(0!=get_data(*data, index_of(*data, label, LABEL_LEN), &check_len, check_data)){
						printf("error[label]: cannot get right data in cov cmd.\n");
						free(check_data);
						free(image.data);
						return 100;
					}
					if(check_len != data_shape.size * sizeof(float)){
						printf("data error.shape not match\n");
						free(check_data);
						free(image.data);
						return 200;
					}
					memcpy(image.data,check_data,check_len);
					free(check_data);
				}
				/*do pooling*/
				struct Tuple ans = pooling(image, kernel_shape, is_pending, stride, pooling_type);
				if(image.data != last_tp->data && last_tp->data != NULL){
					free(last_tp->data);
					last_tp->data = NULL;
				}
				/**/
				if(ex_for_tee != 1){
					last_tp->shape = ans.shape;
					last_tp->data = ans.data;
					memcpy(last_label, out_label, LABEL_LEN); 
				}else{
					add_data(data, out_label, LABEL_LEN, (uint8_t*)ans.data, ans.shape.l*ans.shape.w*ans.shape.h*sizeof(float));
					free(ans.data);
					last_tp->data = NULL;
				}
				/*free image*/
				free(image.data);image.data = NULL;
			}
			break;
			case RELU:{
				printf("do ReLU\n");
				uint8_t label[LABEL_LEN];
				uint8_t out_label[LABEL_LEN];
				struct Shape data_shape;
				float alpha;
				uint8_t ex_for_tee;
				uint32_t step = now_pos + 1;
				memcpy(label, code->S + step, LABEL_LEN);
				step += LABEL_LEN;
				data_shape.l = code->S[step++]*256 + code->S[step++];
				data_shape.w = code->S[step++]*256 + code->S[step++];
				data_shape.h = code->S[step++]*256 + code->S[step++];
				data_shape.size = data_shape.l * data_shape.w * data_shape.h;
				//printf("[%u, %u, %u]",data_shape.l , data_shape.w , data_shape.h);
				memcpy(out_label, code->S + step, LABEL_LEN);
				step += LABEL_LEN;
				memcpy(&alpha, code->S + step, sizeof(float));
				step += sizeof(float);
				ex_for_tee = code->S[step++];

				/*recover the data*/
				struct Tuple image;
				if(memcmp(last_label, label, LABEL_LEN) == 0){
					image = *last_tp;
				}else{

					image.shape = data_shape;
					image.data = (float*)malloc(data_shape.size * sizeof(float));

					uint32_t check_len = data_shape.size * sizeof(float);
					uint8_t* check_data = (uint8_t*)malloc(check_len);
					if(0!=get_data(*data, index_of(*data, label, LABEL_LEN), &check_len, check_data)){
						printf("error[label]: cannot get right data in cov cmd.\n");
						free(check_data);
						free(image.data);
						return 100;
					}

					if(check_len != data_shape.size * sizeof(float)){
						printf("data error.shape not match\n");
						free(check_data);
						free(image.data);
						return 200;
					}
					memcpy(image.data,check_data,check_len);
					free(check_data);
				}
				ReLU(&image, false, alpha);
				
				if(image.data != last_tp->data && last_tp->data != NULL){

					free(last_tp->data);
					last_tp->data = NULL;
				}

				if(ex_for_tee != 1){
					last_tp->shape = image.shape;
					last_tp->data = image.data;
					memcpy(last_label, out_label, LABEL_LEN); 
				}else{
					add_data(data, out_label, LABEL_LEN, (uint8_t*)image.data, image.shape.size*sizeof(float));	
					free(image.data);image.data = NULL;last_tp->data = NULL;
				}


			}
			break;
			case BN_ID:{
				printf("do BN\n");
				uint8_t label[LABEL_LEN];
				struct Shape data_shape;
				uint8_t out_label[LABEL_LEN];
				uint8_t ex_for_tee;
				uint32_t W_pos;
				
				uint32_t step = now_pos + 1;
				memcpy(label, code->S + step, LABEL_LEN);
				step += LABEL_LEN;
				data_shape.l = code->S[step++]*256 + code->S[step++];
				data_shape.w = code->S[step++]*256 + code->S[step++];
				data_shape.h = code->S[step++]*256 + code->S[step++];
				data_shape.size = data_shape.l * data_shape.w * data_shape.h;
				memcpy(out_label, code->S + step, LABEL_LEN);
				step += LABEL_LEN;
				ex_for_tee = code->S[step++];
				memcpy(&W_pos, code->S + step, POS_LEN);
				step += POS_LEN;
				
				/*recover the data*/
				struct Tuple image;
				if(memcmp(last_label, label, LABEL_LEN) == 0){
					image = *last_tp;
				}else{
					image.shape = data_shape;
					image.data = (float*)malloc(data_shape.size * sizeof(float));

					uint32_t check_len = data_shape.size * sizeof(float);
					uint8_t* check_data = (uint8_t*)malloc(check_len);
					if(0!=get_data(*data, index_of(*data, label, LABEL_LEN), &check_len, check_data)){
						printf("error[label]: cannot get right data in cov cmd.\n");
						free(check_data);
						free(image.data);
						return 100;
					}
					if(check_len != data_shape.size * sizeof(float)){
						printf("data error.shape not match\n");
						free(check_data);
						free(image.data);
						return 200;
					}
					memcpy(image.data,check_data,check_len);
					free(check_data);
				}
				/*recover weight*/
				float bn_weight_test[5];
				memcpy(bn_weight_test, code->W + W_pos,5*sizeof(float));
				//printf("[%f,%f,%f,%f,%f]\n",bn_weight_test[0],bn_weight_test[1],bn_weight_test[2],bn_weight_test[3],bn_weight_test[4]);
 				BN_for_test(&image, bn_weight_test[0], bn_weight_test[1], bn_weight_test[2], bn_weight_test[3], bn_weight_test[4]);

 				if(image.data != last_tp->data && last_tp->data != NULL){
					free(last_tp->data);
					last_tp->data = NULL;
				}
				if(ex_for_tee != 1){
					last_tp->shape = image.shape;
					last_tp->data = image.data;
					memcpy(last_label, out_label, LABEL_LEN); 
				}else{
					add_data(data, out_label, LABEL_LEN, (uint8_t*)image.data, image.shape.size*sizeof(float));	
					free(image.data);image.data = NULL;last_tp->data = NULL;
				}

			}
			break;
			case FC_ID:{
				printf("do FC\n");
				uint8_t label[LABEL_LEN];
				struct Shape data_shape;
				uint8_t out_label[LABEL_LEN];
				uint32_t weight_width;
				uint8_t ex_for_tee;
				uint32_t W_pos;
				uint32_t step = now_pos + 1;
				memcpy(label, code->S + step, LABEL_LEN);
				step += LABEL_LEN;
				data_shape.l = code->S[step++]*256 + code->S[step++];
				data_shape.w = code->S[step++]*256 + code->S[step++];
				data_shape.h = code->S[step++]*256 + code->S[step++];
				data_shape.size = data_shape.l * data_shape.w * data_shape.h;
				//printf("[%u, %u, %u]\n", data_shape.l, data_shape.w, data_shape.h);
				memcpy(out_label, code->S + step, LABEL_LEN);
				step += LABEL_LEN;
				memcpy(&weight_width, code->S+step, sizeof(uint32_t));
				step += sizeof(uint32_t);
				ex_for_tee = code->S[step++];
				memcpy(&W_pos, code->S + step, POS_LEN);
				step += POS_LEN;

				/*recover the data*/
				struct Tuple image;
				if(memcmp(last_label, label, LABEL_LEN) == 0){
					image = *last_tp;
				}else{
					image.shape = data_shape;
					image.data = (float*)malloc(data_shape.size * sizeof(float));

					uint32_t check_len = data_shape.size * sizeof(float);
					uint8_t* check_data = (uint8_t*)malloc(check_len);
					if(0!=get_data(*data, index_of(*data, label, LABEL_LEN), &check_len, check_data)){
						printf("error[label]: cannot get right data in cov cmd.\n");
						free(check_data);
						free(image.data);
						return 100;
					}
					if(check_len != data_shape.size * sizeof(float)){
						printf("data error.shape not match\n");
						free(check_data);
						free(image.data);
						return 200;
					}
					memcpy(image.data,check_data,check_len);
					free(check_data);
				}
				/*recover weight*/
				float* weight = (float*)malloc(weight_width*data_shape.size*sizeof(float));
				memcpy(weight, code->W + W_pos, weight_width*data_shape.size*sizeof(float));
				float output[10];

				FC_f(image, output, weight, weight_width);
				printf("check mac successfully. get output: [\n");
				for(int i = 0; i < 10; i ++){
					printf("%f ", output[i]);
				}
				printf("]\n");
				free(weight);
				
			}
			break;
			case SHORTCUT:{
				printf("do shortcut\n");
				uint8_t label1[LABEL_LEN];
				struct Shape data_shape1;
				uint8_t label2[LABEL_LEN];
				struct Shape data_shape2;
				uint8_t out_label[LABEL_LEN];
				uint8_t ex_for_tee;
				uint32_t step = now_pos + 1;
				memcpy(label1, code->S + step, LABEL_LEN);
				step += LABEL_LEN;
				data_shape1.l = code->S[step++]*256 + code->S[step++];
				data_shape1.w = code->S[step++]*256 + code->S[step++];
				data_shape1.h = code->S[step++]*256 + code->S[step++];
				data_shape1.size = data_shape1.l * data_shape1.w * data_shape1.h;
				memcpy(label2, code->S + step, LABEL_LEN);
				step += LABEL_LEN;
				data_shape2.l = code->S[step++]*256 + code->S[step++];
				data_shape2.w = code->S[step++]*256 + code->S[step++];
				data_shape2.h = code->S[step++]*256 + code->S[step++];
				data_shape2.size = data_shape2.l * data_shape2.w * data_shape2.h;
				if(data_shape1.size != data_shape2.size){
					printf("illegal shapes of two layer\n");
					return 400;
				}
				memcpy(out_label, code->S + step, LABEL_LEN);
				step += LABEL_LEN;
				ex_for_tee = code->S[step++];
				/*recover the data*/
				struct Tuple image1;
				if(memcmp(last_label, label1, LABEL_LEN) == 0){
					image1 = *last_tp;
				}else{
					image1.shape = data_shape1;
					image1.data = (float*)malloc(data_shape1.size * sizeof(float));

					uint32_t check_len = data_shape1.size * sizeof(float);
					uint8_t* check_data = (uint8_t*)malloc(check_len);
					if(0!=get_data(*data, index_of(*data, label1, LABEL_LEN), &check_len, check_data)){
						printf("error[label]: cannot get right data in cov cmd.\n");
						free(check_data);
						free(image1.data);
						return 100;
					}
					if(check_len != data_shape1.size * sizeof(float)){
						printf("data error.shape not match\n");
						free(check_data);
						free(image1.data);
						return 200;
					}
					memcpy(image1.data,check_data,check_len);
					free(check_data);
				}
				struct Tuple image2;
				if(memcmp(last_label, label2, LABEL_LEN) == 0){
					image2 = *last_tp;
				}else{
					image2.shape = data_shape2;
					image2.data = (float*)malloc(data_shape2.size * sizeof(float));

					uint32_t check_len = data_shape2.size * sizeof(float);
					uint8_t* check_data = (uint8_t*)malloc(check_len);
					if(0!=get_data(*data, index_of(*data, label2, LABEL_LEN), &check_len, check_data)){
						printf("error[label]: cannot get right data in cov cmd.\n");
						free(check_data);
						free(image2.data);
						return 100;
					}
					if(check_len != data_shape2.size * sizeof(float)){
						printf("data error.shape not match\n");
						free(check_data);
						free(image2.data);
						return 200;
					}
					memcpy(image2.data,check_data,check_len);
					free(check_data);
				}
				struct Tuple ans = add_other(image1, image2);
				if(image1.data != last_tp->data && image2.data != last_tp->data && last_tp->data != NULL){
					free(last_tp->data);
					last_tp->data = NULL;
				}
				/**/
				if(ex_for_tee != 1){
					last_tp->shape = ans.shape;
					last_tp->data = ans.data;
					memcpy(last_label, out_label, LABEL_LEN); 
				}else{
					add_data(data, out_label, LABEL_LEN, (uint8_t*)ans.data, ans.shape.l*ans.shape.w*ans.shape.h*sizeof(float));
					free(ans.data);
					last_tp->data = NULL;
				}
				/*free image*/
				free(image1.data);image1.data = NULL;
				free(image2.data);image2.data = NULL;
			}
			break;

		}
	}
	return 0;
	
}
int test(){
	/*
	+ l1 l2 l3
	- l3 l2 l4
	== l4 l1 l5
	cov org 225 225 3 lay1 64 true 2 3 3 3 false 0
	*/
	struct Code code;

	code.code_size = 9;
	code.pos_s = (uint8_t*)malloc(code.code_size*POS_LEN);
	uint8_t* Structure = (uint8_t*)malloc(INS_LEN*3 + COV_LEN + POOLING_LEN + RELU_LEN + BN_LEN + FC_LEN + SHORTCUT_LEN);
	
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
		arr[i] = 1;
	}
	for(int i = 0; i < 64; i++){
		memcpy(code.W + i * 5*5*3*sizeof(float), arr, 5*5*3*sizeof(float));
	}	

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

	printf("next add weight of FC\n");
	float* w_of_fc = (float*)malloc(57*57*64*10*sizeof(float));
	for(int i = 0; i < 57*57*64*10;i ++){
		w_of_fc[i] = 1;
	}
	memcpy(code.W + 64 * 5*5*3*sizeof(float) + 5*sizeof(float), w_of_fc, 57*57*64*10*sizeof(float));


	printf("itr is %u\n",itr);
	code.S = Structure;
	code.S_len =  INS_LEN*3 + COV_LEN + POOLING_LEN + RELU_LEN + BN_LEN + FC_LEN + SHORTCUT_LEN;

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
	add_data(&data, label1, LABEL_LEN, ll1, l1_len);
	add_data(&data, label2, LABEL_LEN, ll2, l2_len);
	float* image_data = (float*)malloc(225*225*3*sizeof(float));
	for(int i = 0; i < 225*225*3; i++){
		image_data[i] = i/225/225;
	}

	uint32_t image_len = 225*225*3*sizeof(float);
	add_data(&data, cov_label, LABEL_LEN, (uint8_t*)image_data, image_len);
	struct Tuple tmp;
	tmp.data = NULL;
	char tmp_label[5]="";
	run_code(&data, &code,&tmp,tmp_label);
	run_code(&data, &code,&tmp,tmp_label);
	run_code(&data, &code,&tmp,tmp_label);
	uint8_t ans[1];
	uint32_t ans_len = 1;
	if(0 == get_data(data, index_of(data, label5, LABEL_LEN), &ans_len, ans)){
		printf("result is %u\n", ans[0]);
	}
	run_code(&data, &code,&tmp,tmp_label);
	printf("[%u,%u,%u]\n",tmp.shape.l,tmp.shape.w,tmp.shape.h);
	for(int i = 0; i < 113; i++){
		printf("%f ", tmp.data[i]);
	}
	printf("\n");


	run_code(&data, &code,&tmp,tmp_label);
	uint8_t *res = (uint8_t*)malloc(57*57*64*sizeof(float));
	uint32_t res_len = 57*57*64*sizeof(float);
	if(0 == get_data(data, index_of(data, pool_label_out, LABEL_LEN), &res_len, res)){
		for(int i = 0; i < 113; i++){
			printf("%f ", ((float*)res)[i]);
		}
	}
	printf("\n");
	run_code(&data, &code,&tmp,tmp_label);
	printf("[%u,%u,%u]\n",tmp.shape.l,tmp.shape.w,tmp.shape.h);
	for(int i = 0; i < 113; i++){
		printf("%f ", tmp.data[i]);
	}
	printf("\n");
	run_code(&data, &code,&tmp,tmp_label);
	printf("[%u,%u,%u]\n",tmp.shape.l,tmp.shape.w,tmp.shape.h);
	for(int i = 0; i < 113; i++){
		printf("%f ", tmp.data[i]);
	}
	printf("\n");
	run_code(&data, &code,&tmp,tmp_label);
	printf("[%u,%u,%u]\n",tmp.shape.l,tmp.shape.w,tmp.shape.h);
	for(int i = 0; i < 113; i++){
		printf("%f ", tmp.data[i]);
	}
	printf("\n");
	run_code(&data, &code,&tmp,tmp_label);
	printf("free\n");
	free(image_data);
	free(w_of_fc);
	free(res);
	free(code.pos_s);
	free(code.W);
	free(Structure);
	free(data.label);
	free(data.pos);
	free(data.data);
	return 0;
}