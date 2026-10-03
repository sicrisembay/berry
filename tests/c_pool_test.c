#include <string.h>
#include "be_mem.h"
#include "be_vm.h"

int main(void)
{
    enum { POOL16_TEST_COUNT = 32, POOL32_TEST_COUNT = 16 };
    static const size_t sizes[] = { 15, 16, 17, 31, 32, 33 };
    bvm *vm = (bvm*)be_os_malloc(sizeof(bvm));
    bbyte *blocks[sizeof(sizes) / sizeof(sizes[0])] = { 0 };
    bbyte *pool16_blocks[POOL16_TEST_COUNT] = { 0 };
    bbyte *pool32_blocks[POOL32_TEST_COUNT] = { 0 };
    size_t slots_used;
    size_t slots_allocated;
    size_t block_index;

    if (!vm) {
        return 1;
    }
    memset(vm, 0, sizeof(*vm));
    be_gc_init_memory_pools(vm);

    for (block_index = 0; block_index < sizeof(sizes) / sizeof(sizes[0]); ++block_index) {
        size_t octet_index;
        blocks[block_index] = (bbyte*)be_malloc(vm, sizes[block_index]);
        if (!blocks[block_index]) {
            return 2;
        }
        for (octet_index = 0; octet_index < sizes[block_index]; ++octet_index) {
            blocks[block_index][octet_index] = be_octet_from_u32((uint32_t)(octet_index + sizes[block_index]));
        }
    }

    for (block_index = 0; block_index < sizeof(sizes) / sizeof(sizes[0]); ++block_index) {
        size_t octet_index;
        for (octet_index = 0; octet_index < sizes[block_index]; ++octet_index) {
            bbyte expected = be_octet_from_u32((uint32_t)(octet_index + sizes[block_index]));
            if (blocks[block_index][octet_index] != expected) {
                return 3;
            }
        }
    }

    for (block_index = 0; block_index < POOL16_TEST_COUNT; ++block_index) {
        size_t prior;
        pool16_blocks[block_index] = (bbyte*)be_malloc(vm, 16);
        if (!pool16_blocks[block_index]) {
            return 4;
        }
        pool16_blocks[block_index][0] = be_octet_from_u32((uint32_t)block_index);
        for (prior = 0; prior < block_index; ++prior) {
            if (pool16_blocks[prior] == pool16_blocks[block_index]) {
                return 5;
            }
        }
    }

    for (block_index = 0; block_index < POOL32_TEST_COUNT; ++block_index) {
        size_t prior;
        pool32_blocks[block_index] = (bbyte*)be_malloc(vm, 32);
        if (!pool32_blocks[block_index]) {
            return 6;
        }
        pool32_blocks[block_index][0] = be_octet_from_u32((uint32_t)block_index);
        for (prior = 0; prior < block_index; ++prior) {
            if (pool32_blocks[prior] == pool32_blocks[block_index]) {
                return 7;
            }
        }
    }

    be_gc_memory_pools_info(vm, &slots_used, &slots_allocated);
    if (slots_used != 53 || slots_allocated != 92) {
        return 8;
    }

    for (block_index = 0; block_index < sizeof(sizes) / sizeof(sizes[0]); ++block_index) {
        be_free(vm, blocks[block_index], sizes[block_index]);
    }
    for (block_index = 0; block_index < POOL16_TEST_COUNT; ++block_index) {
        be_free(vm, pool16_blocks[block_index], 16);
    }
    for (block_index = 0; block_index < POOL32_TEST_COUNT; ++block_index) {
        be_free(vm, pool32_blocks[block_index], 32);
    }

    be_gc_memory_pools_info(vm, &slots_used, &slots_allocated);
    if (slots_used != 0 || slots_allocated != 92) {
        return 9;
    }
    be_gc_memory_pools(vm);
    be_gc_memory_pools_info(vm, &slots_used, &slots_allocated);
    if (slots_used != 0 || slots_allocated != 0) {
        return 10;
    }

    be_os_free(vm);
    return 0;
}
