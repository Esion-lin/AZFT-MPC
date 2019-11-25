#include <iostream>
class PotocolRead{
	public:
		PotocolRead(std::string file_path);
		~PotocolRead();
		void Reader(std::string file_path);
		void next(std::string &operand1, std::string &operand2, int &op);

};