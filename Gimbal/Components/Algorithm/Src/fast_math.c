#include "fast_math.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#if defined(_MSC_VER)
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

/* ============ 内部辅助函数 ============ */
static inline uint32_t fm_float_to_bits(float f) {
    fm_float_t u = {.f = f};
    return u.u;
}

static inline float fm_bits_to_float(uint32_t u) {
    fm_float_t f = {.u = u};
    return f.f;
}

/* ============ 快速基础运算 ============ */
float fm_sqrt_fast(float x) {
    if (x <= 0.0f) return 0.0f;
    
    /* 使用改进的Quake III算法 */
    fm_float_t u;
    u.f = x;
    u.u = 0x5f375a86 - (u.u >> 1);  /* 比原始常量更精确 */
    
    /* 一次牛顿迭代 */
    float xhalf = 0.5f * x;
    u.f = u.f * (1.5f - xhalf * u.f * u.f);
    
    /* 根据精度需求决定是否进行第二次迭代 */
#if FM_PRECISE_MODE
    u.f = u.f * (1.5f - xhalf * u.f * u.f);
#endif
    
    return x * u.f;
}

float fm_sqrt(float x) {
#if FM_PRECISE_MODE
    return sqrtf(x);
#else
    return fm_sqrt_fast(x);
#endif
}

float fm_rsqrt(float x) {
    if (x <= 0.0f) return FM_INFINITY;
    
    fm_float_t u;
    u.f = x;
    u.u = 0x5f375a86 - (u.u >> 1);
    
    float xhalf = 0.5f * x;
    u.f = u.f * (1.5f - xhalf * u.f * u.f);
    
#if FM_PRECISE_MODE
    u.f = u.f * (1.5f - xhalf * u.f * u.f);
#endif
    
    return u.f;
}

float fm_rcp(float x) {
    if (x == 0.0f) return FM_INFINITY;
    
    fm_float_t u;
    u.f = x;
    u.u = 0x7ef311c3 - u.u;  /* 近似倒数 */
    u.f = u.f * (2.0f - x * u.f);
    return u.f;
}

float fm_abs(float x) {
    fm_float_t u;
    u.f = x;
    u.u &= 0x7FFFFFFF;  /* 清除符号位 */
    return u.f;
}

int32_t fm_abs_i32(int32_t x) {
    int32_t mask = x >> 31;
    return (x + mask) ^ mask;
}

int64_t fm_abs_i64(int64_t x) {
    int64_t mask = x >> 63;
    return (x + mask) ^ mask;
}

float fm_max(float a, float b) {
    return a > b ? a : b;
}

float fm_min(float a, float b) {
    return a < b ? a : b;
}

int32_t fm_max_i32(int32_t a, int32_t b) {
    return a > b ? a : b;
}

int32_t fm_min_i32(int32_t a, int32_t b) {
    return a < b ? a : b;
}

float fm_clamp(float x, float min_val, float max_val) {
    if (x < min_val) return min_val;
    if (x > max_val) return max_val;
    return x;
}

float fm_clamp01(float x) {
    if (x < 0.0f) return 0.0f;
    if (x > 1.0f) return 1.0f;
    return x;
}

float fm_lerp(float a, float b, float t) {
    return a + (b - a) * fm_clamp01(t);
}

float fm_sign(float x) {
    if (x > 0.0f) return 1.0f;
    if (x < 0.0f) return -1.0f;
    return 0.0f;
}

/* ============ 快速三角函数 ============ */
static inline float fm_mod_2pi(float x) {
    /* 快速模2π运算 */
    const float inv_2pi = 0.15915494309189535f;  /* 1/(2π) */
    x -= (float)((int)(x * inv_2pi)) * FM_2PI;
    return x;
}

float fm_sin_fast(float x) {
    /* 使用多项式近似，在[-π, π]范围内误差小于0.001 */
    x = fm_mod_2pi(x);
    
    /* 使用更精确的7阶多项式近似 */
    const float B = 1.27323954f;      /* 4/π */
    const float C = -0.405284735f;    /* -4/(π2) */
    
    float y = B * x + C * x * fm_abs(x);
    
    /* 使用有理函数进一步优化 */
    const float P = 0.225f;
    y = P * (y * fm_abs(y) - y) + y;
    
    return y;
}

float fm_cos_fast(float x) {
    return fm_sin_fast(x + FM_PI_2);
}

void fm_sincos_fast(float x, float* sin_val, float* cos_val) {
    float s = fm_sin_fast(x);
    *sin_val = s;
    *cos_val = fm_sqrt_fast(1.0f - s * s);
    
    /* 根据象限修正符号 */
    float quadrant = fm_mod_2pi(x) / FM_PI_2;
    int q = (int)quadrant;
    
    if (q == 1 || q == 2) {
        *cos_val = -(*cos_val);
    }
    if (q == 2 || q == 3) {
        *sin_val = -(*sin_val);
    }
}

