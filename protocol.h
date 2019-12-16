#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
//Binary
#define U_AND_OP 1
#define U_NOT_OP 2
#define U_OR_OP 3
#define U_NAND_OP 4
#define U_SHF_LE 5
#define U_SHF_RI 6


//Arithmatic
#define U_ADD_OP 11
#define U_SUB_OP 12
#define U_MUL_OP 13
#define U_DIV_OP 14

#define U_SMALL 50
#define U_SMALL_EQ 51
#define U_BIG 52
#define U_BIG_EQ 53
#define U_GOTO_if 101
#define U_GOTO 102

#define OUTPUT 100
/*
1.o = number
{
	false,
	"number",
	"",
	"o",
	""
}
2.o = n1 op n2
{
	false,
	"n1",
	"n2",
	"o",
	"op"
}
3 if n1 op n2 goto o
{
	true,
	"n1",
	"n2",
	"o",
	"op"
}
3. goto o
{
	true,
	"",
	"",
	"o",
	""
}
*/
struct truple{
	bool is_goto;
	std::string operand1;
	std::string operand2;
	std::string output;
	std::string op;
	bool operator==(const truple b) const  
    {  
        return this->is_goto == b.is_goto && this->op == b.op && this->output == b.output;  
    }  
};

//MerkleHash function

class PotocolRead{
	public:
		int now_step = 0;
		std::map<std::string, int> dic_goto;
		PotocolRead(std::string file_path);
		~PotocolRead();
		void Reader(std::string file_path);
		truple next();
		int size_of_protocol();
		void clear_iteam();

	private:
		//this function is desigened for Conversion of data independence
		void transfer();
		std::string arr2item(std::string str); 
		std::vector<std::string> output;
		std::vector<truple> cmd;
		/*
		0:plaintext
		1:cyphertext
		*/
		std::map<std::string, int> dic_var;
};
