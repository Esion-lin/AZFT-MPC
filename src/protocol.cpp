// Auther: Tainpei Lu
// Creation: 11/02 2019 
//protocol.cpp
#include "protocol.h"
using namespace std;
bool pro_test = false;
//deal with array, split array name
string middle(string str){
	if(str.find("[") != str.npos)
		return str.substr(0,str.find("["));
	return str;
}
//deal with array, split array index
string index(string str){
	if(str.find("[") != str.npos)
		return str.substr(str.find("[")+1, str.find("]") - str.find("[")-1);
	return str;
}
//this reader function is data dependence, so if the jimple file style is changed, please rewrite this function
bool PotocolRead::Reader(std::string file_path){
	ifstream file;
	file.open(file_path,ios::in);
	if(!file.is_open()){
        printf("protocol file does not exist\n");
        return false;
    }
    std::string strLine;
    int step = 0;
    while(getline(file,strLine)){
    	//cout<<strLine<<endl;
        if(strLine.empty())
            continue;
        strLine = strLine.substr(strLine.find_first_not_of(" "));
        if(strLine.find("IN") != strLine.npos){
        	string mid = strLine.substr(0,strLine.find(" "));
        	if(strLine.find("[") != strLine.npos){
        		string num = strLine.substr(strLine.find("[")+1, strLine.find("]") - strLine.find("[")-1);

        		int numb = atoi(num.c_str());
        		char ans[10];
        		string tar;
        		for(int i = 0; i < numb; i ++){
					sprintf(ans, "%d", i);
            		tar = mid + "_" + ans;
            		dic_var[tar] = 1;
        		}
        	}
       			dic_var[mid] = 1;	
        	
        	
        	continue;
        }
        if(strLine.find("OUT") != strLine.npos){
        	string mid = strLine.substr(strLine.find(">")+2,strLine.find(";") - strLine.find(">") - 3);
        	output.push_back(mid);
        	if(pro_test){
        		cout<<"out:"<<mid<<"|"<<endl;
        	}
        	cmd.push_back({false,"","",mid,"out"});
        	step ++;
        	continue;
        }
        // if goto
        if(strLine.find("if") != strLine.npos){
        	string mid = strLine.substr(strLine.find("if")+3,strLine.find("goto") - strLine.find("if") - 4);
        	
			string a = mid.substr(0, mid.find(" "));
			mid = mid.substr(mid.find(" ")+1);
			string op = mid.substr(0, mid.find(" "));
			string b = mid.substr(mid.find(" ")+1);
			string target = strLine.substr(strLine.find("label"), strLine.find(";") - strLine.find("label"));
			if(pro_test){
        		cout<<"out:"<<target<<"|"<<a<<"|"<<op<<"|"<<b<<"|"<<endl;
        	}
			cmd.push_back({true,a,b,target,op});
			step ++;
			continue;
    	}
    	//just goto
    	if(strLine.find("goto") != strLine.npos){
    		string mid = strLine.substr(strLine.find("label"),strLine.find(";")-strLine.find("label"));
			if(pro_test){
        		cout<<"out:"<<mid<<"|goto"<<endl;
        	}
    		cmd.push_back({true,"","",mid,"goto"});
        	step ++;
			continue;
    	}
    	// 
    	if(strLine.find("label") != strLine.npos){
    		string mid = strLine.substr(strLine.find("label"),strLine.find(":"));
    		if(pro_test){
        		cout<<"out:"<<mid<<"|"<<endl;
        	}
    		dic_goto[mid] = step;
    		continue;
    	}
    	if(strLine.find(":=") != strLine.npos){
    		continue;
    	}
    	if(strLine.find("=") != strLine.npos){
    		if(strLine.find("MPC") != strLine.npos) continue;
    		if(strLine.find("newarray") != strLine.npos) continue;
    		string mid = strLine.substr(strLine.find_first_not_of("\t"),strLine.find(";") - strLine.find_first_not_of("\t"));
    		string target = mid.substr(0,mid.find(" "));
    		mid = mid.substr(mid.find("=")+2);
    		string a;
    		if(mid.find(" ") != mid.npos){
    			a = mid.substr(0,mid.find(" "));
    			mid = mid.substr(mid.find(" ") + 1);
    			string op = mid.substr(0,mid.find(" "));
    			string b = mid.substr(mid.find(" ")+1);
				if(pro_test){
        			cout<<"out:"<<target<<"|"<<a<<"|"<<op<<"|"<<b<<"|"<<endl;
        		}
    			cmd.push_back({false,a,b,target,op});
    			//if plain_var <- cipher_var, make plain varible become cipher varible
    			if(dic_var[middle(b)] > dic_var[middle(target)]){
    				dic_var[middle(target)] = dic_var[middle(b)];
    			}
    			if(dic_var[middle(a)] > dic_var[middle(target)]){
    				dic_var[middle(target)] = dic_var[middle(a)];
    			}
    		}
    		else{
    			a = mid.substr(0,mid.find(";"));
				if(pro_test){
        			cout<<"out:"<<target<<"|"<<a<<"|"<<endl;
        		}
    			cmd.push_back({false,a,"",target,""});
    			if(dic_var[middle(a)] > dic_var[middle(target)]){
    				dic_var[middle(target)] = dic_var[middle(a)];
    			}
    		}
    		step ++;
    		continue;

    	}
    	
    }

   	if(transfer()){
        if(!expand()){
            printf("Protocol error: Illegal input instruction\n");
            return false;
        }
    }else{
        printf("Protocol error: Illegal input instruction\n");
        return false;
    }
    return true;
}

