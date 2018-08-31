#include "ecc.h"
#include "const256.h"

#define ASSIGN_AFFINE_PONIT(point,x,y) point={{(x),(y)}}
#define ASSIGN_JACOBIAN_POINT(point,x,y,z) point={{(x),(y),(z)}}

static affine_point lowTable[256];
static affine_point highTable[256];

bool equal_to_o_affine_point(const affine_point & point)
{
	return u32_eq_zero(point.x) && u32_eq_zero(point.y);
}

bool equal_to_affine_point(const affine_point & point1, const affine_point & point2)
{
	return u32_eq(point1.x, point2.x) && u32_eq(point1.y, point2.y);
}

bool equal_to_o_jacobian_point(const jacobian_point & point)
{
	return u32_eq_one(point.x) && u32_eq_one(point.y) && u32_eq_zero(point.z);
}

bool equal_to_jacobian_point(const jacobian_point & point1, const jacobian_point & point2)
{
	u32 pz2, pz3, qz2, qz3, u1, u2, s1, s2;
	mul_mod_p(point1.z, point1.z, pz2);
	mul_mod_p(pz2, point1.z, pz3);
	mul_mod_p(point2.z, point2.z, qz2);
	mul_mod_p(qz2, point2.z, qz3);

	mul_mod_p(point1.x, qz2, u1);
	mul_mod_p(point2.x, pz2, u2);
	mul_mod_p(point1.y, qz3, s1);
	mul_mod_p(point2.y, pz3, s2);
	return u32_eq(u1, u2) && u32_eq(s1, s2);
}

bool is_on_curve(const affine_point & point)
{
	// is y^2 = x^3 + ax + b ?
	if (equal_to_o_affine_point(point)){
		return true;
	}
	u32 y2, x3, ax, res;
	mul_mod_p(point.y, point.y, y2);
	mul_mod_p(point.x, point.x, x3);
	mul_mod_p(point.x, x3, x3);
	mul_mod_p(a, point.x, ax);
	add_mod_p(x3, ax, res);
	add_mod_p(res, b, res);
	return u32_eq(y2, res);
}

void affine_to_jacobian(const affine_point & point, jacobian_point & result)
{
	if ( !equal_to_o_affine_point(point) )
	{
		result.x = point.x;
		result.y = point.y;
		result.z = {1, 0, 0, 0}; // ??????
		// ASSIGN_U32(result.z, 1, 0, 0, 0);
	}
	else
	{
		result = zero_jacobian;
	}
}

void jacobian_to_affine(const jacobian_point & point, affine_point & result)
{
	u32 u, u2, u3;
	inverse_for_multiplying_mod_p(point.z, u);
	mul_mod_p(u, u, u2);
	mul_mod_p(u2, u, u3);
	mul_mod_p(point.x, u2, result.x);
	mul_mod_p(point.y, u3, result.y);
}

void get_inversion_affine_point(const affine_point & point, affine_point & result)
{
	result.x = point.x;
	inverse_for_adding_mod_p(point.y, result.y);
}

bool is_affine_point_reciprocal(const affine_point & point1, const affine_point & point2)
{
	u32 inversion_y;
	inverse_for_adding_mod_p(point2.y, inversion_y);
	return u32_eq(point1.x, point2.x) && u32_eq(point1.y, inversion_y);
}

void add_affine_point(const affine_point & point1, const affine_point & point2, affine_point & result)
{
	if (equal_to_o_affine_point(point1) || equal_to_o_affine_point(point2))
	{
		u32_add(point1.x, point2.x, result.x);
		u32_add(point1.y, point2.y, result.y);
	}
	else if (is_affine_point_reciprocal(point1, point2))
	{
		memset(&result, 0, sizeof(affine_point));
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
			add_mod_p(x2, x2, tx2);
			add_mod_p(tx2, x2, tx2);
			// dx = 3x^2+a;
			add_mod_p(tx2, a, dx);
			add_mod_p(point1.y, point2.y, dy);
			inverse_for_multiplying_mod_p(dy, dyi);
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
		mul_mod_p(lambda, lambda, lambda2);
		add_mod_p(point1.x, point2.x, tmp1);
		sub_mod_p(lambda2, tmp1, X);
		sub_mod_p(point1.x, X, tmp1);
		mul_mod_p(lambda, tmp1, tmp1);
		sub_mod_p(tmp1, point1.y, Y);

		result.x = X;
		result.y = Y;
	}
}

void get_inversion_jacobian_point(const jacobian_point & point, jacobian_point & result)
{
	result.x = point.x;
	inverse_for_adding_mod_p(point.y, result.y);
	result.z = point.z;
}

