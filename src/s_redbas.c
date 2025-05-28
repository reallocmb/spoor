#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

typedef struct {
    FILE *f;
    uint32_t size;
    uint32_t items;
} RedbasDB;

uint32_t redbas_db_items(RedbasDB *db)
{
    return db->items;
}

void redbas_db_store(RedbasDB *db, void *data, uint32_t size)
{
    if (db->size == size)
    {
        fseek(db->f, 0, SEEK_END);
        fwrite(data, size, 1, db->f);
        db->items++;
    }
}

void redbas_db_change(RedbasDB *db, void *data, uint32_t size, uint32_t index)
{
    if (db->size == size)
    {
        printf("fseek %d \n", fseek(db->f, index * db->size + sizeof(db->size) + sizeof(db->items), SEEK_SET));
        printf("fwrite %zd\n", fwrite(data, size, 1, db->f));
    }
}

void redbas_db_restore_cursor_set(RedbasDB *db, uint32_t index)
{
    fseek(db->f, index * db->size + sizeof(db->items) + sizeof(db->size), SEEK_SET);
}

void redbas_db_restore(RedbasDB *db, void *data, uint32_t size)
{
    if (db->size == size)
    {
        fread(data, size, 1, db->f);
    }
}

void redbas_db_close(RedbasDB *db)
{
    fseek(db->f, sizeof(db->size), SEEK_SET);
    fwrite(&db->items, sizeof(db->items), 1, db->f);
    fclose(db->f);
    free(db);
}

RedbasDB *redbas_db_size_change(RedbasDB *db, char *path, u32 size_new);

RedbasDB *redbas_db_open(char *path, uint32_t size)
{
    RedbasDB *redbas_db = malloc(sizeof(*redbas_db));
    redbas_db->f = fopen(path, "rb");
    if (redbas_db->f == NULL)
    {
        redbas_db->f = fopen(path, "w+b");
        if (redbas_db->f == NULL)
            return NULL;
        redbas_db->size = size;
        fwrite(&size, sizeof(size), 1, redbas_db->f);
        redbas_db->items = 0;
        fwrite(&redbas_db->items, sizeof(redbas_db->items), 1, redbas_db->f);
        fclose(redbas_db->f);
        redbas_db->f = fopen(path, "r+b");
    }
    else
    {
        fclose(redbas_db->f);
        redbas_db->f = fopen(path, "r+b");
        fread(&redbas_db->size, sizeof(redbas_db->size), 1, redbas_db->f);
        fread(&redbas_db->items, sizeof(redbas_db->items), 1, redbas_db->f);

#if 0
        if (redbas_db->size != size)
            return redbas_db_size_change(redbas_db, path, size);
#endif
            
    }

    return redbas_db;
}

#if 0
RedbasDB *redbas_db_size_change(RedbasDB *db, char *path, u32 size_new)
{
    RedbasDB *db_new = redbas_db_open("tmp_new", size_new);

    void *data = malloc(db->size);

    u32 i;
    for (i = 0; i < db->items; i++)
    {
        redbas_db_restore_cursor_set(db, i);
        redbas_db_restore(db, data, db->size);

        if (size_new > db->size)
            memset(((char *)data) + size_new - db->size, size_new - db->size, 1);

        redbas_db_store(db_new, data, size_new);
    }

    redbas_db_close(db_new);

    fclose(db->f);
    fclose(db_new->f);

    db_new->f = fopen(path, "rb");

    remove(path);
    rename("tmp_new", path);

    return db_new;
}
#endif
