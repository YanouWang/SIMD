#include <iostream>
#include <string>
#include <cstring>
#include<immintrin.h>
#include<vector>

using namespace std;

// 定义了Byte，便于使用
typedef unsigned char Byte;
// 定义了32比特
typedef unsigned int bit32;

// MD5的一系列参数。参数是固定的，其实你不需要看懂这些
#define s11 7
#define s12 12
#define s13 17
#define s14 22
#define s21 5
#define s22 9
#define s23 14
#define s24 20
#define s31 4
#define s32 11
#define s33 16
#define s34 23
#define s41 6
#define s42 10
#define s43 15
#define s44 21

/**
 * @Basic MD5 functions.
 *
 * @param there bit32.
 *
 * @return one bit32.
 */
// 定义了一系列MD5中的具体函数
// 这四个计算函数是需要你进行SIMD并行化的
// 可以看到，FGHI四个函数都涉及一系列位运算，在数据上是对齐的，非常容易实现SIMD的并行化

inline __m128i F_neon(__m128i x, __m128i y, __m128i z) {
    __m128i xy = _mm_and_si128(x, y);
    __m128i notx = _mm_andnot_si128(x, z);
    __m128i result = _mm_or_si128(xy, notx);
    return result;
}

inline __m128i G_neon(__m128i x, __m128i y, __m128i z) {
    __m128i xz = _mm_and_si128(x, z);
    __m128i notz = _mm_andnot_si128(z, y);
    __m128i result = _mm_or_si128(xz, notz);
    return result;
}

inline __m128i H_neon(__m128i x, __m128i y, __m128i z) {
    __m128i xor_xy = _mm_xor_si128(x, y);
    __m128i result = _mm_xor_si128(xor_xy, z);
    return result;
}

inline __m128i I_neon(__m128i x, __m128i y, __m128i z) {
    __m128i notz = _mm_or_si128(z, _mm_cmpeq_epi32(z, z));
    __m128i xor_notz = _mm_or_si128(x, notz);
    __m128i result = _mm_xor_si128(y, xor_notz);
    return result;
}

#define ROTATELEFT_neon(a, s) _mm_or_si128(_mm_slli_epi32(a, s), _mm_srli_epi32(a, 32 - s))

#define FF_neon(a, b, c, d, x, s, ac) \
{ \
    __m128i temp1 = _mm_add_epi32(F_neon(b, c, d), x); \
    __m128i temp2 = _mm_add_epi32(temp1, _mm_set1_epi32(ac)); \
    a = _mm_add_epi32(a, temp2); \
    a = ROTATELEFT_neon(a, s); \
    a = _mm_add_epi32(a, b); \
}

#define GG_neon(a, b, c, d, x, s, ac) \
{ \
    __m128i temp1 = _mm_add_epi32(G_neon(b, c, d), x); \
    __m128i temp2 = _mm_add_epi32(temp1, _mm_set1_epi32(ac)); \
    a = _mm_add_epi32(a, temp2); \
    a = ROTATELEFT_neon(a, s); \
    a = _mm_add_epi32(a, b); \
}

#define HH_neon(a, b, c, d, x, s, ac) \
{ \
    __m128i temp1 = _mm_add_epi32(H_neon(b, c, d), x); \
    __m128i temp2 = _mm_add_epi32(temp1, _mm_set1_epi32(ac)); \
    a = _mm_add_epi32(a, temp2); \
    a = ROTATELEFT_neon(a, s); \
    a = _mm_add_epi32(a, b); \
}

#define II_neon(a, b, c, d, x, s, ac) \
{ \
    __m128i temp1 = _mm_add_epi32(I_neon(b, c, d), x); \
    __m128i temp2 = _mm_add_epi32(temp1, _mm_set1_epi32(ac)); \
    a = _mm_add_epi32(a, temp2); \
    a = ROTATELEFT_neon(a, s); \
    a = _mm_add_epi32(a, b); \
}

void MD5Hash_neon(vector<string> inputs, uint32_t states[4][4]);
