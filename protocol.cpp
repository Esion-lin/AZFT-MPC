//protocol.cpp
#include "protocol.h"
using namespace std;
bool pro_test = false;
string middle(string str){
	if(str.find("[") != str.npos)
		return str.substr(0,str.find("["));
	return str;
}
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
    if(pro_test){
        for(int i = 0; i < cmd.size(); i++){
            cout<<i<<" "<<cmd[i].operand1<<" "<<cmd[i].op<<" "<<cmd[i].operand2<<" = "<<cmd[i].output<<endl;
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
	
}
PotocolRead::PotocolRead(std::string file_path){
	Reader(file_path);

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
int PotocolRead::size_of_protocol(){
	return cmd.size();
}
//for test
/*int main(){
	PotocolRead *p = new PotocolRead("./protocol_file/org.jimple");

	//cout<<one.operand1<<" "<<one.output<<endl;
}*/