bool is_jacobian_point_reciprocal(const jacobian_point & point1, const jacobian_point & point2)
{
	u32 pz2, pz3, qz2, qz3, u1, u2, s1, s2;
	mul_mod_p(point1.z, point1.z, pz2);
	mul_mod_p(pz2, point1.z, pz3);
	mul_mod_p(point2.z, point2.z, qz2);
	mul_mod_p(qz2, point2.z, qz3);

	mul_mod_p(point1.x, qz2, u1);
	mul_mod_p(point2.x, pz2, u2);
	mul_mod_p(point1.y, qz3, s1);
	mul_mod_p(point2.y, pz3, s2);

	u32 inversion_s1;
	inverse_for_adding_mod_p(s1, inversion_s1);
	return u32_eq(u1, u2) && u32_eq(inversion_s1, s2);
}

// Note: this function should
// ALWAYS be called with different point

void add_jacobian_point_and_affine_point(const jacobian_point & point1, const affine_point & point2, jacobian_point & result)
{
	if (equal_to_o_jacobian_point(point1))
		affine_to_jacobian(point2, result);
	else if (equal_to_o_affine_point(point2))
		result = point1;
	else
	{
		u32 z2, A, B, C, D, C2, C3, X1C2, x, y, z, D2, X1C22, tmp;
		mul_mod_p(point1.z, point1.z, z2);
		mul_mod_p(point2.x, z2, A);
		mul_mod_p(point2.y, z2, B);
		mul_mod_p(B, point1.z, B);
		sub_mod_p(A, point1.x, C);
		sub_mod_p(B, point1.y, D);
		mul_mod_p(C, C, C2);
		mul_mod_p(C2, C, C3);
		mul_mod_p(point1.x, C2, X1C2);
		mul_mod_p(D, D, D2);
		add_mod_p(X1C2, X1C2, X1C22);
		sub_mod_p(D2, X1C22, x);
		sub_mod_p(x, C3, x);

		sub_mod_p(X1C2, x, tmp);
		mul_mod_p(D, tmp, y);
		mul_mod_p(point1.y, C3, tmp);
		sub_mod_p(y, tmp, y);
		mul_mod_p(point1.z, C, z);
		result.x = x;
		result.y = y;
		result.z = z;
	}
}


void negative_jacobian_point(const jacobian_point & point, jacobian_point & result)
{
	result.x = point.x;
	result.y = point.y;
	u32_neg(result.y);
	result.z = point.z;
}

