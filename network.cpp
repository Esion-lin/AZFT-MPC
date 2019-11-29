#include "network.h"
void to_byte16(uint64_t org, unsigned char output[]){
    for(int i = 0; i < 8; i++){
        output[i] = 0;
        output[i+8] = org>>((7-i)*8);
    }
}
void to_byte16(uint64_t org1, uint64_t org2, unsigned char output[]){
    for(int i = 0; i < 8; i++){
        output[i] = org1>>((7-i)*8);
        output[i+8] = org2>>((7-i)*8);
    }
}
void to_byte16(uint32_t org1, uint32_t org2, uint32_t org3, uint32_t org4, unsigned char output[]){
    for(int i = 0; i < 4; i++){
        output[i] = org1>>((3-i)*8);
        output[i+4] = org2>>((3-i)*8);
        output[i+8] = org3>>((3-i)*8);
        output[i+12] = org4>>((3-i)*8);
    }
}
void to_ll(unsigned char input[], uint64_t &output){
    for(int i = 0; i < 8; i++){
        output *= 256;
        output += input[i+8];
    }
}
void to_ll(unsigned char input[], uint64_t &output1, uint64_t &output2){
    for(int i = 0; i < 8; i++){
        output1 *= 256;
        output1 += input[i];
        output2 *= 256;
        output2 += input[i+8];
    }
}
void to_int(unsigned char input[], uint32_t &output1, uint32_t &output2,uint32_t &output3,uint32_t &output4){
    for(int i = 0; i < 4; i++){
        output1 *= 256;
        output1 += input[i];
        output2 *= 256;
        output2 += input[i+4];
        output3 *= 256;
        output3 += input[i+8];
        output4 *= 256;
        output4 += input[i+12];

    }
}
netTool::netTool(truthtee* tru){
    this->tru = tru;
}

void netTool::deal_data(Json::Value value){
    int action = value["action"].asInt();
    Json::Value key_part;
    Json::Value data;
    switch(action){
        case key_ex_action:
            data = value["data"];
            accept_key(data);
            break;
        case sign_action:
            break;
        case data_action:
            key_part = value["key"];
            data = value["data"];
            accept_data(key_part,data);
            break;
        default:
            printf("error package\n");
    }
}
void netTool::accept_data(Json::Value key_paty, Json::Value data_part){
    unsigned char key_data[key_paty.size()];
    for(int i = 0; i < key_paty.size(); i++){
        key_data[i] = key_paty[i].asUInt();
    }
    tru->decrypto_key(key_data, key_paty.size());
    Json::Value::Members members = data_part.getMemberNames();
    uint arr[4];
    for (Json::Value::Members::iterator iterMember = members.begin(); iterMember != members.end(); iterMember++){
        std::string key = *iterMember;
        for(int i = 0; i < 4; i++){
            arr[i] = data_part[key][i].asUInt();
        }
        
        to_byte16(arr[0], arr[1], arr[2], arr[3], data_dic[key]);
    }
    is_data_store = true;
    // unsigned char test[16];
    // unsigned int len;
    // tru->decrypto(data_dic["A"],16,test,len);

}
void netTool::accept_key(Json::Value value){
    unsigned char data[value.size()];
    for(int i = 0; i < value.size(); i++){
        data[i] = value[i].asUInt();
    }
    tru->stream_to_key(data);
}
void netTool::set_host_port(std::string host,int port){
    this->conn_host = host;
    this->conn_port = port;
}
void *netTool::init_listen(){
	if((listen_fd = socket(AF_INET , SOCK_STREAM , 0)) == -1){
		perror("socket error.\n");
		exit(1);
	}
	server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(recv_port);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listen_fd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1){
        perror("bind error.\n");
        exit(1);
    }
    if(listen(listen_fd,connect_pool) == -1){
        perror("listen error.\n");
        exit(1);
    }
    socklen_t length = sizeof(client_addr);
    char recvbuf[buffer_size*2];
    Json::Value   value;
    Json::Reader  reader;
    while(true){
        int conn = accept(listen_fd, (struct sockaddr*)&client_addr, &length);
        if(conn<0){
            perror("connect error.\n");
            exit(1);
        }else{
            //printf("connect successful\n");
        }    
        int ret = recv(conn, recvbuf, sizeof(recvbuf),0);
        if(ret <0){
            perror("recv error\n");
        }else{
            //printf("recv size: %d\n",ret); 
        }
        if(reader.parse(recvbuf,value)){
            deal_data(value);        
        }else{
            perror("reader error\n");
        } 
    }
	
}
void *init_listen_static(void *tmp){
    netTool* p = (netTool*) tmp;
    p->init_listen();
}
void netTool::init_conn(){
	if((conn_fd = socket(AF_INET , SOCK_STREAM , 0)) == -1){
		perror("socket error.\n");
		exit(1);
	}
	remote_server_sockaddr.sin_family = AF_INET;
    remote_server_sockaddr.sin_port = htons(conn_port); 
    remote_server_sockaddr.sin_addr.s_addr = inet_addr(conn_host.c_str()); 
    
}


void netTool::send_data(Json::Value js){
    init_conn();
    if(connect(conn_fd, (struct sockaddr *)&remote_server_sockaddr, sizeof(remote_server_sockaddr)) < 0){
        perror("connect");
    } 
    std::string s = js.toStyledString();
    send(conn_fd, s.c_str(), strlen(s.c_str()),0); ///发送
    close(conn_fd);
}
//for testing
void test(Json::Value value){
    int action = value["action"].asInt();
    printf("action is %d\n",action);
    std::cout<<value["data"].asString()<<std::endl;
}
// void netTool::init(){
//     if(pthread_create(&recv_tid , NULL , init_listen_static, (void *)this) == -1){
//         perror("pthread create error.\n");
//         exit(1);
//     }

// }
//for test
/*int main(){
    pthread_t   recv_tid;
    truthtee* tru = new truthtee();
    netTool* nettool = new netTool(tru);
    if(pthread_create(&recv_tid , NULL , init_listen_static, (void *)nettool) == -1){
        perror("pthread create error.\n");
        exit(1);
    }
    while(1){}
    
}*/