bool PotocolRead::transfer(){
    //remove the branch statement which contain cipher text
	int ch = 0;

	while(true){
		ch = 0;
		for(int i = 0; i < cmd.size(); i++){
			if(!cmd[i].is_goto){
				if(dic_var[middle(cmd[i].operand1)] > dic_var[middle(cmd[i].output)]){
					dic_var[middle(cmd[i].output)] = dic_var[middle(cmd[i].operand1)];
					ch ++;
				}
				if(dic_var[middle(cmd[i].operand2)] > dic_var[middle(cmd[i].output)]){
					dic_var[middle(cmd[i].output)] = dic_var[middle(cmd[i].operand2)];
					ch ++;
				}
			}
		}
		if(ch == 0) break;
	}
	bool start_if = false;
	int start_end = 0;
	std::vector<truple> cmd_cp;
	std::map<std::string, int> dic_goto_cp;
	dic_goto_cp.insert(dic_goto.begin(), dic_goto.end());
	int i = 0;
	for(int i = 0; i < cmd.size(); i++){
		if(cmd[i].is_goto){
			if(cmd[i].op != "goto" && dic_var[middle(cmd[i].operand1)] + dic_var[middle(cmd[i].operand2)] >= 1){
                if(start_if){
                    printf("Branch error: invalid jump [%d] if %s %s %s goto %s\n", i, cmd[i].operand1.c_str(), cmd[i].op.c_str(), cmd[i].operand2.c_str(), cmd[i].output.c_str());
                    return false;
                }

				start_if = true;
				start_end = dic_goto_cp[cmd[i].output];
				for(auto &v : dic_goto_cp){
					if(v.second > i){
						dic_goto[v.first] += 3*(start_end - i - 1) + 1;
					}	
				}
				
				cmd_cp.push_back({false, cmd[i].operand1,cmd[i].operand2, "m0", cmd[i].op});
				cmd_cp.push_back({false, "m0", "1", "m1", "xor"});
			}else{
				cmd_cp.push_back(cmd[i]);
			}
		}else{
			if(!start_if){
				cmd_cp.push_back(cmd[i]);
			}else{
				if(i == start_end){
					cmd_cp.push_back(cmd[i]);
					start_if = false;
				}else{
					cmd_cp.push_back({false, "m0", cmd[i].output, "m2", "*"});
					cmd_cp.push_back({false, cmd[i].operand1, cmd[i].operand2, "m3", cmd[i].op});
					cmd_cp.push_back({false, "m3", "m1", "m3", "*"});
					cmd_cp.push_back({false, "m3", "m2", cmd[i].output, "+"});
						
				}
			}
		}
	}
	cmd.clear();
	cmd.swap(cmd_cp);
    

    for(int i = 0; i < cmd.size(); i++){
        //cout<<i<<" "<<cmd[i].operand1<<" "<<cmd[i].op<<" "<<cmd[i].operand2<<" = "<<cmd[i].output<<endl;
    	if(index(cmd[i].operand1)!=cmd[i].operand1 && dic_var[index(cmd[i].operand1)] == 1){
    		cout<<"index of arr is cipher text:"<<cmd[i].operand1;
            return false;
    	}else if(index(cmd[i].operand2)!=cmd[i].operand2 && dic_var[index(cmd[i].operand2)] == 1){
    		cout<<"index of arr is cipher text:"<<cmd[i].operand2;
            return false;
    	}else if(index(cmd[i].output)!=cmd[i].output && dic_var[index(cmd[i].output)] == 1){
    		cout<<"index of arr is cipher text:"<<cmd[i].output;
            return false;
    	}
    }
    dic_var["m0"] = 1;dic_var["m1"] = 1;dic_var["m2"] = 1;dic_var["m3"] = 1;
    /*for(auto &v:dic_var){
        cout<<v.first<<" "<<v.second<<endl;
    }*/
    return true;
    
    
	
}
bool PotocolRead::expand(){
    std::map<std::string, int64_t> org_dir;
    int64_t number_temp;
    std::vector<truple> cmd_cp;
    clear_iteam();
    while(now_step < size_of_protocol()){
        if(now_step < cmd.size()){
            now_step ++;
        }
        truple now_trp = cmd[now_step-1];
        int op1 = dic_var[middle(now_trp.operand1)];
        int op2 = dic_var[middle(now_trp.operand2)];
        int op = dic_var[middle(now_trp.output)];
        now_trp.operand1 = get_item(org_dir,now_trp.operand1);
        now_trp.operand2 = get_item(org_dir,now_trp.operand2);
        now_trp.output = get_item(org_dir,now_trp.output);
        if(now_trp.is_goto){
            if(now_trp.op == "goto"){
                now_step = dic_goto[now_trp.output];
            }else{
                //make sure all goto statement is under the plaintext
                if(is_num(now_trp.operand2,number_temp)){
                    if(org_dir.find(now_trp.operand1) == org_dir.end()){
                        printf("protocol expend error: in [%d]: %s = %s %s %s, invalid variable: %s\n", now_step, now_trp.output.c_str(), now_trp.operand1.c_str(), now_trp.op.c_str(), now_trp.operand2.c_str(), now_trp.operand1.c_str());
                        return false;
                    }
                    if(tran_op(org_dir[now_trp.operand1],number_temp,now_trp.op)){
                        now_step = dic_goto[now_trp.output];
                    }
                }else{
                    if(org_dir.find(now_trp.operand1) == org_dir.end()){
                        printf("protocol expend error: in [%d]: %s = %s %s %s, invalid variable: %s\n", now_step, now_trp.output.c_str(), now_trp.operand1.c_str(), now_trp.op.c_str(), now_trp.operand2.c_str(), now_trp.operand1.c_str());
                        return false;
                    }else if(org_dir.find(now_trp.operand2) == org_dir.end()){
                        printf("protocol expend error: in [%d]: %s = %s %s %s, invalid variable: %s\n", now_step, now_trp.output.c_str(), now_trp.operand1.c_str(), now_trp.op.c_str(), now_trp.operand2.c_str(), now_trp.operand2.c_str());
                        return false;
                    }
                    if(tran_op(org_dir[now_trp.operand1],org_dir[now_trp.operand2],now_trp.op)){
                        now_step = dic_goto[now_trp.output];
                    }
                }
            }
        }else{
            if(now_trp.op == "out"){
                cmd_cp.push_back(now_trp);
            }else if(now_trp.op == ""){
                if(op == 0){
                    if(is_num(now_trp.operand1,number_temp)){
                        org_dir[now_trp.output] = number_temp;
                    }else{
                        org_dir[now_trp.output] = org_dir[now_trp.operand1];
                    }
                }else{
                    if(!is_num(now_trp.operand1,number_temp) && op1 == 0){
                        now_trp.operand1 = std::to_string(org_dir[now_trp.operand1]);
                    }
                    cmd_cp.push_back(now_trp);
                }
            }else{
                /*
                General instructions
                The case of two number is illegal for example: a = 0x00 op 0x00
                */
                //to make sure operand1 is not a number
                if(is_num(now_trp.operand1,number_temp)){
                    int64_t number_temp2;
                    if(is_num(now_trp.operand2,number_temp2)){
                        if(op == 0){
                            org_dir[now_trp.output] = tran_op(number_temp, number_temp2, now_trp.op);
                        }else{
                            cmd_cp.push_back({false,to_string(tran_op(number_temp, number_temp2, now_trp.op)),"",now_trp.output,""});
                        }
                    }else{
                        //a = 0x00 op b
                        if(op == 1){
                            //b is cipher
                            cmd_cp.push_back(now_trp);
                        }else{
                            //b is plain
                            org_dir[now_trp.output] = tran_op(number_temp, org_dir[now_trp.operand2], now_trp.op);
                        }
                        std::string mid = now_trp.operand1;
                        now_trp.operand1 = now_trp.operand2;
                        now_trp.operand2 = mid;
                    }
                }
                else if(is_num(now_trp.operand2,number_temp)){
                    if(op1 == 0){
                        if(op == 0){
                            org_dir[now_trp.output] = tran_op(org_dir[now_trp.operand1], number_temp, now_trp.op);
                        }else{
                            now_trp.operand1 = std::to_string(org_dir[now_trp.operand1]);
                            cmd_cp.push_back(now_trp);
                        }
                    }else{
                        cmd_cp.push_back(now_trp);
                    }
                }else{
                    if(op == 0){
                        org_dir[now_trp.output] = tran_op(org_dir[now_trp.operand1], org_dir[now_trp.operand2], now_trp.op);
                    }else{
                        if(op1 == 0 && op2 == 0){
                            cmd_cp.push_back({false,to_string(tran_op(org_dir[now_trp.operand1], org_dir[now_trp.operand2], now_trp.op)),"",now_trp.output,""});
                        }else{
                            if(op1 == 0){
                                now_trp.operand1 = std::to_string(org_dir[now_trp.operand1]);
                            }
                            if(op2 == 0){
                                now_trp.operand2 = std::to_string(org_dir[now_trp.operand2]);
                            }
                            cmd_cp.push_back(now_trp);
                        }
                        
                    }
                }

            }
        }
        
    }
    //for test
    /*for(int i = 0; i < cmd_cp.size(); i++){
        cout<<i<<" "<<cmd_cp[i].operand1<<" "<<cmd_cp[i].op<<" "<<cmd_cp[i].operand2<<" = "<<cmd_cp[i].output<<endl;
    }*/
    cmd.clear();
    cmd.swap(cmd_cp);
    return true;
}
bool PotocolRead::store_filepath(std::string path){
    std::vector<std::string> files = getFiles(path);
    std::map<std::string, int> layers;
    std::map<std::string, int> layers_w;
    data_path = path;
    for(int i = 0; i < files.size(); i++){
        if(files[i].find("_") != files[i].npos){
            string prefix = files[i].substr(0,files[i].find("_"));
            string suffix = files[i].substr(files[i].find("_") + 1,files[i].find(".") - files[i].find("_") - 1);
            int tmp = suffix[suffix.length() - 1] - '0';
            if(tmp >= 0 && tmp <= 9){
                suffix = suffix.substr(0, suffix.length() - 1);
                if(suffix == "W"){
                    if(tmp != layers_w[prefix]){
                        return false;
                    }
                    layers_w[prefix] = tmp + 1;
                }else if(suffix == "struct"){
                    if(tmp != layers[prefix]){
                        return false;
                    }
                    layers[prefix] = tmp + 1;
                }
                
            }else{
                if(suffix == "W"){
                    layers_w[prefix] = 1;
                }else if(suffix == "struct"){
                    layers[prefix] = 1;
                }
            }
        }
       
    }
    if(layers["input"] == 1 && layers_w["input"] == 1){
        structures.push_back(path + "/input_struct.mdl");
        weights.push_back(path + "/input_W.mdl");
    }else{
        return false;
    }
    int itr = 1;
    while(true){
        string tmp = "cov" + to_string(itr);

        if(layers[tmp] == 0){
            break;
        }
        if(layers[tmp] != layers_w[tmp]){
            return false;
        }
        for(int i = 0; i < layers[tmp];i++){
            structures.push_back(path + "/" + tmp + "_struct" + to_string(i) + ".mdl");
            weights.push_back(path + "/" + tmp + "_W" + to_string(i) + ".mdl");
        }
        itr ++;
    }
    if(layers["output"] == 1 && layers_w["output"] == 1){
        structures.push_back(path + "/output_struct.mdl");
        weights.push_back(path + "/output_W.mdl");
    }else{
        return false;
    }
    return true;
}
PotocolRead::PotocolRead(std::string file_path, bool &init_succ, bool do_mac, bool ML){
    if(ML){
        is_ML = ML;
        is_cmd_mac = do_mac;
        init_succ = store_filepath(file_path);
    }else{
        init_succ = Reader(file_path);
        is_cmd_mac = do_mac;
        store();    
    }
	
}
std::vector<uint8_t[MAC_LEN]> PotocolRead::tran_mac(truthtee_pend *tru){
    std::vector<uint8_t[MAC_LEN]> mac_vector(cmd.size());
    if(is_cmd_mac){
        for(int i = 0; i < cmd.size(); i++){
            uint8_t arr[MAC_LEN];
            uint32_t mac_len;
            uint8_t label1[LABEL_LEN];
            uint8_t label2[LABEL_LEN];
            memcpy(label2, cmd[i].operand2.c_str(), cmd[i].operand2.length());
            if(cmd[i].op == "out"){
                memcpy(label1, cmd[i].output.c_str(), cmd[i].output.length());
                 //if use counter
                //tru->sign_cmd(label1, cmd[i].output.length(), label2, cmd[i].operand2.length(), tran_op(cmd[i].op), mac_vector[i], mac_len);
                tru->sign_cmd_without_counter(label1, cmd[i].output.length(), label2, cmd[i].operand2.length(), tran_op(cmd[i].op), mac_vector[i], mac_len);
            }else{
                memcpy(label1, cmd[i].operand1.c_str(), cmd[i].operand1.length());
                //if use counter
                //tru->sign_cmd(label1, cmd[i].operand1.length(), label2, cmd[i].operand2.length(), tran_op(cmd[i].op), mac_vector[i], mac_len);
                tru->sign_cmd_without_counter(label1, cmd[i].operand1.length(), label2, cmd[i].operand2.length(), tran_op(cmd[i].op), mac_vector[i], mac_len);
            }
           

            
            // truple_mac t1;
            // t1.trup = cmd[i];
            // memcpy(t1.mac, arr, MAC_LEN);
            // cmd_mac.push_back(t1);
        }
    }else{
        printf("No need to call this function, or you use MAC option when create instance.\n");
    }
        
    return mac_vector;
}
std::vector<uint8_t[MAC_LEN]> PotocolRead::tran_mac_last(truthtee_pend *tru){
    std::vector<uint8_t[MAC_LEN]> mac_vector(output.size());
    int j = 0; 
    uint8_t arr_vec[MAC_LEN];
    if(is_cmd_mac){
        for(int i = 0; i < cmd.size(); i++){
            uint8_t arr[MAC_LEN];
            uint32_t mac_len;
            uint8_t label1[LABEL_LEN];
            uint8_t label2[LABEL_LEN];
            int op = dic_var[middle(cmd[i].output)];
            memcpy(label2, cmd[i].operand2.c_str(), cmd[i].operand2.length());
            if(cmd[i].op == "out"){
                memcpy(label1, cmd[i].output.c_str(), cmd[i].output.length());
                 //if use counter
                //tru->sign_cmd(label1, cmd[i].output.length(), label2, cmd[i].operand2.length(), tran_op(cmd[i].op), mac_vector[i], mac_len);
                tru->sign_cmd_accu(label1, cmd[i].output.length(), label2, cmd[i].operand2.length(), tran_op(cmd[i].op), mac_vector[j], mac_len);
                j++;
            }else{
                if(op != 0){
                    memcpy(label1, cmd[i].operand1.c_str(), cmd[i].operand1.length());
                    //if use counter
                    //tru->sign_cmd(label1, cmd[i].operand1.length(), label2, cmd[i].operand2.length(), tran_op(cmd[i].op), mac_vector[i], mac_len);
                    tru->sign_cmd_accu(label1, cmd[i].operand1.length(), label2, cmd[i].operand2.length(), tran_op(cmd[i].op), arr_vec, mac_len);
                }
            }
           

            
            // truple_mac t1;
            // t1.trup = cmd[i];
            // memcpy(t1.mac, arr, MAC_LEN);
            // cmd_mac.push_back(t1);
        }
    }else{
        printf("No need to call this function, or you use MAC option when create instance.\n");
    }
        
    return mac_vector;
}
void PotocolRead::store(){
            ofstream file("store_file");
    for(int i = 0; i < cmd.size(); i++){

        file<<cmd[i].operand1<<"|"<<cmd[i].op<<"|"<<cmd[i].operand2<<"|->"<<cmd[i].output<<std::endl;;
    }
    file.close();
}
void PotocolRead::load_mac(std::vector<uint8_t*> &mac_dir){
    //cmd_mac = std::vector<uint8_t[MAC_LEN]>(mac_dir.size());
    cmd_mac = std::vector<truple_mac>(mac_dir.size());
    if(mac_dir.size() != cmd.size()){
        printf("protocol error. please make sure anyone keep same protocol files\n");
        return ;
    }
    for(int i = 0; i < cmd.size(); i++){
        cmd_mac[i].trup = cmd[i];
        
        memcpy(cmd_mac[i].mac, mac_dir[i], MAC_LEN);
    }
    for(int i = 0; i < mac_dir.size(); i++){
        free(mac_dir[i]); 
    }
    mac_dir.clear();
}
void PotocolRead::load_mac_last(std::vector<uint8_t*> &mac_dir){
    //cmd_mac = std::vector<uint8_t[MAC_LEN]>(mac_dir.size());
    cmd_mac = std::vector<truple_mac>(cmd.size());
    printf("load cmd mac size:%lu\n",mac_dir.size());
    if(mac_dir.size() != output.size()){
        printf("protocol error. please make sure anyone keep same protocol files\n");
        return ;
    }
    int j = 0; 
    for(int i = 0; i < cmd.size(); i++){
        cmd_mac[i].trup = cmd[i];
        if(cmd[i].op == "out"){
            memcpy(cmd_mac[i].mac, mac_dir[j], MAC_LEN);
            j++;
        }
        
    }
    for(int i = 0; i < mac_dir.size(); i++){
        free(mac_dir[i]); 
    }
    mac_dir.clear();
}
void PotocolRead::clear_iteam(){
	now_step = 0; 
}
truple PotocolRead::next(){
	if(now_step < cmd.size()){
		now_step ++;
	}
	return cmd[now_step-1];
}
truple_mac PotocolRead::next_mac(){
    if(now_step < cmd_mac.size()){
        now_step ++;
    }
    return cmd_mac[now_step-1];
}
int PotocolRead::size_of_protocol(){
    if(!is_ML)
	   return cmd.size();
    else{
        return structures.size();
    }
}
//for test
/*int main(){
	PotocolRead *p = new PotocolRead("./protocol_file/org.jimple");

	//cout<<one.operand1<<" "<<one.output<<endl;
}*/