void add_jacobian_point(const jacobian_point & point1, const jacobian_point & point2, jacobian_point & result)
{
	if (equal_to_o_jacobian_point(point1)){
		result = point2;
	}
	else if (equal_to_o_jacobian_point(point2)){
		result = point1;
	}
	else{
		u32 pz2, qz2, pz3, qz3, lambda1, lambda2;
		mul_mod_p(point1.z, point1.z, pz2);
		mul_mod_p(point2.z, point2.z, qz2);
		mul_mod_p(pz2, point1.z, pz3);
		mul_mod_p(qz2, point2.z, qz3);
		mul_mod_p(point1.x, qz2, lambda1);
		mul_mod_p(point2.x, pz2, lambda2);

		if (!u32_eq(lambda1, lambda2)){
			u32 lambda4, lambda5;
			mul_mod_p(point1.y, qz3, lambda4);
			mul_mod_p(point2.y, pz3, lambda5);
			if (!u32_eq(lambda4, lambda5)){
				u32 lambda3, lambda6, lambda7, lambda8, l6l6, l7l3l3, x, lambda9, l9l6, l8l3l3, l8l3l3l3;
				sub_mod_p(lambda1, lambda2, lambda3);
				sub_mod_p(lambda4, lambda5, lambda6);
				add_mod_p(lambda1, lambda2, lambda7);
				add_mod_p(lambda4, lambda5, lambda8);
				mul_mod_p(lambda6, lambda6, l6l6);
				mul_mod_p(lambda3, lambda3, l7l3l3);
				mul_mod_p(lambda7, l7l3l3, l7l3l3);
				sub_mod_p(l6l6, l7l3l3, x);
				add_mod_p(x, x, lambda9);
				sub_mod_p(l7l3l3, lambda9, lambda9);
				mul_mod_p(lambda9, lambda6, l9l6);
				mul_mod_p(lambda3, lambda3, l8l3l3);
				mul_mod_p(lambda8, l8l3l3, l8l3l3);
				mul_mod_p(l8l3l3, lambda3, l8l3l3l3);

				u32 y, z;
				sub_mod_p(l9l6, l8l3l3l3, y);
				mul_mod_p(y, inv2P, y);
				mul_mod_p(point1.z, point2.z, z);
				mul_mod_p(z, lambda3, z);

				result.x = x;
				result.y = y;
				result.z = z;
			}
			else{
				result = zero_jacobian;
			}
		}
		else{
			u32 px2, pz4, px2_2, px2_3, lambda1, py2, py_2, py2_2, py2_4, lambda2, l2_2, py4_4, lambda3, l1l1, x, m1, m2, y, z;
			mul_mod_p(point1.x, point1.x, px2); // px2 = px^2 
			mul_mod_p(pz2, pz2, pz4);	// pz4 = pz^4
			add_mod_p(px2, px2, px2_2); // px2_2 = 2px^2
			add_mod_p(px2_2, px2, px2_3); // px2_3 = 3px^2
			mul_mod_p(a, pz4, lambda1);
			add_mod_p(px2_3, lambda1, lambda1);
			// l1 = 3*px^2+a*pz^4
			mul_mod_p(point1.y, point1.y, py2); // py2 = py^2
			add_mod_p(point1.y, point1.y, py_2); // py_2 = 2*py
			add_mod_p(py2, py2, py2_2); // py2_2 = 2*py^2
			add_mod_p(py2_2, py2_2, py2_4); // py2_4 = 4*py^2
			mul_mod_p(py2_4, point1.x, lambda2); // l2 = 4*px*py^2
			add_mod_p(lambda2, lambda2, l2_2); // l2 = 2*l2
			mul_mod_p(py2_2, py2_2, py4_4); // py4_4 = 4*py^4
			add_mod_p(py4_4, py4_4, lambda3); // l3 = 8^py^4
			mul_mod_p(lambda1, lambda1, l1l1); // l1l1 = l1^2
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
}


// Note: this function return A Jacob Point
void times_point(const affine_point & point, const u32 & times, jacobian_point & result)
{
	jacobian_point T = zero_jacobian;
	int blocki = 0, i = 0;
	u8  tmp=0;
	const u8  l1 = 1;
	//Point_Display(P);
	//u32_Display(times);
	for (blocki = 3; blocki >= 0; blocki--)
	{
		tmp = times.v[blocki];
		for (i = 63; i >= 0; i--)
		{
			add_jacobian_point(T, T, T);

			if (( tmp & (l1 << i)) != 0)
			{	//this place can't use 1 to replace l1
				//printf("%016llX\n",tmp);
				//printf("%016llX\n", l1<<i);
				//printf("%d\n", tmp & (l1 << i));
				add_jacobian_point_and_affine_point(T, point, T);
			}

		}
	}
	result = T;
}

size_t get_u8_bit(u8 input, size_t i)
{
	return (size_t)((input >> i) & 1);
}

size_t to_index(const u32 & input, size_t i)
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

// Speed up using Fixed-base comb
// described in "Software Implementation of the NIST Elliptic
// Curves Over Prime Fields" by M Brown et. al.

//将其写入文件
void gen_tables()
{
	jacobian_point Gj;
	jacobian_point powG[256];
	size_t j = 0;
	jacobian_point T1 = zero_jacobian, T2 = zero_jacobian;
	size_t k = 0;
	affine_to_jacobian(G, Gj);
	powG[0] = Gj;
	for (size_t i = 1; i < 256; i++)
	{
		//printf("gen_tables\n");
		//JacobPoint_Display(powG[i - 1]);
		add_jacobian_point(powG[i - 1], powG[i - 1], powG[i]);
	}

	// find the desired values
	for (size_t i = 0; i < 256; i++)
	{
		j = i;
		T1 = zero_jacobian, T2 = zero_jacobian;
		k = 0;
		while (j != 0){
			if ((j & 1) != 0)
			{
				// T = T + 2^{32p}G
				add_jacobian_point(T1, powG[k << 5], T1);
				add_jacobian_point(T2, powG[(k << 5) + (1 << 4)], T2);
			}
			j >>= 1;
			k += 1;
		}
		jacobian_to_affine(T1, lowTable[i]);
		jacobian_to_affine(T2, highTable[i]);
	}
}

// before using this function, you need use gen_Tables() to genarate tables.
void times_basepoint(const u32 & times, jacobian_point & result)
{
	jacobian_point T = zero_jacobian;
	
	for (int i = 15; i >= 0; i--)
	{
		add_jacobian_point(T, T, T);
		size_t indexLow = to_index(times, i);
		size_t indexHigh = to_index(times, i + 16);
		add_jacobian_point_and_affine_point(T, lowTable[indexLow], T);
		add_jacobian_point_and_affine_point(T, highTable[indexHigh], T);
	}
	result = T;
}