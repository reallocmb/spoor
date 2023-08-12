#include"spoor_internal.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<termios.h>
#include<time.h>

const char UI_TYPES[][17] = {
    "TASK",
    "PROJECT",
    "EVENT",
    "APPOINTMENT",
    "TARGET",
    "HABIT"
};

const char UI_STATUS[][19] = {
    "NOT STARTED",
    "IN PROGRESS",
    "STATUS COMPLETED"
};

void cursor_hide(void)
{
    fprintf(stdout, "\033[?25l");
}

void cursor_show(void)
{
    fprintf(stdout, "\033[?25h");
}

void cursor_move(int x, int y)
{
    fprintf(stdout, "\033[%d;%dH", y, x);
}

void screen_clear(void)
{
    fprintf(stdout, "\033[2J");
}

void screen_draw(char *string)
{
    fprintf(stdout, "%s", string);
}

void screen_render(void)
{
}

void time_format_parse_deadline(SpoorTime *spoor_time, char *time_format)
{
    if (spoor_time->start.tm_year == -1)
    {
        sprintf(time_format, "--.--.---- --:--");
        return;
    }

    char time_format_start[6];

    if (spoor_time->start.tm_hour == -1 || spoor_time->start.tm_min == -1)
        sprintf(time_format_start, "--:--");
    else
        sprintf(time_format_start, "%s%d:%s%d",
                (spoor_time->start.tm_hour < 10) ?"0" :"", spoor_time->start.tm_hour,
                (spoor_time->start.tm_min < 10) ?"0" : "", spoor_time->start.tm_min);

    sprintf(time_format, "%s%d.%s%d.%d %s",
            (spoor_time->start.tm_mday < 10) ?"0" :"",
            spoor_time->start.tm_mday,
            (spoor_time->start.tm_mon + 1 < 10) ?"0" :"",
            spoor_time->start.tm_mon + 1,
            spoor_time->start.tm_year + 1900,
            time_format_start);
}

void time_format_parse_schedule(SpoorTime *spoor_time, char *time_format)
{
    if (spoor_time->start.tm_year == -1)
    {
        sprintf(time_format, "--.--.---- --:-- --:--");
        return;
    }

    char time_format_start[6];
    char time_format_end[6];

    if (spoor_time->start.tm_hour == -1 || spoor_time->start.tm_min == -1)
        sprintf(time_format_start, "--:--");
    else
        sprintf(time_format_start, "%s%d:%s%d",
                (spoor_time->start.tm_hour < 10) ?"0" :"", spoor_time->start.tm_hour,
                (spoor_time->start.tm_min < 10) ?"0" : "", spoor_time->start.tm_min);

    if (spoor_time->end.tm_hour == -1 || spoor_time->end.tm_min == -1)
        sprintf(time_format_end, "--:--");
    else
        sprintf(time_format_end, "%s%d:%s%d",
                (spoor_time->end.tm_hour < 10) ?"0" :"", spoor_time->end.tm_hour,
                (spoor_time->end.tm_min < 10) ?"0" : "", spoor_time->end.tm_min);

    sprintf(time_format, "%s%d.%s%d.%d %s-%s",
            (spoor_time->start.tm_mday < 10) ?"0" :"",
            spoor_time->start.tm_mday,
            (spoor_time->start.tm_mon + 1 < 10) ?"0" :"",
            spoor_time->start.tm_mon + 1,
            spoor_time->start.tm_year + 1900,
            time_format_start,
            time_format_end);
}

#if 0
#define UI_EMTY_C '#'
char *buffer_screen = NULL;
uint32_t buffer_size = 0;
uint32_t window_rows = 0;
uint32_t window_cols = 0;

void window_size_update(void)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    window_rows = w.ws_row;
    window_cols = w.ws_col;
}

