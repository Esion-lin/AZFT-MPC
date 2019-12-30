// Auther: Tainpei Lu
// Creation: 11/02 2019 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "HW/crypto.h"
#include "some_struct.h"
#include "tool.h"
//#include "Merkle_Hash.h"
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
	This page is for dealing with protocol
	Input a file which written with the protocol code
	Scan the code line by line and convert each instruction to "truple" structure
	For convenience this function need tell whither the instruction is plain(no cipertext need)
	And this class also do simplified and analysis of instructions
*/




class PotocolRead{
	public:
		int now_step = 0;
		std::map<std::string, int> dic_goto;
		PotocolRead(std::string file_path, bool do_mac = true);
		~PotocolRead();
		void Reader(std::string file_path);
		std::vector<unsigned char[MAC_LEN]> tran_mac(truthtee *tru);
		void load_mac(std::vector<unsigned char[MAC_LEN]> &mac_dir);
		truple next();
		truple_mac next_mac();
		int size_of_protocol();
		void clear_iteam();
		//merkleTree *cmd_merkle_tree;
	private:
		//this function is desigened for conversion of data independence
		void transfer();
		std::string arr2item(std::string str); 
		std::vector<std::string> output;
		std::vector<truple> cmd;
		std::vector<truple_mac> cmd_mac;
		bool is_cmd_mac = true;
		/*
		dic_var record whether the variable is ciphertext or not
		0:plaintext
		1:ciphertext
		*/
		std::map<std::string, int> dic_var;
};
