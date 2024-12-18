#include <array>
#include <cstdint>
#include <immintrin.h>
#include <cmath>
#include <iostream>

#define UINT_T uint32_t
#define INT_T int32_t
// #define INF std::numeric_limits<double>::infinity()

/**
 * @brief Contains fundamental types and utilities
 */
namespace md {
    using vec3 = std::array<double, 3>;
    using uint3 = std::array<UINT_T, 3>;
    using int3 = std::array<INT_T, 3>;

    struct SIMDVec3 {
        __m256d data; // AVX2 256-bit register (can hold four doubles)

        SIMDVec3() {
            data = _mm256_set_pd(0,0,0,0);
        }

        SIMDVec3(const double v1, const double v2, const double v3) {
            // Load the vec3 into a SIMD register
            data = _mm256_set_pd(0.0, v3, v2, v1); // Add 0.0 padding
        }

        explicit SIMDVec3(const vec3& v) {
            // Load the vec3 into a SIMD register
            data = _mm256_set_pd(0.0, v[2], v[1], v[0]); // Add 0.0 padding
        }

        explicit SIMDVec3(__m256d reg) : data(reg) {}

        [[nodiscard]] vec3 toArray() const {
            // Extract data back to vec3
            alignas(32) double temp[4];
            _mm256_store_pd(temp, data);
            return {temp[0], temp[1], temp[2]};
        }

        SIMDVec3& operator=(const vec3 &v) {
            data = _mm256_set_pd(0.0, v[2], v[1], v[0]);
            return *this;
        }

        SIMDVec3 operator+(const SIMDVec3& other) const {
            return SIMDVec3(_mm256_add_pd(data, other.data));
        }

        SIMDVec3 operator-(const SIMDVec3& other) const {
            return SIMDVec3(_mm256_sub_pd(data, other.data));
        }

        SIMDVec3 operator*(const double scalar) const {
            const __m256d scalarReg = _mm256_set1_pd(scalar);
            return SIMDVec3(_mm256_mul_pd(data, scalarReg));
        }

        friend SIMDVec3 operator*(const double scalar, const SIMDVec3& vec) {
            return vec * scalar; // Reuse vec * scalar operator
        }

        SIMDVec3& operator+=(const SIMDVec3& other) {
            data = _mm256_add_pd(data, other.data);
            return *this;
        }

        SIMDVec3& operator-=(const SIMDVec3& other) {
            data = _mm256_sub_pd(data, other.data);
            return *this;
        }

        SIMDVec3& operator*=(const double scalar) {
            __m256d scalarReg = _mm256_set1_pd(scalar);
            data = _mm256_mul_pd(data, scalarReg);
            return *this;
        }

        [[nodiscard]] double l2Norm() const {
            return std::sqrt(l2NormSquared());
        }

        [[nodiscard]] double l2NormSquared() const {
            const __m256d squared = _mm256_mul_pd(data, data);
            const __m128d high = _mm256_extractf128_pd(squared, 1); // [c², d²]
            const __m128d low  = _mm256_castpd256_pd128(squared);   // [a², b²]

            const __m128d partial_sum = _mm_add_pd(low, high);
            const __m128d final_sum = _mm_hadd_pd(partial_sum, partial_sum);

            return _mm_cvtsd_f64(final_sum);
        }

        double operator[](const std::size_t index) const {
            alignas(32) double temp[4];
            _mm256_store_pd(temp, data);
            return temp[index];
        }

        // operator vec3() const {
        //     alignas(32) double temp[4];
        //     _mm256_store_pd(temp, data);
        //     return {temp[0], temp[1], temp[2]};
        // }
    };
}  // namespace md