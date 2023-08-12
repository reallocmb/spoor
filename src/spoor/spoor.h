#ifndef SPOOR_H
#define SPOOR_H

typedef void SpoorObject;
typedef void SpoorDatabase;
typedef void SpoorFilter;

SpoorObject *spoor_arguments_parse(int argc, char **argv); /* parse the command arguments and create a new SpoorObject */

/* Data Storage */
SpoorDatabase *spoor_storage_init(void);
void spoor_storage_close(SpoorDatabase *spoor_database);
int spoor_storage_object_save(SpoorDatabase *spoor_database, SpoorObject *spoor_object);

/* UI */
void spoor_ui_object_show(SpoorDatabase *spoor_database);

/* Debugging */
#ifdef DEBUG
void spoor_debug_spoor_object_print(SpoorObject *spoor_object); /* print an SpoorObject to the console */
void spoor_debug_storage_print_all(SpoorDatabase *spoor_object);
#endif

#endif
