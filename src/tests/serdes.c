#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <aura/client.h>



AURA_METHOD(echo_u8,
    AURA_U8,
    AURA_U8)
{
    uint8_t ret = aura_get_u8();
    printf("echo_u8: %d\n", ret);
    aura_put_u8(ret);
}


static void check_u8()
{

}

int main(int argc, char const *argv[])
{
    const char data[] = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8};
    aura_call(0, data);
    uint8_t a,b;
    aura_eventqueue_read(&a, sizeof(uint8_t));
    b = (*(uint8_t*) data);
    if (a != b) {
        printf("fail: %d != %d\n", a, b);
        return 1;
    }
    return 0;
}
