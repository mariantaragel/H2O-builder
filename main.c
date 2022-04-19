/**
 * @name main.c
 * @brief Multiple processes programming project
 * @authors Marián Tarageľ
 * @date 19.4.2022
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

int *count_actions = NULL;
int *oxygens = NULL;
int *hydrogens = NULL;

void handle_error(char *error)
{
    perror(error);
    exit(EXIT_FAILURE);
}

void create_shared_memory()
{
    count_actions = mmap(NULL, sizeof(count_actions), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (count_actions == MAP_FAILED) {
        handle_error("mmap");
    }
    else {
        *count_actions = 0;
    }

    oxygens = mmap(NULL, sizeof(oxygens), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (oxygens == MAP_FAILED) {
        handle_error("mmap");
    }
    else {
        *oxygens = 0;
    }

    hydrogens = mmap(NULL, sizeof(oxygens), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (hydrogens == MAP_FAILED) {
        handle_error("mmap");
    }
    else {
        *hydrogens = 0;
    }
}

void delete_shared_memory()
{
    if (munmap(hydrogens, sizeof(hydrogens)) == -1) {
        handle_error("munmap");
    }

    if (munmap(oxygens, sizeof(oxygens)) == -1) {
        handle_error("munmap");
    }

    if (munmap(count_actions, sizeof(count_actions)) == -1) {
        handle_error("munmap");
    }
}

int generate_random_number(int max)
{
    srand(getpid());
    int rand_num = rand() % (max + 1);
    return rand_num;
}

sem_t* create_semaphores(char *name, int value)
{
    sem_unlink(name);
    sem_t *semaphore = sem_open(name, O_CREAT, 0660, value);
    if (semaphore == SEM_FAILED) {
        handle_error("sem_open");
        exit(EXIT_FAILURE);
    }
    return semaphore;
}

void delete_semaphores(sem_t *semaphore, char *name)
{
    sem_close(semaphore);
    sem_unlink(name);
}

void oxygen(int idO, int max_sleep)
{
    sem_t *sem_actions = create_semaphores(SEM_ACTIONS, 1);

    sem_wait(sem_actions);
    *count_actions += 1;
    printf(STARTED, *count_actions, idO);
    sem_post(sem_actions);

    int num = generate_random_number(max_sleep);
    usleep(num);

    //oxygen_create_molecule(idO);

    sem_wait(sem_actions);
    *count_actions += 1;
    printf(ENDED, *count_actions, idO);
    sem_post(sem_actions);

    delete_semaphores(sem_actions, SEM_ACTIONS);
    exit(0);
}

void hydrogen(int idH, int max_sleep)
{
    sem_t *sem_actions = create_semaphores(SEM_ACTIONS, 1);

    sem_wait(sem_actions);
    *count_actions += 1;
    printf(STARTED, *count_actions, idH);
    sem_post(sem_actions);

    int num = generate_random_number(max_sleep);
    usleep(num);

    //hydrogen_create_molecule(idH);

    sem_wait(sem_actions);
    *count_actions += 1;
    printf(ENDED, *count_actions, idH);
    sem_post(sem_actions);

    delete_semaphores(sem_actions, SEM_ACTIONS);
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
        fprintf(stderr, usage);
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
            fprintf(stderr, NO_DIGITS, i);
            return EXIT_FAILURE;
        }

        if ((i == 3 || i == 4) && (args[i - 1] < 0 || args[i - 1] > 1000)) {
            fprintf(stderr, OUT_OF_RANGE, i);
            return EXIT_FAILURE;
        }
    }

    create_shared_memory();

    create_process(args[0], oxygen, args[2]);
    create_process(args[1], hydrogen, args[2]);

    pid_t pid;
    int status = 0;
    int number_of_process = (int)args[0] + (int)args[1];
    
    for (int i = 0; i < number_of_process; i++) {
        pid = wait(&status);
        if (pid == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        else if (status == 1) {
            exit(EXIT_FAILURE);
        }
    }

    delete_shared_memory();

    printf("End: Main process\n");

    return EXIT_SUCCESS;
}