#include <string>
#ifndef _SOME_H_
#define _SOME_H_
typedef struct truple truple;
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
#endif