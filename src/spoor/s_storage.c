#include"spoor_internal.h"
#include <lmdb.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<sys/stat.h>

#define LMDB_DATABASE_LOCATION "./database"

void mdb_func_error_check(int error, char *func_name, int line, char *file)
{
    if (error)
        fprintf(stderr, "%s failed ... line: %d - file: %s\n", func_name, line, file);
}

SpoorDatabase *spoor_storage_init(void)
{
    int error;

    mkdir(LMDB_DATABASE_LOCATION, 0777);

    SpoorDatabase *spoor_database = malloc(sizeof(*spoor_database));
    assert(spoor_database != NULL);

    error = mdb_env_create(&spoor_database->env);
    mdb_func_error_check(error, "mdb_env_create()", __LINE__, __FILE__);

    error = mdb_env_open(spoor_database->env, LMDB_DATABASE_LOCATION, 0, 0777);
    mdb_func_error_check(error, "mdb_env_open()", __LINE__, __FILE__);


    return spoor_database;
}

void spoor_storage_close(SpoorDatabase *spoor_database)
{
    mdb_env_close(spoor_database->env);
}

int spoor_storage_object_save(SpoorDatabase *spoor_database, SpoorObject *spoor_object)
{
    spoor_database->val_key.mv_data = spoor_object->id;
    spoor_database->val_key.mv_size = 36;

    spoor_database->val_data.mv_data = (void *)spoor_object;
    spoor_database->val_data.mv_size = sizeof(*spoor_object);

    int error;

    error = mdb_txn_begin(spoor_database->env, NULL, 0, &spoor_database->txn);
    mdb_func_error_check(error, "mdb_txn_begin()", __LINE__, __FILE__);

    error = mdb_dbi_open(spoor_database->txn, NULL, 0, &spoor_database->dbi);
    mdb_func_error_check(error, "mdb_dbi_open()", __LINE__, __FILE__);

    error = mdb_cursor_open(spoor_database->txn, spoor_database->dbi, &spoor_database->cursor);
    mdb_func_error_check(error, "mdb_cursor_open()", __LINE__, __FILE__);

    error = mdb_cursor_put(spoor_database->cursor, &spoor_database->val_key, &spoor_database->val_data, 0);
    /*
    error = mdb_put(spoor_database->txn, spoor_database->dbi, &spoor_database->val_key, &spoor_database->val_data, 0);
    */
    mdb_func_error_check(error, "mdb_put()", __LINE__, __FILE__);

    mdb_dbi_close(spoor_database->env, spoor_database->dbi);
    error = mdb_txn_commit(spoor_database->txn);
    mdb_func_error_check(error, "mdb_txn_commit", __LINE__, __FILE__);

    return 0;
}