float fm_tan_fast(float x) {
    float s, c;
    fm_sincos_fast(x, &s, &c);
    return s / c;
}

float fm_asin_fast(float x) {
    /* 限制输入范围 */
    x = fm_clamp(x, -1.0f, 1.0f);
    
    /* 使用多项式近似 */
    float absx = fm_abs(x);
    float y = FM_PI_2 - fm_sqrt_fast(1.0f - absx) * 
              (1.5707288f - 0.2121144f * absx + 0.0742610f * absx * absx);
    
    return fm_sign(x) * y;
}

float fm_acos_fast(float x) {
    return FM_PI_2 - fm_asin_fast(x);
}

float fm_atan_fast(float x) {
    /* 使用有理函数近似 */
    float absx = fm_abs(x);
    float y;
    
    if (absx < 1.0f) {
        /* 小角度近似 */
        y = x / (1.0f + 0.28f * x * x);
    } else {
        /* 大角度使用恒等式 atan(x) = π/2 - atan(1/x) */
        y = FM_PI_2 - x / (x * x + 0.28f);
        if (x < 0.0f) y = -y;
    }
    
    return y;
}

float fm_atan2_fast(float y, float x) {
    if (x == 0.0f) {
        if (y > 0.0f) return FM_PI_2;
        if (y < 0.0f) return -FM_PI_2;
        return 0.0f;
    }
    
    float atan = fm_atan_fast(y / x);
    
    if (x < 0.0f) {
        if (y >= 0.0f) atan += FM_PI;
        else atan -= FM_PI;
    }
    
    return atan;
}

float fm_deg2rad(float deg) {
    return deg * (FM_PI / 180.0f);
}

float fm_rad2deg(float rad) {
    return rad * (180.0f / FM_PI);
}

/* ============ 快速指数和对数 ============ */
float fm_exp_fast(float x) {
    /* 限制输入范围防止溢出 */
    if (x > 88.0f) return FM_INFINITY;
    if (x < -88.0f) return 0.0f;
    
    const float log2_e = 1.4426950408889634f;
    return fm_exp2_fast(x * log2_e);
}

float fm_exp2_fast(float x) {
    /* 快速计算2^x */
    if (x >= 128.0f) return FM_INFINITY;
    if (x <= -126.0f) return 0.0f;
    
    /* 分离整数和小数部分 */
    int32_t integer = (int32_t)x;
    float fraction = x - integer;
    
    /* 计算2^fraction的多项式近似 */
    float poly = 1.0f + fraction * (0.695556856f + 
               fraction * (0.226173572f + 
               fraction * 0.0781455737f));
    
    /* 通过调整浮点数指数位实现2^integer */
    fm_float_t result;
    result.u = ((integer + 127) << 23) | ((uint32_t)((poly - 1.0f) * 8388608.0f) & 0x7FFFFF);
    
    return result.f;
}

float fm_exp10_fast(float x) {
    const float log2_10 = 3.321928094887362f;
    return fm_exp2_fast(x * log2_10);
}

float fm_log_fast(float x) {
    if (x <= 0.0f) return -FM_INFINITY;
    
    const float inv_log2_e = 0.6931471805599453f;
    return fm_log2_fast(x) * inv_log2_e;
}

float fm_log2_fast(float x) {
    if (x <= 0.0f) return -FM_INFINITY;
    
    fm_float_t u;
    u.f = x;
    
    /* 提取指数和尾数 */
    int32_t exponent = ((u.u >> 23) & 0xFF) - 127;
    u.u = (u.u & 0x7FFFFF) | 0x3F800000;  /* 将指数设为0，尾数在[1,2) */
    
    /* 尾数在[1,2)范围内，使用多项式近似log2 */
    float mantissa = u.f - 1.0f;
    float poly = mantissa * (1.0f - mantissa * (0.5f - mantissa * (1.0f/3.0f)));
    
    return (float)exponent + poly / 0.6931471805599453f;
}

float fm_log10_fast(float x) {
    const float inv_log2_10 = 0.3010299956639812f;  /* log10(2) */
    return fm_log2_fast(x) * inv_log2_10;
}

float fm_pow_fast(float x, float y) {
    if (x <= 0.0f) {
        if (x == 0.0f) return 0.0f;
        /* 处理负数底数 */
        return NAN;
    }
    
    return fm_exp2_fast(y * fm_log2_fast(x));
}

