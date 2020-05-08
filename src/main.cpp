// Auther: Tainpei Lu
// Creation: 11/02 2019 
#include "network.h"
#include "protocol.h"
#include "tool.h"
#include <time.h>
#include "model.h"
#include <ctime>
#include <tuple.h>
#include "hw_head.h"
bool debug_this = false;
bool notify(){
	printf("choose a operation:\n \t1.send encrypted data\n \t2.run ins\n \t3.send encrypted file\n \t4r.run ins block\n");
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
	truthtee_pend* tru = (truthtee_pend*) tmp;
    unsigned char data[ECC_MAX_XCOORDINATE_BITS_LEN/4];
    tru->sign_key(data);
   
	send_json(sign_action, data, ECC_MAX_XCOORDINATE_BITS_LEN/4, tmp2);
    
}
void send_img(void *tmp, unsigned char key_data[], int key_len, unsigned char * data, int len_data){
    netTool* nettool = (netTool*) tmp;
    Json::Value  json;
    Json::Value  jkey;
    json["action"] = Json::Value(rec_img_data);
    for(int i = 0; i < key_len; i++){
        jkey[i] = key_data[i];
    }
    json["key"] =  jkey;
    nettool->send_data(json);
    sleep(2);
    nettool->send_data(data, len_data);
}
void send_res_data(void *tmp, unsigned char* key, int key_len, unsigned char* data, int data_len){
    netTool* nettool = (netTool*) tmp;
    Json::Value  json;
    Json::Value  key_json;
    Json::Value  data_json;
    for(int i = 0; i < key_len; i ++){
        key_json[i] = key[i];
    }
    for(int i = 0; i < data_len; i ++){
        data_json[i] = data[i];
    }
    json["action"] = Json::Value(res_data_action);
    json["key_hash"] = key_json;
    json["data"] = data_json;
    nettool->send_data(json);
}
void send_cmd_mac(void *tmp, std::vector<unsigned char[MAC_LEN]> data){
 
    netTool* nettool = (netTool*) tmp;
    int n = data.size();
    int t = 0;
    while(n > 0){
        if(n < 100){
            Json::Value  json;
            Json::Value  jdata;
            for(int i = 0; i < n; i++){
                Json::Value mid;
                for(int j = 0; j < MAC_LEN; j++){
                    mid[j] = data[100*t + i][j];
                }
                jdata[i] = mid;
            }
            json["action"] = Json::Value(cmd_mac_action);
            json["data"] = jdata;
            nettool->send_data(json);
            break;
        }else{
            Json::Value  json;
            Json::Value  jdata;
            for(int i = 0; i < 100; i++){
                Json::Value mid;
                for(int j = 0; j < MAC_LEN; j++){
                    mid[j] = data[100*t + i][j];
                }
                jdata[i] = mid;
            }
            json["action"] = Json::Value(cmd_mac_action);
            json["data"] = jdata;
            nettool->send_data(json);
            t ++;
            n-=100;
        }
        
    }
    
}

void send_pub_key(void *tmp,void *tmp2){
	truthtee_pend* tru = (truthtee_pend*) tmp;
	unsigned char data[ECC_MAX_XCOORDINATE_BITS_LEN/2];
    tru->query_pub_stream(data);
	send_json(key_ex_action, data, ECC_MAX_XCOORDINATE_BITS_LEN/2, tmp2);
}
void send_data(void *tmp, unsigned char key_data[], int key_len, std::map<std::string, unsigned char[CIPHER_LEN]> dir, std::map<std::string, unsigned char[MAC_LEN]> dir_mac){
	Json::Value  json;
    Json::Value  jdata;
    Json::Value  jkey;
    Json::Value  jmac;
	netTool* nettool = (netTool*) tmp;
	json["action"] = data_action;
	for(int i = 0; i < key_len; i++){
        jkey[i] = key_data[i];
    }
    uint arr[CIPHER_LEN/4];
    for( auto &v : dir ) {
        
        to_int(v.second,arr[0],arr[1],arr[2],arr[3]);
        Json::Value mid;
        for(int i = 0; i < 4; i++){
            mid[i] = arr[i];
        }
        jdata[v.first] = mid;

    }
    uint mac_arr[MAC_LEN/4];
    for(auto &v : dir_mac){
        to_int(v.second,MAC_LEN,mac_arr);
        Json::Value mid;
        for(int i = 0; i < MAC_LEN/4; i++){
            mid[i] = mac_arr[i];
        }
        jmac[v.first] = mid;
    }

    json["key"] =  jkey;
    json["data"] = jdata;
    json["mac"] = jmac;

    nettool->send_data(json);
}

