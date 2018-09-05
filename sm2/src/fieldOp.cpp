#include <time.h>
#include "utils.h"
#include "const256.h"

void mod(u32 & input, const u32 & m)
{
	while (u32_gte(input, m))
	{
		u32_sub(input, m, input);
	}
}

void add_mod_n(const u32 & x, const u32 & y, u32 &result)
{
	if (u32_add(x, y, result)) // if it is overflowed
		u32_add(rhoN, result, result); 		// add_mod_n(rhoN, result, result);

	if (u32_gte(result, n))
		u32_sub(result, n, result);
}

void add_mod_p(const u32 & x, const u32 & y, u32 &result)
{
	if (u32_add(x, y, result)) // is overflow?
		u32_add(rhoP, result, result); // 	add_mod_p(rhoP, result, result);

	if (u32_gte(result, p))
		u32_sub(result, p, result);
}


void sub_mod_n(const u32 & x, const u32 & y, u32 & result)
{
	u32 inversion_y;
	inv_for_add(y, inversion_y, n);
	add_mod_n(x, inversion_y, result);
}

void sub_mod_p(const u32 & x, const u32 & y, u32 & result)
{
	u32 inversion_y;
	inv_for_add(y, inversion_y, p);
	add_mod_p(x, inversion_y, result);
}

void mul_mod_n(const u32 & x, const u32 & y, u32 & result)
{
	u32 tmp[2];
	montgomery_mul(x, rhoN2, tmp[0]);
	montgomery_mul(y, rhoN2, tmp[1]);
	montgomery_mul(tmp[0], tmp[1], result);
	montgomery_reduce(result);
}

