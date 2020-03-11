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
#include "crypto.h"
#include "config.h"
#include <map>
#include <iostream>
#include "tool.h"
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
	// some cache file
	std::map<std::string, unsigned char[CIPHER_LEN]> data_dic;	
	std::map<std::string, unsigned char[MAC_LEN]> data_mac_dic;	
	std::vector<unsigned char*> mac_dir;
	bool 	is_data_store = false;
	bool	is_data_send = false;
	bool	is_key_store = false;
	bool	is_key_verify = false;
	bool	is_mac_store = false;
	bool	is_mac_send = false;

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
	void 	accept_data(Json::Value key_paty, Json::Value data_part, Json::Value mac);
	void 	accept_sign(Json::Value value);
	void	accept_mac(Json::Value value);
};
void *init_listen_static(void *tmp);
