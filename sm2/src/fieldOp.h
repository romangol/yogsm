#ifndef HEADERS_FN_H
#define HEADERS_FN_H

#include "basicOp.h"

void mod						(u32 & input, const u32 & m);

void add_mod					(const u32 & x, const u32 & y, u32 &result, const u32 & m, const u32 & rhoM);
void sub_mod					(const u32 & x, const u32 &y, u32 & result, const u32 & m, const u32 & rhoM);
void double_mod					(const u32 & x, u32 &result, const u32 & m, const u32 & rhoM);
void mul_mod_n					(const u32 & x, const u32 & y, u32 & result);
void mul_mod_p					(const u32 & x, const u32 & y, u32 & result);
void pow_mod_p					(const u32 & x, u32 & result);
void div_mod_p					(const u32 & x, const u32 &y, u32 & result);

void inv_for_add				(const u32 & input, u32 & result, const u32 & rhoM);
void inv_for_mul				(const u32 & input, u32 & result, const u32 & m, const u32 & rhoM);

void montgomery_reduce			(u32 & result, const u32 & m);
void montgomery_mul				(const u32 & x, const u32 & y, u32 & result, const u32 & m);
void solinas_reduce				(u8 input[8], u32 & result);

#define double_mod_n(a, c) double_mod((a), (c), SM2_N, SM2_rhoN)
#define double_mod_p(a, c) double_mod((a), (c), SM2_P, SM2_rhoP)
#define add_mod_n(a, b, c) add_mod((a), (b), (c), SM2_N, SM2_rhoN)
#define add_mod_p(a, b, c) add_mod((a), (b), (c), SM2_P, SM2_rhoP)
#define sub_mod_n(a, b, c) sub_mod((a), (b), (c), SM2_N, SM2_rhoN)
#define sub_mod_p(a, b, c) sub_mod((a), (b), (c), SM2_P, SM2_rhoP)
#define inv_for_mul_mod_p(a, b)	inv_for_mul((a), (b), SM2_P, SM2_rhoP)
#define inv_for_mul_mod_n(a, b)	inv_for_mul((a), (b), SM2_N, SM2_rhoN)



#endif