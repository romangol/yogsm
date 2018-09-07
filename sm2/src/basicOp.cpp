#include "basicOp.h"
#include "utils.h"
#include "sm2_const.h"
#include <immintrin.h>
#include <intrin.h>

void u32_neg(u32 & input)
{
	u1 carry = 1;
	forloop (i, 0, 4)
		carry = _addcarryx_u64(carry, ~input.v[i], 0, input.v + i);
}

bool u32_add(const u32 & a, const u32 & b, u32 & result)
{
	u1 carry = 0;
	forloop (i, 0, 4)
		carry = _addcarryx_u64(carry, a.v[i], b.v[i], result.v + i);

	return (carry == 1);
}

void u32_sub(const u32 & a, const u32 & b, u32 & result)
{
	u1 carry = 1;
	forloop(i, 0, 4)
		carry = _addcarryx_u64(carry, a.v[i], ~b.v[i], result.v + i);
}

bool u32_shl(u32 & input)
{
	bool carry = (input.v[3] >> 63);

	input.v[3] = __shiftleft128(input.v[2], input.v[3], 1);
	input.v[2] = __shiftleft128(input.v[1], input.v[2], 1);
	input.v[1] = __shiftleft128(input.v[0], input.v[1], 1);
	input.v[0] <<= 1;

	return carry;
}

void u32_shr(u32 & input)
{
	input.v[0] = __shiftright128(input.v[0], input.v[1], 1);
	input.v[1] = __shiftright128(input.v[1], input.v[2], 1);
	input.v[2] = __shiftright128(input.v[2], input.v[3], 1);
	input.v[3] >>= 1;
}

// great than
bool u32_gte(const u32 & a, const u32 & b)
{
	if (a.v[3] != b.v[3])
		return (a.v[3] > b.v[3]);

	if (a.v[2] != b.v[2])
		return (a.v[2] > b.v[2]);

	if (a.v[1] != b.v[1])
		return (a.v[1] > b.v[1]);

	return (a.v[0] >= b.v[0]);
}

bool u32_eq(const u32 & a, const u32 & b)
{
	// constant time comparison
	return !((a.v[0] ^ b.v[0]) | (a.v[1] ^ b.v[1]) | (a.v[2] ^ b.v[2]) | (a.v[3] ^ b.v[3]));
	// return (a.v[0] == b.v[0]) && (a.v[1] == b.v[1]) && (a.v[2] == b.v[2]) && (a.v[3] == b.v[3]);
}


bool u32_eq_zero(const u32 & a)
{
	// constant time comparison
	return !(a.v[0] | a.v[1] | a.v[2] | a.v[3]);
	// return (a.v[0] == 0) && (a.v[1] == 0) && (a.v[2] == 0) && (a.v[3] == 0);
}

bool u32_eq_one(const u32 & a)
{
	return (a.v[0] == 1) && (a.v[1] == 0) && (a.v[2] == 0) && (a.v[3] == 0);
}

void raw_mul(const u32 & x, const u32 & y, u8 result[8])
{
	const static size_t LEN = 8;
	u8 interim[LEN] = { 0 };

	forloop(i, 0, 4)
	{
		forloop(j, 0, 4)
		{
			u1 carry = 0;
			u8 h;
			u8 l = _mulx_u64(x.v[i], y.v[j], &h);
			carry = _addcarryx_u64(carry, interim[i + j + 0], l, interim + i + j + 0);
			carry = _addcarryx_u64(carry, interim[i + j + 1], h, interim + i + j + 1);
			size_t pos = 2 + i + j;
			while (carry && pos < LEN)
			{
				carry = _addcarryx_u64(carry, interim[pos], 0, interim + pos);
				++pos;
			}
		}
	}

	memcpy(result, interim, 64);
}

