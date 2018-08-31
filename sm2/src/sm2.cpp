#include "sm2.h"
#include <time.h>

#include "const256.h"

extern const u32 a;
extern const u32 b;
extern const u32 n;
extern const u32 p;
extern const affine_point G;


void sm2_get_public_key(const u32 & d, affine_point & Pubkey)
{
	jacobian_point rst_jacobian;
	times_basepoint(d, rst_jacobian);
	jacobian_to_affine(rst_jacobian, Pubkey);

}

void sm2_get_id_digest(u1 *id,size_t id_len, const affine_point & public_key, u1 result[32])
{
	//u32 little-endian
	u1 total[300];
	u1 charlist[192];
	/* 2-byte id length in bits */
	//ZA=H256(ENTLA ［ id ［ a ［ b ［ xG ［ yG ［ xA ［ yA)
	total[0] = ((id_len * 8) >> 8) % 256;
	total[1] = (id_len * 8) % 256;
	memcpy(total + 2, id, id_len);
	//low bit...high bit, need reverse
	memcpy(charlist, &a, 32);
	str_reverse_in_place(charlist, 32);

	memcpy(charlist + 32, &b, 32);
	str_reverse_in_place(charlist + 32, 32);

	memcpy(charlist + 64, &(G.x), 32);
	str_reverse_in_place(charlist + 64, 32);

	memcpy(charlist + 96, &(G.y), 32);
	str_reverse_in_place(charlist + 96, 32);

	memcpy(charlist + 128, &(public_key.x), 32);
	str_reverse_in_place(charlist + 128, 32);

	memcpy(charlist + 160, &(public_key.y), 32);
	str_reverse_in_place(charlist + 160, 32);

	memcpy(total + 2 + id_len, charlist, 192);
	sm3(total, 2 + id_len + 192, result);
}

/* msg_md(za || msg) */
void sm2_get_message_digest(u1 *msg,size_t msg_len,u1 za[32],u1 result[32])
{
	u1 msg_md[200];
	memcpy(msg_md, za, 32);
	memcpy(msg_md + 32, msg, msg_len);
	sm3(msg_md, 32 + msg_len, result);
}

void get_random_u32_in_mod_n(u32 & k)
{
	u32_rand(k);
	mod_n(k);

}
void sm2_sign(u1 * id, size_t id_len, u1 *msg, size_t msg_len, const u32 & da, const affine_point & public_key, u32 sig[2])
{
	u1 za[32];
	u1 msg_md[32];
	u32 e, s, r, k, tmp, inversion_da_and_1, k_subtract_rda;
	jacobian_point rand_jacobian_point;
	affine_point rand_affine_point;
	if (!is_on_curve(public_key))
	{
		printf("public key not on curve!");
		return;
	}

	sm2_get_id_digest(id, id_len, public_key, za);
	sm2_get_message_digest(msg, msg_len, za, msg_md);
	
	u1_to_u32(msg_md, e);
	mod_n(e);

	memset(&s, 0, sizeof(u32));
	while (u32_eq_zero(s))
	{
		get_random_u32_in_mod_n(k);
		//yU64x4_new(&k, 1, 0, 0, 0);
		times_basepoint(k, rand_jacobian_point);
		jacobian_to_affine(rand_jacobian_point, rand_affine_point);
		add_mod_n(e, rand_affine_point.x, r);
		add_mod_n(r, k, tmp);

		if (u32_eq_zero(r) || u32_eq_zero(tmp))
		{
			continue;
		}

		// Calculate s = (1+da)^-1 * (k-r*da);
		inversion_da_and_1 = {1, 0, 0, 0};
		add_mod_n(da, inversion_da_and_1, inversion_da_and_1);
		inv_for_mul_mod_n(inversion_da_and_1, inversion_da_and_1);

		mul_mod_n(r, da, k_subtract_rda);
		sub_mod_n(k, k_subtract_rda, k_subtract_rda);
		mul_mod_n(inversion_da_and_1, k_subtract_rda, s);

	}
	sig[0] = r;
	sig[1] = s;
}

bool sm2_verify(u1 *id, size_t id_len, u1 *msg, size_t msg_len, const affine_point & public_key, u32 & r, u32 & s)
{
	// verify that Q is indeed on the curve
	// to prevent false curve attack
	u1 za[32];
	u1 e_array[32];
	u32 e, t, R;
	jacobian_point tmp1, point1_jacobian;
	affine_point point1;
	
	if (!is_on_curve(public_key))
	{
		printf("Public key not on curve!\n");
		return false;
	}
	
	if (u32_gte(r, n) || u32_eq_zero(r))
		return false;

	if (u32_gte(s, n) || u32_eq_zero(s))
		return false;
	
	sm2_get_id_digest(id, id_len, public_key, za);
	sm2_get_message_digest(msg, msg_len, za, e_array);
	u1_to_u32(e_array, e);
	mod_n(e);

	add_mod_n(r, s, t);
	if (u32_eq_zero(t))
		return false;

	times_basepoint(s, tmp1);
	times_point(public_key, t, point1_jacobian);
	add_jacobian_point(tmp1, point1_jacobian, point1_jacobian);
	jacobian_to_affine(point1_jacobian, point1);
	mod_n(e);
	mod_n(point1.x);
	add_mod_n(e, point1.x, R);

	return u32_eq(R, r);
}
