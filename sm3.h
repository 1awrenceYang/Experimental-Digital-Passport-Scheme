#ifndef SM3_H
#define SM3_H
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
uint32_t IV[8] = { 0x7380166f,0x4914b2b9,0x172442d7,0xda8a0600,0xa96f30bc,0x163138aa,0xe38dee4d,0xb0fb0e4e };//³£Á¿IV
uint32_t Tj015 = 0x79cc4519;//Value of Tj 0-15
uint32_t Tj1663 = 0x7a879d8a;//Value of Tj 16-63
#define FlagForW 1 //Used to select whether the output is W or W¡¯
#define FlagForW1 0
#define FFj015(x,y,z) x^y^z//FF function when j is in the interval 0-15
#define FFj1663(x,y,z) (x&y)|(x&z)|(y&z)//FF function when j is in the interval 16-63
#define GGj015(x,y,z) x^y^z//GG function when j is in the interval 0-15
#define GGj1663(x,y,z) (x&y)|(~x&z)//GG function when j is in the interval 16-63
#define P0(x) x^((x<<9)|(x>>23))^((x<<17)|(x>>15))//P0 function
#define P1(x) x^((x<<15)|(x>>17))^((x<<23)|(x>>9))//P1 function
#define MERAGE4(one,two,three,four)	(((uint32_t)one << 24) | ((uint32_t)two << 16) | ((uint16_t)three << 8) | four)//Combine four uint8s into one uint32
#define round_shift_left(x,n) x<<n|x>>32-n//Left rotation calculation
unsigned long long MODNUM = 4294967296;//2^32
void Align8Print(uint8_t* text, unsigned int length)//Input byte length
{
	for (int i = 0; i < length; i++)
	{
		printf("%02X", text[i]);
		if ((i + 1) % 4 == 0)
			printf(" ");
		if ((i + 1) % 32 == 0)
			printf("\n");
	}
	printf("\n");
}
void PrintSM3(uint32_t* sm3)
{
	for (int i = 0; i < 8; i++)
	{
		printf("%08X ", sm3[i]);
	}
	printf("\n");
}
unsigned long long padding(uint8_t* input, uint8_t* Out, unsigned long long length)
{
	/*
	input:Message to be padded
	Out:Padded message
	length:Unit is bit,the length of the input
	*/
	int k = 1;
	int total_length = (length / 512 + 1) * 512;//Bit length  after padded
	int byte_length = length / 8;//Byte length before padding
	int total_byte_length = total_length / 8;//Byte length after padding
	uint8_t* output = (uint8_t*)malloc((total_length / 8) * sizeof(uint8_t));
	for (k; k < total_length; k++)
	{
		if (((length + 1 + k) % 512) == 448)
			break;
	}
	for (int i = 0; i < byte_length; i++)//The front part of output is consistent with input
	{
		output[i] = input[i];
	}

	output[byte_length] = 0x80;//Append one 1 and 7 zeros

	int outfix = byte_length + 1 + (k - 7) / 8;//The number of 0s remaining to be padded is (k-7)/8
	for (int i = byte_length + 1; i < outfix; i++)//Append outfix 0
	{
		output[i] = 0x00;
	}
	int re = 0;
	int bin[64] = { 0 };
	int c = 0;
	int value = 0;
	while (length != 0)
	{
		re = length % 2;
		bin[63 - c] = re;
		c++;
		length = length / 2;
	}
	for (int i = 0; i < 64; i = i + 8)
	{
		value = 128 * bin[i] + 64 * bin[i + 1] + 32 * bin[i + 2] + 16 * bin[i + 3] + 8 * bin[i + 4] + 4 * bin[i + 5] + 2 * bin[i + 6] + bin[i + 7];
		output[total_byte_length - 8 + (i / 8)] = value;
	}
	for (int i = 0; i < total_byte_length; i++)
		Out[i] = output[i];
	free(output);
	return total_length;
}
void PrintW(uint32_t** W, int BlockNum, int choice)
{
	if (choice == FlagForW)
	{
		for (int i = 0; i < BlockNum; i++)
		{
			printf("W for Block %d:\n", i);
			for (int j = 0; j < 68; j++)
			{
				printf("%08x ", W[i][j]);
				if ((j + 1) % 8 == 0)
				{
					printf("\n");
				}
			}
			printf("\n");
		}
	}
	else if (choice == FlagForW1)
	{

		for (int i = 0; i < BlockNum; i++)
		{
			printf("W' for Block %d:\n", i);
			for (int j = 0; j < 64; j++)
			{
				printf("%08x ", W[i][j]);
				if ((j + 1) % 8 == 0)
				{
					printf("\n");
				}
			}
			printf("\n");
		}
	}
}
uint32_t** AllocW(int length, int choice)
// Enter the number of message blocks and choice, and return the allocated memory of W or W1
{
	uint32_t** W = (uint32_t**)malloc(length * sizeof(uint32_t**));
	if (choice == FlagForW)
	{
		for (int i = 0; i < length; i++)
			W[i] = (uint32_t*)malloc(68 * sizeof(uint32_t));

	}
	else if (choice == FlagForW1)
	{
		for (int i = 0; i < length; i++)
			W[i] = (uint32_t*)malloc(64 * sizeof(uint32_t));
	}
	return W;
}
void MessageExtension(uint8_t* B, uint32_t** OutW, uint32_t** OutW_1, unsigned long long Length)//Length is the bit length of the extended message block
{
	uint32_t Wj_16;
	uint32_t Wj_9;
	uint32_t Wj_3;
	uint32_t Wj_13;
	uint32_t Wj_6;
	uint32_t RoundShift1;
	uint32_t RoundShift2;
	uint32_t P1_Input;
	uint32_t P1_Output;
	unsigned int BlockNum = Length / 512;//The number of message blocks used to determine the length of W and W_1
	//-------------------------------------------Dynamically allocate a block for W-------------------------------------------
	uint32_t** W = (uint32_t**)malloc(BlockNum * sizeof(uint32_t*));
	for (int i = 0; i < BlockNum; i++)
	{
		W[i] = (uint32_t*)malloc(68 * sizeof(uint32_t));
	}
	uint32_t** W_1 = (uint32_t**)malloc(BlockNum * sizeof(uint32_t*));
	for (int i = 0; i < BlockNum; i++)
	{
		W_1[i] = (uint32_t*)malloc(64 * sizeof(uint32_t));
	}
	//-------------------------------------------Dynamically allocate a block for W-------------------------------------------

	//---------------------------------------------Initial W0-W15 assignment--------------------------------------------------
	for (int i = 0; i < BlockNum; i++)
	{
		for (int j = 0; j < 64; j = j + 4)
		{
			W[i][j / 4] = MERAGE4(B[i * 64 + j], B[i * 64 + j + 1], B[i * 64 + j + 2], B[i * 64 + j + 3]);
		}
	}
	//---------------------------------------------Initial W0-W15 assignment--------------------------------------------------

	//-------------------------------------------Calculate W16-W67 and W1_0-W1_64---------------------------------------------
	for (int i = 0; i < BlockNum; i++)
	{
		for (int j = 16; j < 68; j++)
		{
			Wj_16 = W[i][j - 16];
			Wj_9 = W[i][j - 9];
			Wj_3 = W[i][j - 3];
			Wj_13 = W[i][j - 13];
			Wj_6 = W[i][j - 6];
			RoundShift1 = round_shift_left(Wj_3, 15);
			RoundShift2 = round_shift_left(Wj_13, 7);
			P1_Input = Wj_16 ^ Wj_9 ^ RoundShift1;
			P1_Output = P1(P1_Input);
			W[i][j] = P1_Output ^ RoundShift2 ^ Wj_6;
		}
	}
	for (int i = 0; i < BlockNum; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			W_1[i][j] = W[i][j] ^ W[i][j + 4];
		}
	}
	//-------------------------------------------Calculate W16-W67 and W1_0-W1_64---------------------------------------------

	//------------------------------------------Get the result-----------------------------------------------------
	for (int i = 0; i < BlockNum; i++)
	{
		for (int j = 0; j < 68; j++)
		{
			OutW[i][j] = W[i][j];
		}
	}
	for (int i = 0; i < BlockNum; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			OutW_1[i][j] = W_1[i][j];
		}
	}
	free(W);
	free(W_1);
	//------------------------------------------Get the result-----------------------------------------------------
}
uint32_t GetTj(int j)
{
	if (j <= 15)
	{
		return Tj015;
	}
	else
	{
		return Tj1663;
	}
}
uint32_t GetFFj(uint32_t A, uint32_t B, uint32_t C, int j)
{
	if (j <= 15)
	{
		return FFj015(A, B, C);
	}
	else
	{
		return FFj1663(A, B, C);
	}
}
uint32_t GetGGj(uint32_t A, uint32_t B, uint32_t C, int j)
{
	if (j <= 15)
	{
		return GGj015(A, B, C);
	}
	else
	{
		return GGj1663(A, B, C);
	}
}
void CompressFunction(uint32_t* Vi, uint32_t* W, uint32_t* W_1, uint32_t* Vi1)
{
	uint32_t SS1, SS2, TT1, TT2;//Intermediate variables
	uint32_t A, B, C, D, E, F, G, H;//word register
	uint32_t A_12 = 0, T_j = 0, F_19 = 0, P0_output = 0, FFj_result = 0, GGj_reulst = 0;;
	A = Vi[0];
	B = Vi[1];
	C = Vi[2];
	D = Vi[3];
	E = Vi[4];
	F = Vi[5];
	G = Vi[6];
	H = Vi[7];
	for (int i = 0; i < 64; i++)
	{
		//printf("%02d      %08X %08X %08X %08X %08X %08X %08X %08X\n", i - 1, A, B, C, D, E, F, G, H);
		A_12 = round_shift_left(A, 12);
		T_j = GetTj(i);
		T_j = round_shift_left(T_j, (i % 32));
		SS1 = round_shift_left((A_12 + E + T_j) % MODNUM, 7);
		SS2 = SS1 ^ A_12;
		FFj_result = GetFFj(A, B, C, i);
		GGj_reulst = GetGGj(E, F, G, i);
		TT1 = FFj_result + D + SS2 + W_1[i];
		TT2 = GGj_reulst + H + SS1 + W[i];
		D = C;
		C = round_shift_left(B, 9);
		B = A;
		A = TT1;
		H = G;
		G = round_shift_left(F, 19);
		F = E;
		E = P0(TT2);

	}
	//("%02d      %08X %08X %08X %08X %08X %08X %08X %08X\n", 62, A, B, C, D, E, F, G, H);
	Vi1[0] = A ^ Vi[0];
	Vi1[1] = B ^ Vi[1];
	Vi1[2] = C ^ Vi[2];
	Vi1[3] = D ^ Vi[3];
	Vi1[4] = E ^ Vi[4];
	Vi1[5] = F ^ Vi[5];
	Vi1[6] = G ^ Vi[6];
	Vi1[7] = H ^ Vi[7];

}
void SM3(uint8_t* InputMessage, uint32_t* SM3Result, unsigned long long Length)// Input hash value and bits of message
{
	//-----------------------------------------------Message Padding Stage-------------------------------------------------------
	int total_length = (Length / 512 + 1) * 512;// Bit length after padded
	int byte_length = total_length / 8;
	uint8_t* PaddingResult = (uint8_t*)malloc(byte_length * sizeof(uint8_t));
	unsigned long long AfterPaddingLength = padding(InputMessage, PaddingResult, Length);
	unsigned int BlockNum = total_length / 512;// Number of message blocks
	//-----------------------------------------------Message padding Stage-------------------------------------------------------
	// After the message padding phase is over, PaddingResult stores the padding result, which is a uint8_t array with a length of 512bit integer multiples


	//-------------------------------------------Message Expansion Stage---------------------------------------------------------
	uint32_t** W = AllocW(BlockNum, FlagForW);//Allocate message word block space in the form of two-dimensional uint32_t array of number of message blocks * 68 or 64
	uint32_t** W_1 = AllocW(BlockNum, FlagForW1);
	MessageExtension(PaddingResult, W, W_1, total_length);
	//-------------------------------------------Message Expansion Stage---------------------------------------------------------
	/*After the message expansion phase is over, Wand W_1 store the message expansion results in the form of two - dimensional uint32_t arrays with the size of message block number * 68
	and message block number*64 respectively. The abscissa selects the message block, and the ordinate selects the message corresponding to the message block Character*/




	//-------------------------------------------Compression Function Stage---------------------------------------------------------
	uint32_t* Vi1 = (uint32_t*)malloc(8 * sizeof(uint32_t));
	uint32_t* Vi = (uint32_t*)malloc(8 * sizeof(uint32_t));
	for (int i = 0; i < 8; i++)
	{
		Vi1[i] = 0;
		Vi[i] = 0;
	}
	for (int i = 0; i < BlockNum; i++)
	{
		if (i == 0)
		{
			//printf("Compression function %d:\n", i+1);
			CompressFunction(IV, W[0], W_1[0], Vi1);//At the beginning of the iteration, the value of V0 is IV
			for (int i = 0; i < 8; i++)
				Vi[i] = Vi1[i];
		}
		else
		{
			//printf("Compression function %d:\n", i+1);
			CompressFunction(Vi, W[i], W_1[i], Vi1);
			for (int i = 0; i < 8; i++)
				Vi[i] = Vi1[i];
		}
	}
	//-------------------------------------------Compression Function Stage---------------------------------------------------------


	for (int i = 0; i < 8; i++)//Return the result
		SM3Result[i] = Vi[i];

}
#endif // !SM3

#pragma once
