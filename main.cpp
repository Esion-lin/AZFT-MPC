#include "network.h"

bool notify(){
	printf("choose a operation:\n \t1.send encrypto msg\n \t2.run protocol\n \t3.other\n \t4.....\n");
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
void send_data(void *tmp, unsigned char key_data[], int key_len, std::map<std::string, unsigned char[16]> dir){
	Json::Value  json;
    Json::Value  jdata;
    Json::Value  jkey;
	netTool* nettool = (netTool*) tmp;
	json["action"] = data_action;
	for(int i = 0; i < key_len; i++){
        jkey[i] = key_data[i];
    }
    uint arr[4];
    for( auto &v : dir ) {
        
        to_int(v.second,arr[0],arr[1],arr[2],arr[3]);
        Json::Value mid;
        for(int i = 0; i < 4; i++){
            mid[i] = arr[i];
        }
        jdata[v.first] = mid;

    }

    json["key"] =  jkey;
    json["data"] = jdata;

    nettool->send_data(json);
}
int main(){
    std::map<std::string, unsigned char[16]> dir;
	pthread_t   recv_tid;
    truthtee* tru = new truthtee();
    netTool* nettool = new netTool(tru);
    printf("please input port to listen:\n");
    std::cin>>nettool->recv_port;
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

    //load protocol

    //
    while(notify()){
    	int act;
    	int ret;
    	int a,b;
    	unsigned char msg[16];
    	unsigned char enc_key[0x100];
    	unsigned char enc_data[0x10];
    	uint64_t value;
        std::string key_po;
    	unsigned int key_len = 0;
    	unsigned int data_len = 0;
    	std::cin>>act;
    	switch(act){
    		case 1:
                dir.clear();
                tru->encrypto_key(enc_key, key_len);
                while(std::cin>>key_po>>value){
                    if(key_po == "0" && value == 0){
                        break;
                    }
                    to_byte16(value,msg);
                    tru->encrypto(msg, 16, dir[key_po], data_len);

                }
    			send_data(nettool, enc_key, key_len, dir);
    			break;
    		case 2:
    			if(!nettool->is_data_store){
                    printf("wait for data exchage");
                    continue;
                }
                //for test
                unsigned char output[16],ans[16];
                uint nn,nn2;
                uint64_t answer;
                tru->operation(dir["B"],16,nettool->data_dic["A"],16,output,nn,AND_OP,SWI_ORG);
                tru->decrypto(output,16,ans,nn2);
                to_ll(ans,answer);
                std::cout<<"get answer: "<<answer<<std::endl;
    			break;
    		case 3:
    			
    			break;
    		case 4:
    			exit(1);
    	}
    }
}