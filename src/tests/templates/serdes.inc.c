/* Shitty templating */

#define xstr(a) str(a)
#define str(a) #a

void rpc_name(const char *testdata)
{
    const char *name = xstr(rpc_name);
    struct aura_object *o = aura_registry_lookup_byname(name);
    if (!o) {
        printf("%s: No such object\n", name);
        exit(1);
    }
    aura_call(o->id, testdata);
    int length = aura_eventqueue_size();
    if (length != o->retlen + sizeof(aura_id_t)) {
        printf("%s: eventqueue legth mismatch (%d != %d + %d)\n",
            name, length, o->retlen, sizeof(aura_id_t));
        exit(1);
    }
    aura_id_t id;
    testtype v,ref;
    ref = *(testtype *) testdata;
    aura_eventqueue_read(&id, sizeof(aura_id_t));
    if (id !=o->id) {
        printf("%s: id mismatch: %lld != %lld\n", name,
        (long long) id, (long long) o->id);
        exit(1);
    }

    aura_eventqueue_read(&v, sizeof(testtype));
    if (ref != v) {
        printf("%s: mismatch: %lld != %lld\n", name,
            (long long) v, (long long) ref);
        exit(1);
    }

}

#undef testtype
#undef rpc_name
