#include <cassert>
#include <db.h>
#include <cstdlib>
#include <cstring>

int compare_keys(DB *db, const DBT *a, const DBT *b) {
    if (a->size < b->size) {
        if (memcmp(a->data, b->data, a->size) <= 0) return -1;
        else return 1;
    } else if (a->size > b->size) {
        if (memcmp(a->data, b->data, b->size) >= 0) return 1;
        else return -1;
    } else {
        return memcmp(a->data, b->data, a->size);
    }
}

const int STATESIZE = 64;
char random_buf[STATESIZE];
struct random_data rbuf;
int mrandom() {
    int32_t result;
    random_r(&rbuf, &result);
    return result;
}

int main() {
    initstate_r(1, random_buf, STATESIZE, &rbuf);
    DB_ENV *env;
    DB *db;
    {
        int r = db_env_create(&env, 0);
        assert(r == 0);
    }
    {
        int r = env->open(env, "env", DB_CREATE | DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_MPOOL | DB_INIT_TXN | DB_RECOVER | DB_THREAD, 0);
        assert(r == 0);
    }
    {
        int r  = db_create(&db, env, 0);
        assert(r == 0);
    }
    {
        int r = db->set_bt_compare(db, compare_keys);
        assert(r == 0);
    }
    {
        int r = db->open(db, NULL, "foo.db", NULL, DB_BTREE, DB_CREATE, 0);
        assert(r == 0);
    }
    for (int i = 0 ; i < 10000; i++) {
        char keydata[1000];
        //int  keysize = ((mrandom()%4) == 0  ) ? 999 : 4;
        int  keysize = 4;
        if (i > 128 && (i%128)==0x3f) keysize = 999;
        //if (i == 0xbf || i == 0x13f || i == 0x1bf || i == 0x23f || i == 0x2bf || i == 0x33f) keysize = 999;
        for (int j = 0; j < keysize; j++) keydata[j] = mrandom();
        keydata[0] = i>>24;
        keydata[1] = (i>>16) % 256;
        keydata[2] = (i>>8) % 256;
        keydata[3] = (i>>0) % 256;
        char data[] = "a";
        DBT key = {.data = keydata, .size= keysize};
        DBT val = {.data = data, .size=0};
        int r = db->put(db, NULL, &key, &val, 0);
    }
    {
        int r = db->close(db, 0);
        assert(r == 0);
    }
    {
        int r = env->close(env, 0);
        assert(r == 0);
    }
    return 0;
}
