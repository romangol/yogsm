#ifndef HEADERS_FN_H
#define HEADERS_FN_H

#include "basicOp.h"

void mod						(u32 & input, const u32 & m);

void add_mod_n					(const u32 & x, const u32 & y, u32 &result);
void sub_mod_n					(const u32 & x, const u32 &y, u32 & result);
void mul_mod_n					(const u32 & x, const u32 & y, u32 & result);
void add_mod_p					(const u32 & x, const u32 & y, u32 &result);
void sub_mod_p					(const u32 & x, const u32 & y, u32 & result);
void mul_mod_p					(const u32 & x, const u32 & y, u32 & result);
void div_mod_p					(const u32 & x, const u32 & y, u32 & result);


void inv_for_add				(const u32 & input, u32 & result, const u32 & m);
void inv_for_mul_mod_p			(const u32 & input, u32 & result);
void inv_for_mul_mod_n			(const u32 & input, u32 & result);

void montgomery_reduce			(u32 & result, const u32 & m);
void montgomery_mul				(const u32 & x, const u32 & y, u32 & result, const u32 & m);
void solinas_reduce				(u8 input[8], u32 & result);

#endif