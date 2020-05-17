#include "protocol.h"
#include "model.h"
using namespace std;
PotocolRead::PotocolRead(std::string path, bool &init_succ, bool do_mac, bool ML){
	uint32_t data_len;
	load_model_len(data_len, path);
	uint8_t* data = (uint8_t*)malloc(data_len);
	load_model<uint8_t>(data,data_len,path);
	protocol = code_serialize(data, data_len);
	free(data);
}
// bool PotocolRead::Reader(std::string file_path){
// 	ifstream file;
// 	file.open(file_path,ios::in);
// 	if(!file.is_open()){
//         printf("protocol file does not exist\n");
//         return false;
//     }
//     std::string strLine;
//     int step = 0;
//     while(getline(file,strLine)){
//     	//cout<<strLine<<endl;
//         if(strLine.empty())
//             continue;
//         strLine = strLine.substr(strLine.find_first_not_of(" "));
//         if(strLine.find("IN") != strLine.npos){
//         	string mid = strLine.substr(0,strLine.find(" "));
//         	if(strLine.find("[") != strLine.npos){
//         		string num = strLine.substr(strLine.find("[")+1, strLine.find("]") - strLine.find("[")-1);

//         		int numb = atoi(num.c_str());
//         		char ans[10];
//         		string tar;
//         		for(int i = 0; i < numb; i ++){
// 					sprintf(ans, "%d", i);
//             		tar = mid + "_" + ans;
//             		dic_var[tar] = 1;
//         		}
//         	}
//        			dic_var[mid] = 1;	
        	
        	
//         	continue;
//         }
//         if(strLine.find("OUT") != strLine.npos){
//         	string mid = strLine.substr(strLine.find(">")+2,strLine.find(";") - strLine.find(">") - 3);
//         	output.push_back(mid);
//         	if(pro_test){
//         		cout<<"out:"<<mid<<"|"<<endl;
//         	}
//         	cmd.push_back({false,"","",mid,"out"});
//         	step ++;
//         	continue;
//         }
//         // if goto
//         if(strLine.find("if") != strLine.npos){
//         	string mid = strLine.substr(strLine.find("if")+3,strLine.find("goto") - strLine.find("if") - 4);
        	
// 			string a = mid.substr(0, mid.find(" "));
// 			mid = mid.substr(mid.find(" ")+1);
// 			string op = mid.substr(0, mid.find(" "));
// 			string b = mid.substr(mid.find(" ")+1);
// 			string target = strLine.substr(strLine.find("label"), strLine.find(";") - strLine.find("label"));
// 			if(pro_test){
//         		cout<<"out:"<<target<<"|"<<a<<"|"<<op<<"|"<<b<<"|"<<endl;
//         	}
// 			cmd.push_back({true,a,b,target,op});
// 			step ++;
// 			continue;
//     	}
//     	//just goto
//     	if(strLine.find("goto") != strLine.npos){
//     		string mid = strLine.substr(strLine.find("label"),strLine.find(";")-strLine.find("label"));
// 			if(pro_test){
//         		cout<<"out:"<<mid<<"|goto"<<endl;
//         	}
//     		cmd.push_back({true,"","",mid,"goto"});
//         	step ++;
// 			continue;
//     	}
//     	// 
//     	if(strLine.find("label") != strLine.npos){
//     		string mid = strLine.substr(strLine.find("label"),strLine.find(":"));
//     		if(pro_test){
//         		cout<<"out:"<<mid<<"|"<<endl;
//         	}
//     		dic_goto[mid] = step;
//     		continue;
//     	}
//     	if(strLine.find(":=") != strLine.npos){
//     		continue;
//     	}
//     	if(strLine.find("=") != strLine.npos){
//     		if(strLine.find("MPC") != strLine.npos) continue;
//     		if(strLine.find("newarray") != strLine.npos) continue;
//     		string mid = strLine.substr(strLine.find_first_not_of("\t"),strLine.find(";") - strLine.find_first_not_of("\t"));
//     		string target = mid.substr(0,mid.find(" "));
//     		mid = mid.substr(mid.find("=")+2);
//     		string a;
//     		if(mid.find(" ") != mid.npos){
//     			a = mid.substr(0,mid.find(" "));
//     			mid = mid.substr(mid.find(" ") + 1);
//     			string op = mid.substr(0,mid.find(" "));
//     			string b = mid.substr(mid.find(" ")+1);
// 				if(pro_test){
//         			cout<<"out:"<<target<<"|"<<a<<"|"<<op<<"|"<<b<<"|"<<endl;
//         		}
//     			cmd.push_back({false,a,b,target,op});
//     			//if plain_var <- cipher_var, make plain varible become cipher varible
//     			if(dic_var[middle(b)] > dic_var[middle(target)]){
//     				dic_var[middle(target)] = dic_var[middle(b)];
//     			}
//     			if(dic_var[middle(a)] > dic_var[middle(target)]){
//     				dic_var[middle(target)] = dic_var[middle(a)];
//     			}
//     		}
//     		else{
//     			a = mid.substr(0,mid.find(";"));
// 				if(pro_test){
//         			cout<<"out:"<<target<<"|"<<a<<"|"<<endl;
//         		}
//     			cmd.push_back({false,a,"",target,""});
//     			if(dic_var[middle(a)] > dic_var[middle(target)]){
//     				dic_var[middle(target)] = dic_var[middle(a)];
//     			}
//     		}
//     		step ++;
//     		continue;

//     	}
    	
//     }
//     /*cnd -> Code*/  
//     protocol.code_size = cmd.size();
//     code.pos_s = (uint8_t*)malloc(code.code_size*POS_LEN);
//     uint8_t* Structure = (uint8_t*)malloc(INS_LEN*cmd.size());
//     for(int i = 0 ; i < cmd.size(); i++){
//     	if(char label3[LABEL_LEN] = "l3";.is_goto){

//     	}else{
//     		char label1[LABEL_LEN] = cmd[i].operand1;
//     		char label2[LABEL_LEN] = cmd[i].operand2;
//     		char label3[LABEL_LEN] = cmd[i].output;
//     		uint8_t = 
//     	}
//     }
//    	if(transfer()){
//         if(!expand()){
//             printf("Protocol error: Illegal input instruction\n");
//             return false;
//         }
//     }else{
//         printf("Protocol error: Illegal input instruction\n");
//         return false;
//     }
//     return true;
// }