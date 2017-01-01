#include <aura/client.h>
#include <stdarg.h>
#include <alloca.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define CONFIG_LIB_AURA_OCOUNT 8

static struct aura_object *registry[CONFIG_LIB_AURA_OCOUNT];
static uint8_t lastid;

int aura_get_registry(struct aura_object ***reg)
{
        *reg = registry;
        return lastid;
}

/* Some data types may be promoted that screws things up */
#define va_arg_ex(ret, ap, type, ptype)                 \
                        ret = (type) va_arg(ap, ptype); \

void aura_raise_event(struct aura_object *o, ...)
{
        va_list ap;
        const unsigned char *rfmt = (unsigned char *) o->ret;
        va_start(ap, o);
        int tocopy;
        void *from;
        union {
                uint8_t  u8;
                uint16_t u16;
                uint32_t u32;
                uint64_t u64;
                int8_t  i8;
                int16_t i16;
                int32_t i32;
                int64_t i64;

        } tmp;

        aura_evt_start(o);
        while (true) {
                switch (*rfmt) {
                case AURA_U8_TOK:
                        va_arg_ex(tmp.u8, ap, uint8_t, unsigned int);
                        from = &tmp.u8;
                        tocopy = sizeof(uint8_t);;
                        break;
                case AURA_S8_TOK: {
                        va_arg_ex(tmp.i8, ap, int8_t, int);
                        from = &tmp.i8;
                        tocopy = sizeof(int8_t);;
                        break;
                }
                case AURA_U16_TOK:
                        va_arg_ex(tmp.u16, ap, uint16_t, unsigned int);
                        from = &tmp.u16;
                        tocopy = sizeof(uint16_t);;
                        break;
                case AURA_S16_TOK:
                        va_arg_ex(tmp.i16, ap, int16_t, int);
                        from = &tmp.i16;
                        tocopy = sizeof(int16_t);;
                        break;
                case AURA_U32_TOK:
                case AURA_S32_TOK:
                        tmp.u32 = va_arg(ap, uint32_t);
                        from = &tmp.u32;
                        tocopy = sizeof(uint32_t);;
                        break;
                case AURA_U64_TOK:
                case AURA_S64_TOK:
                        tmp.u64 = va_arg(ap, uint64_t);
                        from = &tmp.u64;
                        tocopy = sizeof(uint64_t);;
                        break;
                case AURA_BIN_TOK:
                        from = va_arg(ap, char *);
                        tocopy = atoi((char *) rfmt);
                        while (*rfmt && (*(++rfmt) != '.'));
                        break;
                case 0x0:
                        aura_evt_finish();
                        return;
                default:
                        aura_panic("Invalid token");
                        return;
                }
                aura_evt_write(from, tocopy);
                rfmt++;
        }
}


aura_length_t  aura_calc_buffer_size(const char *fmt)
{
        unsigned int len = 0;
        int tmp;

        if (!fmt)
                return 0;

        while (*fmt) {
                switch (*fmt++) {
                case AURA_U8_TOK:
                case AURA_S8_TOK:
                        len += 1;
                        break;
                case AURA_U16_TOK:
                case AURA_S16_TOK:
                        len += 2;
                        break;
                case AURA_U32_TOK:
                case AURA_S32_TOK:
                        len += 4;
                        break;
                case AURA_U64_TOK:
                case AURA_S64_TOK:
                case AURA_BIN_TOK:
                        tmp = atoi(fmt);
                        if (tmp == 0)
                                aura_panic("Internal serilizer bug processing: %s", fmt);
                        len += tmp;
                        while (*fmt && (*fmt++ != '.'));
                        break;
                default:
                        aura_panic("Serializer failed at token: %s", fmt);
                }
        }
        if (len > AURA_LEN_MAX)
                aura_panic("FATAL: increase AURA_LEN_MAX");
        return (aura_length_t) len;
}


int aura_call(aura_id_t id, void *arg)
{
        struct aura_object *o = aura_registry_lookup(id);
        if (!o)
                return -1;
        aura_platform_critical_enter();
        aura_evt_start(o);
        if (o->method)
                o->method(arg);
        aura_evt_finish();
        aura_platform_critical_leave();
        return 0;
}

struct aura_object *aura_registry_lookup(aura_id_t id)
{
        if (id >= lastid)
                return NULL;
        return registry[id];
}

void aura_register(struct aura_object *o)
{
        if (lastid >= CONFIG_LIB_AURA_OCOUNT) {
                aura_panic("Too many objects");
        }
        o->retlen = aura_calc_buffer_size(o->ret);
        if (AURA_EVENT_BUFFER_SIZE < o->retlen)
                aura_panic("AURA_EVENT_BUFFER_SIZE too small to hold object");
        registry[lastid] = o;
        o->id = (uint8_t) lastid++;
}
