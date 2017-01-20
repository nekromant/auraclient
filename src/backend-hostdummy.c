#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <aura/client.h>
#include <stdarg.h>

void aura_platform_critical_enter()
{
    /* blah */
}

void aura_platform_critical_leave()
{

}




void aura_panic(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}
