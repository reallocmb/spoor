#include<stdio.h>
#include<stdint.h>
#include<sys/ioctl.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<time.h>
#include<termios.h>

int main(int argc, char **argv)
{
    signal(SIGWINCH, signal_handle_resize_func); /* when terminal resize */

    struct termios termios_old, termios_new;

    tcgetattr(STDIN_FILENO, &termios_old);
    termios_new = termios_old;
    termios_new.c_lflag &= ~(ICANON | ECHO);
    termios_new.c_cc[VMIN] = 1;
    termios_new.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_new);


    char *buffer = NULL;
    while (1)
    {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        uint32_t buffer_size = w.ws_col * w.ws_row;
        buffer = realloc(buffer, buffer_size * sizeof(*buffer));
        memset(buffer, '#', buffer_size);


        fprintf(stdout, "\033[%d;%dH", 0, 0);
        fprintf(stdout, "\033[2J");
        uint32_t i;
        for (i = 0; i < buffer_size; i++)
            putchar(buffer[i]);

        getchar();
    }

    printf("\e[m");
    fflush(stdout);
    tcsetattr(STDIN_FILENO, TCSANOW, &termios_old);

    return 0;
}
