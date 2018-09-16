#include <time.h>
#include "utils.h"
#include "sm2_const.h"
#include <immintrin.h>

const static size_t BITS = 256;

void mod(u32 & input, const u32 & m)
{
	while (u32_gte(input, m))
	{
		u32_sub(input, m, input);
	}
}

void double_mod(const u32 & x, u32 &result, const u32 & m, const u32 & rhoM)
{
	result = x;
	if (u32_shl(result)) // if it is overflowed
		u32_add(rhoM, result, result);

	if (u32_gte(result, m))
		u32_add(rhoM, result, result);
}

void add_mod(const u32 & x, const u32 & y, u32 &result, const u32 & m, const u32 & rhoM)
{
	if (u32_add(x, y, result)) // if it is overflowed
		u32_add(rhoM, result, result); 		// add_mod_n(rhoN, result, result);

	if (u32_gte(result, m))
		u32_add(rhoM, result, result);
}

void sub_mod(const u32 & x, const u32 & y, u32 & result, const u32 & m, const u32 & rhoM)
{
	u32 inversion_y;
	inv_for_add(y, inversion_y, m);
	add_mod(x, inversion_y, result, m, rhoM);
}

#define NEWMONG
void mul_mod_n(const u32 & x, const u32 & y, u32 & result)
{
#ifdef NEWMONG
	if (u32_eq_one(x))
	{
		result = y;
		return;
	}
	if (u32_eq_one(y))
	{
		result = x;
		return;
	}
	sm2n_mong_mul(x, y, result);
	sm2n_mong_mul(result, SM2_NH, result);

#else
	u32 tmp[2];
	montgomery_mul(x, SM2_rhoN2, tmp[0], SM2_N);
	montgomery_mul(y, SM2_rhoN2, tmp[1], SM2_N);
	montgomery_mul(tmp[0], tmp[1], result, SM2_N);
	montgomery_reduce(result, SM2_N);
#endif // 
}

// #define MONG
void pow_mod_p(const u32 & x, u32 & result)
{
#ifdef MONG
	if (u32_eq_one(x))
	{
		result = x;
		return;
	}

	sm2p_mong_pow(x, result);
	sm2p_mong_mul(result, SM2_H, result);


#else
	u8 res[8];
	raw_pow(x, res);
	solinas_reduce(res, result);
#endif
}

u32 mong_mul(const u32 & x, const u32 & y)
{
	if (u32_eq_one(x))
		return y;
	if (u32_eq_one(y))
		return x;

	u32 r;
	// return mong_mul(x, y, sm2_p);

	return r;
}

void mul_mod_p(const u32 & x, const u32 & y, u32 & result)
{
#ifdef MONG
	if (u32_eq_one(x))
	{
		result = y;
		return;
	}
	if (u32_eq_one(y))
	{
		result = x;
		return;
	}

	sm2p_mong_mul(x, y, result);
	sm2p_mong_mul(result, SM2_H, result);

#else
	u8 res[8];
	raw_mul(x, y, res);
	solinas_reduce(res, result);

#endif
}

void div_mod_p(const u32 & x, const u32 & y, u32 & result)
{
	u32 inversion_y;
	inv_for_mul_mod_p(y, inversion_y);
	mul_mod_p(x, inversion_y, result);
}

void inv_for_add(const u32 & input, u32 & result, const u32 & m)
{
	u32_sub(m, input, result);
}

static inline bool transform(u32 & x, u32 & y, const u32 & m, bool carry)
{
	while (x.v[0] % 2 == 0)
	{
		u32_shr(x);

		if (y.v[0] % 2 == 0)
		{
			u32_shr(y);
		}
		else
		{
			carry = u32_add(y, m, y);
			u32_shr(y);
			if (carry)
			{
				y.v[3] |= 0x8000000000000000;
			}
		}
	}
	return carry;
}

void inv_for_mul(const u32 & input, u32 & result, const u32 & m, const u32 & rhoM)
{
	if (u32_eq_zero(input))
	{
		result = input;
		return;
	}

	u32 u = input;
	u32 v = m;
	u32 x1 = { 1, 0, 0, 0 };
	u32 x2 = { 0, 0, 0, 0 };


	bool overflowFlag = false;

	while ((!u32_eq_one(u)) && (!u32_eq_one(v)))
	{
		overflowFlag = transform(u, x1, m, overflowFlag);
		overflowFlag = transform(v, x2, m, overflowFlag);

		if (u32_gte(u, v))
		{
			sub_mod(u, v, u, m, rhoM);
			sub_mod(x1, x2, x1, m, rhoM);
		}
		else
		{
			sub_mod(v, u, v, m, rhoM);
			sub_mod(x2, x1, x2, m, rhoM);
		}
	}

	if (u32_eq_one(u))
	{
		mod(x1, m);
		result = x1;
	}
	else
	{
		mod(x2, m);
		result = x2;
	}
}

