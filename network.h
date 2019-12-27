// Auther: Tainpei Lu
// Creation: 11/02 2019 
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <string>
#include <json/json.h>
#include "HW/crypto.h"
#include "config.h"
#include <map>
class netTool{
public:
	netTool(truthtee *tru);
	~netTool();

    char 	sendbuf[buffer_size];
	

    void 	set_host_port(std::string host,int port);
	void 	*init_listen();
	void 	acc_and_recv();
	void 	init_conn();
	void	send_data(Json::Value js);
	void 	init();
	void 	deal_data(Json::Value value);
	//for test
	int 			recv_port;
	std::map<std::string, unsigned char[16]> data_dic;	
	bool 	is_data_store = false;
	bool	is_data_send = false;
	bool	is_key_store = false;
	bool	is_key_verify = false;
private:
	truthtee 		*tru;
	std::string		conn_host;
	int 			conn_port;
	
	// socket port;
	int 	listen_fd , conn_fd;
	bool 	is_listen;
	// define sockaddress 
	struct 		sockaddr_in 	server_sockaddr, client_addr;
	//define socket server address when it represent client
	struct 		sockaddr_in remote_server_sockaddr;
	void 	accept_key(Json::Value value);
	void 	accept_data(Json::Value key_paty, Json::Value data_part);
	void 	accept_sign(Json::Value value);
};
void *init_listen_static(void *tmp);
void to_byte16(uint64_t org, unsigned char output[]);
void to_byte16(uint64_t org1, uint64_t org2, unsigned char output[]);
void to_byte16(uint32_t org1, uint32_t org2, uint32_t org3, uint32_t org4, unsigned char output[]);
void to_ll(unsigned char input[], uint64_t &output);
void to_ll(unsigned char input[], uint64_t &output1, uint64_t &output2);
void to_int(unsigned char input[], uint32_t &output1, uint32_t &output2,uint32_t &output3,uint32_t &output4);