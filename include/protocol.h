#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_
#include "component.h"
#include "tee.h"
#include "some_struct.h"
#include <vector>
class PotocolRead{
	public:
		PotocolRead(std::string path, bool &init_succ, bool do_mac = true, bool ML = false);
		~PotocolRead();
		struct Code protocol;

};
#endif