// Auther: Tainpei Lu
// Creation: 11/02 2019 
#include "network.h"
#include "protocol.h"
#include "tool.h"
#include <time.h>
#include <ctime>
#include "tee.h"
#include "component.h"
bool debug_this = false;
bool notify(){
	printf("choose a operation:\n \t1.send encrypted data\n \t2.run ins\n \t3.send encrypted file\n \t4r.run ins block\n");
	return true;
}
void send_json(int action, uint8_t data[], int len, void *tmp){
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
	TEE* tru = (TEE*) tmp;
    uint8_t data[ASYM_KEY_FOR_SIGN_SIZE/8];
    tru->sign_key(data);
   
	send_json(sign_action, data, ASYM_KEY_FOR_SIGN_SIZE/8, tmp2);
    
}
void send_img(void *tmp, uint8_t key_data[], int key_len, uint8_t * data, int len_data){
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
void send_res_data(void *tmp, uint8_t* key, int key_len, uint8_t* data, int data_len){
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

void send_pub_key(void *tmp,void *tmp2){
	TEE* tru = (TEE*) tmp;
    uint32_t data_len = ASYM_KEY_SIZE/8 + ASYM_KEY_EXPONENT_SIZE/8 + ASYM_KEY_FOR_SIGN_SIZE/8 + ASYM_KEY_EXPONENT_SIZE/8;
	uint8_t data[data_len];
    tru->query_pub_stream(data);
	send_json(key_ex_action, data, data_len, tmp2);
}
void send_data(void *tmp, uint8_t* label, uint32_t label_len, uint8_t* data, uint32_t data_len, uint8_t* mac, uint32_t mac_len){
	Json::Value  json;
    // Json::Value  jlabel;
    // Json::Value  jdata;
    // Json::Value  jmac;
	netTool* nettool = (netTool*) tmp;
    std::string label_s(label,label+label_len);
    printf("label len:%u\n",label_s.length());
    std::string data_s(data,data+data_len);
    printf("data len:%u\n",data_s.length());
    std::string mac_s(mac,mac+mac_len);
    printf("mac len:%u\n",mac_s.length());
	json["action"] = data_action;
	// for(int i = 0; i < label_len; i++){
 //        jlabel[i] = label[i];
 //    }
 //    for(int i = 0; i < data_len; i++){
 //        jdata[i] = data[i];
 //    }
 //    for(int i = 0; i < mac_len; i++){
 //        jmac[i] = mac[i];
 //    }
    json["label"] = label_s;
    json["data"] = data_s;
    json["mac"] = mac_s;
    printf("label size:%u\n",json["label"].asString().length());
    printf("data size:%u\n",json["data"].asString().length());
    printf("mac size:%u\n",json["mac"].asString().length());

    nettool->send_data(json);
}


int main(int argc, char* argv[]){
	pthread_t   recv_tid;
    TEE *tru = new TEE();
    //truthtee* tru = new truthtee();
    printf("checking protocol file.....\n");
    bool is_ML = false;
    std::string path = "./code_data.code";
    
    bool load_succ = true;
    PotocolRead* protocol = new PotocolRead(path, load_succ, true, is_ML);
    if(!load_succ){
      printf("protocol error\n");
        exit(0);
    }
    printf("protocol file length:%u\n",protocol->protocol.code_size);
    netTool* nettool = new netTool(tru);
    if(argc>=2){
        nettool->recv_port = atoi(argv[1]);
        printf("set listen point %d\n",nettool->recv_port);

    }else{
        printf("please input port to listen:\n");
        std::cin>>nettool->recv_port;
        
    }
    if(pthread_create(&recv_tid , NULL , init_listen_static, (void *)nettool) == -1){
            perror("pthread create error.\n");
            exit(1);
        }
    //printf("please input host to connect:\n");
    std::string host = "127.0.0.1";
    //std::cin>>host;
    if(argc>=3){
        int port = atoi(argv[2]);
        nettool->set_host_port(host,port);
        printf("set send point%d\n",port);
    }
    else{
        printf("please input port to connect:\n");
        int port;
        std::cin>>port;
        nettool->set_host_port(host,port);
    }
    
    send_pub_key(tru, nettool);
    printf("Key exchanging......\n");
    while(!nettool->is_key_store){
        sleep(1);
    };
    printf("Key exchanging successfully.\n");
    
    /*sign the public key*/
    sign_key(tru,nettool);
    printf("Key verifing.......\n");
    while(!nettool->is_key_verify){
        sleep(1);
    };
    printf("Key verify successfully.\n");
    /*
    from protocol make MerkleTree
    or use counter + MAC
    and exchange it
    */

    //load protocol
    

    /*
    call markle tree
    */

    //merkleTree* data_merkle_tree;
    while(notify()){
    	int act;
    	float value;
        char key_po[LABEL_LEN];
    	std::cin>>act;
    	switch(act){
    		case -2:{
                struct Data data;
                uint8_t msg[sizeof(float)];
                data.item_capacity = 20;
                data.label = (uint8_t*)malloc(data.item_capacity*LABEL_LEN);
                data.pos = (uint8_t*)malloc(data.item_capacity*POS_LEN);
                data.data_capacity = 20*32;
                data.data = (uint8_t*)malloc(data.data_capacity);
                while(std::cin>>key_po>>value){
                    if(key_po[0] == '0' && value == 0){
                        break;
                    }
                    memcpy(msg, &value, sizeof(float));
                    add_data(&data,(uint8_t*)key_po, LABEL_LEN, msg, sizeof(float));
                }
                uint32_t send_label_len = data.label_size;
                uint8_t* label_send = (uint8_t*)malloc(send_label_len*LABEL_LEN);
                uint32_t send_data_len = data.label_size*POS_LEN + data.data_len;
                uint8_t* data_send = (uint8_t*)malloc(data.label_size*POS_LEN + data.data_len);
                deserialize(data, label_send, &send_label_len, data_send, &send_data_len);
                uint32_t cipher_data_len = send_data_len + ASYM_KEY_SIZE/8;
                uint8_t* ciphertex = (uint8_t*)malloc(cipher_data_len);
                uint32_t cipher_mac_len = SYM_KEY_SIZE/8 + ASYM_KEY_SIZE/8;
                uint8_t* mactex = (uint8_t*)malloc(cipher_mac_len);
    			tru->encrypt_with_MAC(label_send, send_label_len, data_send, send_data_len, ciphertex, &cipher_data_len, mactex, &cipher_mac_len);

                send_data(nettool, label_send, send_label_len*LABEL_LEN, ciphertex, cipher_data_len,mactex,cipher_mac_len);
                nettool->is_data_send = true;

    		}
            break;
            case 1:
            {

                //struct Data data;
                
                uint32_t label_stream_len;
                uint32_t data_stream_len;
                load_model_len(label_stream_len, "./label_data.data");
                uint8_t* label_stream = (uint8_t*)malloc(label_stream_len);
                load_model<uint8_t>(label_stream,label_stream_len,"./label_data.data");
                load_model_len(data_stream_len, "./data_data.data");
                uint32_t data_stream_len_edit = data_stream_len + (16 - data_stream_len % 16);
                uint8_t* data_stream = (uint8_t*)malloc(data_stream_len_edit);
                load_model<uint8_t>(data_stream,data_stream_len,"./data_data.data");
                //data = serialize(label_stream, label_stream_len/5, data_stream, data_stream_len); 
                
                uint32_t cipher_data_len = data_stream_len_edit + ASYM_KEY_SIZE/8;
                uint8_t* ciphertex = (uint8_t*)malloc(cipher_data_len);
                uint32_t cipher_mac_len = SYM_KEY_SIZE/8 + ASYM_KEY_SIZE/8;
                uint8_t* mactex = (uint8_t*)malloc(cipher_mac_len);
                tru->encrypt_with_MAC(label_stream, label_stream_len, data_stream, data_stream_len_edit, ciphertex, &cipher_data_len, mactex, &cipher_mac_len);
                printf("need send label:%u data:%u mac:%u\n",label_stream_len, cipher_data_len, cipher_mac_len);
                send_data(nettool, label_stream, label_stream_len, ciphertex, cipher_data_len,mactex,cipher_mac_len);
                nettool->is_data_send = true;

            }    
             break;
             case 2:{
                uint32_t pro_mac_len;
                load_model_len(pro_mac_len, "./code_mac.code");
                uint8_t* pro_mac = (uint8_t*)malloc(pro_mac_len);
                printf("pro_mac len %u\n",pro_mac_len);
                load_model<uint8_t>(pro_mac, pro_mac_len, "./code_mac.code");
                tru->run_op(protocol->data,protocol->data_len,pro_mac,pro_mac_len);
             }break;
             case 3:{
                uint8_t label_input[LABEL_LEN];
                uint32_t query_size;
                printf("input label to continue query:");
                std::cin>>label_input;
                printf("input size to continue query:");
                std::cin>>query_size;
                uint8_t tru_out[query_size];
                tru->query_data(label_input, LABEL_LEN, tru_out, &query_size);
                printf("get data [\n");
                for(int i = 0; i < query_size; i++){
                    printf("%u ",tru_out[i]);
                }
                printf("]\n");
             }break;
    		// case 2:
      //           //test end  
    		// 	// if(!nettool->is_data_store){
      //  //              printf("wait for data exchage");
      //  //              continue;
      //  //          }
      //           if(!nettool->is_data_store || !nettool->is_data_send){
      //               //sign data hash and send it
      //               printf("before run protocol, plz send data or receive data\n");
      //               break;
      //           }
                
      //           if(is_send_output_mac){
      //               protocol->load_mac_last(nettool->mac_dir);
      //           }else{
      //               protocol->load_mac(nettool->mac_dir);
      //           }
               
      //           printf("load protocol successfully......\n");
      //           protocol->clear_iteam();
      //           //take date from network tool
      //           remote_dir = nettool->data_dic;
      //           remote_mac_dir = nettool->data_mac_dic;
      //           //clear plaintext dictionary
      //           org_dic.clear();
      //           // verify data and the hard ware will encrypt with its key
      //           for(auto &v : remote_dir){
      //               //tran all data to local
      //               uint8_t label[LABEL_LEN];
      //               memcpy(label, v.first.c_str(), v.first.length());
      //               if(!tru->verify_data(label, v.first.length(), v.second, CIPHER_LEN, remote_mac_dir[v.first], MAC_LEN, dir[v.first], data_len, dir_mac[v.first], mac_len)){
      //                   printf("illegal data\n");
      //               }
      //           }
      //           /*

      //           //First version of data tansform
      //           to_byte16(0,msg);
      //           for(auto &v : remote_dir){
      //               //tran all data to local
      //               tru->operation(v.second, 16, SWI_REM, msg, 16, SWI_PLA, local_dir[v.first], data_len, ADD_OP);
      //           }*/
      //           /*
      //           store with merkle tree
      //           */
      //           //data_merkle_tree = new merkleTree(local_dir);

      //           while(true){
      //               truple_mac now_trp_mac = protocol->next_mac();
      //               truple now_trp = now_trp_mac.trup;
      //               if(debug_this)
      //                   std::cout<<"now operation |"<<now_trp.operand1<<"| "<<now_trp.op<<" |"<<now_trp.operand2<<"| -> "<<now_trp.output<<std::endl;
      //               if(debug_this)
      //                   std::cin>>a;
      //               deal_cmd(now_trp_mac, protocol->now_step, tru, protocol->dic_goto, dir, dir_mac, org_dic);
      //               //deal_cmd(now_trp, protocol->now_step, tru, protocol->dic_goto, remote_dir, dir, org_dic);
      //               if(debug_this)
      //                   std::cout<<"now_step:"<< protocol->now_step<<std::endl;
      //               if(protocol->now_step >= protocol->size_of_protocol()){
      //                   break;
      //               }
      //           }
      //           //for test
      //           // uint8_t output[16],ans[16];
      //           // uint nn,nn2;
      //           // uint64_t answer;
      //           // tru->operation(dir["B"],16,SWI_ORG,nettool->data_dic["A"],16, SWI_REM,output,nn,AND_OP);
      //           // tru->decrypto(output,16,ans,nn2);
      //           // to_ll(ans,answer);
      //           // std::cout<<"get answer: "<<answer<<std::endl;
    		// 	break;
    		// case 3:
      //       {

      //           baseInt img_data[224*224*3];
      //           std::cout<<"input file path and name \n";
      //           std::cin>>file_path;
      //           std::string file_suffix = file_path.substr(file_path.find_last_of('.') + 1);
      //           if(file_suffix == "mdl"){
      //               tru->encrypto_key(enc_key, key_len);
      //               load_model<baseInt>(img_data, 224*224*3, "./save_data/data.mdl");
      //               /*printf("[");
      //               for(int i = 0; i < 224; i++){
      //                   printf("%f,", img_data[i]);
      //               }
      //               printf("]\n");*/
      //               uint8_t data_of_img[224*224*3*sizeof(baseInt)];
      //               uint32_t data_of_img_len = 224*224*3*sizeof(baseInt);
      //               uint8_t data_of_img_en[224*224*3*sizeof(baseInt)];
      //               uint32_t data_of_img_en_len;
      //               memcpy(data_of_img,img_data,224*224*3*sizeof(baseInt));
                    
      //               tru->encrypto(data_of_img, data_of_img_len, data_of_img_en, data_of_img_en_len);
      //               send_img(nettool,enc_key, key_len, data_of_img_en, data_of_img_en_len);
      //               printf("\n");
      //           }else{
      //               file.open(file_path,std::ios::in);
      //               if(!file.is_open()){
      //                   printf("no such file\n");
      //                   break;    
      //               }
      //               dir.clear();
      //               tru->encrypto_key(enc_key, key_len);

      //               while(getline(file,line)){
      //                   if(line.empty()) continue;
      //                   key_po = line.substr(0,line.find(" "));
      //                   mid = line.substr(line.find(" ")+1);
      //                   value = atoi(mid.c_str());
      //                   if(key_po == "0" && value == 0){
      //                       break;
      //                   }
      //                   to_byte16(value,msg);
      //                   uint8_t label[LABEL_LEN];
      //                   memcpy(label, key_po.c_str(), key_po.length());
      //                   tru->encrypt_MAC(label, key_po.length(), msg, CIPHER_LEN, dir[key_po], data_len, dir_mac[key_po], mac_len);

      //               }
      //               send_data(nettool, enc_key, key_len, dir, dir_mac);
      //               nettool->is_data_send = true;
      //               if(is_send_output_mac){
      //                   send_cmd_mac(nettool, protocol->tran_mac_last(tru));
      //               }else{
      //                   send_cmd_mac(nettool, protocol->tran_mac(tru));
      //               }
      //               nettool->is_mac_send = true;
      //           }
      //       }    
    		// 	break;
      //       case 4:
      //       {
      //           time_t t;
      //           t = time(NULL);
      //           if(!nettool->is_data_store){
      //               //sign data hash and send it
      //               printf("before run protocol, plz send data or receive data\n");
      //               break;
      //           }
      //           baseInt arr[1000];
      //           tru->data_input(nettool->rev_img_data, img_size, 224,224,3);
      //           for(int i = 0; i < protocol->structures.size(); i++){
      //               printf("struct %s\n",  protocol->structures[i].c_str());
      //               int len;
      //               load_model_len(len, protocol->structures[i]);
      //               uint8_t structure[len];
      //               load_model<uint8_t>(structure,len,protocol->structures[i]);
      //               int W_len;
      //               load_model_len(W_len, protocol->weights[i]);
      //               baseInt *weight = new baseInt[W_len];
      //               load_model<baseInt>(weight,W_len,protocol->weights[i]);
      //               tru->block(weight, W_len, structure, len,arr);
      //               delete weight;
      //               weight = NULL;
      //           }
      //           baseInt result[1000];
      //           uint8_t result_un[4000];
      //           softmax(arr,result,1000);
      //           for(int i = 0; i < 100; i ++){
      //               printf("%f ", arr[i]);
      //           }
      //           memcpy(result_un, arr, sizeof(baseInt) * 1000);
      //           uint8_t data_key[16] = {113,26,66,150,170,194,214,179,160,144,202,72,108,74,136,63};
      //           /*encrypto the data with data_key*/
      //           uint8_t ciphertex[4000];
      //           uint32_t len_out;
      //           //tru->gen_sym_key(data_key,16);
      //           store_data_to_file(data_key, 16, "./key.data.b");
      //           if(SG_SymEnc(SGD_SMS4_ECB ,data_key,16,data_key,16,result_un,4000,ciphertex,&len_out) != SAR_OK){
      //               perror("encrypto data error\n");
      //           }

      //           /*generate the hash of data_key*/
      //           uint8_t key_hash[32];
      //           std::string hash_key = sha3_k(data_key, 16);
      //           for(int i = 0; i < 32; i ++){
      //               sscanf(hash_key.c_str()+i*2, "%02X", key_hash);
      //               //printf("%u ", key_hash[i]);
      //           }
      //           printf("\n");
      //           /*send key_hash and ciphertex to the other party*/
      //           send_res_data(nettool, key_hash, 32, ciphertex, len_out);
      //           /*store the key in local*/
      //           std::string key_str = "711a4296aac2d6b3a090ca486c4a883f";
      //           store_file_with_string(key_str,"./key.data");

                
      //           /*store plaintext for test*/
      //           store_data_to_file(result_un, 4000, "./plaintext.data");

      //           double cost_t = time(NULL) - t;
      //           /*printf("output:\n[");
      //           for(int i = 0; i < 1000; i ++ ){
      //               printf("%f ", arr[i]);
      //           }*/
      //           //printf("running time %f\n", cost_t);
      //           break;
      //       }
    		case 5:
    			exit(1);
            default:
                printf("illegal instruction\n");
    	}
    }
}