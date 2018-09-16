#include "ecc.h"
#include "sm2_const.h"

#define ASSIGN_AFFINE_PONIT(point,x,y) point={{(x),(y)}}
#define ASSIGN_JACOBIAN_POINT(point,x,y,z) point={{(x),(y),(z)}}

static AFPoint lowTable[256];
static AFPoint highTable[256];


static size_t get_u8_bit(u8 input, size_t i)
{
	return (size_t)((input >> i) & 1);
}

static size_t to_index(const u32 & input, size_t i)
{
	return get_u8_bit(input.v[0], i)
		+ (get_u8_bit(input.v[0], 32 + i) << 1)
		+ (get_u8_bit(input.v[1], i) << 2)
		+ (get_u8_bit(input.v[1], 32 + i) << 3)
		+ (get_u8_bit(input.v[2], i) << 4)
		+ (get_u8_bit(input.v[2], 32 + i) << 5)
		+ (get_u8_bit(input.v[3], i) << 6)
		+ (get_u8_bit(input.v[3], 32 + i) << 7);
}


bool equ_to_AFPoint_one(const AFPoint & point)
{
	return u32_eq_zero(point.x) && u32_eq_zero(point.y);
}

bool equ_to_AFPoint(const AFPoint & point1, const AFPoint & point2)
{
	return u32_eq(point1.x, point2.x) && u32_eq(point1.y, point2.y);
}

bool equ_to_JPoint_one(const JPoint & point)
{
	return u32_eq_one(point.x) && u32_eq_one(point.y) && u32_eq_zero(point.z);
}

bool equ_to_JPoint(const JPoint & point1, const JPoint & point2)
{
	u32 pz, qz, u1, u2;

	pow_mod_p(point1.z, pz);
	pow_mod_p(point2.z, qz);
	mul_mod_p(point1.x, qz, u1);
	mul_mod_p(point2.x, pz, u2);

	if (!u32_eq(u1, u2))
		return false;

	mul_mod_p(pz, point1.z, pz);
	mul_mod_p(qz, point2.z, qz);
	mul_mod_p(point1.y, qz, u1);
	mul_mod_p(point2.y, pz, u2);

	return u32_eq(u1, u2);
}

bool is_on_curve(const AFPoint & point)  	// is y^2 = x^3 + ax + b ?
{
	if (equ_to_AFPoint_one(point))
		return true;

	u32 y2, x3, ax, res;
	pow_mod_p(point.y, y2);
	pow_mod_p(point.x, x3);
	mul_mod_p(point.x, x3, x3);
	mul_mod_p(SM2_a, point.x, ax);
	add_mod_p(x3, ax, res);
	add_mod_p(res, SM2_b, res);

	return u32_eq(y2, res);
}

void affine_to_jacobian(const AFPoint & point, JPoint & result)
{
	if ( !equ_to_AFPoint_one(point) )
	{
		result.x = point.x;
		result.y = point.y;
		result.z = {1, 0, 0, 0};
		return;
	}

	result = JPoint_ZERO;
}

void jacobian_to_affine(const JPoint & point, AFPoint & result)
{
	u32 u, u2, u3;

	inv_for_mul_mod_p(point.z, u);
	pow_mod_p(u, u2);
	mul_mod_p(u, u, u2);
	mul_mod_p(u2, u, u3);

	mul_mod_p(point.x, u2, result.x);
	mul_mod_p(point.y, u3, result.y);
}

#ifdef USE_AFFINE
void get_inversion_AFPoint(const AFPoint & point, AFPoint & result)
{
	result.x = point.x;
	inv_for_add(point.y, result.y, SM2_P);
}

bool is_AFPoint_reciprocal(const AFPoint & point1, const AFPoint & point2)
{
	u32 inversion_y;
	inv_for_add(point2.y, inversion_y, SM2_P);
	return u32_eq(point1.x, point2.x) && u32_eq(point1.y, inversion_y);
}


