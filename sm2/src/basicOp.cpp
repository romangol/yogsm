#include "basicOp.h"
#include "utils.h"
#include <immintrin.h>

void u32_neg(u32 & input)
{
	u1 carry = 1;
	forloop(i, 0, 4)
		carry = _addcarryx_u64(carry, ~input.v[i], 0, input.v + i);
}


bool u32_add(const u32 & a, const u32 & b, u32 & result)
{
	u1 carry = 0;

	forloop(i, 0, 4)
		carry = _addcarryx_u64(carry, a.v[i], b.v[i], result.v + i);

	return (carry == 1);
}

void u32_sub(const u32 & a, const u32 & b, u32 & result)
{
	u1 carry = 1;

	forloop(i, 0, 4)
		carry = _addcarryx_u64(carry, a.v[i], ~b.v[i], result.v + i);
}

void u32_shl(u32 & input)
{
	input.v[3] <<= 1;
	input.v[3] |= (input.v[2] >> 63);
	input.v[2] <<= 1;
	input.v[2] |= (input.v[1] >> 63);
	input.v[1] <<= 1;
	input.v[1] |= (input.v[1] >> 63);
	input.v[0] <<= 1;
}

void u32_shr(u32 & input)
{
	input.v[0] >>= 1;
	input.v[0] |= (input.v[1] << 63);
	input.v[1] >>= 1;
	input.v[1] |= (input.v[2] << 63);
	input.v[2] >>= 1;
	input.v[2] |= (input.v[3] << 63);
	input.v[3] >>= 1;
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
	u8 interim[9] = { 0 };

	u1 carry = 0;
	forloop(i, 0, 4)
	{
		forloop(j, 0, 4)
		{
			u8 h;
			u8 l = _mulx_u64(x.v[i], y.v[j], &h);
			carry = _addcarryx_u64(carry, interim[i + j + 0], l, interim + i + j + 0);
			interim[i + j + 2] += _addcarryx_u64(carry, interim[i + j + 1], h, interim + i + j + 1);
			carry = 0;
		}
	}

	memcpy(result, interim, 64);
}
