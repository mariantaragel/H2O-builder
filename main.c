/**
 * @name main.c
 * @brief 
 * @authors Marián Tarageľ
 * @date 17.4.2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
#include "main.h"
#include <sys/wait.h>
#include <sys/mman.h>

int *a = 0;

void oxygen(int idO) {
    *a++;
    printf("%d: O %d: started\n", a, idO);
    exit(0);
}

void hydrogen(int idH) {
    *a++;
    printf("%d: H %d: started\n", a, idH);
    exit(0);
}

void create_process(int number, void (*function)(int)) {
    pid_t child;
    for (int i = 0; i < number; i++) {
        child = fork();
        if (child == 0) {
            (*function)(i + 1);
        }
        else if (child == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }
    return;
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        fprintf(stderr, "Usage: ./proj2 NO NH TI TB\n");
        return EXIT_FAILURE;
    }

    char *endptr;
    long args[4];

    for(int i = 1; i < argc; i++) {
        errno = 0;
        args[i - 1] = strtol(argv[i], &endptr, 10);

        if ((errno == ERANGE && (args[i - 1] == LONG_MAX || args[i - 1] == LONG_MIN))
            || (errno != 0 && args[i - 1] == 0)) {
               perror("strtol");
               exit(EXIT_FAILURE);
        }

        if (endptr == argv[i]) {
            fprintf(stderr, "No digits were found in argument %d\n", i);
            return EXIT_FAILURE;
        }

        if ((i == 3 || i == 4) && (args[i - 1] < 0 || args[i - 1] > 1000)) {
            fprintf(stderr, "Argument %d out of range\n", i);
            return EXIT_FAILURE;
        }
    }

    pid_t pid;
    int status = 0;

    create_process(args[0], oxygen);
    create_process(args[1], hydrogen);

    pid = wait(&status);
    if (pid == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }

    printf("End: Main process\n");

    return EXIT_SUCCESS;
}