void add_AFPoint(const AFPoint & point1, const AFPoint & point2, AFPoint & result)
{
	if (equ_to_AFPoint_one(point1) || equ_to_AFPoint_one(point2))
	{
		u32_add(point1.x, point2.x, result.x);
		u32_add(point1.y, point2.y, result.y);
	}
	else if (is_AFPoint_reciprocal(point1, point2))
	{
		memset(&result, 0, sizeof(AFPoint));
	}
	else
	{
		u32 lambda;
		if (u32_eq(point1.x, point2.x))
		{
			u32 x2, tx2, dx, dy, dyi;

			//x2 = x^2
			mul_mod_p(point1.x, point1.x, x2);

			// tx2 = 3x^2
			double_mod_p(x2, tx2);
			add_mod_p(tx2, x2, tx2);

			// dx = 3x^2+a;
			add_mod_p(tx2, SM2_a, dx);
			add_mod_p(point1.y, point2.y, dy);
			inv_for_mul_mod_p(dy, dyi);
			div_mod_p(dx, dy, lambda);
		}
		else
		{
			u32 s1, s2;
			sub_mod_p(point2.y, point1.y, s1);
			sub_mod_p(point2.x, point1.x, s2);
			div_mod_p(s1, s2, lambda);
		}

		u32 lambda2, tmp1, X, Y;
		pow_mod_p(lambda, lambda2);
		add_mod_p(point1.x, point2.x, tmp1);
		sub_mod_p(lambda2, tmp1, X);
		sub_mod_p(point1.x, X, tmp1);
		mul_mod_p(lambda, tmp1, tmp1);
		sub_mod_p(tmp1, point1.y, Y);

		result.x = X;
		result.y = Y;
	}
}
#endif

/*
void get_inversion_JPoint(const JPoint & point, JPoint & result)
{
	result.x = point.x;
	inv_for_add(point.y, result.y, SM2_P);
	result.z = point.z;
}


bool is_JPoint_reciprocal(const JPoint & point1, const JPoint & point2)
{
	u32 z2, z3, u1, u2, s1, s2;
	pow_mod_p(point1.z, z2);
	mul_mod_p(z2, point1.z, z3);
	mul_mod_p(point2.x, z2, u2);
	mul_mod_p(point2.y, z3, s2);

	pow_mod_p(point2.z, z2);
	mul_mod_p(z2, point2.z, z3);
	mul_mod_p(point1.x, z2, u1);
	mul_mod_p(point1.y, z3, s1);

	u32 s1Inv;
	inv_for_add(s1, s1Inv, SM2_P);

	return u32_eq(u1, u2) && u32_eq(s1Inv, s2);
}
*/

// Note: this function should
// ALWAYS be called with different point

void add_JPoint_and_AFPoint(const JPoint & point1, const AFPoint & point2, JPoint & result)
{
	if (equ_to_JPoint_one(point1))
	{
		affine_to_jacobian(point2, result);
		return;
	}
	
	if (equ_to_AFPoint_one(point2))
	{
		result = point1;
		return;
	}

	u32 z2, A, B, C2, C3, X1C2, D2, X1C22, tmp;
	pow_mod_p(point1.z, z2);
	mul_mod_p(point2.x, z2, A);
	mul_mod_p(point2.y, z2, B);
	mul_mod_p(B, point1.z, B);

	sub_mod_p(A, point1.x, A);
	sub_mod_p(B, point1.y, B);

	pow_mod_p(A, C2);
	mul_mod_p(C2, A, C3);
	mul_mod_p(point1.x, C2, X1C2);
	pow_mod_p(B, D2);

	add_mod_p(X1C2, X1C2, X1C22);
	sub_mod_p(D2, X1C22, result.x);
	sub_mod_p(result.x, C3, result.x);
	sub_mod_p(X1C2, result.x, tmp);

	mul_mod_p(B, tmp, B);
	mul_mod_p(point1.y, C3, tmp);
	sub_mod_p(B, tmp, result.y);

	mul_mod_p(point1.z, A, result.z);
}


void negative_JPoint(const JPoint & point, JPoint & result)
{
	result.x = point.x;
	result.y = point.y;
	result.z = point.z;
	u32_neg(result.y);
}

