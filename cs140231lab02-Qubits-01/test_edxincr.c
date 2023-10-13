#include "types.h"
#include "user.h"

int main()
{
    int child = fork();

    if (child == 0)
    {
        asm("movb $48, %dl");

        while (1)
        {
            asm volatile("sub $2, %esp");
            asm volatile("movb %dl, 0(%esp)");
            asm volatile("movb $'\n', 1(%esp)");
            asm volatile("movl %esp, %eax");
            asm volatile("pushl $2");
            asm volatile("pushl %eax");
            asm volatile("pushl $1");
            asm volatile("pushl %esp");
            asm volatile("movl $16, %eax");
            asm volatile("int $64");
            asm volatile("add $18, %esp");
        }
    }
    else
    {
        char buffer[100];

        while (1)
        {
            gets(buffer, 100);
            if (buffer[0] == 'q')
                break;
            edxincr(child);
        }
    }

    exit();
}
