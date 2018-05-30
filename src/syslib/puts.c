#include <stdio.h>
#include <cpm_sysfunc.h>

int puts(char *s)
{
    char *p = (const char *) s;
    while (p[0] != '\0') {
        cpm_putchar(p[0]);
        p++;
    }

    return 0;

}