void mul_mod_p(const u32 & x, const u32 & y, u32 & result)
{
	u8 res[8];
	raw_mul(x, y, res);
	solinas_reduce(res, result);
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


void inv_for_mul_mod_p(const u32 & input, u32 & result)
{
	if (u32_eq_zero(input))
	{
		result = input;
		return;
	}

	u32 u = input;
	u32 v = p;
	u32 x1 = { 1, 0, 0, 0 };
	u32 x2 = { 0, 0, 0, 0 };


	bool overflowFlag = false;

	while ((!u32_eq_one(u)) && (!u32_eq_one(v)))
	{
		overflowFlag = transform(u, x1, p, overflowFlag);
		overflowFlag = transform(v, x2, p, overflowFlag);

		if (u32_gte(u, v))
		{
			sub_mod_p(u, v, u);
			sub_mod_p(x1, x2, x1);
		}
		else
		{
			sub_mod_p(v, u, v);
			sub_mod_p(x2, x1, x2);
		}
	}

	if (u32_eq_one(u))
	{
		mod(x1, p);
		result = x1;
	}
	else
	{
		mod(x2, p);
		result = x2;
	}
}

void inv_for_mul_mod_n(const u32 & input, u32 & result)
{
	if (u32_eq_zero(input))
	{
		result = input;
		return;
	}
	u32 u = input;
	u32 v = n;
	u32 x1 = { 1, 0, 0, 0 };
	u32 x2 = { 0, 0, 0, 0 };

	bool overflowFlag = false;
	while ((!u32_eq_one(u)) && (!u32_eq_one(v)))
	{
		overflowFlag = transform(u, x1, n, overflowFlag);
		overflowFlag = transform(v, x2, n, overflowFlag);

		if (u32_gte(u, v))
		{
			sub_mod_n(u, v, u);
			sub_mod_n(x1, x2, x1);
		}
		else
		{
			sub_mod_n(v, u, v);
			sub_mod_n(x2, x1, x2);
		}
	}

	if (u32_eq_one(u))
	{
		mod(x1, n);
		result = x1;
	}
	else
	{
		mod(x2, n);
		result = x2;
	}
}


void montgomery_mul(const u32 & x, const u32 & y, u32 & result)
{
	u32 z = { 0,0,0,0 };

	forloop(i, 0, 256)
	{
		if (u32_get_bit(y, i) == 1)
			add_mod_n(z, x, z);

		if (z.v[0] % 2 == 1)
		{
			bool overflow_flag = u32_add(z, n, z);
			u32_shr(z);
			if (overflow_flag)
				z.v[3] |= 0x8000000000000000;
		}
		else
			u32_shr(z);
	}

	if (u32_gte(z, n))
		u32_sub(z, n, result);
	else
		result = z;
}

void montgomery_reduce(u32 & result)
{
	u32 t = result;
	forloop(i, 0, 256)
	{
		if (t.v[0] % 2 == 1)
		{
			bool overflow_flag = u32_add(t, n, t);
			u32_shr(t);
			if (overflow_flag)
			{
				t.v[3] |= 0x8000000000000000;
			}
		}
		else
		{
			u32_shr(t);
		}
	}

	if (u32_gte(t, n))
		add_mod_n(t, n, result);
	else
		result = t;
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

	// the following should be added twice (suffix D)
	u32 S15D, S14D, S13D, S12D;
	// u4 tmp15D[8] = { A[15], A[15], 0, 0, 0, A[15], 0, A[15] };
	// u4_to_u32(tmp15D, S15D);
	S15D.v[1] = 0;
	S15D.v[2] = S15D.v[3] = ((u8)A[15] << 32);
	S15D.v[0] = (S15D.v[3] | A[15]);

	// u4 tmp14D[8] = { A[14], A[14], 0, 0, A[14], 0, 0, A[14] };
	// u4_to_u32(tmp14D, S14D);
	S14D.v[1] = 0;
	S14D.v[2] = ((u8)A[14]);
	S14D.v[3] = ((u8)A[14] << 32);
	S14D.v[0] = (S14D.v[3] | S14D.v[2]);

	// u4 tmp13D[8] = { A[13], 0, 0, A[13], 0, 0, 0, A[13] };
	// u4_to_u32(tmp13D, S13D);
	S13D.v[2] = 0;
	S13D.v[0] = ((u8)A[13]);
	S13D.v[1] = ((u8)A[13] << 32);
	S13D.v[3] = S13D.v[1];

	// u4 tmp12D[8] = { 0, 0, 0, 0, 0, 0, 0, A[12] };
	// u4_to_u32(tmp12D, S12D);
	S12D.v[0] = S12D.v[1] = S12D.v[2] = 0;
	S12D.v[3] = ((u8)A[12] << 32);

	// find the sum
	u32 sum1, sum2, sumD;
	add_mod_p(S15D, S14D, sum1);
	add_mod_p(S13D, S12D, sum2);
	add_mod_p(sum1, sum2, sumD);

	// find other sum (hard coded by sight)
	u32 S8_13, S9_14, S10_12, S11, S15_12;
	// u4 tmp8_13[8] = { A[8], A[13], 0, A[8], A[13], A[13], 0, A[8] };
	// u4_to_u32(tmp8_13, S8_13); 
	S8_13.v[0] = ((u8)A[13] << 32) | A[8];
	S8_13.v[1] = S8_13.v[3] = ((u8)A[8] << 32);
	S8_13.v[2] = ((u8)A[13] << 32) | A[13];

	u4 tmp9_14[8] = { A[9], A[9], 0, A[14], A[9], A[14], A[14], A[9] };
	u4_to_u32(tmp9_14, S9_14);

	u4 tmp10_12[8] = { A[10], A[10], 0, A[12], A[12], A[10], 0, A[10] };
	u4_to_u32(tmp10_12, S10_12);

	u4 tmp11[8] = { A[11], A[11], 0, A[11], 0, 0, A[11], A[11] };
	u4_to_u32(tmp11, S11);

	u4 tmp15_12[8] = { A[12], A[12], 0, A[15], A[15], 0, A[15], A[15] };
	u4_to_u32(tmp15_12, S15_12);

	// sum all the stuffs together
	u32 sum3, sum4, sum5;
	add_mod_p(S8_13, S9_14, sum1);
	add_mod_p(S10_12, S11, sum2);
	add_mod_p(sum1, sum2, sum3);
	add_mod_p(S, sumD, sum4);
	add_mod_p(sum4, sum3, sum5);
	add_mod_p(S15_12, sumD, sum1);
	add_mod_p(sum1, sum5, S);

	// find the subtrahend
	result.v[0] = result.v[2] = result.v[3] = 0;
	result.v[1] = (u8)A[8] + (u8)A[9] + (u8)A[13] + (u8)A[14];

	sub_mod_p(S, result, result);

	// should the following op be removed ???
	// if (u32_gte(result, p)) sub_mod_p(result, p, result);
}