float fm_pow_int(float x, int n) {
    /* 快速整数幂运算 */
    if (n == 0) return 1.0f;
    if (n == 1) return x;
    if (x == 0.0f) return 0.0f;
    
    bool negative = n < 0;
    unsigned int exp = negative ? -n : n;
    
    float result = 1.0f;
    float base = x;
    
    while (exp > 0) {
        if (exp & 1) {
            result *= base;
        }
        base *= base;
        exp >>= 1;
    }
    
    return negative ? 1.0f / result : result;
}

/* ============ 插值函数 ============ */
float fm_smoothstep(float edge0, float edge1, float x) {
    x = fm_clamp01((x - edge0) / (edge1 - edge0));
    return x * x * (3.0f - 2.0f * x);
}

float fm_smootherstep(float edge0, float edge1, float x) {
    x = fm_clamp01((x - edge0) / (edge1 - edge0));
    return x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
}

float fm_bezier_quadratic(float p0, float p1, float p2, float t) {
    float one_minus_t = 1.0f - t;
    return one_minus_t * one_minus_t * p0 + 
           2.0f * one_minus_t * t * p1 + 
           t * t * p2;
}

float fm_bezier_cubic(float p0, float p1, float p2, float p3, float t) {
    float one_minus_t = 1.0f - t;
    float one_minus_t2 = one_minus_t * one_minus_t;
    float t2 = t * t;
    
    return one_minus_t2 * one_minus_t * p0 + 
           3.0f * one_minus_t2 * t * p1 + 
           3.0f * one_minus_t * t2 * p2 + 
           t2 * t * p3;
}

/* ============ 随机数生成器 ============ */
void fm_rng_init(fm_rng_t* rng, uint32_t seed) {
    rng->state[0] = seed + 0x6C078965U;
    rng->state[1] = seed ^ 0x19660D43U;
    rng->state[2] = seed + 0x5D588B65U;
    rng->state[3] = seed ^ 0x76E5A0EBU;
    
    /* 预热 */
    for (int i = 0; i < 20; ++i) {
        fm_rng_u32(rng);
    }
}

uint32_t fm_rng_u32(fm_rng_t* rng) {
    uint32_t t = rng->state[3];
    uint32_t s = rng->state[0];
    
    rng->state[3] = rng->state[2];
    rng->state[2] = rng->state[1];
    rng->state[1] = s;
    
    t ^= t >> 2;
    t ^= t << 1;
    t ^= s ^ (s << 4);
    rng->state[0] = t;
    
    return t + rng->state[1];
}

float fm_rng_float(fm_rng_t* rng) {
    /* 生成[0,1)范围内的浮点数 */
    uint32_t u = fm_rng_u32(rng);
    fm_float_t f;
    f.u = (u >> 9) | 0x3F800000;  /* 生成[1,2)范围内的浮点数 */
    return f.f - 1.0f;
}

float fm_rng_float_range(fm_rng_t* rng, float min, float max) {
    return min + fm_rng_float(rng) * (max - min);
}

int32_t fm_rng_int_range(fm_rng_t* rng, int32_t min, int32_t max) {
    uint32_t range = max - min + 1;
    return min + (fm_rng_u32(rng) % range);
}

/* ============ 特殊数学函数 ============ */
float fm_gamma_to_linear(float x, float gamma) {
    if (gamma == 1.0f) return x;
    if (gamma == 2.0f) return x * x;
    return fm_pow_fast(x, gamma);
}

float fm_linear_to_gamma(float x, float gamma) {
    if (gamma == 1.0f) return x;
    if (gamma == 2.0f) return fm_sqrt_fast(x);
    return fm_pow_fast(x, 1.0f / gamma);
}

float fm_sigmoid_fast(float x) {
    /* 快速sigmoid近似 */
    return 0.5f + 0.5f * x / (1.0f + fm_abs(x));
}

float fm_sigmoid(float x) {
#if FM_PRECISE_MODE
    return 1.0f / (1.0f + fm_exp_fast(-x));
#else
    return fm_sigmoid_fast(x);
#endif
}

float fm_sinh_fast(float x) {
    float ex = fm_exp_fast(x);
    return 0.5f * (ex - 1.0f / ex);
}

float fm_cosh_fast(float x) {
    float ex = fm_exp_fast(x);
    return 0.5f * (ex + 1.0f / ex);
}

float fm_tanh_fast(float x) {
    if (x > 4.0f) return 1.0f;
    if (x < -4.0f) return -1.0f;
    
    float x2 = x * x;
    return x * (1.0f + x2 * (0.125f + 0.004f * x2));
}

