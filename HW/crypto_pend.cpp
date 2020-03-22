#include "crypto_pend.h"
#include <map>
int truthtee_pend::data_input(unsigned char data[], unsigned int data_len, int l, int w, int h){
	unsigned char data_internal[data_len];
	unsigned int data_in_len;
	/*for(int i = 224*224*3*sizeof(float)-100; i < 224*224*3*sizeof(float); i++){
        printf("%u ", data[i]);
    }*/
    
	transfer_data(data, data_len, data_internal, data_in_len, DECRYPTO, remote_key);

    printf("\n");
	baseInt data_in_type[data_in_len/sizeof(baseInt)];

	memcpy(data_in_type, data_internal, data_in_len);
	data_image.reshape({l,w,h}, data_in_type);

}

int truthtee_pend::test_with_remote(unsigned char input_data[], unsigned int input_len, unsigned char output_data[], unsigned int &output_len){
	transfer_data(input_data, input_len, output_data, output_len, ENCRYPTO, org_key);
}
int truthtee_pend::block(unsigned char W[], unsigned int W_len, unsigned char structure[], unsigned int struct_len, baseInt output[]){
	/*structure:
		target_layer:1
		layer:1
		
		type:1 s.t cov,pooling,ReLU,BN,FC
		cov{
			size_of_kenerl:2
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
	baseInt w_data[w_in_len / sizeof(baseInt)];
	memcpy(w_data, w_internal, w_in_len);
	//scran and transform the data;
	int itr = 0;
	int w_itr = 0;
	int layers = 0;
	std::vector<Image> dic;
	dic.push_back(data_image);
	int layer_id,target_layer;
	printf("block size [%d], start block....\n",struct_len);
	while(itr < struct_len){
		target_layer = structure[itr++];
		layer_id = structure[itr++];
		
		printf("layer : %d layer_id :%d\n", layer_id,target_layer);

		switch(structure[itr++]){
			case COV:
			{
				int size_of_kenerl = structure[itr++];
				size_of_kenerl = size_of_kenerl*256 + structure[itr++];
				std::vector<Tuple> tuples;
				int is_pending = structure[itr++];
				int stride = structure[itr++];
				int l = structure[itr++];
				int w = structure[itr++];
				int h = structure[itr++];
				h = h*256 + structure[itr++];
				while(size_of_kenerl --){
					Tuple kernel({l,w,h});
					kernel.unserialize(w_data + w_itr);
					w_itr += l*w*h;
					tuples.push_back(kernel);
				}
				dic.push_back(dic[target_layer].convolution(tuples, is_pending == 0? false:true, stride));

			}
				break;
			case POOLING:
			{
				int type_pooling = structure[itr++];
				int is_pending = structure[itr++];
				int stride = structure[itr++];
				int l = structure[itr++];
				int w = structure[itr++];
				dic.push_back(dic[target_layer].pooling({l,w,1},is_pending == 0? false:true, stride, type_pooling));
				printf("now print the pooling ans size [%d,%d,%d]\n", dic[layer_id].shape.l,dic[layer_id].shape.w, dic[layer_id].shape.h);
			}
				break;
			case RELU:
			{

				baseInt alpha;
				memcpy(&alpha, structure + itr, sizeof(baseInt)); 
				itr += sizeof(baseInt);
				if(alpha == 0){
					dic[target_layer].ReLU();	
				}else{
					dic[target_layer].ReLU(false, alpha);
				}
			}
				break;
			case BN_ID:
			{
				baseInt mu = w_data[w_itr++];
				baseInt sigma = w_data[w_itr++];
				baseInt gamma = w_data[w_itr++];
				baseInt beta = w_data[w_itr++];
				baseInt epsilon = w_data[w_itr++];
				dic[target_layer].BN_for_test(mu, sigma, gamma, beta, epsilon);
			}
				break;
			case FC_ID:
			{	int weight_width = structure[itr++];
				baseInt weight[weight_width*dic[target_layer].shape.size()];
				memcpy(weight, w_data + w_itr, weight_width*dic[target_layer].shape.size()*sizeof(baseInt));
				dic[target_layer].FC(output, weight, weight_width);
			}
				break;
			case SHORTCUT:
			{
				int layer_num = structure[itr++];
				dic[target_layer].add_other(dic[layer_num]);
			}
				break;
		}
	}
	data_image = dic[dic.size() - 1];
	return 0;

}
int truthtee_pend::block(baseInt* w_data, unsigned int W_len, unsigned char structure[], unsigned int struct_len, baseInt output[]){
	/*structure:
		target_layer:1
		layer:1
		
		type:1 s.t cov,pooling,ReLU,BN,FC
		cov{
			size_of_kenerl:2
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

	//scran and transform the data;
	int itr = 0;
	int w_itr = 0;
	int layers = 0;
	std::vector<Image> dic;
	dic.push_back(data_image);
	int layer_id,target_layer;
	printf("block size [%d], start block....\n",struct_len);
	while(itr < struct_len){
		target_layer = structure[itr++];
		layer_id = structure[itr++];
		
		printf("layer : %d layer_id :%d\n", layer_id,target_layer);

		switch(structure[itr++]){
			case COV:
			{
				int size_of_kenerl = structure[itr++];
				size_of_kenerl = size_of_kenerl*256 + structure[itr++];
				std::vector<Tuple> tuples;
				int is_pending = structure[itr++];
				int stride = structure[itr++];
				int l = structure[itr++];
				int w = structure[itr++];
				int h = structure[itr++];
				h = h*256 + structure[itr++];
				while(size_of_kenerl --){
					Tuple kernel({l,w,h});
					kernel.unserialize(w_data + w_itr);
					w_itr += l*w*h;
					tuples.push_back(kernel);
				}
				dic.push_back(dic[target_layer].convolution(tuples, is_pending == 0? false:true, stride));

			}
				break;
			case POOLING:
			{
				int type_pooling = structure[itr++];
				int is_pending = structure[itr++];
				int stride = structure[itr++];
				int l = structure[itr++];
				int w = structure[itr++];
				dic.push_back(dic[target_layer].pooling({l,w,1},is_pending == 0? false:true, stride, type_pooling));
			}
				break;
			case RELU:
			{

				baseInt alpha;
				memcpy(&alpha, structure + itr, sizeof(baseInt)); 
				itr += sizeof(baseInt);
				if(alpha == 0){
					dic[target_layer].ReLU();	
				}else{
					dic[target_layer].ReLU(false, alpha);
				}
			}
				break;
			case BN_ID:
			{
				baseInt mu = w_data[w_itr++];
				baseInt sigma = w_data[w_itr++];
				baseInt gamma = w_data[w_itr++];
				baseInt beta = w_data[w_itr++];
				baseInt epsilon = w_data[w_itr++];
				dic[target_layer].BN_for_test(mu, sigma, gamma, beta, epsilon);
			}
				break;
			case FC_ID:
			{	int weight_width;
				memcpy(&weight_width, structure + itr, sizeof(int));
				itr+=sizeof(int);
				/*baseInt weight[weight_width*dic[target_layer].shape.size()];
				memcpy(weight, w_data + w_itr, weight_width*dic[target_layer].shape.size()*sizeof(baseInt));*/
				dic[target_layer].FC(output, w_data + w_itr, weight_width);
			}
				break;
			case SHORTCUT:
			{
				int layer_num = structure[itr++];
				dic[target_layer].add_other(dic[layer_num]);
			}
				break;
		}
	}
	data_image = dic[dic.size() - 1];
	return 0;

}