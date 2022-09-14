#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string.h>
#include <cstring>
#include <string>
// #include "unordered_dense.h"
using namespace std;

#        define ANKERL_UNORDERED_DENSE_LIKELY(x) (x)
#        define ANKERL_UNORDERED_DENSE_UNLIKELY(x) (x)

namespace wyhash {

static inline void mum(uint64_t* a, uint64_t* b) {
#    if defined(__SIZEOF_INT128__)
    __uint128_t r = *a;
    r *= *b;
    *a = static_cast<uint64_t>(r);
    *b = static_cast<uint64_t>(r >> 64U);
#    elif defined(_MSC_VER) && defined(_M_X64)
    *a = _umul128(*a, *b, b);
#    else
    uint64_t ha = *a >> 32U;
    uint64_t hb = *b >> 32U;
    uint64_t la = static_cast<uint32_t>(*a);
    uint64_t lb = static_cast<uint32_t>(*b);
    uint64_t hi{};
    uint64_t lo{};
    uint64_t rh = ha * hb;
    uint64_t rm0 = ha * lb;
    uint64_t rm1 = hb * la;
    uint64_t rl = la * lb;
    uint64_t t = rl + (rm0 << 32U);
    auto c = static_cast<uint64_t>(t < rl);
    lo = t + (rm1 << 32U);
    c += static_cast<uint64_t>(lo < t);
    hi = rh + (rm0 >> 32U) + (rm1 >> 32U) + c;
    *a = lo;
    *b = hi;
#    endif
}

// multiply and xor mix function, aka MUM
[[nodiscard]] static inline auto mix(uint64_t a, uint64_t b) -> uint64_t {
    mum(&a, &b);
    return a ^ b;
}

// read functions. WARNING: we don't care about endianness, so results are different on big endian!
[[nodiscard]] static inline auto r8(const uint8_t* p) -> uint64_t {
    uint64_t v{};
    std::memcpy(&v, p, 8);
    return v;
}

[[nodiscard]] static inline auto r4(const uint8_t* p) -> uint64_t {
    uint32_t v{};
    std::memcpy(&v, p, 4);
    return v;
}

// reads 1, 2, or 3 bytes
[[nodiscard]] static inline auto r3(const uint8_t* p, size_t k) -> uint64_t {
    return (static_cast<uint64_t>(p[0]) << 16U) | (static_cast<uint64_t>(p[k >> 1U]) << 8U) | p[k - 1];
}

[[nodiscard]] static inline auto hash(void const* key, size_t len) -> uint64_t {
    static constexpr uint64_t secret[4] = {UINT64_C(0xa0761d6478bd642f),
                                              UINT64_C(0xe7037ed1a0b428db),
                                              UINT64_C(0x8ebc6af09c88c6e3),
                                              UINT64_C(0x589965cc75374cc3)};

    auto const* p = static_cast<uint8_t const*>(key);
    uint64_t seed = secret[0];
    uint64_t a{};
    uint64_t b{};
    if (ANKERL_UNORDERED_DENSE_UNLIKELY(len <= 16)) {
        if (ANKERL_UNORDERED_DENSE_LIKELY(len >= 4)) {
            a = (r4(p) << 32U) | r4(p + ((len >> 3U) << 2U));
            b = (r4(p + len - 4) << 32U) | r4(p + len - 4 - ((len >> 3U) << 2U));
        } else if (ANKERL_UNORDERED_DENSE_LIKELY(len > 0)) {
            a = r3(p, len);
            b = 0;
        } else {
            a = 0;
            b = 0;
        }
    } else {
        size_t i = len;
        if (ANKERL_UNORDERED_DENSE_LIKELY(i > 48)) {
            uint64_t see1 = seed;
            uint64_t see2 = seed;
            do {
                seed = mix(r8(p) ^ secret[1], r8(p + 8) ^ seed);
                see1 = mix(r8(p + 16) ^ secret[2], r8(p + 24) ^ see1);
                see2 = mix(r8(p + 32) ^ secret[3], r8(p + 40) ^ see2);
                p += 48;
                i -= 48;
            } while (ANKERL_UNORDERED_DENSE_LIKELY(i > 48));
            seed ^= see1 ^ see2;
        }
        while (ANKERL_UNORDERED_DENSE_LIKELY(i > 16)) {
            seed = mix(r8(p) ^ secret[1], r8(p + 8) ^ seed);
            i -= 16;
            p += 16;
        }
        a = r8(p + i - 16);
        b = r8(p + i - 8);
    }

    return mix(secret[1] ^ len, mix(a ^ secret[1], b ^ seed));
}

} // namespace detail::wyhash

// 左闭右闭区间
int getRand(int min, int max) {
    return ( rand() % (max - min + 1) ) + min ;
}

int main () {
    srand(time(0));
    unordered_set<int64_t> mp;
    const uint64_t bucket_cnt = uint32_t(1<<31) - 1;
    const uint64_t insert_cnt = 2e8;
    // uint64_t bucket_cnt = uint32_t(1<<25) - 1;
    // uint64_t insert_cnt = int(2e6);
    mp.reserve(bucket_cnt);
    cout << "mp.bucket_count(): " << mp.bucket_count() << std::endl;
    const int key_len = 64;
    char key[key_len];
    for (int64_t i = 0; i < insert_cnt; i++) {
        if (i % int(2e7) == 0) {
            cout << "i = " << i << endl;
        }
        for (int j = 0; j < key_len/8; j++) {
            memcpy(key + j * 8, &i, 8);
        }
        mp.insert(wyhash::hash(key, key_len));
    }

    cout << "mp.size() = " << mp.size() << endl;
    map<int, int> cnt;
    for (uint32_t idx = 0; idx < mp.bucket_count(); idx++) {
        if (idx % int(1e9) == 0) {
            cout << "idx = " << idx << endl;
        }
        cnt[mp.bucket_size(idx)]++;
    }

    for (auto iter = cnt.begin(); iter != cnt.end(); iter++) {
        cout << "size = " << iter->first << ", cnt = " << iter->second << endl;
    }
    
    return 0;
}