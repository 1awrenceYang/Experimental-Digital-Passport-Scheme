/************************************************************************************************************
             SM2 Elliptic Curve Recommended Parameters: GM/T 0003.1-2012 Appendix C.2 Fp-256 Curve
*************************************************************************************************************/
//const char Sm2CurveParamPrime[] = "8542D69E4C044F18E8B92435BF6FF7DE457283915C45517D722EDB8B08F1DFC3";
//const char Sm2CurveParam_a[] = "787968B4FA32C3FD2417842E73BBFEFF2F3C848B6831D7E0EC65228B3937E498";
//const char Sm2CurveParam_b[] = "63E4C6D3B23B0C849CF84241484BFE48F61D59A5B16BA06E6E12D1DA27C5249A";
//const char Sm2CurveParamG_x[] = "421DEBD61B62EAB6746434EBC3CC315E32220B3BADD50BDC4C4E6C147FEDD43D";
//const char Sm2CurveParamG_y[] = "0680512BCBB42C07D47349D2153B70C4E5D7FDFCBFA36EA1A85841B9E46E09A2";
//const char Sm2CurveParamG_Order[] = "8542D69E4C044F18E8B92435BF6FF7DD297720630485628D5AE74EE7C32E79B7";
#ifndef TEST
#define TEST
#include "miracl.h"
#include "mirdef.h"
#include <stdio.h>
#include<stdlib.h>
#include<time.h>
const char Sm2CurveParamPrime[32] = { 0x85,0x42,0xd6,0x9e,0x4c,0x04,0x4f,0x18,
                                0xe8,0xb9,0x24,0x35,0xbf,0x6f,0xf7,0xde,
                                0x45,0x72,0x83,0x91,0x5c,0x45,0x51,0x7d,
                                0x72,0x2e,0xdb,0x8b,0x08,0xf1,0xdf,0xc3 };
const char Sm2CurveParam_a[32] = { 0x78,0x79,0x68,0xb4,0xfa,0x32,0xc3,0xfd,
                             0x24,0x17,0x84,0x2e,0x73,0xbb,0xfe,0xff,
                             0x2f,0x3c,0x84,0x8b,0x68,0x31,0xd7,0xe0,
                             0xec,0x65,0x22,0x8b,0x39,0x37,0xe4,0x98 };
const char Sm2CurveParam_b[32] = { 0x63,0xe4,0xc6,0xd3,0xb2,0x3b,0x0c,0x84,
                             0x9c,0xf8,0x42,0x41,0x48,0x4b,0xfe,0x48,
                             0xf6,0x1d,0x59,0xa5,0xb1,0x6b,0xa0,0x6e,
                             0x6e,0x12,0xd1,0xda,0x27,0xc5,0x24,0x9a };
const char Sm2CurveParamG_x[32] = { 0x42,0x1d,0xeb,0xd6,0x1b,0x62,0xea,0xb6,
                              0x74,0x64,0x34,0xeb,0xc3,0xcc,0x31,0x5e,
                              0x32,0x22,0x0b,0x3b,0xad,0xd5,0x0b,0xdc,
                              0x4c,0x4e,0x6c,0x14,0x7f,0xed,0xd4,0x3d };
const char Sm2CurveParamG_y[32] = { 0x06,0x80,0x51,0x2b,0xcb,0xb4,0x2c,0x07,
                              0xd4,0x73,0x49,0xd2,0x15,0x3b,0x70,0xc4,
                              0xe5,0xd7,0xfd,0xfc,0xbf,0xa3,0x6e,0xa1,
                              0xa8,0x58,0x41,0xb9,0xe4,0x6e,0x09,0xa2 };
const char Sm2CurveParamG_Order[32] = { 0x85,0x42,0xd6,0x9e,0x4c,0x04,0x4f,0x18,
                                  0xe8,0xb9,0x24,0x35,0xbf,0x6f,0xf7,0xdd,
                                  0x29,0x77,0x20,0x63,0x04,0x85,0x62,0x8d,
                                  0x5a,0xe7,0x4e,0xe7,0xc3,0x2e,0x79,0xb7 };
