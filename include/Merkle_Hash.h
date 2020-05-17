// Auther: Tainpei Lu
// Creation: 11/02 2019 
#include <openssl/sha.h>
#include <vector>
#include <queue>
#include <map>
#include "some_struct.h"
#include "crypto.h"
using namespace std;
class merkleTree{
public:
	merkleTree(vector<truple> cmds);
	merkleTree(map<std::string, uint8_t[16]> datas);
	void output_hash(uint8_t output[]);
	string output_hash();
	vector<string> quary_path(truple trp);
	vector<string> quary_path(string label, uint8_t cipher[]);
private:
	map<string, string> brother_map;
	map<string, string> father_map;
	queue<string> init_hash(vector<truple> cmds);
	queue<string> init_hash(map<std::string, uint8_t[16]>);
	void make_brother(queue<string> leaf);
	void sha256(const string str, uint8_t output[]);
	string sha256(const string str);
	void sha256(uint8_t input[], int len, uint8_t output[]);
	uint8_t out_hash[SHA256_DIGEST_LENGTH];
	string out_str;
};