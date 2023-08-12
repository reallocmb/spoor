#include"spoor/spoor.h"
#include<string.h>
#include<stdlib.h>

int main(int argc, char **argv)
{
    if (strcmp(argv[1], "-c") == 0)
    {
        SpoorObject *spoor_object = spoor_arguments_parse(argc - 2, argv + 2);
        spoor_debug_spoor_object_print(spoor_object);

        SpoorDatabase *spoor_database = spoor_storage_init();
        spoor_storage_object_save(spoor_database, spoor_object);
        spoor_storage_close(spoor_database);

        free(spoor_object);
    }

    if (strcmp(argv[1], "-s") == 0)
    {
        SpoorDatabase *spoor_database = spoor_storage_init();
#if 0
        spoor_debug_storage_print_all(spoor_database);
#else
        spoor_ui_object_show(spoor_database);
#endif
        spoor_storage_close(spoor_database);
    }

    if (strcmp(argv[1], "-p") == 0)
    {
    }

    return 0;
}