void double_JPoint(const JPoint & pt, JPoint & result)
{
	if (equ_to_JPoint_one(pt))
	{
		result = pt;
		return;
	}

	u32 pz2, px2, pz4, px2_2, px2_3, lambda1, py2, py_2, py2_2, py2_4, lambda2, l2_2, py4_4, lambda3, l1l1, m1, m2;

	pow_mod_p(pt.x, px2); // px2 = px^2 
	pow_mod_p(pt.y, py2); // py2 = py^2
	pow_mod_p(pt.z, pz2);
	pow_mod_p(pz2, pz4);	// pz4 = pz^4

	double_mod_p(px2, px2_2); // px2_2 = 2px^2
	add_mod_p(px2_2, px2, px2_3); // px2_3 = 3px^2
	mul_mod_p(SM2_a, pz4, lambda1);
	add_mod_p(px2_3, lambda1, lambda1);
	// lambda1 = 3*px^2+a*pz^4

	double_mod_p(pt.y, py_2); // py_2 = 2*py
	double_mod_p(py2, py2_2); // py2_2 = 2*py^2
	double_mod_p(py2_2, py2_4); // py2_4 = 4*py^2
	mul_mod_p(py2_4, pt.x, lambda2); // l2 = 4*px*py^2

	double_mod_p(lambda2, l2_2); // l2 = 2*l2
	pow_mod_p(py2_2, py4_4); // py4_4 = 4*py^4
	double_mod_p(py4_4, lambda3); // l3 = 8^py^4

	pow_mod_p(lambda1, l1l1); // l1l1 = l1^2
	sub_mod_p(l1l1, l2_2, result.x); // x3 = l1^2 - 2*l2
	sub_mod_p(lambda2, result.x, m1); // m1 = l2 - x3

	mul_mod_p(lambda1, m1, m2); // m2 = l1*(l2-x3)
	sub_mod_p(m2, lambda3, result.y); // y = l1*(l2-x3)-l3

	mul_mod_p(py_2, pt.z, result.z); // z = 2*py*pz
}


void add_JPoint(const JPoint & point1, const JPoint & point2, JPoint & result)
{
	if (equ_to_JPoint_one(point1))
	{
		result = point2;
		return;
	}
	if (equ_to_JPoint_one(point2))
	{
		result = point1;
		return;
	}

	u32 pz2, qz2, pz3, qz3, lambda1, lambda2;

	pow_mod_p(point1.z, pz2);
	pow_mod_p(point2.z, qz2);
	mul_mod_p(pz2, point1.z, pz3);
	mul_mod_p(qz2, point2.z, qz3);
	mul_mod_p(point1.x, qz2, lambda1);
	mul_mod_p(point2.x, pz2, lambda2);

	if (!u32_eq(lambda1, lambda2))
	{
		u32 lambda4, lambda5;
		mul_mod_p(point1.y, qz3, lambda4);
		mul_mod_p(point2.y, pz3, lambda5);

		if (!u32_eq(lambda4, lambda5))
		{
			u32 lambda3, lambda6, lambda7, lambda8, l6l6, l7l3l3, x, lambda9, l9l6, l8l3l3, l8l3l3l3;
			sub_mod_p(lambda1, lambda2, lambda3);
			sub_mod_p(lambda4, lambda5, lambda6);
			add_mod_p(lambda1, lambda2, lambda7);
			add_mod_p(lambda4, lambda5, lambda8);
			pow_mod_p(lambda6, l6l6);
			pow_mod_p(lambda3, l7l3l3);
			mul_mod_p(lambda7, l7l3l3, l7l3l3);
			sub_mod_p(l6l6, l7l3l3, x);
			add_mod_p(x, x, lambda9);
			sub_mod_p(l7l3l3, lambda9, lambda9);
			mul_mod_p(lambda9, lambda6, l9l6);
			pow_mod_p(lambda3, l8l3l3);
			mul_mod_p(lambda8, l8l3l3, l8l3l3);
			mul_mod_p(l8l3l3, lambda3, l8l3l3l3);

			u32 y, z;
			sub_mod_p(l9l6, l8l3l3l3, y);
			mul_mod_p(y, SM2_inv2P, y);
			mul_mod_p(point1.z, point2.z, z);
			mul_mod_p(z, lambda3, z);

			result.x = x;
			result.y = y;
			result.z = z;
		}
		else
		{
			result = JPoint_ZERO;
		}
	}
	else
	{
		u32 px2, pz4, px2_2, px2_3, lambda1, py2, py_2, py2_2, py2_4, lambda2, l2_2, py4_4, lambda3, l1l1, x, m1, m2, y, z;
		pow_mod_p(point1.x, px2); // px2 = px^2 
		pow_mod_p(pz2, pz4);	// pz4 = pz^4
		add_mod_p(px2, px2, px2_2); // px2_2 = 2px^2
		add_mod_p(px2_2, px2, px2_3); // px2_3 = 3px^2
		mul_mod_p(SM2_a, pz4, lambda1);
		add_mod_p(px2_3, lambda1, lambda1);
		// l1 = 3*px^2+a*pz^4
		pow_mod_p(point1.y, py2); // py2 = py^2
		add_mod_p(point1.y, point1.y, py_2); // py_2 = 2*py
		add_mod_p(py2, py2, py2_2); // py2_2 = 2*py^2
		add_mod_p(py2_2, py2_2, py2_4); // py2_4 = 4*py^2
		mul_mod_p(py2_4, point1.x, lambda2); // l2 = 4*px*py^2
		add_mod_p(lambda2, lambda2, l2_2); // l2 = 2*l2
		pow_mod_p(py2_2, py4_4); // py4_4 = 4*py^4
		add_mod_p(py4_4, py4_4, lambda3); // l3 = 8^py^4
		pow_mod_p(lambda1, l1l1); // l1l1 = l1^2
		sub_mod_p(l1l1, l2_2, x); // x3 = l1^2 - 2*l2
		sub_mod_p(lambda2, x, m1); // m1 = l2 - x3
		mul_mod_p(lambda1, m1, m2); // m2 = l1*(l2-x3)
		sub_mod_p(m2, lambda3, y); // y = l1*(l2-x3)-l3
		mul_mod_p(py_2, point1.z, z); // z = 2*py*pz

		result.x = x;
		result.y = y;
		result.z = z;
	}
}

