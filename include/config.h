// Auther: Tainpei Lu
// Creation: 11/02 2019 
#ifndef _CONFIGFILE_H_
#define _CONFIGFILE_H_
#define send_port 60002
//#define recv_port 60001
#define connect_pool 20 
#define remote_ip "127.0.0.1"
#define buffer_size 2048
#define pk_ex 0
#define sign_ex 1
#define encrpyt_msg 2

#define img_size 224*224*3*sizeof(float)

#define key_ex_action 1
#define rec_img_data 10
#define sign_action 2
#define data_action 3
#define cmd_mac_action 22
#define data_sign_action 4
#define res_data_action 15
#define is_debug true
#define is_send_output_mac true


#endif