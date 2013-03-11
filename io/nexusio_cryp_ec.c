#ifndef NEXUSIO_CRYP_EC_C
#define NEXUSIO_CRYP_EC_C
/*****************************************************************************/
#include "nexusio_cryp_ec.h"
/*****************************************************************************/
static void NEXUSIO_Ecc_Point_Add(mp_int *x1, mp_int *y1, mp_int *x2, mp_int *y2,
                          mp_int *x3, mp_int *y3, mp_int *aa, mp_int *pp)
{
    mp_int xx[1];
    mp_int yy[1];
    mp_int tk[1];
    mp_int t1[1];

    mp_init(xx);
    mp_init(yy);
    mp_init(tk);
    mp_init(t1);

    if(mp_cmp_d(x1, 0) == 0 && mp_cmp_d(y1, 0) == 0)
    {
       mp_copy(x2, x3);
       mp_copy(y2, y3);
       goto CLEAN;
    }

    if(mp_cmp_d(x2, 0) == 0 && mp_cmp_d(y2, 0) == 0)
    {
       mp_copy(x1, x3);
       mp_copy(y1, y3);
       goto CLEAN;
    }

    mp_submod(x2, x1, pp, xx);

    if(mp_cmp_d(xx, 0) == 0)
    {
       mp_addmod(y2, y1, pp, yy);

       if(mp_cmp_d(yy, 0) == 0)
       {
          mp_zero(x3);
          mp_zero(y3);
          goto CLEAN;
       }
       else
       {
          mp_invmod(yy, pp, tk);
          mp_expt_d(x1,  2, t1);
          mp_mul_d (t1,  3, t1);
          mp_addmod(t1, aa, pp, t1);
          mp_mulmod(t1, tk, pp, tk);
       }
    }
    else
    {
       mp_invmod(xx, pp, tk);
       mp_submod(y2, y1, pp, yy);
       mp_mulmod(yy, tk, pp, tk);
    }

    mp_sqr(tk, t1);
    mp_sub(t1, x1, t1);
    mp_submod(t1, x2, pp, x3);
    mp_sub(x1, x3, t1);
    mp_mul(t1, tk, t1);
    mp_submod(t1, y1, pp, y3);

CLEAN:

    mp_clear(xx);
    mp_clear(yy);
    mp_clear(tk);
    mp_clear(t1);
}
/*****************************************************************************/
static void NEXUSIO_Ecc_Point_Mul(mp_int *qx, mp_int *qy, mp_int *px, mp_int *py,
                          mp_int *dd, mp_int *aa, mp_int *pp)
{
	mp_int XX[1];
	mp_int YY[1];

	char Bt[800];

	mp_init(XX);
	mp_init(YY);

    mp_toradix(dd, Bt, 2);

    mp_zero(qx);
    mp_zero(qy);

    int i;

	for(i = 0; Bt[i] != '\0'; i ++)
	{
	   NEXUSIO_Ecc_Point_Add(qx, qy, qx, qy, XX, YY, aa, pp);

       mp_copy(XX, qx);
	   mp_copy(YY, qy);

	   if(Bt[i] == '1')
	   {
		  NEXUSIO_Ecc_Point_Add(qx, qy, px, py, XX, YY, aa, pp);

		  mp_copy(XX, qx);
		  mp_copy(YY, qy);
	   }
	}

    mp_clear(XX);
    mp_clear(YY);
}
/*****************************************************************************/
static long NEXUSIO_Ecc_Check_Key(mp_int *QX, mp_int *QY, mp_int *sk, mp_int *nn,
                          mp_int *aa, mp_int *bb, mp_int *pp)
{
	mp_int *k;
	
	/* unused parameter 'sk' , why not del ?*/
	k = sk;

	if(mp_cmp_d(QX, 0) == 0 && mp_cmp_d(QY, 0) == 0)
		return 0;

	if(mp_cmp_d(QX, 0) == -1 || mp_cmp(QX, pp) >= 0)
		return 0;

	if(mp_cmp_d(QY, 0) == -1 || mp_cmp(QY, pp) >= 0)
		return 0;

	mp_int tx[1];
	mp_int ty[1];

	mp_init(tx);
	mp_init(ty);

	mp_expt_d(QX, 3, tx);
	mp_mod(tx, pp, tx);

	mp_mulmod(QX, aa, pp, ty);
	mp_addmod(tx, ty, pp, tx);
	mp_addmod(tx, bb, pp, tx);

	mp_expt_d(QY, 2, ty);
	mp_mod(ty, pp, ty);

	if(mp_cmp(tx, ty))
		return 0;

	mp_int rx[1];
	mp_int ry[1];

	mp_init(rx);
	mp_init(ry);

	NEXUSIO_Ecc_Point_Mul(rx, ry, QX, QY, nn, aa, pp);

	if(mp_cmp_d(rx, 0) == 0 && mp_cmp_d(ry, 0) == 0)
		return 1;
	else
		return 0;
}
/*****************************************************************************/
void NEXUSIO_Ecc_Encrypt(mp_int *GX, mp_int *GY, mp_int *QX, mp_int *QY,
                 mp_int *aa, mp_int *pp, mp_int *nn, void *data,
                 mp_int *rx, mp_int *ry, mp_int *cc)
{
	//data is 20 byte;

	mp_int tk[1];
	mp_int tr[1];
	mp_int mm[1];
	mp_int ff[1];
	mp_int tx[1];
	mp_int ty[1];

	unsigned char *pm = data;

	mp_init(tk);
	mp_init(tr);
	mp_init_set(mm, 0);
	mp_init_set(ff, 256);
	mp_init(tx);
	mp_init(ty);

	int ii;

	for(ii = 0; ii < 20; ii ++)
	{
		mp_mulmod(mm, ff, pp, mm);
		mp_add_d(mm, *(pm++), mm);
	}

	while(1)
	{
		mp_init_set(tk, rand());
		mp_mod(tk, nn, tk);

		if(mp_cmp_d(tk, 0) == 0)
			mp_add_d(tk, 1, tk);

		NEXUSIO_Ecc_Point_Mul(rx, ry, GX, GY, tk, aa, pp);

		NEXUSIO_Ecc_Point_Mul(tx, ty, QX, QY, tk, aa, pp);

		if(mp_cmp_d(tx, 0) == 0)
			continue;

		mp_mulmod(mm, tx, pp, cc);

		return;
	};
}
/*****************************************************************************/
long NEXUSIO_Ecc_Decrypt(mp_int *GX, mp_int *GY, mp_int *aa, mp_int *bb,
                 mp_int *pp, mp_int *sk, void *data, mp_int *rx,
                 mp_int *ry, mp_int *cc)
{
	mp_int *hh;
	/* unused parameter 'GX / GY' , why not del ?*/
	hh = GX;
	hh = GY;

	//data is 20 byte;

	mp_int tx[1];
	mp_int ty[1];
	mp_int mm[1];
	mp_int ff[1];
	mp_int tr[1];

	char ts[8];

	unsigned char *pm = data;

	mp_init(tx);
	mp_init(ty);

	mp_init(mm);
	mp_init_set(ff, 256);
	mp_init(tr);

	mp_expt_d(rx, 3, tx);
	mp_mod(tx, pp, tx);

	mp_mulmod(rx, aa, pp, ty);
	mp_addmod(tx, ty, pp, tx);
	mp_addmod(tx, bb, pp, tx);
	mp_expt_d(ry, 2, ty);
	mp_mod(ty, pp, ty);

	if(mp_cmp(tx, ty) != 0)
		return 0;

	NEXUSIO_Ecc_Point_Mul(tx, ty, rx, ry, sk, aa, pp);

	if(mp_cmp_d(tx, 0) == 0)
		return 0;

	mp_invmod(tx, pp, tx);
	mp_mulmod(cc, tx, pp, mm);

	int ii;

	for(ii = 0; ii < 20; ii ++)
	{
		mp_div(mm, ff, mm, tr);

		mp_toradix(tr, ts, 10);

		pm[19 - ii] = atoi(ts);
	}

	return 1;
}
/*****************************************************************************/
void NEXUSIO_Ecc_MakeKey(mp_int *QX, mp_int *QY, mp_int *GX, mp_int *GY,
                 mp_int *sk, mp_int *nn, mp_int *aa, mp_int *bb,
                 mp_int *pp)
{
	mp_int nm[1];

	mp_init(nm);

	mp_sub_d(nn, 1, nm);

	while(mp_cmp(sk, nm) == -1)
	{
		NEXUSIO_Ecc_Point_Mul(QX, QY, GX, GY, sk, aa, pp);

		if(NEXUSIO_Ecc_Check_Key(QX, QY, sk, nn, aa, bb, pp))
			return;

		mp_add_d(sk, 1, sk);
	}

	mp_set(sk, 0);
}
/*****************************************************************************/
#endif//cryp_ec_C
