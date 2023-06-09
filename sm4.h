#ifndef SM4_H
#define SM4_H
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include"sm3.h"
#define EncryptionFlag 1
#define KeyExpansionFlag 0
#define Encryption 1
#define Decryption 0
const uint8_t Sbox[256] = 
{
	0xd6,0x90,0xe9,0xfe,0xcc,0xe1,0x3d,0xb7,0x16,0xb6,0x14,0xc2,0x28,0xfb,0x2c,0x05,
	0x2b,0x67,0x9a,0x76,0x2a,0xbe,0x04,0xc3,0xaa,0x44,0x13,0x26,0x49,0x86,0x06,0x99,
	0x9c,0x42,0x50,0xf4,0x91,0xef,0x98,0x7a,0x33,0x54,0x0b,0x43,0xed,0xcf,0xac,0x62,
	0xe4,0xb3,0x1c,0xa9,0xc9,0x08,0xe8,0x95,0x80,0xdf,0x94,0xfa,0x75,0x8f,0x3f,0xa6,
	0x47,0x07,0xa7,0xfc,0xf3,0x73,0x17,0xba,0x83,0x59,0x3c,0x19,0xe6,0x85,0x4f,0xa8,
	0x68,0x6b,0x81,0xb2,0x71,0x64,0xda,0x8b,0xf8,0xeb,0x0f,0x4b,0x70,0x56,0x9d,0x35,
	0x1e,0x24,0x0e,0x5e,0x63,0x58,0xd1,0xa2,0x25,0x22,0x7c,0x3b,0x01,0x21,0x78,0x87,
	0xd4,0x00,0x46,0x57,0x9f,0xd3,0x27,0x52,0x4c,0x36,0x02,0xe7,0xa0,0xc4,0xc8,0x9e,
	0xea,0xbf,0x8a,0xd2,0x40,0xc7,0x38,0xb5,0xa3,0xf7,0xf2,0xce,0xf9,0x61,0x15,0xa1,
	0xe0,0xae,0x5d,0xa4,0x9b,0x34,0x1a,0x55,0xad,0x93,0x32,0x30,0xf5,0x8c,0xb1,0xe3,
	0x1d,0xf6,0xe2,0x2e,0x82,0x66,0xca,0x60,0xc0,0x29,0x23,0xab,0x0d,0x53,0x4e,0x6f,
	0xd5,0xdb,0x37,0x45,0xde,0xfd,0x8e,0x2f,0x03,0xff,0x6a,0x72,0x6d,0x6c,0x5b,0x51,
	0x8d,0x1b,0xaf,0x92,0xbb,0xdd,0xbc,0x7f,0x11,0xd9,0x5c,0x41,0x1f,0x10,0x5a,0xd8,
	0x0a,0xc1,0x31,0x88,0xa5,0xcd,0x7b,0xbd,0x2d,0x74,0xd0,0x12,0xb8,0xe5,0xb4,0xb0,
	0x89,0x69,0x97,0x4a,0x0c,0x96,0x77,0x7e,0x65,0xb9,0xf1,0x09,0xc5,0x6e,0xc6,0x84,
	0x18,0xf0,0x7d,0xec,0x3a,0xdc,0x4d,0x20,0x79,0xee,0x5f,0x3e,0xd7,0xcb,0x39,0x48
};
const uint32_t FK[4] = {
	0xa3b1bac6, 0x56aa3350, 0x677d9197, 0xb27022dc
};
const uint32_t CK[32] = {
	0x00070e15, 0x1c232a31, 0x383f464d, 0x545b6269,
	0x70777e85, 0x8c939aa1, 0xa8afb6bd, 0xc4cbd2d9,
	0xe0e7eef5, 0xfc030a11, 0x181f262d, 0x343b4249,
	0x50575e65, 0x6c737a81, 0x888f969d, 0xa4abb2b9,
	0xc0c7ced5, 0xdce3eaf1, 0xf8ff060d, 0x141b2229,
	0x30373e45, 0x4c535a61, 0x686f767d, 0x848b9299,
	0xa0a7aeb5, 0xbcc3cad1, 0xd8dfe6ed, 0xf4fb0209,
	0x10171e25, 0x2c333a41, 0x484f565d, 0x646b7279
};

uint32_t SboxFunction(uint32_t input)
{
	uint32_t First, Second, Third, Fourth;
	uint32_t Result = 0;
	First = input >> 24;
	Second = input >> 16 & 0x000000ff;
	Third = input >> 8 & 0x000000ff;
	Fourth = input & 0x000000ff;
	First = Sbox[First];
	Second = Sbox[Second];
	Third = Sbox[Third];
	Fourth = Sbox[Fourth];
	Result = Result | First<<24;
	Result = Result | Second << 16;
	Result = Result | Third << 8;
	Result = Result | Fourth;
	return Result;
}
uint32_t L1_Function(uint32_t input)
{
	return input ^ round_shift_left(input, 2) ^ round_shift_left(input, 10) ^ round_shift_left(input, 18) ^ round_shift_left(input, 24);
}
uint32_t L2_Function(uint32_t input)
{
	return input ^ round_shift_left(input, 13) ^ round_shift_left(input, 23);
}
uint32_t T_Function(uint32_t input, int flag)
{
	uint32_t SboxResult = SboxFunction(input);
	return flag == EncryptionFlag ? L1_Function(SboxResult) : L2_Function(SboxResult);//1 for encryption step, 0 for key expansion step
}
void KeyExpansion1(uint32_t *MK, uint32_t *K)
{
	for (int i = 0; i < 4;i++) 
		K[i] = MK[i] ^ FK[i];
}
void KeyExpansion2(uint32_t *RK, uint32_t *K) {
	for (int i = 0; i < 32; i++) 
	{
		K[(i + 4) % 4] = K[i % 4] ^ T_Function(K[(i + 1) % 4] ^ K[(i + 2) % 4] ^ K[(i + 3) % 4] ^ CK[i], 2);
		RK[i] = K[(i + 4) % 4];
	}
}

