#include <openssl/sha.h>
#include <vector>
#include <queue>
#include <map>
#include "protocol.h"
using namespace std;
class merkleTree{
public:
	merkleTree(vector<truple> cmds);
	merkleTree(map<std::string, unsigned char[16]> datas);
	void output_hash(unsigned char output[]);
	vector<string> quary_path(truple trp);
	vector<string> quary_path(string label, unsigned char cipher[]);
private:
	map<string, string> brother_map;
	map<string, string> father_map;
	queue<string> init_hash(vector<truple> cmds);
	queue<string> init_hash(map<std::string, unsigned char[16]>);
	void make_brother(queue<string> leaf);
	unsigned char out_hash[SHA256_DIGEST_LENGTH];
	string out_str;
};