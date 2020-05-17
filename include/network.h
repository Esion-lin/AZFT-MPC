// Auther: Tainpei Lu
// Creation: 11/02 2019 
#ifndef _NETWORK_H_
#define _NETWORK_H_
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
#include "tee.h"
#include "config.h"
#include <map>
#include <iostream>
#include "tool.h"
class netTool{
public:
	netTool(TEE *tru);
	~netTool();

    char 	sendbuf[buffer_size];
	

    void 	set_host_port(std::string host,int port);
	void 	*init_listen();
	void 	acc_and_recv();
	void 	init_conn();
	void	send_data(Json::Value js);
	void	send_data(uint8_t * data, int len_data);
	void 	init();
	void 	deal_data(Json::Value value);
	//for test
	int 			recv_port;
	// some cache file
	
	bool 	is_data_store = false;
	bool	is_data_send = false;
	bool	is_key_store = false;
	bool	is_key_verify = false;
	bool	is_mac_store = false;
	bool	is_mac_send = false;

private:
	TEE 			*tru;
	std::string		conn_host;
	int 			conn_port;
	bool 			rec_serial_data = true;
	bool			rec_image_data = false;
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
#endif