#include "crypto_pend.h"
int truthtee_pend::data_input(unsigned char data[], unsigned int data_len){
	transfer_data(data, data_len, data_internal, dtat_in_len, DECRYPTO, remote_key);
}
int truthtee_pend::block(unsigned char W[], unsigned int W_len, unsigned char structure[], unsigned int struct_len, unsigned char output[]){
	

}