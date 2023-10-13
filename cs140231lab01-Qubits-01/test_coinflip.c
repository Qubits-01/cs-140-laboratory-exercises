#include "types.h"
#include "user.h"

int main()
{
    printf(1, "NOTE: test_coinflip should only be executed once per boot\n");
    printf(2, "because of the way the pseudorandom number generator works.\n");

    // NOTE:
    // The fist 25 results of the pseudorandom number generator is as follows,
    // 11011 11011 11111 10010 00101 (from left to right),
    // where 0 means the syscall will fail, and 1 means the syscall will succeed.

    // TEST CASE 1.
    // Will run test_hello 10 times to show that the coinflip effect is not yet
    // active on this said syscall.
    // These hello syscalls should all run successfully 10 times.
    printf(1, "\n[ TEST CASE 1 ]\n");
    printf(1, "[ hello should SUCCEED 10 times ]\n");
    for (int i = 0; i < 10; i++)
    {
        hello();
    }

    // TEST CASE 2.
    // Will run test_getname 10 times to show that the coinflip effect is not yet
    // active on this said syscall.
    // These test_getname syscalls should all run successfully 10 times.
    printf(1, "\n[ TEST CASE 2 ]\n");
    printf(1, "[ getname should SUCCEED 10 times ]\n");

    char buffer[100];
    int length;

    for (int i = 0; i < 10; i++)
    {
        length = getname(buffer);
        printf(1, "Process name is: %s\n", buffer);
        printf(1, "Name is %d characters long\n", length);
    }

    // TEST CASE 3.
    printf(1, "\n[ TEST CASE 3 ]\n");
    printf(1, "[ put coinflip effect on hello (s.n. 24) ]\n");
    printf(1, "[ hello should SUCCEED (1st random usage) ]\n");

    coinflip(24); // hello
    hello();

    // TEST CASE 4.
    printf(1, "\n[ TEST CASE 4 ]\n");
    printf(1, "[ put coinflip effect on getname (s.n. 25) ]\n");
    printf(1, "[ getname should SUCCEED (2nd random usage) ]\n");

    coinflip(25); // getname
    length = getname(buffer);
    printf(1, "Process name is: %s\n", buffer);
    printf(1, "Name is %d characters long\n", length);

    // TEST CASE 5.
    printf(1, "\n[ TEST CASE 5 ]\n");
    printf(1, "[ put coinflip effect on coinflip (s.n. 27) ]\n");
    printf(1, "[ put coinflip effect on shutdown (s.n. 23) ]\n");
    printf(1, "[ coinflip should FAIL (3rd random usage) ]\n");

    coinflip(27); // coinflip
    coinflip(23); // shutdown

    // TEST CASE 6.
    printf(1, "\n[ TEST CASE 6 ]\n");
    printf(1, "[ put coinflip effect on shutdown again (s.n. 23) ]\n");
    printf(1, "[ coinflip should SUCCEED (4th random usage) ]\n");

    coinflip(23); // shutdown

    // TEST CASE 7.
    printf(1, "\n[ TEST CASE 7 ]\n");
    printf(1, "[ getname should SUCCEED (5th random usage) ]\n");

    length = getname(buffer);
    printf(1, "Process name is: %s\n", buffer);
    printf(1, "Name is %d characters long\n", length);

    // TEST CASE 8.
    printf(1, "\n[ TEST CASE 8 ]\n");
    printf(1, "[ hello should SUCCEED (6th random usage) ]\n");

    hello();

    // TEST CASE 9.
    printf(1, "\n[ TEST CASE 9 ]\n");
    printf(1, "[ hello should SUCCEED (7th random usage) ]\n");

    hello();

    // TEST CASE 10.
    printf(1, "\n[ TEST CASE 10 ]\n");
    printf(1, "[ shutdown should FAIL (8th random usage) ]\n");

    shutdown();

    // TEST CASE 11.
    printf(1, "\n[ TEST CASE 11 ]\n");
    printf(1, "[ appendname should SUCCEED (9th random usage) ]\n");

    appendname("XY");

    // TEST CASE 12.
    printf(1, "\n[ TEST CASE 12 ]\n");
    printf(1, "[ shutdown should SUCCEED (10th random usage) ]\n");

    shutdown();

    exit();
}