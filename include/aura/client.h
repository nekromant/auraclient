#ifndef AURA_H
#define AURA_H

#include <stdint.h>
#include <stdio.h>

#define aura_cli_dbg(fmt, ...) printf(fmt, ## __VA_ARGS__)


#define AURA_ID_BITS 16
#define AURA_LEN_BITS 16
#define AURA_EVENT_BUFFER_SIZE 128

#if AURA_ID_BITS == 8
typedef uint16_t aura_id_t;
#define AURA_ID_MAX (0xffUL-1)
#endif

#if AURA_ID_BITS == 16
typedef uint16_t aura_id_t;
#define AURA_ID_MAX (0xffffUL-1)
#endif

#if AURA_ID_BITS == 32
typedef uint32_t aura_id_t;
#define AURA_ID_MAX (0xffffffffUL-1)
#endif

#if AURA_LEN_BITS == 8
typedef uint8_t aura_length_t;
#define AURA_LEN_MAX (0xffUL-1)
#endif

#if AURA_LEN_BITS == 16
typedef uint16_t aura_length_t;
#define AURA_LEN_MAX (0xffffUL-1)
#endif

#if AURA_LEN_BITS == 32
typedef uint32_t aura_length_t;
#define AURA_LEN_MAX (0xffffffffUL-1)
#endif

#define AURA_ID_INVALID (AURA_ID_MAX+1)
#define AURA_LEN_INVALID (AURA_LEN_MAX+1)

#ifdef __GNUC__
#define AURA_CONSTRUCTOR __attribute__((constructor))
#else
#error "Compiler is missing constructors"
#endif

#ifdef __AVR__
#include <util/atomic.h>
#define aura_platform_critical_enter() ATOMIC_BLOCK(ATOMIC_FORCEON) {
#define aura_platform_critical_leave() }
#define AURA_PGM_P PGM_P
#define AURA_PSTR(s) PSTR(s)
#define AURA_STRLEN(s) strlen_P(s)
#define AURA_STRCPY(d, s) strcpy_PF(d,s)
#endif


#ifndef min_t
#define min_t(type, a, b) (((type)(a)<(type)(b))?(type)(a):(type)(b))
#endif

#ifndef max_t
#define max_t(type, a, b) (((type)(a)>(type)(b))?(type)(a):(type)(b))
#endif

#define AURA_U8_TOK   '1'
#define AURA_U16_TOK  '2'
#define AURA_U32_TOK  '3'
#define AURA_U64_TOK  '4'

#define AURA_S8_TOK   '6'
#define AURA_S16_TOK  '7'
#define AURA_S32_TOK  '8'
#define AURA_S64_TOK  '9'

#define AURA_BIN_TOK 's'


#define AURA_NONE ""
#define AURA_U8   "1"
#define AURA_U16  "2"
#define AURA_U32  "3"
#define AURA_U64  "4"

#define AURA_S8   "6"
#define AURA_S16  "7"
#define AURA_S32  "8"
#define AURA_S64  "9"

#define AURA_BIN_TOK 's'
#define AURA_BIN(n) "s" #n "."

#define aura_arg_get(tp, argptr)                        \
        (*((tp *) argptr)); argptr += sizeof(tp);     \

#define aura_ret_put(tp, v)                   \
        { \
        tp tmp__ = v; \
        aura_evt_write(&tmp__, sizeof(tp)); \
        };

#define aura_get_u8()  aura_arg_get(uint8_t, arg)
#define aura_get_u16() aura_arg_get(uint16_t, arg)
#define aura_get_u32() aura_arg_get(uint32_t, arg)
#define aura_get_u64() aura_arg_get(uint64_t, arg)

#define aura_get_i8()  aura_arg_get(int8_t, arg)
#define aura_get_i16() aura_arg_get(int16_t, arg)
#define aura_get_i32() aura_arg_get(int32_t, arg)
#define aura_get_i64() aura_arg_get(int64_t, arg)

#define aura_put_u8(v)  aura_ret_put(uint8_t, v)
#define aura_put_u16(v) aura_ret_put(uint16_t, v)
#define aura_put_u32(v) aura_ret_put(uint32_t, v)
#define aura_put_u64(v) aura_ret_put(uint64_t, v)

#define aura_put_i8(v)  aura_ret_put(int8_t,  v)
#define aura_put_i16(v) aura_ret_put(int16_t, v)
#define aura_put_i32(v) aura_ret_put(int32_t, v)
#define aura_put_i64(v) aura_ret_put(int64_t, v)


#define aura_get_bin(len) \
        (char *) arg; argptr += len;

struct aura_object {
        aura_id_t id;
		aura_length_t retlen;
        const char* name;
        const char* arg;
        const char* ret;
        void (*method)(const void *arg);
};

#define aura_object_is_method(o) (o->arg != NULL)
#define aura_object_is_event(o)  (o->arg == NULL)


#ifndef AURA_METHOD
#define AURA_METHOD(_name, arglist, retlist)                            \
        void aura_method_ ## _name(const void *arg);              \
        static struct aura_object aura_obj_method_ ## _name = {         \
                .name = #_name,                                         \
                .arg = arglist,                                         \
                .ret = retlist,                                         \
                .method = aura_method_##_name,                          \
        };                                                              \
        void AURA_CONSTRUCTOR register_method_##_name() {                     \
                aura_register(&aura_obj_method_##_name);                \
        };                                                              \
        void aura_method_##_name(const void *arg)

#endif

#ifndef AURA_EVENT
#define AURA_EVENT(_name, retlist)                       \
        struct aura_object g_aura_obj_event_##_name = { \
                .name = #_name,                          \
                .ret = retlist,                         \
        };                                              \
        void AURA_CONSTRUCTOR register_event_##_name() {                     \
                aura_register(&g_aura_obj_event_##_name);                \
        };

#endif

int aura_get_registry(struct aura_object ***reg);
void aura_register(struct aura_object *o);
int aura_evt_start(struct aura_object *o);
void aura_evt_write(const void *buf, aura_length_t len);
void aura_evt_finish();
void aura_panic(const char *fmt, ...);
int aura_call(aura_id_t id, const void *arg);
struct aura_object *aura_registry_lookup(aura_id_t id);
struct aura_object *aura_registry_lookup_byname(const char *name);
void aura_eventqueue_read(void *buf, aura_length_t len);
void aura_eventqueue_peek(void *buf, aura_length_t len);
aura_length_t aura_eventqueue_size();
aura_length_t aura_eventqueue_next();
#endif