float fm_erf_fast(float x) {
    /* 误差函数近似，最大误差约0.0001 */
    float a1 =  0.254829592f;
    float a2 = -0.284496736f;
    float a3 =  1.421413741f;
    float a4 = -1.453152027f;
    float a5 =  1.061405429f;
    float p  =  0.3275911f;
    
    int sign = (x < 0) ? -1 : 1;
    x = fm_abs(x);
    
    float t = 1.0f / (1.0f + p * x);
    float y = 1.0f - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * fm_exp_fast(-x * x);
    
    return sign * y;
}

uint64_t fm_factorial(int n) {
    static const uint64_t factorials[] = {
        1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800,
        39916800, 479001600, 6227020800, 87178291200, 1307674368000,
        20922789888000, 355687428096000, 6402373705728000,
        121645100408832000, 2432902008176640000
    };
    
    if (n < 0 || n > 20) return 0;  /* 20!是64位无符号整数的上限 */
    return factorials[n];
}

uint64_t fm_combination(int n, int k) {
    if (k < 0 || k > n) return 0;
    if (k == 0 || k == n) return 1;
    
    /* 使用对称性减少计算量 */
    if (k > n / 2) k = n - k;
    
    uint64_t result = 1;
    for (int i = 1; i <= k; i++) {
        result = result * (n - k + i) / i;
    }
    
    return result;
}

/* ============ 数值计算 ============ */
int fm_solve_quadratic(float a, float b, float c, float* x1, float* x2) {
    if (a == 0.0f) {
        if (b == 0.0f) return 0;
        *x1 = -c / b;
        return 1;
    }
    
    float discriminant = b * b - 4.0f * a * c;
    
    if (discriminant < 0.0f) {
        return 0;
    }
    
    if (discriminant == 0.0f) {
        *x1 = -b / (2.0f * a);
        return 1;
    }
    
    float sqrt_disc = fm_sqrt_fast(discriminant);
    float q = -0.5f * (b + fm_sign(b) * sqrt_disc);
    
    *x1 = q / a;
    *x2 = c / q;
    
    if (*x1 > *x2) {
        float temp = *x1;
        *x1 = *x2;
        *x2 = temp;
    }
    
    return 2;
}

float fm_rsqrt_newton(float x, int iterations) {
    if (x <= 0.0f) return FM_INFINITY;
    
    fm_float_t u;
    u.f = x;
    u.u = 0x5f375a86 - (u.u >> 1);
    
    float xhalf = 0.5f * x;
    for (int i = 0; i < iterations; i++) {
        u.f = u.f * (1.5f - xhalf * u.f * u.f);
    }
    
    return u.f;
}

/* ============ 位操作相关 ============ */
bool fm_is_power_of_two(uint32_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}

uint32_t fm_next_power_of_two(uint32_t x) {
    if (x == 0) return 1;
    
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

int32_t fm_log2_int(uint32_t x) {
    if (x == 0) return -1;
    
    int32_t result = 0;
    while (x >>= 1) {
        result++;
    }
    return result;
}

/* ============ 批量运算 ============ */
void fm_sqrt_batch(float* dst, const float* src, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        dst[i] = fm_sqrt_fast(src[i]);
    }
}

void fm_sin_batch(float* dst, const float* src, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        dst[i] = fm_sin_fast(src[i]);
    }
}

void fm_exp_batch(float* dst, const float* src, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        dst[i] = fm_exp_fast(src[i]);
    }
}

void fm_normalize_batch(float* data, size_t count) {
    if (count == 0) return;
    
    /* 找到最小值和最大值 */
    float min_val = data[0];
    float max_val = data[0];
    
    for (size_t i = 1; i < count; ++i) {
        if (data[i] < min_val) min_val = data[i];
        if (data[i] > max_val) max_val = data[i];
    }
    
    /* 归一化到[0,1] */
    float range = max_val - min_val;
    if (range == 0.0f) {
        /* 所有值相同，设为0.5 */
        for (size_t i = 0; i < count; ++i) {
            data[i] = 0.5f;
        }
    } else {
        float inv_range = 1.0f / range;
        for (size_t i = 0; i < count; ++i) {
            data[i] = (data[i] - min_val) * inv_range;
        }
    }
}

float fm_dot_product_batch(const float* a, const float* b, size_t count) {
    float sum = 0.0f;
    for (size_t i = 0; i < count; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

float fm_sum_batch(const float* data, size_t count) {
    float sum = 0.0f;
    for (size_t i = 0; i < count; ++i) {
        sum += data[i];
    }
    return sum;
}

float fm_mean_batch(const float* data, size_t count) {
    if (count == 0) return 0.0f;
    return fm_sum_batch(data, count) / (float)count;
}

