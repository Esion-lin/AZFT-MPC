#include <string>
#ifndef _SOME_H_
#define _SOME_H_
typedef struct truple truple;
/*
truple is a structure for command
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
#endif