void buffer_screen_resize(void)
{
    uint32_t window_rows_old = window_rows;
    uint32_t window_cols_old = window_cols;

    window_size_update();

    buffer_size = window_rows * window_cols;
    buffer_screen = realloc(buffer_screen, buffer_size * sizeof(*buffer_screen));
    if (buffer_screen == NULL)
        exit(1);
    if (window_rows_old == 0 && window_cols_old == 0)
        return;

    uint32_t diff = window_cols_old - window_cols;
    char line[window_cols_old];
    memcpy(line, buffer_screen + 1 * window_cols_old, window_cols_old);
    memcpy(buffer_screen + 1 * window_cols, line, window_cols_old);
    if (diff < 0)
        memset(buffer_screen + 1 * window_cols_old, UI_EMTY_C, window_cols - window_cols_old);

    int bytes;
    memset(buffer_screen + (window_rows - 2) * window_cols, '-', 30 + 18 + 24 + 2 * 13 + 10);
    bytes = sprintf(buffer_screen + (window_rows - 1) * window_cols, "-- Elements: [%d - %d](%d)", 1, 26, 0);
    buffer_screen[bytes] = UI_EMTY_C;
}
void ui_render(void)
{
    cursor_move(0, 0);
    screen_clear();
    uint32_t i;
    for (i = 0; i < buffer_size; i++)
        putchar(buffer_screen[i]);
}
#endif

int spoor_time_compare(struct tm *time1, struct tm *time2)
{
    if (time1->tm_year > time2->tm_year)
    {
        return 1;
    }
    else if (time1->tm_year < time2->tm_year)
    {
        return -1;
    }
    else if (time1->tm_year == time2->tm_year)
    {
        if (time1->tm_mon > time2->tm_mon)
            return 1;
        else if (time1->tm_mon < time2->tm_mon)
            return -1;
        else if (time1->tm_mon == time2->tm_mon)
        {
            if (time1->tm_mday > time2->tm_mday)
                return 1;
            else if (time1->tm_mday < time2->tm_mday)
                return -1;
            else if (time1->tm_mday == time2->tm_mday)
            {
                if (time1->tm_hour > time2->tm_hour)
                    return 1;
                else if (time1->tm_hour < time2->tm_hour)
                    return -1;
                else if (time1->tm_hour == time2->tm_hour)
                {
                    if (time1->tm_min > time2->tm_min)
                        return 1;
                    else if (time1->tm_min < time2->tm_min)
                        return -1;
                    else if (time1->tm_min == time2->tm_min)
                    {
                        return 0;
                    }
                }
            }
        }
    }

    return 0;
}

