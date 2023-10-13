#include "types.h"
#include "user.h"

int main()
{
    int pid = 2; // sh

    printf(1, "pcbclear(%d)\n", pid);
    pcbclear(pid);

    exit();
}