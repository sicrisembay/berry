#include <limits.h>
#include <stdint.h>
#include "berry.h"
#include "be_string.h"

#if CHAR_BIT != 8 && CHAR_BIT != 16
#error "Unsupported C byte width for Berry octet tests."
#endif

#if CHAR_BIT == 16
typedef char string_limit_fits_length[
    BE_STRING_MAX_LEN == INT_MAX ? 1 : -1
];
#else
typedef char string_limit_preserves_host_cap[
    BE_STRING_MAX_LEN == 16777216u ? 1 : -1
];
#endif

int main(void)
{
    uint32_t value;

    for (value = 0; value <= 511; ++value) {
        bbyte octet = be_octet_from_u32(value);
        if ((uint32_t)octet != (value & 0xFFu)) {
            return 1;
        }
    }

    for (value = 0; value <= 255; ++value) {
        int32_t expected = (value & 0x80u) ? (int32_t)value - 0x100 : (int32_t)value;
        bsbyte signed_octet = be_octet_to_sbyte(be_octet_from_u32(value));
        if ((int32_t)signed_octet != expected) {
            return 2;
        }
    }

#if CHAR_BIT == 16
    if (sizeof(bbyte) != 1 || sizeof(bsbyte) != 1) {
        return 3;
    }
#endif

    return 0;
}
