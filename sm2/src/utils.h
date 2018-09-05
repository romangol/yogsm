#ifndef _UTILS_H_
#define _UTILS_H_

#include "dataType.h"
#include "ecc.h"

u8   u8_rand				();
void u32_rand				(u32 & input);
void random_fill			(u1 * buffer, size_t len);
void random_fill_non_zero	(u1 * buffer, size_t len);

void print_u32				(const u32 & input);
void print_affine_point		(const AFPoint & point);
void print_jacobian_point	(const JPoint & point);

void u1_to_u32				(u1 input[32], u32 & result);
void u4_to_u32				(u4 input[8], u32 & result);

void str_reverse_in_place	(u1 *str, int len);

u4   rol					(const uint32_t value, const size_t bits);

#endif