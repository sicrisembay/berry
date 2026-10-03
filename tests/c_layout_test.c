#include <limits.h>
#include "be_constobj.h"

static const bmapkey compact_key = be_const_key_int(0x123456, 0xABCDEF);

#if CHAR_BIT == 16
typedef char bmapkey_storage_check[
    sizeof(bmapkey) == sizeof(union bvaldata) + sizeof(uint32_t) ? 1 : -1
];
typedef char pointer_width_check[sizeof(void*) == 2 ? 1 : -1];
#endif

int main(void)
{
    bmapkey assigned_key = { 0 };
    assigned_key.type = 0xA5;
    assigned_key.next = 0x123456;

    if (compact_key.type != BE_INT || compact_key.next != 0xABCDEF || compact_key.v.i != 0x123456) {
        return 1;
    }
    if (assigned_key.type != 0xA5 || assigned_key.next != 0x123456) {
        return 2;
    }
    return 0;
}
