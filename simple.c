#include <stdlib.h>
#include <string.h>









int main()
{
    char *text = strdup("terminal commands what the hello");

    printf("before: %s\n", text);

    memmove(text, text + 8, 8);

    printf("after: %s\n", text);

    return 0;
}