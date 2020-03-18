#include "crypto_pend.h"
int truthtee_pend::data_input(unsigned char data[], unsigned int data_len, int l, int w, int h){
	unsigned char data_internal[data_len * 2];
	unsigned int data_in_len;
	transfer_data(data, data_len, data_internal, data_in_len, DECRYPTO, remote_key);
	baseInt data_in_type[data_in_len/sizeof(baseInt)];
	memcpy(data_in_type, data_internal, data_in_len);
	data_image.reshape({l,w,h}, data_in_type);

}
int truthtee_pend::block(unsigned char W[], unsigned int W_len, unsigned char structure[], unsigned int struct_len, unsigned char output[]){
	/*structure:
		layer:1
		target_layer:1
		type:1 s.t cov,pooling,ReLU,BN,FC
		cov{
			size_of_kenerl:1
			is_pending:1
			stride:1
			shape_of_kennerl:l,w,h = 3
		}
		pooling{
			is_pending:1
			stride:1
			shape_of_kennerl:l,w = 2
		}
		ReLU{
			alpha:sizeof(baseInt)
		}
		BN{
		}
		FC{
			weight_width:sizeof(int);
		}
		add{
			layer1:1
			layer2:1
		}
	*/
	/*W:
		cov{
			array:l*w*h*size_of_kenerl*sizeof(baseInt)
		}
		BN{
			mu:sizeof(baseInt)
			sigma:sizeof(baseInt)
			gammma:sizeof(baseInt)
			beta:sizeof(baseInt)
			epsilon:sizeof(baseInt)
		}
		FC{
			array:weight_width*last_layer_output_size
		}
		
	*/
	unsigned char w_internal[W_len * 2];
	unsigned int w_in_len;
	transfer_data(W, W_len, w_internal, w_in_len, DECRYPTO, org_key);
	while(struct_len>0){
		
	}
	return 0;

}