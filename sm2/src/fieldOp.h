#ifndef HEADERS_FN_H
#define HEADERS_FN_H

#include "basicOp.h"

void add_mod_n						(const u32 & x, const u32 & y, u32 &result);
void sub_mod_n						(const u32 & x, const u32 &y, u32 & result);

void mod_n		(u32 & input);
void inverse_for_adding_mod_n		(const u32 & input, u32 & result);
void inverse_for_multiplying_mod_n	(const u32 & input, u32 & result);

void montgomery_reduce				(u32 & result);
void montgomery_mul					(const u32 & x, const u32 & y, u32 & result);
void mul_mod_n						(const u32 & x, const u32 & y, u32 & result);


void add_mod_p						(const u32 & x, const u32 & y, u32 &result);
void sub_mod_p						(const u32 & x, const u32 & y, u32 & result);
void mul_mod_p						(const u32 & x, const u32 & y, u32 & result);
void div_mod_p						(const u32 & x, const u32 & y, u32 & result);

void mod_p		(u32 & input);
void inverse_for_adding_mod_p		(const u32 & input, u32 & result);
void inverse_for_multiplying_mod_p	(const u32 & input, u32 & result);
void raw_multiply					(const u32 & x, const u32 & y, u8 result[8]);
void helper_split_u8_to_u4			(u8 x, u4 result[2]);
void reduce							(u8 input[8], u32 & result);

#endif