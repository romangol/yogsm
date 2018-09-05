#ifndef HEADERS_SM2_H
#define HEADERS_SM2_H

#include "utils.h"
#include "sm3.h"

/*
void sm2_get_id_digest		(u1 *id, size_t id_len, const affine_point & public_key, u1 result[32]);
void sm2_get_msg_digest		(u1 *msg, size_t msg_len, u1 za[32], u1 result[32]);
void get_rand_u32_in_mod_n	(u32 & k);
*/

void sm2_get_public_key		(const u32 & d, AFPoint & Pubkey);
void sm2_sign				(u1 * id, size_t id_len, u1 *msg, size_t msg_len, const u32 & da, const AFPoint & public_key, u32 sig[2]);
bool sm2_verify				(u1 *id, size_t id_len, u1 *msg, size_t msg_len, const AFPoint & public_key, u32 & r, u32 & s);

#endif