void epoint_print(epoint* point)
{
    big x, y;
    x = mirvar(0);
    y = mirvar(0);
    epoint_get(point, x, y);
    cotnum(x, stdout);
    cotnum(y, stdout);
}
big* GenPolyParam(int t)// Generate random parameters for a polynomial
{
    big* aij = (big*)malloc(t * sizeof(big));// aij declaration and initialization
    for (int i = 0; i < t; i++)
        aij[i] = mirvar(0);
    for (int i = 0; i < t; i++)
        bigbits(255, aij[i]);
    return aij;
    // From left to right, they are a0, a1, a2, a3.....
    // Starting from the constant term, the coefficient of the first term, the coefficient of the second term...
}
big Expoent(int n, big a)// Calculate the nth power of a, a cannot be large numbers
{
    big result = mirvar(1);
    for (int i = 0; i < n; i++)
        multiply(result, a, result);
    return result;
}
big GenYij(int t, int j, big* aij)//Given polynomial parameters aij and input j, calculate output
{
    big fj = mirvar(0);
    big p = mirvar(1);
    big temp = mirvar(1);
    for (int a = 0; a < t; a++)
    {
        if (a == 0)
            add(aij[a], fj, fj);
        else
        {
            p = mirvar(j);
            temp = mirvar(1);
            multiply(aij[a], Expoent(a, p), temp);//aij*x^(j)
            add(temp, fj, fj);
        }
    }
    return fj;
    mirkill(p);
    mirkill(temp);
}
big* CalFij(int n, int t, big* aij)//Given polynomial parameters aij, calculate j different input
{
    big* ui = (big*)malloc(n * sizeof(big));
    big temp = mirvar(0);
    for (int i = 0; i < n; i++)
    {
        temp = GenYij(t, i + 1, aij);
        ui[i] = temp;
    }
    return ui;
    mirkill(temp);
}
big* CalSecretShareGiven(int n, int t, big** aij)//Calculate Secret share Yj
{
    big q = mirvar(0);
    big temp = mirvar(0);
    bytes_to_big(32, Sm2CurveParamG_Order, q);
    big* result = (big*)malloc(n * sizeof(big));
    for (int i = 0; i < n; i++)
        result[i] = mirvar(0);
    for (int i = 0; i < n; i++)//Sum each column of the matrix, using the row-first algorithm to prevent performance degradation when there are too many people
    {
        for (int j = 0; j < n; j++)
            add(result[j], aij[i][j], result[j]);
    }
    for (int i = 0; i < n; i++)
        divide(result[i], q, temp);
    return result;
    mirkill(q);
    mirkill(temp);
}
big CalNumerator(int r, int t)//Computes the numerator part of the delta function with given r and t
{
    big acc = mirvar(1);
    big j = mirvar(0);
    for (int i = 1; i <= t; i++)
    {
        j = mirvar(i);
        if (i == r)continue;
        negify(j, j);
        multiply(acc, j, acc);
    }
    mirkill(j);
    return acc;
}
big CalDominator(int r, int t)//Computes the denominator part of the delta function with given r and t
{
    big acc = mirvar(1);
    big temp = mirvar(0);
    big r_big = mirvar(r);
    big j = mirvar(0);
    for (int i = 1; i <= t; i++)
    {
        if (r == i)
            continue;
        j = mirvar(i);
        subtract(r_big, j, temp);//temp=r-j
        multiply(acc, temp, acc);
    }
    mirkill(temp);
    mirkill(r_big);
    mirkill(j);
    return acc;
}
big SecretShareSk(big* Yr, int t, big q)////Computes the secret value sk with the given SecretShare Yr
{
    big acc = mirvar(0);
    big NumAcc = mirvar(1);
    big Numerator = mirvar(0);//CalNumerator(r, t);
    big temp = mirvar(0);
    big Dominator = mirvar(0);//CalDominator(r, t);
    for (int i = 1; i <= t; i++)
    {
        Numerator = CalNumerator(i, t);
        Dominator = CalDominator(i, t);
        multiply(Numerator, Yr[i - 1], NumAcc);
        divide(NumAcc, Dominator, temp);
        divide(temp, q, q);//mod q
        add(acc, temp, acc);
        temp = mirvar(0);
    }
    mirkill(NumAcc);
    mirkill(Numerator);
    mirkill(temp);
    mirkill(Dominator);
    return acc;
}
epoint* SecretSharePk(big* Yr, big q, int t, epoint* G)//Compute the public key with the given SecretShare Yr without revealing the private key
{
    epoint* SharePk = epoint_init();
    epoint* PkAcc = epoint_init();
    big acc = mirvar(0);
    big NumAcc = mirvar(1);
    big Numerator = mirvar(0);//CalNumerator(r, t);
    big temp = mirvar(0);
    big Dominator = mirvar(0);//CalDominator(r, t);
    for (int i = 1; i <= t; i++)
    {
        Numerator = CalNumerator(i, t);
        Dominator = CalDominator(i, t);
        multiply(Numerator, Yr[i - 1], NumAcc);
        divide(NumAcc, Dominator, temp);
        divide(temp, q, q);//mod q
        ecurve_mult(temp, G, SharePk);
        ecurve_add(SharePk, PkAcc);
        temp = mirvar(0);
    }
    mirkill(NumAcc);
    mirkill(Numerator);
    mirkill(temp);
    mirkill(Dominator);
    epoint_free(SharePk);
    return PkAcc;
}
big* GenPkbySecretShare(int n, int t, epoint* G, epoint* PkOut, big q)
{
    /*
    n:Total people
    t:Total number of participants
    G:Generator
    Pkout:Output public key
    q:Secret sharing parameter, used to generate sk*C1 when decrypting
    */
    big** RandomPolyParam = (big**)malloc((n) * sizeof(big*));
    for (int i = 0; i < n; i++)
        RandomPolyParam[i] = (big*)malloc(n * sizeof(big));
    big** aij = (big**)malloc((n) * sizeof(big*));
    for (int i = 0; i < n; i++)
        aij[i] = (big*)malloc(n * sizeof(big));
    big* SecretShare = (big*)malloc(n * sizeof(big));
    big sk = mirvar(0);
    epoint* pk = epoint_init();
    printf("Secrect Share Initilization Complete£¬Generating Public & Secret Key Pair\n");
    /***************************************Generate n random polynomials of degree t-1**********************************************/
    printf("Generating Random Polynomial\n\n");
    for (int i = 0; i < n; i++)
        RandomPolyParam[i] = GenPolyParam(t);//Prohibition of release

    printf("Random Polynomial Generation Complete\n\n");
    /*****************Calculate the corresponding Ui through the generated n polynomials of degree t-1******************************/
    printf("Evaluating Polynomial\n\n");
    for (int i = 0; i < n; i++)
        aij[i] = CalFij(n, t, RandomPolyParam[i]);
    printf("Evaluation Complete\n\n");
    /*Through the calculated Ui, the simulation is sent to each participant through the star topology to calculate the Secret Share*/
    printf("Calculating Secret Share\n\n");
    SecretShare = CalSecretShareGiven(n, t, aij);
    printf("Secret Share Calculation Complete\n\n");
    /***************************Through Secret Share, calculate sk*G(pubk) without revealing sk(prik)*******************************/
    printf("Generateing Public Key\n\n");
    pk = SecretSharePk(SecretShare, q, t, G);
    printf("Public Key Generation Complete\n\n");
    epoint_copy(pk, PkOut);
    /******************************************free memory*************************************************/
    for (int i = 0; i < n; i++)
    {
        free(RandomPolyParam[i]);
        free(aij[i]);
    }
    free(RandomPolyParam);
    free(aij);
    mirkill(sk);
    epoint_free(pk);
    return SecretShare;
}
big GenSkBySecretShare(int t, big* SecretShare, big q)
{
    big sk = mirvar(0);
    sk = SecretShareSk(SecretShare, t, q);
    return sk;
}
#endif