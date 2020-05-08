#ifndef _ELEMENT_H_
#define _ELEMENT_H_
/*
S & W: 
Binary\Arithmatic\Logic s:{type:1} w:{w1:1, w2:1, w3:1}
cov s:{
	type:1
	target_layer:1
	layer:1
	size_of_kenerl:2
	is_pending:1
	stride:1
	shape_of_kennerl:l,w,h = 1+1+2 = 4
}	
w:{
	array:l*w*h*size_of_kenerl*sizeof(baseInt)
}

pooling s:{
	type:1
	target_layer:1
	layer:1
	pooling_type:1
	is_pending:1
	stride:1
	shape_of_kennerl:l,w = 2
}
ReLU s:{
	type:1
	target_layer:1
	layer:1
	alpha:sizeof(baseInt)
}

BN s:{
	type:1
	target_layer:1
	layer:1
}
w: {
	mu:sizeof(baseInt)
	sigma:sizeof(baseInt)
	gammma:sizeof(baseInt)
	beta:sizeof(baseInt)
	epsilon:sizeof(baseInt)
}
FC s:{
	type:1
	target_layer:1
	layer:1
	weight_width:sizeof(int);
}
w: {
	array:weight_width*last_layer_output_size
}
SHORTCUT s:{
	type:1
	target_layer:1
	layer:1
	target_layer2:1
}
*/
//Binary
#define AND_OP 1
#define NOT_OP 2
#define OR_OP 3
#define NAND_OP 4
#define SHF_LE 5
#define SHF_RI 6


//Arithmatic
#define ADD_OP 11
#define SUB_OP 12
#define MUL_OP 13
#define DIV_OP 14
#define POW_OP 15
//Logic
#define GREAT_OP 21
#define GE_OP 22
#define LESS_OP 23
#define LE_OP 24
#define EQ_OP 25

//defined op
#define COV 0 
#define POOLING 1 
#define RELU 2 
#define BN_ID 3 
#define FC_ID 4
#define SHORTCUT 5

//length of S
#define SIZE_COV 11 
#define SIZE_POOLING 8 
#define SIZE_RELU (3 + sizeof(baseInt))
#define SIZE_BN 3
#define SIZE_FC (3 + sizeof(int))
#define SIZE_SHORTCUT 4

/*

data{
	label: 2
	type: 1 
	array_size: sizeof(int)
}
content{
	content:array_size*sizeof(int)
}
D{
	data_size:2
	data_1, data_2, ... data_n:(3 + sizeof(int)) * data_size
	content_1, content_2 .... content_n:sum_of(array_size_i)*sizeof(int)
}
*/
#define INT 	1
#define FLOAT 	2
#define INT_ARRAY	3
#define FLOAT_ARRAY 4
#define DATA_SIZE 	sizeof(int)
#define ARRAY_SIZE 	sizeof(int)

#endif