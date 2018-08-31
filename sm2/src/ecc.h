#ifndef HEADERS_ECCGROUP_H
#define HEADERS_ECCGROUP_H

#include "fieldOp.h"

struct affine_point
{
	u32 x;
	u32 y;
};

struct jacobian_point
{
	u32 x;
	u32 y;
	u32 z;
};

bool equal_to_o_affine_point				(const affine_point & point);
bool equal_to_affine_point					(const affine_point & point1, const affine_point & point2);
bool equal_to_o_jacobian_point				(const jacobian_point & point);
bool equal_to_jacobian_point				(const jacobian_point & point1, const jacobian_point & point2);
bool is_on_curve							(const affine_point & point);
void affine_to_jacobian						(const affine_point & point, jacobian_point & result);
void jacobian_to_affine						(const jacobian_point & point, affine_point & result);
void get_inversion_affine_point				(const affine_point & point, affine_point & result);
bool is_affine_point_reciprocal				(const affine_point & point1, const affine_point & point2);
void add_affine_point						(const affine_point & point1, const affine_point & point2, affine_point & result);
void get_inversion_jacobian_point			(const jacobian_point & point, jacobian_point & result);
bool is_jacobian_point_reciprocal			(const jacobian_point & point1, const jacobian_point & point2);
void add_jacobian_point_and_affine_point	(const jacobian_point & point1, const affine_point & point2, jacobian_point & result);;
void add_jacobian_point						(const jacobian_point & point1, const jacobian_point & point2, jacobian_point & result);
void times_point							(const affine_point & point, const u32 & times, jacobian_point & result);
void times_basepoint						(const u32 & times, jacobian_point & result);

size_t get_u8_bit							(u8 input, size_t i);
size_t to_index								(const u32 & input, size_t i);
void gen_tables();

#endif