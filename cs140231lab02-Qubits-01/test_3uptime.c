#include "types.h"
#include "user.h"

int main()
{
    char user_input[100];
    int pid;

    printf(1, "pid: ");
    gets(user_input, 100);
    pid = atoi(user_input);

    printf(1, "treeuptime(%d): %d\n", pid, treeuptime(pid));

    exit();
}