/**
 * @name main.c
 * @brief Multiple processes programming project
 * @authors Marián Tarageľ
 * @date 18.4.2022
 */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "main.h"
#include "main.h"

int *a = NULL;

void handle_error(char *error) {
    perror(error);
    exit(EXIT_FAILURE);
}

int generate_random_number(int max) {
    srand(getpid());
    int num = rand() % (max + 1);
    return num;
}

void create_molecule()
{
    sem_unlink("xtarag01_sem_create_molecule");
    sem_t *sem_molecule = sem_open("xtarag01_sem_create_molecule", O_CREAT, 0660, 0);
    if (sem_molecule == SEM_FAILED) {
        /* code */
    }
    

}

void oxygen(int idO, int max_sleep) {
    sem_unlink("xtarag01_sem_count_actions");
    sem_t *sem_actions = sem_open("xtarag01_sem_count_actions", O_CREAT, 0660, 1);
    if (sem_actions == SEM_FAILED) {
        handle_error("sem_open/xtarag01_sem_count_actions");
        exit(EXIT_FAILURE);
    }
    
    sem_wait(sem_actions);
    *a += 1;
    printf("%d: O %d: started\n", *a, idO);
    sem_post(sem_actions);

    int num = generate_random_number(max_sleep);
    usleep(num);

    sem_wait(sem_actions);
    *a += 1;
    printf("%d: O %d: going to queue\n", *a, idO);
    sem_post(sem_actions);
    
    sem_wait(sem_actions);
    *a += 1;
    printf("%d: O %d: ended\n", *a, idO);
    sem_post(sem_actions);

    sem_close(sem_actions);
    sem_unlink("xtarag01_sem_count_actions");
    exit(0);
}

void hydrogen(int idH, int max_sleep)
{
    sem_unlink("xtarag01_sem_count_actions");
    sem_t *sem_actions = sem_open("xtarag01_sem_count_actions", O_CREAT, 0660, 1);
    if (sem_actions == SEM_FAILED) {
        handle_error("sem_open/xtarag01_sem_count_actions");
        exit(EXIT_FAILURE);
    }

    sem_wait(sem_actions);
    *a += 1;
    printf("%d: H %d: started\n", *a, idH);
    sem_post(sem_actions);

    int num = generate_random_number(max_sleep);
    usleep(num);
    
    sem_wait(sem_actions);
    *a += 1;
    printf("%d: H %d: going to queue\n", *a, idH);
    sem_post(sem_actions);

    create_molecule();

    sem_wait(sem_actions);
    *a += 1;
    printf("%d: H %d: ended\n", *a, idH);
    sem_post(sem_actions);

    sem_close(sem_actions);
    sem_unlink("xtarag01_sem_count_actions");
    exit(0);
}

void create_process(int number, void (*function)(int, int), int max_sleep)
{
    pid_t child;
    for (int i = 0; i < number; i++) {
        child = fork();
        if (child == 0) {
            (*function)(i + 1, max_sleep);
        }
        else if (child == -1) {
            handle_error("fork");
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
               handle_error("strtol");
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

    a = mmap(NULL, sizeof(a), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (a == MAP_FAILED) {
        handle_error("mmap");
    }
    else {
        *a = 0;
    }

    pid_t pid;
    int status = 0;

    create_process(args[0], oxygen, args[2]);
    create_process(args[1], hydrogen, args[2]);

    for (int i = 0; i < (args[0] + args[1]); i++) {
        pid = wait(&status);
        if (pid == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        else if (status == 1) {
            exit(EXIT_FAILURE);
        }
        
    }

    if (munmap(a, sizeof(a)) == -1) {
        handle_error("munmap");
    }

    printf("End: Main process\n");

    return EXIT_SUCCESS;
}