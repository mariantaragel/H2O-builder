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
int *count_molecules = NULL;

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

    count_molecules = mmap(NULL, sizeof(count_molecules), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (count_molecules == MAP_FAILED) {
        handle_error("mmap");
    }
    else {
        *count_molecules = 0;
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

    if (munmap(count_molecules, sizeof(count_molecules)) == -1) {
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
    sem_t *semaphore = sem_open(name, O_CREAT, 0666, value);
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

void oxygen_create_molecule(int idO, int max_time_of_creation)
{
    sem_t *sem_actions = create_semaphores(SEM_ACTIONS, 1);
    sem_t *sem_create_molecule = create_semaphores(SEM_CREATE_MOL, 0);

    *count_molecules += 1;

    sem_wait(sem_actions);
    *count_actions += 1;
    printf(CREATING_MOL_O, *count_actions, idO, *count_molecules);
    sem_post(sem_actions);

    int num = generate_random_number(max_time_of_creation);
    usleep(num);

    sem_post(sem_create_molecule);
    sem_post(sem_create_molecule);

    sem_wait(sem_actions);
    *count_actions += 1;
    printf(CREATED_MOL_O, *count_actions, idO, *count_molecules);
    sem_post(sem_actions);

    delete_semaphores(sem_create_molecule, SEM_CREATE_MOL);
    delete_semaphores(sem_actions, SEM_ACTIONS);
}

void hydrogen_create_molecule(int idH)
{
    sem_t *sem_actions = create_semaphores(SEM_ACTIONS, 1);
    sem_t *sem_create_molecule = create_semaphores(SEM_CREATE_MOL, 0);

    sem_wait(sem_actions);
    *count_actions += 1;
    printf(CREATING_MOL_H, *count_actions, idH, *count_molecules);
    sem_post(sem_actions);

    sem_wait(sem_create_molecule);
    sem_wait(sem_create_molecule);

    sem_wait(sem_actions);
    *count_actions += 1;
    printf(CREATED_MOL_H, *count_actions, idH, *count_molecules);
    sem_post(sem_actions);

    delete_semaphores(sem_create_molecule, SEM_CREATE_MOL);
    delete_semaphores(sem_actions, SEM_ACTIONS);
}

void oxygen(int idO, long args[])
{
    sem_t *sem_actions = create_semaphores(SEM_ACTIONS, 1);
    sem_t *sem_mutex = create_semaphores(SEM_ACTIONS, 1);
    sem_t *sem_oxygen_queue = create_semaphores(SEM_OXY_QUEUE, 0);
    sem_t *sem_hydrogen_queue = create_semaphores(SEM_HYDRO_QUEUE, 0);

    sem_wait(sem_actions);
    *count_actions += 1;
    printf(STARTED_O, *count_actions, idO);
    sem_post(sem_actions);

    int num = generate_random_number((int)args[2]);
    usleep(num);

    sem_wait(sem_mutex);
    *oxygens += 1;
    if ((*hydrogens >= 2) && (*oxygens >= 1)) {
        sem_post(sem_hydrogen_queue);
        sem_post(sem_hydrogen_queue);
        *hydrogens -= 2;
        sem_post(sem_oxygen_queue);
        *oxygens -= 1;
    }
    sem_post(sem_mutex);
    
    sem_wait(sem_actions);
    *count_actions += 1;
    printf(QUEUE_O, *count_actions, idO);
    sem_post(sem_actions);

    //sem_wait(sem_oxygen_queue);

    //oxygen_create_molecule(idO, (int)args[3]);

    sem_wait(sem_actions);
    *count_actions += 1;
    printf(ENDED_O, *count_actions, idO);
    sem_post(sem_actions);

    delete_semaphores(sem_oxygen_queue, SEM_OXY_QUEUE);
    delete_semaphores(sem_hydrogen_queue, SEM_HYDRO_QUEUE);
    delete_semaphores(sem_mutex, SEM_MUTEX);
    delete_semaphores(sem_actions, SEM_ACTIONS);
    exit(0);
}

void hydrogen(int idH, long args[])
{
    sem_t *sem_actions = create_semaphores(SEM_ACTIONS, 1);
    sem_t *sem_mutex = create_semaphores(SEM_ACTIONS, 1);
    sem_t *sem_oxygen_queue = create_semaphores(SEM_OXY_QUEUE, 0);
    sem_t *sem_hydrogen_queue = create_semaphores(SEM_HYDRO_QUEUE, 0);

    sem_wait(sem_actions);
    *count_actions += 1;
    printf(STARTED_H, *count_actions, idH);
    sem_post(sem_actions);

    int num = generate_random_number((int)args[2]);
    usleep(num);

    sem_wait(sem_mutex);
    *hydrogens += 1;
    if ((*hydrogens >= 2) && (*oxygens >= 1)) {
        sem_post(sem_hydrogen_queue);
        sem_post(sem_hydrogen_queue);
        *hydrogens -= 2;
        sem_post(sem_oxygen_queue);
        *oxygens -= 1;
    }
    sem_post(sem_mutex);
    
    sem_wait(sem_actions);
    *count_actions += 1;
    printf(QUEUE_H, *count_actions, idH);
    sem_post(sem_actions);

    //sem_wait(sem_hydrogen_queue);

    //hydrogen_create_molecule(idH);

    sem_wait(sem_actions);
    *count_actions += 1;
    printf(ENDED_H, *count_actions, idH);
    sem_post(sem_actions);

    delete_semaphores(sem_oxygen_queue, SEM_OXY_QUEUE);
    delete_semaphores(sem_hydrogen_queue, SEM_HYDRO_QUEUE);
    delete_semaphores(sem_mutex, SEM_MUTEX);
    delete_semaphores(sem_actions, SEM_ACTIONS);
    exit(0);
}

void create_process(int number, void (*function)(int, long *), long args[])
{
    pid_t child;
    for (int i = 0; i < number; i++) {
        child = fork();
        if (child == 0) {
            (*function)(i + 1, args);
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

    create_process(args[0], oxygen, args);
    create_process(args[1], hydrogen, args);

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