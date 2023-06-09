#ifndef _SM2_SIGN_H
#define _SM2_SIGN_H
#include "miracl.h"
#include "mirdef.h"
#include <stdio.h>
#include"secret_share.h"
char Sm2CurveTestPublicKey_x[32] = { 0x0a,0xe4,0xc7,0x79,0x8a,0xa0,0xf1,0x19,
                                  0x47,0x1b,0xee,0x11,0x82,0x5b,0xe4,0x62,
                                  0x02,0xbb,0x79,0xe2,0xa5,0x84,0x44,0x95,
                                  0xe9,0x7c,0x04,0xff,0x4d,0xf2,0x54,0x8a
};
char Sm2CurveTestPublicKey_y[32] = { 0x7c,0x02,0x40,0xf8,0x8f,0x1c,0xd4,0xe1,
                                  0x63,0x52,0xa7,0x3c,0x17,0xb7,0xf1,0x6f,
                                  0x07,0x35,0x3e,0x53,0xa1,0x76,0xd6,0x84,
                                  0xa9,0xfe,0x0c,0x6b,0xb7,0x98,0xe8,0x57
};
char TestSecretKey[32] = {
    0x12,0x8b,0x2f,0xa8,0xbd,0x43,0x3c,0x6c,
    0x06,0x8c,0x8d,0x80,0x3d,0xff,0x79,0x79,
    0x2a,0x51,0x9a,0x55,0x17,0x1b,0x1b,0x65,
    0x0c,0x23,0x66,0x1d,0x15,0x89,0x72,0x63
};
char ZA[32] = {
    0xf4,0xa3,0x84,0x89,0xe3,0x2b,0x45,0xb6,
    0xf8,0x76,0xe3,0xac,0x21,0x68,0xca,0x39,
    0x23,0x62,0xdc,0x8f,0x23,0x45,0x9c,0x1d,
    0x11,0x46,0xfc,0x3e,0xbf,0xb7,0xbc,0x9a
};
char ZA_sm3[32] = {
    0xb5,0x24,0xf5,0x52,0xcd,0x82,0xb8,0xb0,
    0x28,0x47,0x6e,0x00,0x5c,0x37,0x7f,0xb1,
    0x9a,0x87,0xe6,0xfc,0x68,0x2d,0x48,0xbb,
    0x5d,0x42,0xe3,0xd9,0xb9,0xef,0xfe,0x76
};
char TestRandom_k[32] = {
    0x6c,0xb2,0x8d,0x99,0x38,0x5c,0x17,0x5c,
    0x94,0xf9,0x4e,0x93,0x48,0x17,0x66,0x3f,
    0xc1,0x76,0xd9,0x25,0xdd,0x72,0xb7,0x27,
    0x26,0x0d,0xba,0xae,0x1f,0xb2,0xf9,0x6f
};
void uint32_to_uint8(uint32_t* input, uint8_t* output)
{
    uint8_t* ptr = (uint8_t*)input;
    for (int i = 0; i < 32; i = i + 4)
    {
        output[i + 3] = *ptr;
        output[i + 2] = *(ptr + 1);
        output[i + 1] = *(ptr + 2);
        output[i + 0] = *(ptr + 3);
        ptr = ptr + 4;
    }
}
big inverse_mod(big x, big p)
{
    big xd, yd, gcd;
    xd = mirvar(0);
    yd = mirvar(0);
    gcd = mirvar(0);
    xgcd(x, p, xd, yd, gcd);
    mirkill(yd);
    mirkill(gcd);
    return xd;

}
int SM2_sign(uint8_t*Message, int MessageLength, big SecretKey, big r_out, big s_out)//MessageLength is the message byte length
{
    uint32_t MessageSM3[8] = { 0 };
    uint8_t DigestMessage[32] = { 0 };
    SM3(Message, MessageSM3, MessageLength * 8);
    printf("Signature Message Digest:\n");
    PrintSM3(MessageSM3);
    uint32_to_uint8(MessageSM3, DigestMessage);
    big g_x, g_y, k;
    big kG_x, kG_y;
    big r, s, e;
    big n, r_mod_n;
    big r_add_k;
    big da, da_add_1;
    big rda;
    big k_rda;
    epoint* G, * kG;
    G = epoint_init();
    kG = epoint_init();
    g_x = mirvar(0); g_y = mirvar(0);
    k = mirvar(0);
    kG_x = mirvar(0); kG_y = mirvar(0);
    e = mirvar(0);
    r = mirvar(0); s = mirvar(0);
    n = mirvar(0); r_mod_n = mirvar(0);
    r_add_k = mirvar(0);
    da_add_1 = mirvar(0);
    da = mirvar(0);
    rda = mirvar(0);
    k_rda = mirvar(0);
    bytes_to_big(32, DigestMessage, e);
    bytes_to_big(32, Sm2CurveParamG_x, g_x);
    bytes_to_big(32, Sm2CurveParamG_y, g_y);
    bytes_to_big(32, Sm2CurveParamG_Order, n);
    bytes_to_big(32, TestSecretKey, da);
    copy(SecretKey, da);
    epoint_set(g_x, g_y, 0, G);
    printf("Generator in Signing:\n");
    epoint_print(G);
    bigbits(256, k);
    add(r, k, r_add_k);

    //bytes_to_big(32, TestRandom_k, k);
    printf("Random Number:\n");
    cotnum(k, stdout);
    ecurve_mult(k, G, kG);
    printf("k*G:\n");
    epoint_print(kG);
    epoint_get(kG, kG_x, kG_y);
    add(e, kG_x, r);
    fmodulo(r, n, r_mod_n);
    if (fcomp(r_mod_n, mirvar(0)) == 0 || fcomp(r_add_k, n)==0)
    {
        printf("r is zero or r+k=n, error\n");
        return -1;
    }
    printf("r:\n");
    cotnum(r_mod_n, stdout);
    add(mirvar(1), da, da_add_1);
    big inverse_da_add_1 = inverse_mod(da_add_1, n);
    printf("Inverse of da+1:\n");
    cotnum(inverse_da_add_1, stdout);
    multiply(r, da, rda);
    subtract(k, rda, k_rda);
    multiply(inverse_da_add_1, k_rda, s);
    fmodulo(s, n, s);
    if(fcomp(s,mirvar(0))==-1)
        add(s, n, s);
    printf("s:\n");
    cotnum(s, stdout);
    printf("SM2 Signature Generation Complete:\n");
    cotnum(r_mod_n, stdout);
    cotnum(s, stdout);
    copy(r_mod_n, r_out);
    copy(s, s_out);
    mirkill(g_x); mirkill(g_y);
    return 1;
}
int SM2_verify(uint8_t *Message, int MessageLength, big r, big s, epoint *PA)
{
    printf("Signature Received:\n");
    printf("r:\n");
    cotnum(r, stdout);
    printf("s:\n");
    cotnum(s, stdout);
    uint32_t MessageSM3[8] = { 0 };
    SM3(Message, MessageSM3, MessageLength * 8);
    uint8_t DigestMessage[32] = { 0 };
    uint32_to_uint8(MessageSM3, DigestMessage);
    printf("Verfication Message Digest:\n");
    PrintSM3(MessageSM3);
    big n, n_1;
    big e;
    big t;
    big g_x, g_y;
    big R;
    big x1, y1;
    epoint* G, * sG_tPA;
    epoint* sG, * tPA;
    G = epoint_init(); sG_tPA = epoint_init();
    sG = epoint_init(); tPA = epoint_init();
    t = mirvar(0);
    e = mirvar(0);
    n = mirvar(0); n_1 = mirvar(0);
    g_x = mirvar(0); g_y = mirvar(0);
    R = mirvar(0);
    x1 = mirvar(0); y1 = mirvar(0);
    bytes_to_big(32, Sm2CurveParamG_Order, n);
    bytes_to_big(32, DigestMessage, e);
    bytes_to_big(32, Sm2CurveParamG_x, g_x);
    bytes_to_big(32, Sm2CurveParamG_y, g_y);
    epoint_set(g_x, g_y, 0, G);
    incr(n, -1, n_1);
    printf("Public Key:\n");
    epoint_print(PA);
    if (fcomp(r, mirvar(1)) == -1 || fcomp(r, n_1) == 1 || fcomp(s, mirvar(1)) == -1 || fcomp(s, n_1) == 1)//Range Check
    {
        printf("Signature Out of Range, error\n");
        return -1;
    }
    add(r, s, t);
    fmodulo(t, n, t);
    
    if (fcomp(t, mirvar(0)) == 0)
    {
        printf("r+s=0, error\n");
        return -1;
    }
    printf("r+s:\n");
    cotnum(t, stdout);
    ecurve_mult(t, PA, tPA);
    ecurve_mult(s, G, sG);
    printf("s*G:\n");
    epoint_print(sG);
    
    printf("tPA:\n");
    epoint_print(tPA);
    ecurve_add(sG, tPA);
    epoint_copy(tPA, sG_tPA);
    printf("s*G+t*PA:\n");
    epoint_print(sG_tPA);
    epoint_get(sG_tPA, x1, y1);
    add(e, x1, R);
    fmodulo(R, n, R);
    printf("R:\n");
    cotnum(R, stdout);
    printf("r:\n");
    cotnum(r, stdout);
    if (fcomp(R, r) == 0)
    {
        printf("Signature Valid\n");
        return 1;
    }
    else
    {
        printf("Signature InValid\n");
        return -1;
    }
}
#endif // !_SM2_SIGN_H
