#ifndef FAST_MATH_H
#define FAST_MATH_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============ 配置宏 ============ */
#define FM_PRECISE_MODE     0  /* 0: 快速模式, 1: 高精度模式 */

/* ============ 基础类型 ============ */
typedef union {
    float f;
    uint32_t u;
} fm_float_t;

/* 数学常量 */
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

/* ============ 快速基础运算 ============ */
/* 快速平方根（近似，精度约0.1%） */
float fm_sqrt_fast(float x);

/* 快速平方根（高精度） */
float fm_sqrt(float x);

/* 快速倒数平方根（Quake III算法改进版） */
float fm_rsqrt(float x);

/* 快速倒数 */
float fm_rcp(float x);

/* 快速绝对值（避免分支） */
float fm_abs(float x);
int32_t fm_abs_i32(int32_t x);
int64_t fm_abs_i64(int64_t x);

/* 快速最大值/最小值 */
float fm_max(float a, float b);
float fm_min(float a, float b);
int32_t fm_max_i32(int32_t a, int32_t b);
int32_t fm_min_i32(int32_t a, int32_t b);

/* 快速夹紧 */
float fm_clamp(float x, float min_val, float max_val);
float fm_clamp01(float x);

/* 快速线性插值 */
float fm_lerp(float a, float b, float t);

/* 快速符号函数 */
float fm_sign(float x);

/* ============ 快速三角函数 ============ */
/* 角度/弧度转换 */
float fm_deg2rad(float deg);
float fm_rad2deg(float rad);

/* 快速正弦/余弦（使用多项式近似） */
 float fm_sin_fast(float x);
 float fm_cos_fast(float x);
 void fm_sincos_fast(float x, float* sin_val, float* cos_val);

/* 快速正切 */
float fm_tan_fast(float x);

/* 快速反正弦/反余弦（限制输入在[-1,1]） */
float fm_asin_fast(float x);
float fm_acos_fast(float x);

/* 快速反正切 */
float fm_atan_fast(float x);
float fm_atan2_fast(float y, float x);

/* ============ 指数和对数函数 ============ */
/* 快速指数函数（基于exp2的近似） */
float fm_exp_fast(float x);
float fm_exp2_fast(float x);
float fm_exp10_fast(float x);

/* 快速自然对数 */
float fm_log_fast(float x);
float fm_log2_fast(float x);
float fm_log10_fast(float x);

/* 快速幂函数 */
float fm_pow_fast(float x, float y);

/* 整数快速幂（x^n，n为整数） */
float fm_pow_int(float x, int n);

/* ============ 插值函数 ============ */
/* 平滑插值 */
float fm_smoothstep(float edge0, float edge1, float x);
float fm_smootherstep(float edge0, float edge1, float x);

/* 贝塞尔曲线 */
float fm_bezier_quadratic(float p0, float p1, float p2, float t);
float fm_bezier_cubic(float p0, float p1, float p2, float p3, float t);

/* ============ 伪随机数生成 ============ */
/* 快速随机数生成器（Xorshift算法） */
typedef struct {
    uint32_t state[4];
} fm_rng_t;

void fm_rng_init(fm_rng_t* rng, uint32_t seed);
uint32_t fm_rng_u32(fm_rng_t* rng);
float fm_rng_float(fm_rng_t* rng);          /* [0, 1) */
float fm_rng_float_range(fm_rng_t* rng, float min, float max);
int32_t fm_rng_int_range(fm_rng_t* rng, int32_t min, int32_t max);

/* ============ 特殊数学函数 ============ */
/* 伽马校正 */
float fm_gamma_to_linear(float x, float gamma);
float fm_linear_to_gamma(float x, float gamma);

/* sigmoid函数 */
float fm_sigmoid(float x);
float fm_sigmoid_fast(float x);

/* 双曲函数 */
float fm_sinh_fast(float x);
float fm_cosh_fast(float x);
float fm_tanh_fast(float x);

/* 误差函数近似 */
float fm_erf_fast(float x);

/* 阶乘（n!，最大20） */
uint64_t fm_factorial(int n);

/* 组合数 C(n, k) */
uint64_t fm_combination(int n, int k);

/* ============ 数值计算 ============ */
/* 求解二次方程 ax2 + bx + c = 0 */
int fm_solve_quadratic(float a, float b, float c, float* x1, float* x2);

/* 快速倒数平方根迭代（牛顿法） */
float fm_rsqrt_newton(float x, int iterations);

/* ============ 位操作相关 ============ */
/* 判断是否为2的幂 */
bool fm_is_power_of_two(uint32_t x);

/* 下一个2的幂 */
uint32_t fm_next_power_of_two(uint32_t x);

/* 快速整数对数2（向下取整） */
int32_t fm_log2_int(uint32_t x);


/* ============ 批量运算 ============ */
/* 批量平方根 */
void fm_sqrt_batch(float* dst, const float* src, size_t count);

/* 批量正弦 */
extern void fm_sin_batch(float* dst, const float* src, size_t count);

/* 批量指数 */
void fm_exp_batch(float* dst, const float* src, size_t count);

/* 批量归一化（将数组缩放到[0,1]） */
void fm_normalize_batch(float* data, size_t count);

/* 点积累加 */
float fm_dot_product_batch(const float* a, const float* b, size_t count);

/* 数组求和 */
float fm_sum_batch(const float* data, size_t count);

/* 数组求平均值 */
float fm_mean_batch(const float* data, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* FAST_MATH_H */