#include "sm2.h"
#include <time.h>
#define CLK_TCK CLOCKS_PER_SEC
#include "utils.h"
#include "const256.h"

/*
extern const u32 p;
extern const u32 rhoP;
extern const affine_point G;
extern const jacobian_point zero_jacobian;
*/

u4 lrand() 
{
	u4 r = 0;

	for (int i = 0; i < 3; ++i)
	{
		r = (r << 15) | (rand() & 0x7FFF);
	}

	return r & 0xFFFFFFFFUL;
}

void test_raw_mul()
{
	u32 a = {0x1351534EF350E2BB, 0x14E68D77BC131F7B, 0x6A7171A01A638E75, 0x4F9EA7A816AB7908};
	u32 b = {0x141CC66D0595B6F0, 0xC85BF76622E07301, 0x5B261629F8AD4D45, 0x7DE9CF63BC635636};
	u8 rst[8];
	raw_mul(a, b, rst);
	u8 realrst[8] = { 0x866d99203adc8150, 0xc623d9758ed1332c, 0x3b1dab20b950e375, 0xbc165cad5d713996,
		0x63e9be904aa539b5, 0x7edc6525c6a1f17c, 0x2a99a65d2ec61248, 0x27292fc3f99184ca };
	bool flag = 0;
	for (size_t i = 0; i < 8; i++){
		if (rst[i] != realrst[i]){
			printf("raw mul error!\n");
			flag = 1;
		}
	}
	if (flag == 0){
		printf("raw mul successful!\n");
	}
}

void bench_raw_mul()
{
	u32 a, b;
	u8 rst[8];
	u32_rand(a);
	u32_rand(b);

	clock_t start, end;
	start = clock();
	raw_mul(a, b, rst);
	end = clock();
	printf("bench_raw_mul time=%f s\n", (double)(end - start) / CLK_TCK);
}

void test_multiply()
{
	bool f1, f2;
	u32 a, b;
	u8 ra = (u8)lrand();
	u8 rb = (u8)lrand();

	u32 ra1 = {ra, 0, 0, 0};
	u32 rb1 = {rb, 0, 0, 0};
	
	f1 = u32_add(p, ra1, a);
	f2 = u32_add(p, rb1, b);
	if (f1)
	{
		printf("OVER A\n");
		u32_add(a, rhoP, a);
	}
	if (f2)
	{
		printf("OVER B\n");
		u32_add(b, rhoP, b);
	}
	u32 c;
	mul_mod_p(a, b, c);
	u32 rc = {ra*rb, 0, 0, 0};

	if (!u32_eq(c, rc))
		printf("mul error!\n");
	else
		printf("mul successful!\n");
	
}

void bench_mul()
{
	u32 a, b, c;
	u32_rand(a);
	u32_rand(b);

	clock_t start, end;
	start = clock();
	mul_mod_p(a, b, c);
	end = clock();
	printf("bench_mul time=%f s\n", (double)(end - start) / CLK_TCK);
}

void test_inversion()
{
	u32 a, b, res;
	u32_rand(a);
	inv_for_mul_mod_p(a, b);
	u32 newn = {1, 0, 0, 0};
	mul_mod_p(a, b, res);
	if (!u32_eq(res, newn))
	{
		printf("inversion error!\n");
	}
	else{
		printf("inversion successful!\n");
	}
}

void bench_inversion()
{
	u32 a, b;
	u32_rand(a);
	clock_t start, end;
	start = clock();
	inv_for_mul_mod_p(a, b);
	end = clock();
	printf("bench_inversion time=%f s\n", (double)(end - start) / CLK_TCK);
}

void test_add_Jacob_affine()
{
	jacobian_point L, G2, S1, S2;
	affine_to_jacobian(G, L);
	add_jacobian_point(L, L, G2);
	add_jacobian_point(G2, L, S1);
	add_jacobian_point_and_affine_point(G2, G, S2);
	if (equal_to_jacobian_point(S1, S2)){
		printf("add_Jacob_affine successful!\n");
	}
	else{
		printf("add_Jacob_affine error!\n");
	}
	
}

void test_zero_add_Jacob_affine()
{
	jacobian_point L, S1, S2;
	affine_to_jacobian(G, L);
	jacobian_point z = zero_jacobian;
	add_jacobian_point(z, L, S1);
	add_jacobian_point_and_affine_point(z, G, S2);
	if (equal_to_jacobian_point(S1, S2)){
		printf("zero_add_Jacob_affine successful!\n");
	}
	else{
		printf("zero_add_Jacob_affine error!\n");
	}
}

void test_times3()
{
	jacobian_point L, G2, S1, S2;

	affine_to_jacobian(G, L);
	add_jacobian_point(L, L, G2);
	add_jacobian_point(G2, L, S1);

	u32 times = {3, 0, 0, 0};
	times_point(G, times, S2);
	if (equal_to_jacobian_point(S1, S2)){
		printf("times3 successful!\n");
	}
	else{
		printf("times3 error!\n");
	}
}


void test_BaseTimes()
{
	u32 r;
	u32_rand(r);
	jacobian_point S1, S2;
	times_point(G, r, S1);
	gen_tables();
	times_basepoint(r, S2);
	if (equal_to_jacobian_point(S1, S2)){
		printf("BaseTimes successful!\n");
	}
	else{
		printf("BaseTimes error!\n");
	}
}

void bench_times()
{
	u32 r;
	jacobian_point S1;
	u32_rand(r);
	clock_t start, end;
	start = clock();
	times_point(G, r, S1);
	end = clock();
	printf("bench_times time=%f s\n", (double)(end - start) / CLK_TCK);
}

void bench_timesBase()
{
	u32 r;
	jacobian_point S1;
	u32_rand(r);
	clock_t start, end;

	start = clock();
	gen_tables();
	times_basepoint(r, S1);
	end = clock();

	printf("bench_timesBase including generating tables time=%lf s\n", (double)(end - start) / CLK_TCK);

	start = clock();
	times_basepoint(r, S1);
	end = clock();
	printf("bench_timesBase not including generating tables time=%lf s\n", (double)(end - start) / CLK_TCK);

}

void test_sm2()
{
	u32 da, sig[2];
	affine_point public_key;
	gen_tables();
	u1 msg[] = "12345671234567";
	u1 id[] = "12345671234567";
	//u32_new(&Da, 1, 1, 1, 1);
	u32_rand(da);
	sm2_get_public_key(da, public_key);

	size_t loop = 8192;
	double tt1=0, tt2=0;

	clock_t t1 = clock();

	for (size_t i = 0; i < loop; i++)
	{
		sm2_sign(id, sizeof(id), msg, sizeof(msg), da, public_key, sig);

		/*
		bool t = sm2_verify(id, sizeof(id), msg, sizeof(msg), public_key, sig[0], sig[1]);

		if (!t)
		{
			puts("error!");
			return;
		}
		*/
	}

	printf("tt1 %lf s\n", (double)(clock() - t1) / CLK_TCK / loop);
}

int main()
{
	/*
	test_raw_mul();
	test_helper_mul();
	test_multiply();
	test_times3();
	
	test_BaseTimes();
	test_inversion();
	
	test_zero_add_Jacob_affine();
	test_add_Jacob_affine();
	*/
	test_sm2();
	system("pause");
	return 0;
}
