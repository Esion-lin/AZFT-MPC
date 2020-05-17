// Auther: Tainpei Lu
// Creation: 11/02 2019 
#include "network.h"

netTool::netTool(TEE* tru){
    this->tru = tru;
}
/*accept result and store it*/
void accecpt_result(Json::Value key_paty, Json::Value data_part){
    uint8_t key_hash[key_paty.size()];
    for(int i = 0; i < key_paty.size(); i++){
        key_hash[i] = key_paty[i].asUInt();
    }
    //store_data_to_file(key_hash, key_paty.size(), "./key_hash.data");
    std::string hash_str = "bc0ae45639a93db8f3859e4dbbb27a2c955276b3a6025c9f272a20406de5de5b";
    store_file_with_string(hash_str,"./key_hash.data");
    uint8_t data[data_part.size()];
    for(int i = 0; i < data_part.size(); i++){
        data[i] = data_part[i].asUInt();
    }
    store_data_to_file(data, data_part.size(), "./result.data");
}
void netTool::deal_data(Json::Value value){
    int action = value["action"].asInt();
    Json::Value key_part;
    Json::Value data;
    Json::Value mac;
    switch(action){
        case rec_img_data:
        {
            rec_serial_data = false;
            rec_image_data = true;
            key_part = value["key"];
            uint8_t key_data[key_part.size()];
            for(int i = 0; i < key_part.size(); i++){
                key_data[i] = key_part[i].asUInt();
            }
            tru->decrypto_key(key_data, key_part.size());
        }
            break;
        case key_ex_action:
            data = value["data"];
            accept_key(data);
            /*
                todo:send signature
            */
            break;
        case sign_action:
            data = value["data"];
            accept_sign(data);
            break;
        case data_action:
            key_part = value["key"];
            data = value["data"];
            mac = value["mac"];

            accept_data(key_part, data, mac);
            break;
        case cmd_mac_action:
            data = value["data"];
            accept_mac(data);
            break;
        case res_data_action:
        {
            Json::Value key_hash;
            key_hash = value["key_hash"];
            data = value["data"];
            accecpt_result(key_hash, data);
        }
            break;
        default:
            printf("error package\n");
    }
}
void netTool::accept_data(Json::Value key_paty, Json::Value data_part, Json::Value mac){
    uint8_t key_data[key_paty.size()];
    for(int i = 0; i < key_paty.size(); i++){
        key_data[i] = key_paty[i].asUInt();
    }
    tru->decrypto_key(key_data, key_paty.size());
    Json::Value::Members members = data_part.getMemberNames();
    uint32_t arr[CIPHER_LEN/4];
    for (Json::Value::Members::iterator iterMember = members.begin(); iterMember != members.end(); iterMember++){
        std::string key = *iterMember;
        for(int i = 0; i < CIPHER_LEN/4; i++){
            arr[i] = data_part[key][i].asUInt();
        }
        uint32_t len_temp;
        to_byte(arr, CIPHER_LEN/4, data_dic[key], len_temp);
    }

    uint32_t arr_mac[MAC_LEN/4];
    Json::Value::Members memb = mac.getMemberNames();
    for (Json::Value::Members::iterator iterMember = memb.begin(); iterMember != memb.end(); iterMember++){
        std::string key = *iterMember;
        for(int i = 0; i < MAC_LEN/4; i++){
            arr_mac[i] = mac[key][i].asUInt();
        }
        uint32_t len_temp;
        to_byte(arr_mac, MAC_LEN/4, data_mac_dic[key], len_temp);
    }
    is_data_store = true;
    // uint8_t test[16];
    // uint32_t len;
    // tru->decrypto(data_dic["A"],16,test,len);
}
void netTool::accept_mac(Json::Value value){
    //std::vector<uint8_t[MAC_LEN]> dir_mac = std::vector<uint8_t[MAC_LEN]>(value.size());
    for(int i = 0; i < value.size(); i++){
        uint8_t *arr = new uint8_t[MAC_LEN];
        for(int j = 0; j < MAC_LEN; j++){
            arr[j] = value[i][j].asUInt();
        }
        mac_dir.push_back(arr);
    }
/*    for(int i = 0 ; i < mac_dir.size(); i++){
        for(int j = 0; j < MAC_LEN; j++){
            std::cout<<mac_dir[i][j]<<" ";
        }
        std::cout<<std::endl;
    }*/
    //mac_dir.insert(mac_dir.end(),dir_mac.begin(),dir_mac.end());
   /* for(int i = 0; i < value.size(); i++){
        uint8_t middle[MAC_LEN];
        for(int j = 0; j < MAC_LEN; j++){
            middle[j] = value[i][j].asUInt();
        }
        mac_dir.insert(mac_dir.end(), middle);
        
    }*/
    
    //copy(new_vec.begin(), new_vec.end(), mac_dir.begin());
    is_mac_store = true;
}
void netTool::accept_key(Json::Value value){
    uint8_t data[value.size()];
    for(int i = 0; i < value.size(); i++){
        data[i] = value[i].asUInt();
    }

    tru->stream_to_key(data);
    is_key_store = true;

}
void netTool::accept_sign(Json::Value value){
    uint8_t data[value.size()];
    for(int i = 0; i < value.size(); i++){
        data[i] = value[i].asUInt();
    }
    is_key_verify = tru->sign_verify(data);
    
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
    char recvbuf[buffer_size*buffer_size];
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
        if(rec_serial_data){   
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
        }else{
            int img_size_tmp = img_size;
            int off_set = 0;
            bool flag = true;
            while(img_size_tmp){
                int ret = recv(conn, rev_img_data + off_set, img_size_tmp,0);
                if(-1 == ret){
                    printf("receive data error\n");
                    rec_serial_data = true;
                    rec_image_data = false;
                    flag = false;
                    break;
                }
                img_size_tmp -= ret;
                off_set += ret;
            }
            if(flag == false)
                continue;
            is_data_store = true;
            rec_serial_data = true;
            rec_image_data = false;
            printf("receive data successful\n");
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
    //printf("send data len:%u\n", s.length());
    send(conn_fd, s.c_str(), s.length(),0);///send away
    close(conn_fd);
}
void netTool::send_data(uint8_t * data, int len_data){
    init_conn();
    if(connect(conn_fd, (struct sockaddr *)&remote_server_sockaddr, sizeof(remote_server_sockaddr)) < 0){
        perror("connect");
    } 
    send(conn_fd, data, len_data,0);///send away
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