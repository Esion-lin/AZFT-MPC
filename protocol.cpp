//protocol.cpp
#include "protocol.h"
using namespace std;

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
        	string mid = strLine.substr(strLine.find("if")+3,strLine.find("goto") - strLine.find("if") - 3);
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
    		}
    		else{
    			a = mid.substr(0,mid.find(";"));
    			cmd.push_back({false,a,"",target,""});
    			
    		}
    		step ++;
    		continue;

    	}
    	
    }
   
}
PotocolRead::PotocolRead(std::string file_path){
	Reader(file_path);

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
// int main(){
// 	PotocolRead *p = new PotocolRead("./protocol_file/T.jimple");
// 	truple one = p->next();
// 	cout<<one.operand1<<" "<<one.output<<endl;
// }