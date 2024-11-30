#pragma once

#include <array>
#include <limits>
#include <cstdint>
#include <functional>


#define UINT_T uint32_t
#define INT_T int32_t
#define PARTICLE_TYPE_DEAD std::numeric_limits<int>::max()

namespace md {
    using vec3 = std::array<double, 3>;
    using uint3 = std::array<UINT_T, 3>;
    using int3 = std::array<INT_T, 3>;
}


// function taken from boost::hash_combine
// see https://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
inline size_t hash_combine(const size_t lhs, const size_t rhs ) {
    return lhs ^ (rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2));
}

// maybe move to ArrayUtils.h?
struct Int3Hasher {
    size_t operator()(const std::array<INT_T, 3>& arr) const {
        std::size_t seed = 0x123456789; // initial value
        for (const INT_T x : arr) {
            seed = hash_combine(seed, std::hash<size_t>{}(x));
        }
        return seed;
    }
};