// Speed up using Fixed-base comb
// described in "Software Implementation of the NIST Elliptic
// Curves Over Prime Fields" by M Brown et. al.

//将其写入文件
void gen_tables()
{
	JPoint Gj;
	JPoint powG[256];
	size_t j = 0;
	JPoint T1 = JPoint_ZERO, T2 = JPoint_ZERO;
	size_t k = 0;
	affine_to_jacobian(SM2_G, Gj);
	powG[0] = Gj;

	forloop (i, 1, 256)
	{
		double_JPoint(powG[i - 1], powG[i]);
	}

	// find the desired values
	forloop(i, 0, 256)
	{
		j = i;
		T1 = JPoint_ZERO, T2 = JPoint_ZERO;
		k = 0;
		while (j != 0)
		{
			if ((j & 1) != 0)
			{
				// T = T + 2^{32p}G
				add_JPoint(T1, powG[k << 5], T1);
				add_JPoint(T2, powG[(k << 5) + (1 << 4)], T2);
			}
			j >>= 1;
			k += 1;
		}
		jacobian_to_affine(T1, lowTable[i]);
		jacobian_to_affine(T2, highTable[i]);
	}
}

// before using this function, you need use gen_Tables() to genarate tables.
void times_basepoint(const u32 & times, JPoint & result)
{
	JPoint T = JPoint_ZERO;
	
	for (int i = 15; i >= 0; i--)
	{
		double_JPoint(T, T);
		add_JPoint_and_AFPoint(T, lowTable[to_index(times, i)], T);
		add_JPoint_and_AFPoint(T, highTable[to_index(times, i + 16)], T);
	}

	result = T;
}


// Note: this function return A Jacob Point
void times_point(const AFPoint & point, const u32 & times, JPoint & result)
{
	JPoint T = JPoint_ZERO; // should be double_JPoint(T, T);
	const u8 l1 = 1;

	for (int blocki = 3; blocki >= 0; blocki--)
	{
		for (int i = 63; i >= 0; i--)
		{
			// add_JPoint(T, T, T);
			double_JPoint(T, T);

			if ((times.v[blocki] & (l1 << i)) != 0) //this place can't use 1 to replace l1
			{
				add_JPoint_and_AFPoint(T, point, T);
			}

		}
	}
	result = T;
}