#include "component.h"
#include "model.h"
#include <iostream>
#include <map>
using namespace std;
void fake_gen_data(){
	struct Data data;
	char label[LABEL_LEN];
	printf("input data size:\n");
	cin>>data.item_capacity;
	printf("input data len:\n");
	cin>>data.data_capacity;
	uint8_t *ll;
	uint32_t ll_len;
	data.label_size = 0;
	data.data_len = 0;
	data.data_capacity = data.data_capacity*2*sizeof(float);
	data.label = (uint8_t*)malloc(data.item_capacity*LABEL_LEN);
	data.pos = (uint8_t*)malloc(data.item_capacity*POS_LEN);
	data.data = (uint8_t*)malloc(data.data_capacity);
	for(int i = 0; i < data.item_capacity; i++){
		printf("input label(len <= 5, input 0 to exit):\n");
		cin>>label;
		
		int type;
		printf("input type(1.image 2.float, input 0 to exit):\n");
		cin>>type;
		switch(type){
			case 1:{
				printf("input size of image:\n");
				uint32_t size_m;
				cin>>size_m;
				ll_len = size_m*sizeof(float);
				ll = (uint8_t*)malloc(ll_len);
				for(int i = 0; i < size_m; i++){
					((float*)ll)[i] = 0.01;
				}
				add_data(&data, (uint8_t*)label, LABEL_LEN, ll, ll_len);
				free(ll);
			}break;
			case 2:{
				ll_len = 4;
				ll = (uint8_t*)malloc(ll_len);
				float data_f;
				printf("input data:\n");
				cin>>data_f;
				memcpy(ll, &data_f, 4);
				add_data(&data, (uint8_t*)label, LABEL_LEN, ll, ll_len);
			}
			break;
		}
	}
	uint8_t* label_stream = (uint8_t*)malloc(data.label_size*LABEL_LEN);
	uint32_t label_stream_len;
	uint8_t* data_stream = (uint8_t*)malloc(data.label_size*POS_LEN + data.data_len);
	uint32_t data_stream_len;
	deserialize(data, label_stream, &label_stream_len, data_stream, &data_stream_len);
	save_model<uint8_t>(label_stream,label_stream_len*LABEL_LEN,"./label_data.data");
	save_model<uint8_t>(data_stream,data_stream_len,"./data_data.data");
	

}
void fake_gen_weight(){
	string path;
	printf("input weight name:\n");
	cin>>path;
	uint32_t weight_size;
	printf("input weight len(float):\n");
	cin>>weight_size;
	float* arr = (float*) malloc(weight_size*sizeof(float));
	save_model<float>(arr , weight_size, path);
	free(arr);
}
int main(){
	int a;
	printf("1.gen data\n2.gen weight\n");
	
	cin>>a;
	if(a == 1){
		fake_gen_data();
	}else if(a == 2){
		fake_gen_weight();
	}

}