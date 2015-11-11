#include <cassert>
#include <db.h>


int main() {
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
        int r = db->open(db, NULL, "foo.db", NULL, DB_BTREE, DB_CREATE, 0);
        assert(r == 0);
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
