//protocol.cpp
#include "protocol.h"
using namespace std;
//this reader function is data dependence, so if the jimple file style is changed, please rewrite this function
void PotocolRead::Reader(std::string file_path){
	ifstream file;
	file.open(file_path,ios::in);
	if(!file.is_open())
        return ;
    std::string strLine;
    int step = 0;
    while(getline(file,strLine)){
        if(strLine.empty())
            continue;
        if(strLine.find("IN") != strLine.npos){
        	string mid = strLine.substr(0,strLine.find(" "));
        	dic_var[mid] = 1;
        	continue;
        }
        if(strLine.find("OUT") != strLine.npos){
        	string mid = strLine.substr(strLine.find(">")+2,strLine.find(";") - strLine.find(">") - 3);
        	output.push_back(mid);

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

			cmd.push_back({true,a,b,target,op});
			step ++;
			continue;
    	}
    	//just goto
    	if(strLine.find("goto") != strLine.npos){
    		string mid = strLine.substr(strLine.find("label"),strLine.find(";")-strLine.find("label"));

    		cmd.push_back({true,"","",mid,"goto"});
        	step ++;
			continue;
    	}
    	// 
    	if(strLine.find("label") != strLine.npos){
    		string mid = strLine.substr(strLine.find("label"),strLine.find(":"));
    		dic_goto[mid] = step;
    		continue;
    	}
    	if(strLine.find("=") != strLine.npos){
    		string mid = strLine.substr(strLine.find_first_not_of("\t"),strLine.find(";") - strLine.find_first_not_of("\t"));
    		
    		string target = mid.substr(0,mid.find(" "));
    		mid = mid.substr(mid.find("=")+2);
    		string a;
    		if(mid.find(" ") != mid.npos){
    			a = mid.substr(0,mid.find(" "));
    			mid = mid.substr(mid.find(" ") + 1);
    			string op = mid.substr(0,mid.find(" "));
    			string b = mid.substr(mid.find(" ")+1);

    			cmd.push_back({false,a,b,target,op});
    			//if plain_var <- cipher_var, make plain varible become cipher varible
    			if(dic_var[b] > dic_var[target]){
    				dic_var[target] = dic_var[b];
    			}
    			if(dic_var[a] > dic_var[target]){
    				dic_var[target] = dic_var[a];
    			}
    		}
    		else{
    			a = mid.substr(0,mid.find(";"));

    			cmd.push_back({false,a,"",target,""});
    			if(dic_var[a] > dic_var[target]){
    				dic_var[target] = dic_var[a];
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
				if(dic_var[cmd[i].operand1] > dic_var[cmd[i].output]){
					dic_var[cmd[i].output] = dic_var[cmd[i].operand1];
					ch ++;
				}
				if(dic_var[cmd[i].operand2] > dic_var[cmd[i].output]){
					dic_var[cmd[i].output] = dic_var[cmd[i].operand2];
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
			if(cmd[i].op != "goto" && dic_var[cmd[i].operand1] + dic_var[cmd[i].operand2] >= 1){
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
	/*for(int i = 0; i < cmd.size(); i++){
		cout<<i<<" "<<cmd[i].operand1<<" "<<cmd[i].op<<" "<<cmd[i].operand2<<" = "<<cmd[i].output<<endl;
	}
	for(auto &v : dic_goto){
		cout<<v.first<<" "<<v.second<<endl;
	}*/
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