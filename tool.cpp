// Auther: Tainpei Lu
// Creation: 11/02 2019 
#include "tool.h"

Json::Value stream2json(unsigned char stream[]){
	Json::Value  data;
	for( int i = 0; i < sizeof(stream)/sizeof(unsigned char); i ++){
		data[i] = stream[i];
	}
	return data;
}
unsigned char * json2stream(Json::Value value){
	unsigned char *data = new unsigned char(value.size());
	for(int i = 0; i < value.size(); i++){
		data[i] = value[i].asUInt();
	}
	printf("get data successful");
	return data;
}