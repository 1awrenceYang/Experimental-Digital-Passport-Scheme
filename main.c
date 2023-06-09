#define _CRT_SECURE_NO_WARNINGS
#include"secret_share.h"
#include"sm3.h"
#include"sm2_sign.h"
#include"sm4.h"
#include"simulation.h"
int main() 
{
    big g_x, g_y, a, b, p, q;
    big pk_x, pk_y;
    epoint* G, *pk;
    miracl* mip = mirsys(36, MAXBASE);
    time_t seed;
    time(&seed);
    irand((unsigned long long)seed);
    mip->IOBASE = 16;
    g_x = mirvar(0); g_y = mirvar(0);
    a = mirvar(0); b = mirvar(0);
    p = mirvar(0); q = mirvar(0);
    G = epoint_init(); pk = epoint_init();
    pk_x = mirvar(0);
    pk_y = mirvar(0);
    bytes_to_big(32, Sm2CurveParamG_x, g_x);
    bytes_to_big(32, Sm2CurveParamG_y, g_y);
    bytes_to_big(32, Sm2CurveParam_a, a);
    bytes_to_big(32, Sm2CurveParam_b, b);
    bytes_to_big(32, Sm2CurveParamPrime, p);
    bytes_to_big(32, Sm2CurveParamG_Order, q);
    bytes_to_big(32, Sm2CurveTestPublicKey_x, pk_x);
    bytes_to_big(32, Sm2CurveTestPublicKey_y, pk_y);
    ecurve_init(a, b, p, MR_BEST);
    epoint_set(g_x, g_y, 0, G);
    epoint_set(pk_x, pk_y, 0, pk);



    ImmigrationEntitySimulation();

    
}