void KeyExpansion(uint32_t* MK, uint32_t* K, uint32_t* RK)
{
	KeyExpansion1(MK, K);
	KeyExpansion2(RK, K);
}
void Iteration_of_SM4(uint32_t* X, uint32_t* RK)
{
	for (int i = 0; i < 32; i++)
		X[(i + 4) % 4] = X[i % 4] ^ T_Function(X[(i + 1) % 4] ^ X[(i + 2) % 4] ^ X[(i + 3) % 4] ^ RK[i], 1);
}
void FinalReverse(uint32_t *X, uint32_t *output)
{
	for (int i = 0; i < 4; i++)
		output[i] = X[4 - 1 - i];
}
void SM4_round_key(uint32_t *X, uint32_t *RK, uint32_t *output)
{
	Iteration_of_SM4(X, RK);
	FinalReverse(X, output);
}
void SM4(uint32_t* Plaintext, uint32_t* MasterKey, uint32_t* Ciphertext)
{
	uint32_t K[4] = { 0 };
	uint32_t RK[32] = { 0 };
	KeyExpansion(MasterKey, K, RK);
	/*for (int i = 0; i < 32; i++)
	{
		printf("%X ", RK[i]);
		if ((i + 1) % 4 == 0)
			printf("\n");
	}*/
	SM4_round_key(Plaintext, RK, Ciphertext);
}
void uint8_to_uint32(uint8_t* input, unsigned int input_length, uint32_t* output)//input_length is the byte length of the input
{
	unsigned int uint32_num_full = (input_length / 4);
	unsigned int uint32_num_lack = (input_length % 4);
	unsigned int total = uint32_num_full + (uint32_num_lack == 0 ? 0 : 1);
	uint32_t temp = 0;
	int a = 24;
	for (int i = 0; i < uint32_num_full; i = i + 1)
	{
		temp = temp | (input[i * 4] << 24);
		temp = temp | (input[i * 4 + 1] << 16);
		temp = temp | (input[i * 4 + 2] << 8);
		temp = temp | input[i * 4 + 3];
		output[i] = temp;
		temp = 0;
	}
	for (int i = 0; i < uint32_num_lack; i++)
	{
		temp = temp | (input[i + 4 * uint32_num_full] << a);
		a = a - 8;
		if (i == uint32_num_lack - 1)
			output[total - 1] = temp;
	}
	
}
void uint32_to_uint8_sm4(uint32_t* input, unsigned int input_length, uint8_t* output)//input_length is the byte length of the input
{
	int full_length = input_length / 4;
	int lack_length = input_length % 4;
	int a = 24;
	for (int i = 0; i < full_length; i++)
	{
		uint32_t temp = input[i];
		output[i * 4] = (temp & 0xff000000) >> 24;
		output[i * 4 + 1] = (temp & 0x00ff0000) >> 16;
		output[i * 4 + 2] = (temp & 0x0000ff00) >> 8;
		output[i * 4 + 3] = temp & 0x000000ff;
	}
	for (int i = 0; i < lack_length; i++)
	{
		uint32_t temp = input[full_length];
		output[4 * full_length + i] = (temp >> a) & 0x000000ff;
		a = a - 8;
	}
}
void SM4_OFB(uint8_t* Plaintext, unsigned int PlaintextLength, uint32_t* MasterKey, uint8_t* Ciphertext, int mode)//input the plaintext byte length
{
	int length32 = PlaintextLength / 4 + ((PlaintextLength % 4 == 0) ? 0 : 1);
	uint32_t* SM4_input = (uint32_t*)malloc(length32 * sizeof(uint32_t));
	uint32_t* SM4_output = (uint32_t*)malloc(length32 * sizeof(uint32_t));
	int ptr = 0;
	int counter = 0;
	int BlockLength = length32 / 4 + ((length32 % 4 == 0) ? 0 : 1);
	int FullLength = length32 / 4;
	int KeyChainLength = 4 * BlockLength;
	uint32_t* KeyChain = (uint32_t*)malloc(KeyChainLength * sizeof(uint32_t));
	uint32_t IV[4] = { 0x681edf34 ,0xd206965e ,0x86b3e94f ,0x536e4246 };
	uint32_t TempKey[4] = { 0 };
	if (mode == Encryption)
		uint8_to_uint32(Plaintext, PlaintextLength, SM4_input);
	for (int i = 0; i < BlockLength; i++)
	{
		SM4(IV, MasterKey, TempKey);
		for (int j = 0; j < 4; j++)
			IV[j] = TempKey[j];
		for (int j = 0; j < 4; j++)
			KeyChain[4 * i + j] = TempKey[j];
	}
	if (mode == Encryption)
	{
		uint8_to_uint32(Plaintext, PlaintextLength, SM4_input);
		for (int i = 0; i < length32; i++)
			SM4_output[i] = SM4_input[i] ^ KeyChain[i];
		uint32_to_uint8_sm4(SM4_output, PlaintextLength, Ciphertext);
	}
	else if (mode == Decryption)
	{
		uint8_to_uint32(Ciphertext, PlaintextLength, SM4_input);
		for (int i = 0; i < length32; i++)
			SM4_output[i] = SM4_input[i] ^ KeyChain[i];

		uint32_to_uint8_sm4(SM4_output, PlaintextLength, Plaintext);
	}
}
#endif // SM4_H