void spoor_ui_object_show(SpoorDatabase *spoor_database)
{
#define UI_PRINT_LINE_PER_LINE
#define SORT_ARRAY
    /* load spoor objects */
    SpoorObject *spoor_objects = NULL;

    uint32_t error;

    error = mdb_txn_begin(spoor_database->env, NULL, 0, &spoor_database->txn);
    mdb_func_error_check(error, "mdb_txn_begin()", __LINE__, __FILE__);

    error = mdb_dbi_open(spoor_database->txn, NULL, 0, &spoor_database->dbi);
    mdb_func_error_check(error, "mdb_dbi_open()", __LINE__, __FILE__);

    error = mdb_cursor_open(spoor_database->txn, spoor_database->dbi, &spoor_database->cursor);
    mdb_func_error_check(error, "mdb_cursor_open()", __LINE__, __FILE__);


#ifdef SORT_ARRAY
    uint32_t spoor_object_count = 0;
#endif
    while (!mdb_cursor_get(spoor_database->cursor, &spoor_database->val_key, &spoor_database->val_data, MDB_NEXT))
    {
        char buffer_key[spoor_database->val_key.mv_size + 1];
        memcpy(buffer_key, (char *)spoor_database->val_key.mv_data, spoor_database->val_key.mv_size);
        buffer_key[spoor_database->val_key.mv_size] = 0;
#ifdef SORT_ARRAY
        spoor_object_count++;
#else

        SpoorObject *spoor_object = malloc(sizeof(*spoor_object));
        *spoor_object = *(SpoorObject *)spoor_database->val_data.mv_data;

        /* append */
        spoor_object->next = spoor_objects;
        spoor_objects = spoor_object;
#endif

    }

    mdb_dbi_close(spoor_database->env, spoor_database->dbi);
    error = mdb_txn_commit(spoor_database->txn);
    mdb_func_error_check(error, "mdb_txn_commit", __LINE__, __FILE__);

#if 0
    /* print all spoor_objects */
    SpoorObject *ptr = spoor_objects;
    while (ptr)
    {
        spoor_debug_spoor_object_print(ptr);
        ptr = ptr->next;
    }

#else

    /* soart by deadline */
#ifdef SORT_ARRAY
#if 1
    spoor_objects = malloc(spoor_object_count * sizeof(*spoor_objects));

    error = mdb_txn_begin(spoor_database->env, NULL, 0, &spoor_database->txn);
    mdb_func_error_check(error, "mdb_txn_begin()", __LINE__, __FILE__);

    error = mdb_dbi_open(spoor_database->txn, NULL, 0, &spoor_database->dbi);
    mdb_func_error_check(error, "mdb_dbi_open()", __LINE__, __FILE__);

    error = mdb_cursor_open(spoor_database->txn, spoor_database->dbi, &spoor_database->cursor);
    mdb_func_error_check(error, "mdb_cursor_open()", __LINE__, __FILE__);


    uint32_t i = 0;
    while (!mdb_cursor_get(spoor_database->cursor, &spoor_database->val_key, &spoor_database->val_data, MDB_NEXT))
    {
        spoor_objects[i] = *(SpoorObject *)spoor_database->val_data.mv_data;
        i++;
    }

    mdb_dbi_close(spoor_database->env, spoor_database->dbi);
    error = mdb_txn_commit(spoor_database->txn);
    mdb_func_error_check(error, "mdb_txn_commit", __LINE__, __FILE__);

    /* sort */
    SpoorObject temp;
    int elements = spoor_object_count;
    while (elements--)
    {
        for (i = 1; i <= elements; i++)
        {
            if (spoor_time_compare(&spoor_objects[i - 1].deadline.start, &spoor_objects[i].deadline.start) == 1)
            {
                temp = spoor_objects[i - 1];
                spoor_objects[i - 1] = spoor_objects[i];
                spoor_objects[i] = temp;
            }
        }
    }
#endif

#else
#if 0
    SpoorObject *ptr = spoor_objects;
    SpoorObject *parent = NULL;
    SpoorObject *point = spoor_objects;
    while (ptr)
    {
        while (ptr->next != NULL)
        {
            if (spoor_time_compare(&ptr->deadline.start, &ptr->next->deadline.start) == -1)
            {
                if (parent)
                {
                    if (parent->next == point)
                        point = ptr->next;

                    parent->next = ptr->next;
                    ptr->next = parent->next->next;
                    parent->next->next = ptr;

                    parent = parent->next;
                    continue;
                }
                else
                {
                    spoor_objects = ptr->next;
                    ptr->next = spoor_objects->next;
                    spoor_objects->next = ptr;

                    parent = spoor_objects;
                    point = parent;
                    continue;
                }
            }

            parent = ptr;
            ptr = ptr->next;
        }

        ptr = point->next;
        point = ptr;
    }
#endif
#endif


#if 1
    struct termios termios_old, termios_new;

    tcgetattr(STDIN_FILENO, &termios_old);
    termios_new = termios_old;
    termios_new.c_lflag &= ~(ICANON | ECHO);
    termios_new.c_cc[VMIN] = 1;
    termios_new.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_new);

#endif

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    uint32_t window_rows = w.ws_row;
    uint32_t window_cols = w.ws_col;

