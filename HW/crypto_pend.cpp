#include "crypto_pend.h"
#include <map>
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
			pooling_type:1
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
			layer:1
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
	//scran and transform the data;
	int itr = 0;
	int w_itr = 0;
	int layers = 0;
	std::map<int, Image> dic;
	dic[0] = data_image;
	int layer_id,target_layer;
	while(itr >= struct_len){
		layer_id = structure[itr++];
		target_layer = structure[itr++];
		switch(structure[itr++]){
			case cov:
				int size_of_kenerl = structure[itr++];
				std::vector<Tuple> tuples;
				int is_pending = structure[itr++];
				int stride = structure[itr++];
				int l = structure[itr++];
				int w = structure[itr++];
				int h = structure[itr++];
				while(size_of_kenerl --){
					Tuple kernel({l,w,h});
					kernel.serialize(w_internal + w_itr);
					w_itr += l*w*h;
					tuples.push_back(kernel);
				}
				dic[layer_id] = dic[target_layer].convolution(tuples, is_pending == 0? false:true, stride);
				break;
			case pooling:
				int type_pooling = structure[itr++];
				bool is_pending = (structure[itr++] == 0);
				int stride = structure[itr++];
				int l = structure[itr++];
				int w = structure[itr++];
				dic[layer_id] = dic[target_layer].pooling({l,w,1},is_pending, stride, type_pooling);
				break;
			case ReLU:
				double alpha;
				memcpy(alpha, structure + itr, sizeof(baseInt)); 
				itr += sizeof(baseInt);
				if(alpha == 0){
					dic[target_layer].ReLU();	
				}else{
					dic[target_layer].ReLU(false, alpha);
				}
				break;
			case BN:
				double mu;
				double sigma;
				double gammma;
				double beta;
				double epsilon;
				memcpy(mu, structure + w_itr, sizeof(baseInt));
				w_itr += sizeof(baseInt);
				memcpy(sigma, structure + w_itr, sizeof(baseInt));
				w_itr += sizeof(baseInt); 
				memcpy(gammma, structure + w_itr, sizeof(baseInt));
				w_itr += sizeof(baseInt);
				memcpy(beta, structure + w_itr, sizeof(baseInt));
				w_itr += sizeof(baseInt);
				memcpy(epsilon, structure + w_itr, sizeof(baseInt));
				w_itr += sizeof(baseInt);
				dic[target_layer].BN_for_test(mu, sigma, gamma, beta, 0.0001);
				break;
			case FC:
				int weight_width = structure[itr++];
				baseInt weight[weight_width*dic[target_layer].shape.size()];
				dic[target_layer].FC(output, weight, weight_width);
				break;
			case shortcut:
				int layer_num = structure[itr++];
				dic[target_layer].add_other(dic[layer_num]);
				break;
		}
	}
	return 0;

}