void raw_pow(const u32 & x, u8 result[8])
{
	const static size_t LEN = 8;
	u8 interim[LEN] = { 0 };
	u8 h, l;

	forloop(i, 0, LEN / 2)
	{
		interim[i * 2 + 0] = _mulx_u64(x.v[i], x.v[i], interim + i * 2 + 1);
	}

	forloop(i, 0, 3)
	{
		forloop(j, i + 1, 4)
		{
			u1 carry = 0;
			size_t index = i + j;
			l = _mulx_u64(x.v[i], x.v[j], &h);
			carry = _addcarryx_u64(carry, interim[index + 0], (l << 1), interim + index + 0);
			carry = _addcarryx_u64(carry, interim[index + 1], (h << 1) | (l >> 63), interim + index + 1);
			carry += (h >> 63);
			size_t pos = index + 2;
			while (carry && pos < LEN)
			{
				carry = _addcarryx_u64(0, interim[pos], carry, interim + pos);
				++pos;
			}
		}
	}

	memcpy(result, interim, 64);
}

static void sm2p_mong_mul_core(u8 interim[9], u32 & result)
{
	const static size_t LEN = 9;
	forloop(i, 0, 4)
	{
		u1 carry;
		u8 h;
		u8 l;
		size_t pos;

		u8 factor = interim[i];
		if (factor == 0)
			continue;
		if (factor == 1)
		{
			forloop(j, 0, 4)
			{
				carry = _addcarryx_u64(carry, interim[i + j + 0], SM2_P.v[j], interim + i + j + 0);
				size_t pos = 1 + i + j;
				while (carry && pos < LEN)
				{
					carry = _addcarryx_u64(carry, interim[pos], 0, interim + pos);
					++pos;
				}
			}
		}

		carry = 0;
		h = factor - 1;
		l = ~factor + 1;
		carry = _addcarryx_u64(carry, interim[i + 0], l, interim + i + 0);
		carry = _addcarryx_u64(carry, interim[i + 1], h, interim + i + 1);
		pos = 2 + i;
		while (carry && pos < LEN)
		{
			carry = _addcarryx_u64(carry, interim[pos], 0, interim + pos);
			++pos;
		}

		// SM2_P[0] * factor == SM2_P[2] * factor
		carry = 0;
		carry = _addcarryx_u64(carry, interim[i + 2], l, interim + i + 2);
		carry = _addcarryx_u64(carry, interim[i + 3], h, interim + i + 3);
		pos = 4 + i;
		while (carry && pos < LEN)
		{
			carry = _addcarryx_u64(carry, interim[pos], 0, interim + pos);
			++pos;
		}

		carry = 0;
		l = _mulx_u64(factor, SM2_P.v[1], &h);
		carry = _addcarryx_u64(carry, interim[i + 1], l, interim + i + 1);
		carry = _addcarryx_u64(carry, interim[i + 2], h, interim + i + 2);
		pos = 3 + i;
		while (carry && pos < LEN)
		{
			carry = _addcarryx_u64(carry, interim[pos], 0, interim + pos);
			++pos;
		}

		carry = 0;
		l = _mulx_u64(factor, SM2_P.v[3], &h);
		carry = _addcarryx_u64(carry, interim[i + 3], l, interim + i + 3);
		carry = _addcarryx_u64(carry, interim[i + 4], h, interim + i + 4);
		pos = 5 + i;
		while (carry && pos < LEN)
		{
			carry = _addcarryx_u64(carry, interim[pos], 0, interim + pos);
			++pos;
		}
	}

	memcpy(result.v, interim + 4, 32);

	if (interim[8] != 0)
		u32_add(result, SM2_rhoP, result);

	if (u32_gte(result, SM2_P))
		u32_add(result, SM2_rhoP, result);
}

void sm2p_mong_pow(const u32 & x, u32 & result)
{
	u8 interim[9] = { 0 };
	raw_pow(x, interim);

	sm2p_mong_mul_core(interim, result);
}

void sm2p_mong_mul(const u32 & x, const u32 & y, u32 & result)
{
	u8 interim[9] = { 0 };
	raw_mul(x, y, interim);

	sm2p_mong_mul_core(interim, result);
}


//total 256 bits
u1 u32_get_bit(const u32 & input, size_t pos)
{
	if (pos > 255)
	{
		puts("pos error at get_bit!");
		return pos & 0xff;
	}
	return ((input.v[pos / 64] >> (pos % 64)) & 1);
}

//total 32 bytes
u1 u32_get_byte(const u32 & input, size_t pos)
{
	if (pos > 32)
	{
		puts("pos error at get_byte!");
		return pos % 32;
	}

	return (u1)(input.v[pos / 8] >> ((pos % 8) * 8));
}
