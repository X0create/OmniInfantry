#ifndef FAST_MATH_H
#define FAST_MATH_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============ ���ú� ============ */
#define FM_PRECISE_MODE     0  /* 0: ����ģʽ, 1: �߾���ģʽ */

/* ============ �������� ============ */
typedef union {
    float f;
    uint32_t u;
} fm_float_t;

/* ��ѧ���� */
#define FM_PI           3.14159265358979323846f
#define FM_2PI          6.28318530717958647692f
#define FM_PI_2         1.57079632679489661923f
#define FM_PI_4         0.78539816339744830962f
#define FM_1_PI         0.31830988618379067154f
#define FM_2_PI         0.63661977236758134308f
#define FM_E            2.71828182845904523536f
#define FM_SQRT2        1.41421356237309504880f
#define FM_SQRT1_2      0.70710678118654752440f
#define FM_INFINITY     HUGE_VALF
#define FM_NAN          NAN

/* ============ ���ٻ������� ============ */
/* ����ƽ���������ƣ�����Լ0.1%�� */
float fm_sqrt_fast(float x);

/* ����ƽ�������߾��ȣ� */
float fm_sqrt(float x);

/* ���ٵ���ƽ������Quake III�㷨�Ľ��棩 */
float fm_rsqrt(float x);

/* ���ٵ��� */
float fm_rcp(float x);

/* ���پ���ֵ�������֧�� */
float fm_abs(float x);
int32_t fm_abs_i32(int32_t x);
int64_t fm_abs_i64(int64_t x);

/* �������ֵ/��Сֵ */
float fm_max(float a, float b);
float fm_min(float a, float b);
int32_t fm_max_i32(int32_t a, int32_t b);
int32_t fm_min_i32(int32_t a, int32_t b);

/* ���ټн� */
float fm_clamp(float x, float min_val, float max_val);
float fm_clamp01(float x);

/* �������Բ�ֵ */
float fm_lerp(float a, float b, float t);

/* ���ٷ��ź��� */
float fm_sign(float x);

/* ============ �������Ǻ��� ============ */
/* �Ƕ�/����ת�� */
float fm_deg2rad(float deg);
float fm_rad2deg(float rad);

/* ��������/���ң�ʹ�ö���ʽ���ƣ� */
float fm_sin_fast(float x);
float fm_cos_fast(float x);
void fm_sincos_fast(float x, float* sin_val, float* cos_val);

/* �������� */
float fm_tan_fast(float x);

/* ���ٷ�����/�����ң�����������[-1,1]�� */
float fm_asin_fast(float x);
float fm_acos_fast(float x);

/* ���ٷ����� */
float fm_atan_fast(float x);
float fm_atan2_fast(float y, float x);

/* ============ ָ���Ͷ������� ============ */
/* ����ָ������������exp2�Ľ��ƣ� */
float fm_exp_fast(float x);
float fm_exp2_fast(float x);
float fm_exp10_fast(float x);

/* ������Ȼ���� */
float fm_log_fast(float x);
float fm_log2_fast(float x);
float fm_log10_fast(float x);

/* �����ݺ��� */
float fm_pow_fast(float x, float y);

/* ���������ݣ�x^n��nΪ������ */
float fm_pow_int(float x, int n);

/* ============ ��ֵ���� ============ */
/* ƽ����ֵ */
float fm_smoothstep(float edge0, float edge1, float x);
float fm_smootherstep(float edge0, float edge1, float x);

/* ���������� */
float fm_bezier_quadratic(float p0, float p1, float p2, float t);
float fm_bezier_cubic(float p0, float p1, float p2, float p3, float t);

/* ============ α��������� ============ */
/* �����������������Xorshift�㷨�� */
typedef struct {
    uint32_t state[4];
} fm_rng_t;

void fm_rng_init(fm_rng_t* rng, uint32_t seed);
uint32_t fm_rng_u32(fm_rng_t* rng);
float fm_rng_float(fm_rng_t* rng);          /* [0, 1) */
float fm_rng_float_range(fm_rng_t* rng, float min, float max);
int32_t fm_rng_int_range(fm_rng_t* rng, int32_t min, int32_t max);

/* ============ ������ѧ���� ============ */
/* ٤��У�� */
float fm_gamma_to_linear(float x, float gamma);
float fm_linear_to_gamma(float x, float gamma);

/* sigmoid���� */
float fm_sigmoid(float x);
float fm_sigmoid_fast(float x);

/* ˫������ */
float fm_sinh_fast(float x);
float fm_cosh_fast(float x);
float fm_tanh_fast(float x);

/* �������� */
float fm_erf_fast(float x);

/* �׳ˣ�n!�����20�� */
uint64_t fm_factorial(int n);

/* ����� C(n, k) */
uint64_t fm_combination(int n, int k);

/* ============ ��ֵ���� ============ */
/* �����η��� ax2 + bx + c = 0 */
int fm_solve_quadratic(float a, float b, float c, float* x1, float* x2);

/* ���ٵ���ƽ����������ţ�ٷ��� */
float fm_rsqrt_newton(float x, int iterations);

/* ============ λ������� ============ */
/* �ж��Ƿ�Ϊ2���� */
bool fm_is_power_of_two(uint32_t x);

/* ��һ��2���� */
uint32_t fm_next_power_of_two(uint32_t x);

/* ������������2������ȡ���� */
int32_t fm_log2_int(uint32_t x);

/* ============ �������� ============ */
/* ����ƽ���� */
void fm_sqrt_batch(float* dst, const float* src, size_t count);

/* �������� */
void fm_sin_batch(float* dst, const float* src, size_t count);

/* ����ָ�� */
void fm_exp_batch(float* dst, const float* src, size_t count);

/* ������һ�������������ŵ�[0,1]�� */
void fm_normalize_batch(float* data, size_t count);

/* ����ۼ� */
float fm_dot_product_batch(const float* a, const float* b, size_t count);

/* ������� */
float fm_sum_batch(const float* data, size_t count);

/* ������ƽ��ֵ */
float fm_mean_batch(const float* data, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* FAST_MATH_H */