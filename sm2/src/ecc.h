#ifndef HEADERS_ECCGROUP_H
#define HEADERS_ECCGROUP_H

#include "fieldOp.h"

struct AFPoint
{
	u32 x;
	u32 y;
};

struct JPoint
{
	u32 x;
	u32 y;
	u32 z;
};

const JPoint JPoint_ZERO = { { 1, 0, 0, 0 },{ 1, 0, 0, 0 },{ 0, 0, 0, 0 } };

bool equ_to_AFPoint_one			(const AFPoint & point);
bool equ_to_AFPoint			(const AFPoint & point1, const AFPoint & point2);
bool equ_to_JPoint_one			(const JPoint & point);
bool equ_to_JPoint			(const JPoint & point1, const JPoint & point2);
bool is_on_curve				(const AFPoint & point);
void affine_to_jacobian			(const AFPoint & point, JPoint & result);
void jacobian_to_affine			(const JPoint & point, AFPoint & result);
void get_inversion_AFPoint		(const AFPoint & point, AFPoint & result);
bool is_AFPoint_reciprocal		(const AFPoint & point1, const AFPoint & point2);
void add_AFPoint				(const AFPoint & point1, const AFPoint & point2, AFPoint & result);
void get_inversion_JPoint		(const JPoint & point, JPoint & result);
bool is_JPoint_reciprocal		(const JPoint & point1, const JPoint & point2);
void add_JPoint_and_AFPoint		(const JPoint & point1, const AFPoint & point2, JPoint & result);;
void add_JPoint					(const JPoint & point1, const JPoint & point2, JPoint & result);
void times_point				(const AFPoint & point, const u32 & times, JPoint & result);
void times_basepoint			(const u32 & times, JPoint & result);

void gen_tables();

#endif