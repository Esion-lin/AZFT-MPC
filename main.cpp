#include "network.h"
bool notify(){
	printf("choose a operation:\n \t1.send encrypto msg\n \t2.add\n \t3.xor\n \t4.....\n");
	return true;
}
void send_json(int action, unsigned char data[], int len, void *tmp){
	Json::Value  json;
    Json::Value  jdata;
	netTool* nettool = (netTool*) tmp;
	json["action"] = Json::Value(action);
	for(int i = 0; i < len; i++){
        jdata[i] = data[i];
    }
    json["data"] = jdata;

    nettool->send_data(json);
}
void sign_key(void *tmp,void *tmp2){
	truthtee* tru = (truthtee*) tmp;
    unsigned char data[ECC_MAX_XCOORDINATE_BITS_LEN/4];
    tru->sign_key(data);
	send_json(sign_action, data, ECC_MAX_XCOORDINATE_BITS_LEN/4, tmp2);
    
}
void send_pub_key(void *tmp,void *tmp2){
	truthtee* tru = (truthtee*) tmp;
	unsigned char data[ECC_MAX_XCOORDINATE_BITS_LEN/2];
    tru->query_pub_stream(data);
	send_json(key_ex_action, data, ECC_MAX_XCOORDINATE_BITS_LEN/2, tmp2);
}
void send_data(void *tmp, unsigned char data[], int len){
	send_json(data_action, data, len, tmp);
}
int main(){
	pthread_t   recv_tid;
    truthtee* tru = new truthtee();
    netTool* nettool = new netTool(tru);
    if(pthread_create(&recv_tid , NULL , init_listen_static, (void *)nettool) == -1){
        perror("pthread create error.\n");
        exit(1);
    }
    printf("please input host to connect:\n");
    std::string host;
    std::cin>>host;
    printf("please input port to connect:\n");
    int port;
    std::cin>>port;
    nettool->set_host_port(host,port);
    send_pub_key(tru, nettool);
    while(notify()){
    	int act;
    	int ret;
    	int a,b;
    	unsigned char msg[2];
    	unsigned char enc[0x100];
    	int enc_len = 0;
    	std::cin>>act;
    	switch(act){
    		case 1:
    			printf("input a number\n");
    			std::cin>>a>>b;
    			msg[0] = a;msg[1] = b;
    			tru->en_date(msg,2,enc,enc_len);
    			send_data(nettool, enc, enc_len);
    			break;
    		case 2:
    			ret = tru->add_op(enc, enc_len);
    			if(ret == -1){
    				printf("not recv msg\n");
    			}else if(ret == 0){
    				printf("please send msg first\n");
    			}else{
    				printf("get result:\n");
    				for(int i = 0; i < enc_len; i++){
    					printf("%d ", (int)enc[i]);
    				}
    				printf("\n");
    			}
    			break;
    		case 3:
    			ret = tru->test_and_op(enc, enc_len);
    			if(ret == -1){
    				printf("not recv msg\n");
    			}else if(ret == 0){
    				printf("please send msg first\n");
    			}else{
    				printf("get result:\n");
    				for(int i = 0; i < enc_len; i++){
    					printf("%d ", (int)enc[i]);
    				}
    				printf("\n");
    			}
    			break;
    		case 4:
    			exit(1);
    	}
    }
}