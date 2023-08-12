#ifndef SPOOR_INTERNAL_H
#define SPOOR_INTERNAL_H

#include<time.h>
#include<stdint.h>
#include<lmdb.h>
#include<uuid/uuid.h>

typedef enum {
    STATUS_NOT_STARTED,
    STATUS_IN_PROGRESS,
    STATUS_COMPLETED,
} SpoorStatus;

typedef enum {
    TYPE_TASK,
    TYPE_PROJECT,
    TYPE_EVENT,
    TYPE_APPOINTMENT,
    TYPE_TARGET,
    TYPE_HABIT,
} SpoorType;

typedef struct SpoorTime {
    struct tm start;
    struct tm end;
    struct SpoorTime *next;
} SpoorTime;

typedef struct SpoorObject {
    char id[37];
    char title[250];
    SpoorTime deadline;
    SpoorTime schedule;
    SpoorTime tracked;
    SpoorTime complete;
    SpoorTime created;
    SpoorStatus status;
    SpoorType type;
    char parent_title[250];
    char parent_id[37];
    struct SpoorObject *next;
    uint32_t childrens_count;
    char childrens_id[1][37];
} SpoorObject;

typedef struct {
    MDB_env *env;
    MDB_txn *txn;
    MDB_dbi dbi;
    MDB_cursor *cursor;
    MDB_val val_key;
    MDB_val val_data;
} SpoorDatabase;

void mdb_func_error_check(int error, char *func_name, int line, char *file);
void spoor_debug_spoor_object_print(SpoorObject *spoor_object);

#endif