//Determine which collection the data belongs to
int judge(std::string str,std::map<std::string, unsigned char[16]>remote_dir, std::map<std::string, unsigned char[16]>local_dir, std::map<std::string, int64_t> org_dir){
    if(local_dir.find(str) != local_dir.end()){
        return 2;
    }else if(remote_dir.find(str) != remote_dir.end()){
        return 1;
    }else{
        return 3;
    }
}
/*
thank to shift all data to local, just judge if it belong plaintext or ciphertext
*/
int judge(std::string str, std::map<std::string, unsigned char[16]>local_dir, std::map<std::string, int64_t> org_dir){
    if(local_dir.find(str) != local_dir.end()){
        return 2;
    }else{
        return 3;
    }
}

/*
rewrite cmd deal function :
*/
void deal_cmd(truple_mac now_trp_mac, int &now_step, void *tmp, std::map<std::string,int>goto_dir, std::map<std::string, unsigned char[CIPHER_LEN]>&local_dir, std::map<std::string, unsigned char[MAC_LEN]>&local_mac_dir, std::map<std::string, int64_t> &org_dir){
    //get the cmd 

    truple now_trp = now_trp_mac.trup;
    unsigned int data_len;
    unsigned int mac_len;
    unsigned char temp_msg[CIPHER_LEN];
    truthtee_pend* tru = (truthtee_pend*) tmp;
    //conversion of array element
    now_trp.operand1 = get_item(org_dir,now_trp.operand1);
    now_trp.operand2 = get_item(org_dir,now_trp.operand2);
    now_trp.output = get_item(org_dir,now_trp.output);
    //a number store operand if Operator is number
    int64_t number_temp;

    unsigned char label[LABEL_LEN];
    unsigned char label2[LABEL_LEN];
    unsigned char out_label[LABEL_LEN];
    //if the instruction is jump statement
    if(now_trp.is_goto){
        if(now_trp.op == "goto"){
            now_step = goto_dir[now_trp.output];
        }else{
            //make sure all goto statement is under the plaintext
            if(is_num(now_trp.operand2,number_temp)){
                if(tran_op(org_dir[now_trp.operand1],number_temp,now_trp.op)){
                    now_step = goto_dir[now_trp.output];
                }
            }else{
                if(tran_op(org_dir[now_trp.operand1],org_dir[now_trp.operand2],now_trp.op)){
                    now_step = goto_dir[now_trp.output];
                }
            }
        }

    }else{
        //check if it is time to stop caculate
        if(now_trp.op == "out"){
            //tell the truthted third party label, ciphertext and MAC
            memcpy(label, now_trp.output.c_str(), now_trp.output.length());
            if(tru->decrypto(label, now_trp.output.length(), local_dir[now_trp.output],CIPHER_LEN, local_mac_dir[now_trp.output], MAC_LEN, now_trp_mac.mac, MAC_LEN, temp_msg,data_len)){
                uint64_t answer;
                to_ll(temp_msg,answer);
                std::cout<<now_trp.output<<" is "<<answer<<std::endl;
                return;
            }else{
                return;
            }
            
        }
        /*
        op = '' means that it is a direct assignment statement, like a = b, or a = 0x00
        handle these two cases separately
        */
        if(now_trp.op == ""){
            //judge if the data is under the ciphertext
            
            if(is_num(now_trp.operand1,number_temp)){
                int tp = judge(now_trp.output, local_dir, org_dir);
                //the second case: a = 0x00
                switch(tp){
                    case 2:
                    //a is ciphertext
                        to_byte16((uint64_t)number_temp,temp_msg);
                        //old version without MAC check
                        //tru->encrypto(temp_msg, CIPHER_LEN, local_dir[now_trp.output], data_len);
                        memcpy(label, now_trp.output.c_str(), now_trp.output.length());
                        tru->encrypt_MAC(label, now_trp.output.length(), temp_msg, CIPHER_LEN, local_dir[now_trp.output], data_len, local_mac_dir[now_trp.output], mac_len);
                        //memcpy(local_dir[now_trp.output],local_dir[now_trp.operand1],16);
                        //local_dir[now_trp.output] = local_dir[now_trp.operand1];
                        break;
                    default:
                        //plaintext do not need MAC
                        org_dir[now_trp.output] = number_temp;
                        break;
                }

            }
            else{
                //the first case: a = b
                int tp = judge(now_trp.operand1, local_dir, org_dir);
                int tp2 = judge(now_trp.output, local_dir, org_dir);
                switch(tp){
                    case 2:
                        //memcpy(local_dir[now_trp.output],local_dir[now_trp.operand1],CIPHER_LEN);
                        memcpy(label, now_trp.operand1.c_str(), now_trp.operand1.length());
                        memcpy(out_label, now_trp.output.c_str(), now_trp.output.length());
                        tru->operation(label, now_trp.operand1.length(), local_dir[now_trp.operand1],CIPHER_LEN, SWI_ORG, local_mac_dir[now_trp.operand1], MAC_LEN, NULL, 0, NULL, 0, 0, NULL, 0, out_label, now_trp.output.length(), local_dir[now_trp.output],data_len, local_mac_dir[now_trp.output],mac_len, now_trp_mac.mac, MAC_LEN, 0);
                        //tru->operation(remote_dir[now_trp.operand1], 16, SWI_REM, temp_msg, 16, SWI_PLA, local_dir[now_trp.output], data_len, tran_op(now_trp.op));
                        //local_dir[now_trp.output] = local_dir[now_trp.operand1];
                        break;
                    case 3:
                        //a is ciphertext and b is plaintext
                        if(tp2 == 2){
                            to_byte16((uint64_t)org_dir[now_trp.operand1],temp_msg);
                            memcpy(label, now_trp.output.c_str(), now_trp.output.length());
                            tru->encrypt_MAC(label, now_trp.output.length(), temp_msg, CIPHER_LEN, local_dir[now_trp.output], data_len, local_mac_dir[now_trp.output], mac_len);
                        }else
                        //both plaintext
                            org_dir[now_trp.output] = org_dir[now_trp.operand1];
            
                }
            }
        }else{
            /*
            General instructions
            The case of two number is illegal for example: a = 0x00 op 0x00
            */
            //to make sure operand1 is not a number
            memcpy(label, now_trp.operand1.c_str(), now_trp.operand1.length());
            memcpy(label2, now_trp.operand2.c_str(), now_trp.operand2.length());
            memcpy(out_label, now_trp.output.c_str(), now_trp.output.length());
            int tp = judge(now_trp.operand1, local_dir, org_dir);
            if(is_num(now_trp.operand1,number_temp)){
                if(is_num(now_trp.operand2,number_temp)){
                    printf("[%d] %s op %s:Instruction error!!! invalid equation from number operation\n", now_step, now_trp.operand1.c_str(), now_trp.operand2.c_str());
                    return ;
                }else{
                    to_byte16((uint64_t)number_temp,temp_msg);
                    tru->operation(label, now_trp.operand1.length(), temp_msg,CIPHER_LEN, SWI_PLA, NULL, 0, label2, now_trp.operand2.length(), local_dir[now_trp.operand2], CIPHER_LEN, SWI_ORG, local_mac_dir[now_trp.operand2], MAC_LEN, out_label, now_trp.output.length(), local_dir[now_trp.output],data_len, local_mac_dir[now_trp.output],mac_len, now_trp_mac.mac, MAC_LEN, tran_op(now_trp.op));
                    //printf("[%d] %s op %s:Instruction error!!! invalid equation from number operation\n", now_step, now_trp.operand1.c_str(), now_trp.operand2.c_str());
                    //return ;
                }
            }
            
            //case of "a = b op 0x00"
            if(is_num(now_trp.operand2,number_temp)){
                to_byte16((uint64_t)number_temp,temp_msg);
                switch(tp){
                    case 2:
                        //b is under ciphertext 
                        tru->operation(label, now_trp.operand1.length(), local_dir[now_trp.operand1],CIPHER_LEN, SWI_ORG, local_mac_dir[now_trp.operand1], MAC_LEN, label2, now_trp.operand2.length(), temp_msg, CIPHER_LEN, SWI_PLA, NULL, 0, out_label, now_trp.output.length(), local_dir[now_trp.output],data_len, local_mac_dir[now_trp.output],mac_len, now_trp_mac.mac, MAC_LEN, tran_op(now_trp.op));
                        //tru->operation(local_dir[now_trp.operand1], CIPHER_LEN, SWI_ORG, temp_msg, CIPHER_LEN, SWI_PLA, local_dir[now_trp.output], data_len, tran_op(now_trp.op));
                        break;
                    case 3:
                        //b is under plaintext
                        org_dir[now_trp.output] = tran_op(org_dir[now_trp.operand1], number_temp, now_trp.op);
                }
            }
            else{
                //case of "a = b op c"
                int tp2 = judge(now_trp.operand2, local_dir, org_dir);
                if(tp == 2){
                    if(tp2 == 2){
                        //b,c both under ciphertext
                        tru->operation(label, now_trp.operand1.length(), local_dir[now_trp.operand1],CIPHER_LEN, SWI_ORG, local_mac_dir[now_trp.operand1], MAC_LEN, label2, now_trp.operand2.length(), local_dir[now_trp.operand2], CIPHER_LEN, SWI_ORG, local_mac_dir[now_trp.operand2], MAC_LEN, out_label, now_trp.output.length(), local_dir[now_trp.output],data_len, local_mac_dir[now_trp.output],mac_len, now_trp_mac.mac, MAC_LEN, tran_op(now_trp.op));
                    }else{
                        //c:cipher , b:plain 
                        to_byte16((uint64_t)org_dir[now_trp.operand2],temp_msg);
                        tru->operation(label, now_trp.operand1.length(), local_dir[now_trp.operand1],CIPHER_LEN, SWI_ORG, local_mac_dir[now_trp.operand1], MAC_LEN, label2, now_trp.operand2.length(), temp_msg, CIPHER_LEN, SWI_PLA, NULL, 0, out_label, now_trp.output.length(), local_dir[now_trp.output],data_len, local_mac_dir[now_trp.output],mac_len, now_trp_mac.mac, MAC_LEN, tran_op(now_trp.op));
                    }
                }else{
                    //b:plain
                    to_byte16((uint64_t)org_dir[now_trp.operand1],temp_msg);
                    if(tp2 == 2){

                        tru->operation(label, now_trp.operand1.length(), temp_msg,CIPHER_LEN, SWI_PLA, NULL, 0, label2, now_trp.operand2.length(), local_dir[now_trp.operand2], CIPHER_LEN, SWI_ORG, local_mac_dir[now_trp.operand2], MAC_LEN, out_label, now_trp.output.length(), local_dir[now_trp.output],data_len, local_mac_dir[now_trp.output],mac_len, now_trp_mac.mac, MAC_LEN, tran_op(now_trp.op));
                    }else{
                        org_dir[now_trp.output] = tran_op(org_dir[now_trp.operand1], org_dir[now_trp.operand2], now_trp.op);
                    }
                }
            }
        }

    }
}
//old version
/*void deal_cmd(truple now_trp, int &now_step, void *tmp, std::map<std::string,int>goto_dir, std::map<std::string, unsigned char[16]>&remote_dir, std::map<std::string, unsigned char[16]>&local_dir, std::map<std::string, int> &org_dir){
    int temp;
    unsigned int data_len;
    unsigned char temp_msg[16];
    truthtee* tru = (truthtee*) tmp;
    now_trp.operand1 = get_item(org_dir,now_trp.operand1);
    now_trp.operand2 = get_item(org_dir,now_trp.operand2);
    now_trp.output = get_item(org_dir,now_trp.output);
    if(now_trp.is_goto){
        if(now_trp.op == "goto"){
            now_step = goto_dir[now_trp.output];
        }else{
            if(is_num(now_trp.operand2,temp)){
                if(tran_op(org_dir[now_trp.operand1],temp,now_trp.op)){
                    now_step = goto_dir[now_trp.output];
                }
            }else{
                if(tran_op(org_dir[now_trp.operand1],org_dir[now_trp.operand2],now_trp.op)){
                    now_step = goto_dir[now_trp.output];
                }
            }
        }

    }else{
        if(now_trp.op == "out"){
            
            tru->decrypto(local_dir[now_trp.output],16,temp_msg,data_len);
            uint64_t answer;
            to_ll(temp_msg,answer);
            std::cout<<now_trp.output<<" is "<<answer<<std::endl;
            return;
            
        }
        if(now_trp.op == ""){
            int tp = judge(now_trp.output, remote_dir, local_dir, org_dir);
            if(is_num(now_trp.operand1,temp)){
                switch(tp){
                    case 2:
                        to_byte16((uint64_t)temp,temp_msg);
                        tru->encrypto(temp_msg, 16, local_dir[now_trp.output], data_len);
                        //memcpy(local_dir[now_trp.output],local_dir[now_trp.operand1],16);
                        //local_dir[now_trp.output] = local_dir[now_trp.operand1];
                        break;
                    default:
                        org_dir[now_trp.output] = temp;
                        break;
                }

            }
            else{
                int tp = judge(now_trp.operand1, remote_dir, local_dir, org_dir);
                int tp2 = judge(now_trp.output, remote_dir, local_dir, org_dir);
                switch(tp){
                    // case 1:
                    //     if(tp2 == 1){
                    //         memcpy(remote_dir[now_trp.output],remote_dir[now_trp.operand1],16);    
                    //     }else{
                    //         memcpy(remote_dir[now_trp.output],remote_dir[now_trp.operand1],16);
                    //     }
                        
                    //     //remote_dir[now_trp.output] = remote_dir[now_trp.operand1];
                    //     break;
                    case 2:
                        memcpy(local_dir[now_trp.output],local_dir[now_trp.operand1],16);
                        //local_dir[now_trp.output] = local_dir[now_trp.operand1];
                        break;
                    case 3:
                        if(tp2 == 2){
                            to_byte16((uint64_t)org_dir[now_trp.operand1],temp_msg);
                            tru->encrypto(temp_msg, 16, local_dir[now_trp.output], data_len);
                        }else
                            org_dir[now_trp.output] = org_dir[now_trp.operand1];
            
                }
            }
        }else{
            int tp = judge(now_trp.operand1, remote_dir, local_dir, org_dir);
            if(is_num(now_trp.operand2,temp)){
                to_byte16((uint64_t)temp,temp_msg);
                switch(tp){
                    case 2:
                        tru->operation(local_dir[now_trp.operand1], 16, SWI_ORG, temp_msg, 16, SWI_PLA, local_dir[now_trp.output], data_len, tran_op(now_trp.op));
                        break;
                    case 1:
                        tru->operation(remote_dir[now_trp.operand1], 16, SWI_REM, temp_msg, 16, SWI_PLA, local_dir[now_trp.output], data_len, tran_op(now_trp.op));
                        break;
                    case 3:
                        org_dir[now_trp.output] = tran_op(org_dir[now_trp.operand1], temp, now_trp.op);
                }
            }
            else{
                int tp2 = judge(now_trp.operand2, remote_dir, local_dir, org_dir);
                if(tp == 1){
                    if(tp2 == 1){
                        tru->operation(remote_dir[now_trp.operand1], 16, SWI_REM, remote_dir[now_trp.operand2], 16, SWI_REM, local_dir[now_trp.output], data_len, tran_op(now_trp.op));
                    }else if(tp2 == 2){
                        tru->operation(remote_dir[now_trp.operand1], 16, SWI_REM, local_dir[now_trp.operand2], 16, SWI_ORG, local_dir[now_trp.output], data_len, tran_op(now_trp.op));
                    }else{
                        to_byte16((uint64_t)org_dir[now_trp.operand2],temp_msg);
                        tru->operation(remote_dir[now_trp.operand1], 16, SWI_REM, temp_msg, 16, SWI_PLA, local_dir[now_trp.output], data_len, tran_op(now_trp.op));
                    }
                }else if(tp == 2){
                    if(tp2 == 1){
                        tru->operation(local_dir[now_trp.operand1], 16, SWI_ORG, remote_dir[now_trp.operand2], 16, SWI_REM, local_dir[now_trp.output], data_len, tran_op(now_trp.op));
                    }else if(tp2 == 2){
                        tru->operation(local_dir[now_trp.operand1], 16, SWI_ORG, local_dir[now_trp.operand2], 16, SWI_ORG, local_dir[now_trp.output], data_len, tran_op(now_trp.op));
                    }else{
                        to_byte16((uint64_t)org_dir[now_trp.operand2],temp_msg);
                        tru->operation(local_dir[now_trp.operand1], 16, SWI_ORG, temp_msg, 16, SWI_PLA, local_dir[now_trp.output], data_len, tran_op(now_trp.op));
                    }
                }else{
                    to_byte16((uint64_t)org_dir[now_trp.operand1],temp_msg);
                    if(tp2 == 1){
                        tru->operation(temp_msg, 16, SWI_PLA, remote_dir[now_trp.operand2], 16, SWI_REM, local_dir[now_trp.output], data_len, tran_op(now_trp.op));
                    }else if(tp2 == 2){
                        tru->operation(temp_msg, 16, SWI_PLA, local_dir[now_trp.operand2], 16, SWI_ORG, local_dir[now_trp.output], data_len, tran_op(now_trp.op));
                    }else{
                        org_dir[now_trp.output] = tran_op(org_dir[now_trp.operand1], org_dir[now_trp.operand2], now_trp.op);
                    }
                }
            }
        }
    }
}*/

