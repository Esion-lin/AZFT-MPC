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
void PotocolRead::Reader(std::string file_path){
	ifstream file;
	file.open(file_path,ios::in);
	if(!file.is_open())
        return ;
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

   	transfer();
}

void PotocolRead::transfer(){
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
    	}else if(index(cmd[i].operand2)!=cmd[i].operand2 && dic_var[index(cmd[i].operand2)] == 1){
    		cout<<"index of arr is cipher text:"<<cmd[i].operand2;
    	}else if(index(cmd[i].output)!=cmd[i].output && dic_var[index(cmd[i].output)] == 1){
    		cout<<"index of arr is cipher text:"<<cmd[i].output;
    	}
    }
    for(auto &v : dic_var){
        cout<<v.first<<" "<<v.second<<endl;
    }
    
	
}

PotocolRead::PotocolRead(std::string file_path, bool do_mac){
	Reader(file_path);
    is_cmd_mac = do_mac;

}
std::vector<unsigned char[MAC_LEN]> PotocolRead::tran_mac(truthtee *tru){
    std::vector<unsigned char[MAC_LEN]> mac_vector(cmd.size());
    if(is_cmd_mac){
        for(int i = 0; i < cmd.size(); i++){
            unsigned char arr[MAC_LEN];
            unsigned int mac_len;
            unsigned char label1[LABEL_LEN];
            unsigned char label2[LABEL_LEN];
            memcpy(label2, cmd[i].operand2.c_str(), cmd[i].operand2.length());
            if(cmd[i].op == "out"){
                memcpy(label1, cmd[i].output.c_str(), cmd[i].output.length());
                tru->sign_cmd_without_counter(label1, cmd[i].output.length(), label2, cmd[i].operand2.length(), tran_op(cmd[i].op), mac_vector[i], mac_len);
            }else{
                memcpy(label1, cmd[i].operand1.c_str(), cmd[i].operand1.length());
                tru->sign_cmd_without_counter(label1, cmd[i].operand1.length(), label2, cmd[i].operand2.length(), tran_op(cmd[i].op), mac_vector[i], mac_len);
            }
            
            
            
            // truple_mac t1;
            // t1.trup = cmd[i];
            // memcpy(t1.mac, arr, MAC_LEN);
            // cmd_mac.push_back(t1);
        }
    }else{
        printf("Do not need make MAC. please use mac option when create instance.\n");
    }
        
    return mac_vector;
}
void PotocolRead::load_mac(std::vector<unsigned char[MAC_LEN]> &mac_dir){
    //cmd_mac = std::vector<unsigned char[MAC_LEN]>(mac_dir.size());
    cmd_mac = std::vector<truple_mac>(mac_dir.size());
    if(mac_dir.size() != cmd.size()){
        printf("protocol error. please make sure anyone keep same protocol files\n");
        return ;
    }
    for(int i = 0; i < cmd.size(); i++){
        cmd_mac[i].trup = cmd[i];
        
        memcpy(cmd_mac[i].mac, mac_dir[i], MAC_LEN);
    }
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
	return cmd.size();
}
//for test
/*int main(){
	PotocolRead *p = new PotocolRead("./protocol_file/org.jimple");

	//cout<<one.operand1<<" "<<one.output<<endl;
}*/