void solinas_reduce(u8 input[8], u32 & result)
{
	/* fast reduction 256bit to 128bit*/
	/* ref: http://cacr.uwaterloo.ca/techreports/1999/corr99-39.pdf */

	// first split the input
	u4 * A = (u4 *)(input);

	// prepare the summands
	// given by LFSR with [1,0,0,0,1,-1,0,1] and proper re-combination
	// of mantissa
	u32 S = { input[0], input[1], input[2], input[3] };

	// { A[15],	A[15],	0, 0,		0, A[15],	0, A[15] };
	// { A[14],	A[14],	0, 0,		A[14], 0,	0, A[14] };
	// { A[13],	0,		0, A[13],	0, 0,		0, A[13] };
	// { 0,		0,		0, 0,		0, 0,		0, A[12] };
	// sumD = S15D + S14D + S13D + S12D
	u32 sumD = { 0,0,0,0 };

	u1 carry = _addcarryx_u64(0, ((u8)A[15] << 32) | ((u8)A[15]), ((u8)A[14] << 32) | ((u8)A[14]), sumD.v);
	carry += _addcarryx_u64(0, sumD.v[0], A[13], sumD.v);
	sumD.v[1] = ((u8)A[13] << 32) + carry;
	sumD.v[2] = ((u8)A[15] << 32) + (u8)A[14];
	carry = _addcarryx_u64(0, (u8)A[15] << 32, (u8)A[14] << 32, sumD.v + 3);
	if (carry) u32_add(SM2_rhoP, sumD, sumD);
	carry = _addcarryx_u64(0, sumD.v[3], (u8)A[13] << 32, sumD.v + 3);
	if (carry) u32_add(SM2_rhoP, sumD, sumD);
	carry = _addcarryx_u64(0, sumD.v[3], (u8)A[12] << 32, sumD.v + 3);
	if (carry) u32_add(SM2_rhoP, sumD, sumD);

	// A[8], A[13],		0, A[8],	A[13], A[13],	0, A[8]
	// A[9], A[9],		0, A[14],	A[9], A[14],	A[14], A[9]
	// A[10], A[10],	0, A[12],	A[12], A[10],	0, A[10]
	// A[11], A[11],	0, A[11],	0, 0,			A[11], A[11]
	// A[12], A[12],	0, A[15],	A[15], 0,		A[15], A[15]
	if (u32_shl(sumD)) u32_add(SM2_rhoP, sumD, sumD);
	if (u32_add(S, sumD, S)) u32_add(SM2_rhoP, S, S);

	carry = _addcarryx_u64(0, ((u8)A[9] << 32) | ((u8)A[9]), ((u8)A[10] << 32) | ((u8)A[10]), sumD.v);
	carry += _addcarryx_u64(0, ((u8)A[11] << 32) | ((u8)A[11]), ((u8)A[12] << 32) | ((u8)A[12]), sumD.v + 1);
	carry += _addcarryx_u64(0, ((u8)A[13] << 32) | ((u8)A[8]), sumD.v[1], sumD.v + 1);
	carry += _addcarryx_u64(0, sumD.v[0], sumD.v[1], sumD.v);

	carry = _addcarryx_u64(0, ((u8)A[8] << 32) | carry, ((u8)A[14] << 32), sumD.v + 1);
	carry += _addcarryx_u64(0, (u8)A[11] << 32, (u8)A[12] << 32, sumD.v + 2);
	carry += _addcarryx_u64(0, (u8)A[15] << 32, sumD.v[2], sumD.v + 2);
	carry += _addcarryx_u64(0, sumD.v[1], sumD.v[2], sumD.v + 1);

	carry = _addcarryx_u64(0, (u8)A[15] + carry, ((u8)A[13] << 32) | ((u8)A[13]), sumD.v + 2);
	carry += _addcarryx_u64(0, ((u8)A[14] << 32) | ((u8)A[9]), ((u8)A[10] << 32) | ((u8)A[12]), sumD.v + 3);
	carry += _addcarryx_u64(0, sumD.v[2], sumD.v[3], sumD.v + 2);

	carry = _addcarryx_u64(0, ((u8)A[8] << 32) | carry, ((u8)A[10] << 32), sumD.v + 3);
	if (carry) u32_add(SM2_rhoP, sumD, sumD);
	carry = _addcarryx_u64(0, sumD.v[3], ((u8)A[11] << 32) | ((u8)A[11]), sumD.v + 3);
	if (carry) u32_add(SM2_rhoP, sumD, sumD);
	carry = _addcarryx_u64(0, sumD.v[3], ((u8)A[15] << 32) | ((u8)A[15]), sumD.v + 3);
	if (carry) u32_add(SM2_rhoP, sumD, sumD);
	carry = _addcarryx_u64(0, sumD.v[3], ((u8)A[9] << 32) | ((u8)A[14]), sumD.v + 3);
	if (carry) u32_add(SM2_rhoP, sumD, sumD);

	if (u32_add(S, sumD, S)) u32_add(SM2_rhoP, S, S);

	// find the subtrahend
	result.v[0] = result.v[2] = result.v[3] = 0;
	result.v[1] = (u8)A[8] + (u8)A[9] + (u8)A[13] + (u8)A[14];
	sub_mod_p(S, result, result);

	// should the following op be removed ???
	// if (u32_gte(result, p)) sub_mod_p(result, p, result);
}
