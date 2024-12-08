#pragma once

#include <array>
#include <cstdint>
#include <functional>


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

    namespace env {

    }
}  // namespace md

/**
 * @brief Combines two hash values into a single hash value.
 * Function taken from boost::hash_combine.
 * See: https://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine.
 *
 * @param lhs The first hash value.
 * @param rhs The second hash value.
 * @return The combined hash value.
 */
inline size_t hash_combine(const size_t lhs, const size_t rhs) {
    return lhs ^ (rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2));
}

// TODO: maybe move to ArrayUtils.h?
/**
 * @brief Functor for creating a hash from an array<INT_T, 3>
 */
struct Int3Hasher {
    /**
     * @brief hash int3.
     * @param arr array to be hashed.
     * @return hash.
     */
    size_t operator()(const std::array<INT_T, 3>& arr) const {
        std::size_t seed = 0x123456789;  // initial value
        for (const INT_T x : arr) {
            seed = hash_combine(seed, std::hash<size_t>{}(x));
        }
        return seed;
    }
};


