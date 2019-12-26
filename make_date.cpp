// Auther: Tainpei Lu
// Creation: 11/02 2019 
// #include <iostream>
// #include <fstream>
// #include <time.h>
// using namespace std;
// int main(){
// 	ofstream out("./A.date");
// 	ofstream out2("./B.date");
// 	if(!out.is_open()) return 0;
// 	srand((unsigned)time(NULL));
// 	for(int i = 0; i < 200 ; i ++){
// 		out<<"r1_"<<i<<" "<<rand()%1024<<endl;
// 		out2<<"r2_"<<i<<" "<<rand()%1024<<endl;
// 	}
// 	out.close();
// 	out2.close();
// 	return 0;
// }