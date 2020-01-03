#include <iostream>
#include <fstream>
using namespace std;
int main(){
	ifstream file;
	file.open("./sootOutput/P.jimple",ios::in);
	ofstream out("./protocol_file/orgin.jimple");
	if(!file.is_open()){
        printf("Please compile protocol first\n");
        return false;
    }
    std::string strLine;
    bool cutheard = false;
    bool start = false;
    while(getline(file,strLine)){
    	if(strLine.find("main(java.lang.String[])") != strLine.npos){
    		cutheard = true;
    		continue;
    	}
    	if(cutheard){
    		start = true;
    		cutheard = false;
    		continue;
    	}
    	if(start){
    		if(strLine.find("return") != strLine.npos){
    			start = false;
    			continue;
    		}
    		else{
    			out<<strLine<<endl;
    		}
    	}
    }
    file.close();
    out.close();
}