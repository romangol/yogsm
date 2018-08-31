#ifndef HEADERS_FN_H
#define HEADERS_FN_H

#include "basicOp.h"

void add_mod_n						(const u32 & x, const u32 & y, u32 &result);
void sub_mod_n						(const u32 & x, const u32 &y, u32 & result);
void mul_mod_n						(const u32 & x, const u32 & y, u32 & result);
void add_mod_p						(const u32 & x, const u32 & y, u32 &result);
void sub_mod_p						(const u32 & x, const u32 & y, u32 & result);
void mul_mod_p						(const u32 & x, const u32 & y, u32 & result);
void div_mod_p						(const u32 & x, const u32 & y, u32 & result);

void mod_n							(u32 & input);
void mod_p							(u32 & input);


void inv_for_add_mod_n				(const u32 & input, u32 & result);
void inv_for_mul_mod_n				(const u32 & input, u32 & result);
void inv_for_add_mod_p				(const u32 & input, u32 & result);
void inv_for_mul_mod_p				(const u32 & input, u32 & result);

void raw_mul						(const u32 & x, const u32 & y, u8 result[8]);
void montgomery_reduce				(u32 & result);
void montgomery_mul					(const u32 & x, const u32 & y, u32 & result);
void reduce							(u8 input[8], u32 & result);

#endif