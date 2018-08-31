#include "basicOp.h"
#include "utils.h"

//Bits that are 0 become 1, and those that are 1 become 0.
inline void u32_not(u32 & input)
{
	input.v[0] = ~input.v[0];
	input.v[1] = ~input.v[1];
	input.v[2] = ~input.v[2];
	input.v[3] = ~input.v[3];
}

inline void u32_and(const u32 & a, const u32 & b, u32 & result)
{
	result.v[0] = a.v[0] & b.v[0];
	result.v[1] = a.v[1] & b.v[1];
	result.v[2] = a.v[2] & b.v[2];
	result.v[3] = a.v[3] & b.v[3];
}

inline void u32_or(const u32 & a, const u32 & b, u32 & result)
{
	result.v[0] = a.v[0] | b.v[0];
	result.v[1] = a.v[1] | b.v[1];
	result.v[2] = a.v[2] | b.v[2];
	result.v[3] = a.v[3] | b.v[3];
}

inline void u32_xor(const u32 & a, const u32 & b, u32 & result)
{
	result.v[0] = a.v[0] ^ b.v[0];
	result.v[1] = a.v[1] ^ b.v[1];
	result.v[2] = a.v[2] ^ b.v[2];
	result.v[3] = a.v[3] ^ b.v[3];
}

void u32_neg(u32 & input)
{
	u8 one = 1;
	forloop(i, 0, 4)
	{
		input.v[i] = ~input.v[i] + one;
		if (input.v[i] != 0) 
			one = 0;
	}
}

/* 此处直接定义函数来进行OVERFLOWING_ADD操作
* 无符号整数溢出位判断条件为：对于c = a + b，如果c < a || c < b 则已经溢出了
*/
bool u32_add(const u32 & a, const u32 & b, u32 & result)
{
	u8 isOverflow = 0;

	forloop (i, 0, 4)
	{
		u8 r = isOverflow + a.v[i];
		u8 s = r + b.v[i];

		// if (r == 0) isOverflow = isOverflow;
		if (r != 0)
		{
			if (s < r || s < b.v[i])
				isOverflow = 1;
			else
				isOverflow = 0;
		}
	
		result.v[i] = s;
	}

	return (isOverflow == 1);
}

void u32_sub(const u32 & a, const u32 & b, u32 & result)
{
	bool isOverflow = true;

	forloop(i, 0, 4)
	{
		u8 r = isOverflow + (~b.v[i]);
		u8 s = r + a.v[i];
		if ( r != 0 )
		{
			if (s < r || s < a.v[i])
				isOverflow = 1;
			else
				isOverflow = 0;
		}
		result.v[i] = s;
	}	
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
		printf("error pos!\n");
		return 0;
	}
	return ((input.v[pos / 64] >> (pos % 64)) & 1);
}

//total 32 bytes
u1 u32_get_byte(const u32 & input, size_t pos)
{
	if (pos > 32)
	{
		printf("error pos!\n");
		return 0;
	}

	return (u1)(input.v[pos / 8] >> ((pos % 8) * 8));
}

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
	return (a.v[0] == b.v[0]) && (a.v[1] == b.v[1]) && (a.v[2] == b.v[2]) && (a.v[3] == b.v[3]);
}


bool u32_eq_zero(const u32 & a)
{
	return (a.v[0] == 0) && (a.v[1] == 0) && (a.v[2] == 0) && (a.v[3] == 0);
}

bool u32_eq_one(const u32 & a)
{
	return (a.v[0] == 1) && (a.v[1] == 0) && (a.v[2] == 0) && (a.v[3] == 0);
}