int main(int argc, char* argv[]){
    test_hw_head();
	pthread_t   recv_tid;
    truthtee_pend *tru = new truthtee_pend();
    //truthtee* tru = new truthtee();
    printf("checking protocol file.....\n");
    bool is_ML = false;
    std::string path = "./protocol_file/orgin.jimple";
    if(argc>=3){
        if(0 == strcmp(argv[1], "ML")){
            is_ML = true;
            path = argv[2];
        }

    }
    bool load_succ;
    PotocolRead* protocol = new PotocolRead(path, load_succ, true, is_ML);
    if(!load_succ){
        exit(0);
    }
    printf("protocol file length:%d\n",protocol->size_of_protocol());
    netTool* nettool = new netTool(tru);
    
    printf("please input port to listen:\n");
    std::cin>>nettool->recv_port;
    if(pthread_create(&recv_tid , NULL , init_listen_static, (void *)nettool) == -1){
        perror("pthread create error.\n");
        exit(1);
    }
    //printf("please input host to connect:\n");
    std::string host = "127.0.0.1";
    //std::cin>>host;
    printf("please input port to connect:\n");
    int port;
    std::cin>>port;
    nettool->set_host_port(host,port);
    send_pub_key(tru, nettool);
    printf("Key exchanging......\n");
    while(!nettool->is_key_store){
    };
    printf("Key exchanging successfully.\n");
    sleep(2);
    /*sign the public key*/
    sign_key(tru,nettool);
    printf("Key verifing.......\n");
    while(!nettool->is_key_verify){};
    printf("Key verify successfully.\n");
    /*
    from protocol make MerkleTree
    or use counter + MAC
    and exchange it
    */

    //load protocol
    
    //dictionary for local data ciphertext and MAC
    std::map<std::string, unsigned char[CIPHER_LEN]> dir;
    std::map<std::string, unsigned char[MAC_LEN]> dir_mac;

    //dictionary for remote data ciphertext and MAC
    std::map<std::string, unsigned char[CIPHER_LEN]> remote_dir;
    std::map<std::string, unsigned char[MAC_LEN]> remote_mac_dir;
    // use the "org_dir" to store the variables that do not need to be calculated under ciphertext, such as counters.
    std::map<std::string, int64_t> org_dic;
    //
    /*
    call markle tree
    */
    //merkleTree* data_merkle_tree;
    while(notify()){
    	int act;
    	int ret;
    	int a,b;
    	unsigned char msg[CIPHER_LEN];
        unsigned char label_temp[LABEL_LEN];
    	unsigned char enc_key[0x100];
    	unsigned char enc_data[0x10];
    	uint64_t value;
        std::string mid;
        std::string key_po;
        std::string line;
        std::string file_path;
        std::ifstream file;
    	unsigned int key_len = 0;
    	unsigned int data_len = 0;
        unsigned int mac_len = 0;
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
                    unsigned char label[LABEL_LEN];
                    memcpy(label, key_po.c_str(), key_po.length());
                    tru->encrypt_MAC(label, key_po.length(), msg, CIPHER_LEN, dir[key_po], data_len, dir_mac[key_po], mac_len);

                }
    			send_data(nettool, enc_key, key_len, dir, dir_mac);
                nettool->is_data_send = true;
                if(is_send_output_mac){
                    send_cmd_mac(nettool, protocol->tran_mac_last(tru));
                }else{
                    send_cmd_mac(nettool, protocol->tran_mac(tru));
                }
                
                nettool->is_mac_send = true;
    			break;
    		case 2:
                //test end  
    			// if(!nettool->is_data_store){
       //              printf("wait for data exchage");
       //              continue;
       //          }
                if(!nettool->is_data_store || !nettool->is_data_send){
                    //sign data hash and send it
                    printf("before run protocol, plz send data or receive data\n");
                    break;
                }
                if(!nettool->is_mac_store || !nettool->is_mac_send){
                    //sign data hash and send it
                    printf("before run protocol, plz send protocol\n");
                    break;
                }
                if(is_send_output_mac){
                    protocol->load_mac_last(nettool->mac_dir);
                }else{
                    protocol->load_mac(nettool->mac_dir);
                }
               
                printf("load protocol successfully......\n");
                protocol->clear_iteam();
                //take date from network tool
                remote_dir = nettool->data_dic;
                remote_mac_dir = nettool->data_mac_dic;
                //clear plaintext dictionary
                org_dic.clear();
                // verify data and the hard ware will encrypt with its key
                for(auto &v : remote_dir){
                    //tran all data to local
                    unsigned char label[LABEL_LEN];
                    memcpy(label, v.first.c_str(), v.first.length());
                    if(!tru->verify_data(label, v.first.length(), v.second, CIPHER_LEN, remote_mac_dir[v.first], MAC_LEN, dir[v.first], data_len, dir_mac[v.first], mac_len)){
                        printf("illegal data\n");
                    }
                }
                /*

                //First version of data tansform
                to_byte16(0,msg);
                for(auto &v : remote_dir){
                    //tran all data to local
                    tru->operation(v.second, 16, SWI_REM, msg, 16, SWI_PLA, local_dir[v.first], data_len, ADD_OP);
                }*/
                /*
                store with merkle tree
                */
                //data_merkle_tree = new merkleTree(local_dir);

                while(true){
                    truple_mac now_trp_mac = protocol->next_mac();
                    truple now_trp = now_trp_mac.trup;
                    if(debug_this)
                        std::cout<<"now operation |"<<now_trp.operand1<<"| "<<now_trp.op<<" |"<<now_trp.operand2<<"| -> "<<now_trp.output<<std::endl;
                    if(debug_this)
                        std::cin>>a;
                    deal_cmd(now_trp_mac, protocol->now_step, tru, protocol->dic_goto, dir, dir_mac, org_dic);
                    //deal_cmd(now_trp, protocol->now_step, tru, protocol->dic_goto, remote_dir, dir, org_dic);
                    if(debug_this)
                        std::cout<<"now_step:"<< protocol->now_step<<std::endl;
                    if(protocol->now_step >= protocol->size_of_protocol()){
                        break;
                    }
                }
                //for test
                // unsigned char output[16],ans[16];
                // uint nn,nn2;
                // uint64_t answer;
                // tru->operation(dir["B"],16,SWI_ORG,nettool->data_dic["A"],16, SWI_REM,output,nn,AND_OP);
                // tru->decrypto(output,16,ans,nn2);
                // to_ll(ans,answer);
                // std::cout<<"get answer: "<<answer<<std::endl;
    			break;
    		case 3:
            {

                baseInt img_data[224*224*3];
                std::cout<<"input file path and name \n";
                std::cin>>file_path;
                std::string file_suffix = file_path.substr(file_path.find_last_of('.') + 1);
                if(file_suffix == "mdl"){
                    tru->encrypto_key(enc_key, key_len);
                    load_model<baseInt>(img_data, 224*224*3, "./save_data/data.mdl");
                    /*printf("[");
                    for(int i = 0; i < 224; i++){
                        printf("%f,", img_data[i]);
                    }
                    printf("]\n");*/
                    unsigned char data_of_img[224*224*3*sizeof(baseInt)];
                    unsigned int data_of_img_len = 224*224*3*sizeof(baseInt);
                    unsigned char data_of_img_en[224*224*3*sizeof(baseInt)];
                    unsigned int data_of_img_en_len;
                    memcpy(data_of_img,img_data,224*224*3*sizeof(baseInt));
                    
                    tru->encrypto(data_of_img, data_of_img_len, data_of_img_en, data_of_img_en_len);
                    send_img(nettool,enc_key, key_len, data_of_img_en, data_of_img_en_len);
                    printf("\n");
                }else{
                    file.open(file_path,std::ios::in);
                    if(!file.is_open()){
                        printf("no such file\n");
                        break;    
                    }
                    dir.clear();
                    tru->encrypto_key(enc_key, key_len);

                    while(getline(file,line)){
                        if(line.empty()) continue;
                        key_po = line.substr(0,line.find(" "));
                        mid = line.substr(line.find(" ")+1);
                        value = atoi(mid.c_str());
                        if(key_po == "0" && value == 0){
                            break;
                        }
                        to_byte16(value,msg);
                        unsigned char label[LABEL_LEN];
                        memcpy(label, key_po.c_str(), key_po.length());
                        tru->encrypt_MAC(label, key_po.length(), msg, CIPHER_LEN, dir[key_po], data_len, dir_mac[key_po], mac_len);

                    }
                    send_data(nettool, enc_key, key_len, dir, dir_mac);
                    nettool->is_data_send = true;
                    if(is_send_output_mac){
                        send_cmd_mac(nettool, protocol->tran_mac_last(tru));
                    }else{
                        send_cmd_mac(nettool, protocol->tran_mac(tru));
                    }
                    nettool->is_mac_send = true;
                }
            }    
    			break;
            case 4:
            {
                time_t t;
                t = time(NULL);
                if(!nettool->is_data_store){
                    //sign data hash and send it
                    printf("before run protocol, plz send data or receive data\n");
                    break;
                }
                baseInt arr[1000];
                tru->data_input(nettool->rev_img_data, img_size, 224,224,3);
                for(int i = 0; i < protocol->structures.size(); i++){
                    printf("struct %s\n",  protocol->structures[i].c_str());
                    int len;
                    load_model_len(len, protocol->structures[i]);
                    unsigned char structure[len];
                    load_model<unsigned char>(structure,len,protocol->structures[i]);
                    int W_len;
                    load_model_len(W_len, protocol->weights[i]);
                    baseInt *weight = new baseInt[W_len];
                    load_model<baseInt>(weight,W_len,protocol->weights[i]);
                    tru->block(weight, W_len, structure, len,arr);
                    delete weight;
                    weight = NULL;
                }
                baseInt result[1000];
                unsigned char result_un[4000];
                softmax(arr,result,1000);
                for(int i = 0; i < 100; i ++){
                    printf("%f ", arr[i]);
                }
                memcpy(result_un, arr, sizeof(baseInt) * 1000);
                unsigned char data_key[16] = {113,26,66,150,170,194,214,179,160,144,202,72,108,74,136,63};
                /*encrypto the data with data_key*/
                unsigned char ciphertex[4000];
                unsigned int len_out;
                //tru->gen_sym_key(data_key,16);
                store_data_to_file(data_key, 16, "./key.data.b");
                if(SG_SymEnc(SGD_SMS4_ECB ,data_key,16,data_key,16,result_un,4000,ciphertex,&len_out) != SAR_OK){
                    perror("encrypto data error\n");
                }

                /*generate the hash of data_key*/
                unsigned char key_hash[32];
                std::string hash_key = sha3_k(data_key, 16);
                for(int i = 0; i < 32; i ++){
                    sscanf(hash_key.c_str()+i*2, "%02X", key_hash);
                    //printf("%u ", key_hash[i]);
                }
                printf("\n");
                /*send key_hash and ciphertex to the other party*/
                send_res_data(nettool, key_hash, 32, ciphertex, len_out);
                /*store the key in local*/
                std::string key_str = "711a4296aac2d6b3a090ca486c4a883f";
                store_file_with_string(key_str,"./key.data");

                
                /*store plaintext for test*/
                store_data_to_file(result_un, 4000, "./plaintext.data");

                double cost_t = time(NULL) - t;
                /*printf("output:\n[");
                for(int i = 0; i < 1000; i ++ ){
                    printf("%f ", arr[i]);
                }*/
                //printf("running time %f\n", cost_t);
                break;
            }
    		case 5:
    			exit(1);
            default:
                printf("illegal instruction\n");
    	}
    }
}