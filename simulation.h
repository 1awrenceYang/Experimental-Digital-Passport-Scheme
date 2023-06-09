#ifndef SIMULATION_H
#define SIMULATION_H
const char* Passport = "P|CHN|EA14XXX30|WANG,CHENXU|M|CHINESE|31MAR2002|YUNNAN|14JUL2017|YUNNAN|13JUL2022|MPS Exit&Entry Administration";
#include"miracl.h"
#include"mirdef.h"
#include"secret_share.h"
#include<string.h>
typedef epoint* SignaturePK;
void HotelSimulation()
{
	big g_x, g_y, q;
	epoint* G;
	SignaturePK HotelPK;
	g_x = mirvar(0); g_y = mirvar(0); q = mirvar(0);
	bytes_to_big(32, Sm2CurveParamG_x, g_x);
	bytes_to_big(32, Sm2CurveParamG_y, g_y);
	bytes_to_big(32, Sm2CurveParamG_Order, q);
	G = epoint_init();
	HotelPK = epoint_init();
	epoint_set(g_x, g_y, 0, G);
	big* SecretShare = GenPkbySecretShare(3, 2, G, HotelPK, q);
	uint32_t HotelEncryptionKey[4] = { 0 };
	for (int i = 0; i < 4; i++)
		HotelEncryptionKey[i] = rand() % 0xffffffff;
	int PhotoSize = rand() % 5000;
	uint8_t* Photo_of_Customer = (uint8_t*)malloc(PhotoSize * sizeof(uint8_t));
	for (int i = 0; i < PhotoSize; i++)
		Photo_of_Customer[i] = rand() % 255;
	int PassportLength = sizeof(Passport);
	PassportLength = 112;
	uint8_t* Passport_add_Visa = (uint8_t*)malloc((PassportLength + PhotoSize) * sizeof(uint8_t));
	memcpy(Passport_add_Visa, Passport, PassportLength);
	memcpy(Passport_add_Visa + PassportLength, Photo_of_Customer, PhotoSize);
	printf("Passport and Photo Plaintext:\n");
	for (int i = 0; i < PassportLength + PhotoSize; i++)
	{
		printf("%02X", Passport_add_Visa[i]);
		if ((i + 1) % 32 == 0)
		{
			printf("\n");
			continue;
		}
		if ((i + 1) % 4 == 0)
			printf(" ");
	}
	uint8_t* Encrypted_Passport_Info = (uint8_t*)malloc(PassportLength * sizeof(uint8_t));
	uint8_t* PassportPlaintext = (uint8_t*)malloc(PassportLength * sizeof(uint8_t));
	memset(Encrypted_Passport_Info, 0, PassportLength);
	memcpy(PassportPlaintext, Passport, PassportLength);
	SM4_OFB(PassportPlaintext, PassportLength, HotelEncryptionKey, Encrypted_Passport_Info, Encryption);
	printf("\nEncrypted Passport:\n");
	for (int i = 0; i < PassportLength; i++)
	{
		printf("%02X", Encrypted_Passport_Info[i]);
		if ((i + 1) % 32 == 0)
		{
			printf("\n");
			continue;
		}
		if ((i + 1) % 4 == 0)
			printf(" ");
	}
	SM4_OFB(PassportPlaintext, PassportLength, HotelEncryptionKey, Encrypted_Passport_Info, Decryption);
	
	big HotelSignatureSecretKey = GenSkBySecretShare(2, SecretShare, q);
	big r = mirvar(0); big s = mirvar(0);
	printf("\nStart Signing\n");
	SM2_sign(Encrypted_Passport_Info, PassportLength, HotelSignatureSecretKey, r, s);
	
	printf("\nHotel Receive the Signature, Next Verify the Signature\n");
	SM2_verify(Encrypted_Passport_Info, PassportLength, r, s, HotelPK);
	printf("\nHotel Decrypt the Encrypted Message\n");

	printf("\nDecrypted Passport:\n");
	for (int i = 0; i < PassportLength; i++)
	{
		printf("%02X", PassportPlaintext[i]);
		if ((i + 1) % 32 == 0)
		{
			printf("\n");
			continue;
		}
		if ((i + 1) % 4 == 0)
			printf(" ");
	}
	printf("\nASCII Format:\n");
	for (int i = 0; i < PassportLength; i++)
		printf("%c", PassportPlaintext[i]);
}
void ImmigrationEntitySimulation()
{
	big g_x, g_y, q;
	epoint* G;
	SignaturePK IEPK;
	g_x = mirvar(0); g_y = mirvar(0); q = mirvar(0);
	bytes_to_big(32, Sm2CurveParamG_x, g_x);
	bytes_to_big(32, Sm2CurveParamG_y, g_y);
	bytes_to_big(32, Sm2CurveParamG_Order, q);
	G = epoint_init();
	IEPK = epoint_init();
	epoint_set(g_x, g_y, 0, G);
	big* SecretShare = GenPkbySecretShare(3, 2, G, IEPK, q);
	uint32_t IE_EncryptionKey[4] = { 0 };
	for (int i = 0; i < 4; i++)
		IE_EncryptionKey[i] = rand() % 0xffffffff;
	int FingerPrintDataLength = rand() % 5000;
	uint8_t* FingerPrint = (uint8_t*)malloc(FingerPrintDataLength * sizeof(uint8_t));
	for (int i = 0; i < FingerPrintDataLength; i++)
		FingerPrint[i] = rand() % 255;

	int PassportLength = sizeof(Passport);
	PassportLength = 112;
	uint8_t* Passport_add_Visa = (uint8_t*)malloc((PassportLength + FingerPrintDataLength) * sizeof(uint8_t));
	memcpy(Passport_add_Visa, Passport, PassportLength);
	memcpy(Passport_add_Visa + PassportLength, FingerPrint, FingerPrintDataLength);
	printf("Passport and Photo Plaintext:\n");
	for (int i = 0; i < PassportLength + FingerPrintDataLength; i++)
	{
		printf("%02X", Passport_add_Visa[i]);
		if ((i + 1) % 32 == 0)
		{
			printf("\n");
			continue;
		}
		if ((i + 1) % 4 == 0)
			printf(" ");
	}
	uint8_t* Encrypted_Passport_Info = (uint8_t*)malloc((PassportLength + FingerPrintDataLength) * sizeof(uint8_t));
	uint8_t* PassportPlaintext = (uint8_t*)malloc((PassportLength + FingerPrintDataLength) * sizeof(uint8_t));
	memset(Encrypted_Passport_Info, 0, (PassportLength + FingerPrintDataLength));
	memcpy(PassportPlaintext, Passport_add_Visa, PassportLength + FingerPrintDataLength);
	SM4_OFB(PassportPlaintext, PassportLength + FingerPrintDataLength, IE_EncryptionKey, Encrypted_Passport_Info, Encryption);
	printf("\nEncrypted Passport:\n");
	for (int i = 0; i < PassportLength + FingerPrintDataLength; i++)
	{
		printf("%02X", Encrypted_Passport_Info[i]);
		if ((i + 1) % 32 == 0)
		{
			printf("\n");
			continue;
		}
		if ((i + 1) % 4 == 0)
			printf(" ");
	}
	SM4_OFB(PassportPlaintext, PassportLength + FingerPrintDataLength, IE_EncryptionKey, Encrypted_Passport_Info, Decryption);
	big IE_Signature_SK = GenSkBySecretShare(2, SecretShare, q);
	big r = mirvar(0); big s = mirvar(0);
	printf("\nStart Signing\n");
	SM2_sign(Encrypted_Passport_Info, PassportLength+ FingerPrintDataLength/10, IE_Signature_SK, r, s);
	printf("\nImmigration Entity Receive the Signature, Next Verify the Signature\n");
	SM2_verify(Encrypted_Passport_Info, PassportLength+ FingerPrintDataLength/10, r, s, IEPK);
	printf("\nImmigration Entity Decrypt the Encrypted Message\n");

	printf("\nDecrypted Passport:\n");
	
	for (int i = 0; i < PassportLength + FingerPrintDataLength; i++)
	{
		printf("%02X", PassportPlaintext[i]);
		if ((i + 1) % 32 == 0)
		{
			printf("\n");
			continue;
		}
		if ((i + 1) % 4 == 0)
			printf(" ");
	}
}
#endif // !SIMULATION_H