#define UI_EMTY_C ' '
    uint32_t buffer_size = window_rows * window_cols;
    char *buffer_screen = malloc(buffer_size * sizeof(*buffer_screen));
    memset(buffer_screen, UI_EMTY_C, buffer_size);



    cursor_hide();
    while (1)
    {
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        window_rows = w.ws_row;
        window_cols = w.ws_col;
#ifdef UI_PRINT_LINE_PER_LINE
        cursor_move(0, 0);
        screen_clear();
        /* print table title */
        fprintf(stdout, "%-30s%-18s%-24s%-13s%-13s%s\n", "TITLE", "DEADLINE", "SCHEDULE", "STATUS", "TYPE", "PROJECT");
        cursor_move(0, 2);
        fprintf(stdout, "---------------------------------------------------------------------------------------------------------");

        /* print spoor_objects */
        /* fill buffer with spoor_objects */
        char time_format_deadline[50];
        char time_format_schedule[50];

        uint32_t i;
        uint32_t position;
        for (i = 0, position = 3; i < spoor_object_count; i++, position++)
        {
            time_format_parse_deadline(&spoor_objects[i].deadline, time_format_deadline);
            time_format_parse_schedule(&spoor_objects[i].schedule, time_format_schedule);
            cursor_move(0, position);
            fprintf(stdout,
                    "%-30s%-18s%-24s%-13s%-13s%s",
                    spoor_objects[i].title,
                    time_format_deadline,
                    time_format_schedule,
                    UI_STATUS[spoor_objects[i].status],
                    UI_TYPES[spoor_objects[i].type],
                    spoor_objects[i].parent_title);
        }

        /* print status bar */
        cursor_move(0, window_rows - 1);
        fprintf(stdout, "---------------------------------------------------------------------------------------------------------");
        cursor_move(0, window_rows);
        fprintf(stdout, "-- Elements: [%d - %d](%d)", 1, 26, spoor_object_count);

#else
        /* fill buffer title */
        bytes = sprintf(buffer_screen, "%-30s%-18s%-24s%-13s%-13s%s", "TITLE", "DEADLINE", "SCHEDULE", "STATUS", "TYPE", "PROJECT");
        buffer_screen[bytes] = UI_EMTY_C;
        memset(buffer_screen + 1 * window_cols, '-', 30 + 18 + 24 + 2 * 13 + 10);

        /* fill buffer with spoor_objects */
        char time_format_deadline[50];
        char time_format_schedule[50];

        uint32_t bytes;
#ifdef SORT_ARRAY
        uint32_t i;
        for (i = 0; i < spoor_object_count; i++)
        {
            time_format_parse_deadline(&spoor_objects[i].deadline, time_format_deadline);
            time_format_parse_schedule(&spoor_objects[i].schedule, time_format_schedule);

            bytes = sprintf(buffer_screen + (2 + i) * window_cols,
                    "%-30s%-18s%-24s%-13s%-13s%s",
                    spoor_objects[i].title,
                    time_format_deadline,
                    time_format_schedule,
                    UI_STATUS[spoor_objects[i].status],
                    UI_TYPES[spoor_objects[i].type],
                    spoor_objects[i].parent_title);
            buffer_screen[(2 + i) * window_cols + bytes] = UI_EMTY_C;

        }
#else
        SpoorObject *ptr = spoor_objects;
        uint32_t i = 0;
        while (ptr)
        {
            time_format_parse_deadline(&ptr->deadline, time_format_deadline);
            time_format_parse_schedule(&ptr->schedule, time_format_schedule);

            bytes = sprintf(buffer_screen + (2 + i) * window_cols,
                    "%-30s%-18s%-24s%-13s%-13s%s",
                    ptr->title,
                    time_format_deadline,
                    time_format_schedule,
                    UI_STATUS[ptr->status],
                    UI_TYPES[ptr->type],
                    ptr->parent_title);
            buffer_screen[(2 + i) * window_cols + bytes] = UI_EMTY_C;

            ptr = ptr->next;
            i++;
        }
#endif

        /* status bar fill */
        memset(buffer_screen + (window_rows - 2) * window_cols, '-', 30 + 18 + 24 + 2 * 13 + 10);
        bytes = sprintf(buffer_screen + (window_rows - 1) * window_cols, "-- Elements: [%d - %d](%d)", 1, 26, 0);
        buffer_screen[bytes] = UI_EMTY_C;

        /* render */
        cursor_move(0, 0);
        screen_clear();
        for (i = 0; i < buffer_size; i++)
            putchar(buffer_screen[i]);
#endif

        /* key input */
        uint32_t c = getchar();
        if (c == 0x1B || c == 'q')
        {
            cursor_move(0, 0);
            screen_clear();
            break;
        }
    }
    cursor_show();


#if 1
    printf("\e[m");
    fflush(stdout);
    tcsetattr(STDIN_FILENO, TCSANOW, &termios_old);
#endif
#endif
}
