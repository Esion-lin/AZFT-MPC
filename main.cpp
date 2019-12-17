#include "network.h"
#include "protocol.h"
#include "Merkle_Hash.h"
bool debug_this = false;
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
std::string get_item(std::map<std::string,int> dic, std::string str){
    if(str.find("[") == str.npos){
        return str;
    }else{
        std::string arr = str.substr(0,str.find("["));
        std::string num = str.substr(str.find("[")+1, str.find("]") - str.find("[")-1);
        //std::cout<<"num is "<<num<<"|"<<std::endl;
        int now_num;char ans[10];
        if(num[0]>='0' && num[0]<='9'){
            arr = arr + "_" + num;
        }else if(dic.find(num) != dic.end()){
            sprintf(ans, "%d", dic[num]);
            arr = arr + "_" + ans;
        }else{
            printf("index error(%s):array index is cipher text.\n", num.c_str());
        }
        return arr;

    }
}
int tran_op(std::string op){
    if(op == ">>>" || op == ">>"){
        return SHF_RI;
    }else if(op == "<<<" || op == "<<"){
        return SHF_LE;
    }else if(op == "&"){
        return AND_OP;
    }else if(op == "|"){
        return OR_OP;
    }else if(op == "+"){
        return ADD_OP;
    }else if(op == "-"){
        return SUB_OP;
    }else if(op == "*"){
        return MUL_OP;
    }else if(op == "/"){
        return DIV_OP;
    }else if(op == "xor"){
        return NAND_OP;
    }else if(op == ">"){
        return GREAT_OP;
    }else if(op == ">="){
        return GE_OP;
    }else if(op == "<"){
        return LESS_OP;
    }else if(op == ">="){
        return LE_OP;
    }else if(op == "=="){
        return EQ_OP;
    }
}
int tran_op(int a, int b, std::string op){
    int ans;
    if(op == ">>>" || op == ">>"){
        ans = a >> b;
    }else if(op == "<<<" || op == "<<"){
        ans = a << b;
    }else if(op == "&"){
        ans = a & b;
    }else if(op == "|"){
        ans = a | b;
    }else if(op == "+"){
        ans = a + b;
    }else if(op == "-"){
        ans = a - b;
    }else if(op == "*"){
        ans = a * b;
    }else if(op == "/"){
        ans = a / b;
    }else if(op == "xor"){
        ans = a ^ b;
    }else if(op == ">"){
        ans = a > b;
    }else if(op == ">="){
        ans = a >= b;
    }else if(op == "<"){
        ans = a < b;
    }else if(op == ">="){
        ans = a >= b;
    }else if(op == "=="){
        ans = a == b;
    }
    if(debug_this)
        std::cout<<"plain ans is "<<ans<<std::endl;
    return ans;
}
int judge(std::string str,std::map<std::string, unsigned char[16]>remote_dir, std::map<std::string, unsigned char[16]>local_dir, std::map<std::string, int> org_dir){
    if(local_dir.find(str) != local_dir.end()){
        return 2;
    }else if(remote_dir.find(str) != remote_dir.end()){
        return 1;
    }else{
        return 3;
    }
}
bool is_num(std::string data , int &num){
    if((data[0]>='0' && data[0]<='9' )||data[0]=='-' ){
        num = atoi(data.c_str());
        return true;
    }else{
        return false;
    }
} 
void deal_cmd(truple now_trp, int &now_step, void *tmp, std::map<std::string,int>goto_dir, std::map<std::string, unsigned char[16]>&remote_dir, std::map<std::string, unsigned char[16]>&local_dir, std::map<std::string, int> &org_dir){
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
                            to_byte16((uint64_t)temp,temp_msg);
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
}

int main(){
    std::map<std::string, unsigned char[16]> dir;
	pthread_t   recv_tid;
    truthtee* tru = new truthtee();
    netTool* nettool = new netTool(tru);
    PotocolRead* protocol = new PotocolRead("./protocol_file/org.jimple");
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
    /*
    from protocol make MerkleTree
    and exchange it
    */
    //load protocol
    std::map<std::string, unsigned char[16]> remote_dir;
    std::map<std::string, int> org_dic;
    std::map<std::string, unsigned char[16]> local_dir;
    //
    merkleTree* data_merkle_tree;
    while(notify()){
    	int act;
    	int ret;
    	int a,b;
    	unsigned char msg[16];
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
                is_data_send = true;
    			break;
    		case 2:
                //test end  
    			// if(!nettool->is_data_store){
       //              printf("wait for data exchage");
       //              continue;
       //          }

                if(!is_data_store || !is_data_send){
                    //sign data hash and send it
                    printf("before run protocol, plz send data or receive data");
                    break;
                }
                protocol->clear_iteam();
                remote_dir = nettool->data_dic;
                org_dic.clear();
                local_dir = dir;
                to_byte16(0,msg);
                for(auto &v : remote_dir){
                    tru->operation(v.second, 16, SWI_REM, msg, 16, SWI_PLA, local_dir[v.first], data_len, ADD_OP);
                }
                data_merkle_tree = new merkleTree(local_dir);

                while(true){
                    truple now_trp = protocol->next();
                    if(debug_this)
                        std::cout<<"now operation |"<<now_trp.operand1<<"| "<<now_trp.op<<" |"<<now_trp.operand2<<"| -> "<<now_trp.output<<std::endl;
                    if(debug_this)
                        std::cin>>a;
                    deal_cmd(now_trp, protocol->now_step, tru, protocol->dic_goto, remote_dir, local_dir, org_dic);
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
                std::cout<<"input file path and name \n";
                std::cin>>file_path;
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
                    tru->encrypto(msg, 16, dir[key_po], data_len);

                }
                send_data(nettool, enc_key, key_len, dir);
    			break;
    		case 4:
    			exit(1);
    	}
    }
}