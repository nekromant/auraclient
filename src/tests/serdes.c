#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <aura/client.h>



AURA_METHOD(echo_u8,
    AURA_U8,
    AURA_U8)
    {
        uint8_t ret = aura_get_u8();
        printf("echo_u8: %lld\n", (long long) ret);
        aura_put_u8(ret);
    }

AURA_METHOD(echo_u16,
    AURA_U16,
    AURA_U16)
{
    uint16_t ret = aura_get_u16();
    printf("echo_u16: %lld\n", ret);
    aura_put_u16(ret);
}

AURA_METHOD(echo_u32,
    AURA_U32,
    AURA_U32)
    {
        uint32_t ret = aura_get_u32();
        printf("echo_u32: %d\n", ret);
        aura_put_u32(ret);
    }

AURA_METHOD(echo_u64,
    AURA_U64,
    AURA_U64)
{
    uint64_t ret = aura_get_u64();
    printf("echo_u64: %lld\n", (long long)ret);
    aura_put_u64(ret);
}

AURA_METHOD(echo_i8,
    AURA_S8,
    AURA_S8)
{
    uint8_t ret = aura_get_i8();
    printf("echo_i8: %lld\n", (long long) ret);
    aura_put_i8(ret);
}


AURA_METHOD(echo_i16,
    AURA_S16,
    AURA_S16)
{
    uint16_t ret = aura_get_i16();
    printf("echo_i16: %lld\n", ret);
    aura_put_i16(ret);
}

AURA_METHOD(echo_i32,
    AURA_S32,
    AURA_S32)
    {
        uint32_t ret = aura_get_i32();
        printf("echo_i32: %d\n", ret);
        aura_put_i32(ret);
    }

AURA_METHOD(echo_i64,
    AURA_S64,
    AURA_S64)
{
    int64_t ret = aura_get_i64();
    printf("echo_i64: %lld\n", (long long)ret);
    aura_put_i64(ret);
}


#define testtype uint8_t
#define rpc_name echo_u8
#include "templates/serdes.inc.c"

#define testtype uint16_t
#define rpc_name echo_u16
#include "templates/serdes.inc.c"

#define testtype uint32_t
#define rpc_name echo_u32
#include "templates/serdes.inc.c"

#define testtype uint64_t
#define rpc_name echo_u64
#include "templates/serdes.inc.c"


#define testtype int8_t
#define rpc_name echo_i8
#include "templates/serdes.inc.c"

#define testtype int16_t
#define rpc_name echo_i16
#include "templates/serdes.inc.c"

#define testtype int32_t
#define rpc_name echo_i32
#include "templates/serdes.inc.c"

#define testtype int64_t
#define rpc_name echo_i64
#include "templates/serdes.inc.c"


int main(int argc, char const *argv[])
{
    const char data[] = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8};
    echo_u8(data);
    echo_u16(data);
    echo_u32(data);
    echo_u64(data);

    echo_i8(data);
    echo_i16(data);
    echo_i32(data);
    echo_i64(data);

    return 0;
}
