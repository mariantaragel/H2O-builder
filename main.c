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
#include <sys/stat.h>
#include "main.h"

int *count_actions = NULL;
int *oxygens = NULL;
int *hydrogens = NULL;
int *count_molecules = NULL;
int *num_of_ox = NULL;
int *num_of_hyd = NULL;
int *count = NULL;
FILE **file = NULL;

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

    count = mmap(NULL, sizeof(count), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (count == MAP_FAILED) {
        handle_error("mmap");
    }
    else {
        *count = 0;
    }

    num_of_hyd = mmap(NULL, sizeof(num_of_hyd), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (num_of_hyd == MAP_FAILED) {
        handle_error("mmap");
    }
    
    num_of_ox = mmap(NULL, sizeof(num_of_ox), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (num_of_ox == MAP_FAILED) {
        handle_error("mmap");
    }

    file = mmap(NULL, sizeof(file), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (file == MAP_FAILED) {
        handle_error("mmap");
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

    if (munmap(num_of_hyd, sizeof(num_of_hyd)) == -1) {
        handle_error("munmap");
    }

    if (munmap(num_of_ox, sizeof(num_of_ox)) == -1) {
        handle_error("munmap");
    }

    if (munmap(count, sizeof(count)) == -1) {
        handle_error("munmap");
    }

    if (munmap(file, sizeof(file)) == -1) {
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
    sem_t *semaphore = sem_open(name, O_CREAT, 0666, value);
    if (semaphore == SEM_FAILED) {
        handle_error("sem_open");
        exit(EXIT_FAILURE);
    }
    return semaphore;
}

void delete_semaphores(sem_t *semaphore)
{
    sem_close(semaphore);
}

void delete_sem_files()
{
    sem_unlink(SEM_ACTIONS);
    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_OXY_QUEUE);
    sem_unlink(SEM_HYDRO_QUEUE);
    sem_unlink(SEM_CREATE_MOL_H);
    sem_unlink(SEM_CREATE_MOL_O);
    sem_unlink(SEM_BARIER);
}

int count_num_of_mol(int oxy, int hyd)
{
    int result = 0;
	while(oxy >= 1 && hyd >= 2) {
		oxy -= 1;
		hyd -= 2;
		result += 1;
	}

    return result;
}

void oxygen_create_molecule(int idO,int molecule_num, int max_time_of_creation)
{
    sem_t *sem_actions = create_semaphores(SEM_ACTIONS, 1);
    sem_t *sem_create_molecule_o = create_semaphores(SEM_CREATE_MOL_O, 0);
    sem_t *sem_create_molecule_h = create_semaphores(SEM_CREATE_MOL_H, 0);

    sem_wait(sem_actions);
    *count_actions += 1;
    fprintf(*file, CREATING_MOL_O, *count_actions, idO, molecule_num);
    fflush(*file);
    sem_post(sem_actions);

    sem_wait(sem_create_molecule_o);
    sem_wait(sem_create_molecule_o);

    int num = generate_random_number(max_time_of_creation);
    usleep(num * 1000);

    sem_wait(sem_actions);
    *count_actions += 1;
    fprintf(*file, CREATED_MOL_O, *count_actions, idO, molecule_num);
    fflush(*file);
    sem_post(sem_actions);

    sem_post(sem_create_molecule_h);
    sem_post(sem_create_molecule_h);

    delete_semaphores(sem_create_molecule_o);
    delete_semaphores(sem_create_molecule_h);
    delete_semaphores(sem_actions);
}

void hydrogen_create_molecule(int idH, int molecule_num)
{
    sem_t *sem_actions = create_semaphores(SEM_ACTIONS, 1);
    sem_t *sem_create_molecule_h = create_semaphores(SEM_CREATE_MOL_H, 0);
    sem_t *sem_create_molecule_o = create_semaphores(SEM_CREATE_MOL_O, 0);

    sem_wait(sem_actions);
    *count_actions += 1;
    fprintf(*file, CREATING_MOL_H, *count_actions, idH, molecule_num);
    fflush(*file);
    sem_post(sem_actions);

    sem_post(sem_create_molecule_o);

    sem_wait(sem_create_molecule_h);

    sem_wait(sem_actions);
    *count_actions += 1;
    fprintf(*file, CREATED_MOL_H, *count_actions, idH, molecule_num);
    fflush(*file);
    sem_post(sem_actions);

    delete_semaphores(sem_create_molecule_o);
    delete_semaphores(sem_create_molecule_h);
    delete_semaphores(sem_actions);
}

void oxygen(int idO, long args[])
{
    sem_t *sem_actions = create_semaphores(SEM_ACTIONS, 1);
    sem_t *sem_mutex = create_semaphores(SEM_MUTEX, 1);
    sem_t *sem_oxygen_queue = create_semaphores(SEM_OXY_QUEUE, 0);
    sem_t *sem_hydrogen_queue = create_semaphores(SEM_HYDRO_QUEUE, 0);
    sem_t *sem_barier = create_semaphores(SEM_BARIER, 0);

    int molecules = count_num_of_mol(args[0], args[1]);

    sem_wait(sem_actions);
    *count_actions += 1;
    fprintf(*file, STARTED_O, *count_actions, idO);
    fflush(*file);
    sem_post(sem_actions);

    int num = generate_random_number((int)args[2]);
    usleep(num * 1000);

    sem_wait(sem_mutex);
    *oxygens += 1;
    if ((*hydrogens >= 2) && (*oxygens >= 1)) {
        *count_molecules += 1;
        sem_post(sem_hydrogen_queue);
        sem_post(sem_hydrogen_queue);
        *hydrogens -= 2;
        sem_post(sem_oxygen_queue);
        *oxygens -= 1;
    }
    else {
        sem_post(sem_mutex);
    }

    sem_wait(sem_actions);
    *count_actions += 1;
    fprintf(*file, QUEUE_O, *count_actions, idO);
    fflush(*file);
    sem_post(sem_actions);

    if (args[0] < 1 || args[1] < 2) {
        sem_wait(sem_actions);
        *count_actions += 1;
        fprintf(*file, NOT_ENOUGH_O, *count_actions, idO);
        fflush(*file);
        sem_post(sem_actions);
    }
    else {
        sem_wait(sem_oxygen_queue);
        if (*num_of_ox >= 1 && *num_of_hyd >= 2) {
            oxygen_create_molecule(idO, *count_molecules, args[3]);
            sem_wait(sem_barier);
            sem_wait(sem_barier);
            *num_of_hyd -= 2;
            *num_of_ox -= 1;
            if (molecules == *count_molecules) {
                for (int i = 0; i < (int)args[0] - *count_molecules; i++) {
                    sem_post(sem_oxygen_queue);
                }
                for (int i = 0; i < (int)args[1] - *count_molecules * 2; i++) {
                    sem_post(sem_hydrogen_queue);
                }
            }
        }
        else {
            sem_wait(sem_actions);
            *count_actions += 1;
            fprintf(*file, NOT_ENOUGH_O, *count_actions, idO);
            fflush(*file);
            sem_post(sem_actions);
        }
        sem_post(sem_mutex);
    }

    delete_semaphores(sem_barier);
    delete_semaphores(sem_oxygen_queue);
    delete_semaphores(sem_hydrogen_queue);
    delete_semaphores(sem_mutex);
    delete_semaphores(sem_actions);
    exit(0);
}

void hydrogen(int idH, long args[])
{
    sem_t *sem_actions = create_semaphores(SEM_ACTIONS, 1);
    sem_t *sem_mutex = create_semaphores(SEM_MUTEX, 1);
    sem_t *sem_oxygen_queue = create_semaphores(SEM_OXY_QUEUE, 0);
    sem_t *sem_hydrogen_queue = create_semaphores(SEM_HYDRO_QUEUE, 0);
    sem_t *sem_barier = create_semaphores(SEM_BARIER, 0);

    sem_wait(sem_actions);
    *count_actions += 1;
    fprintf(*file, STARTED_H, *count_actions, idH);
    fflush(*file);
    sem_post(sem_actions);

    int num = generate_random_number((int)args[2]);
    usleep(num * 1000);

    sem_wait(sem_mutex);
    *hydrogens += 1;
    if ((*hydrogens >= 2) && (*oxygens >= 1)) {
        *count_molecules += 1;
        sem_post(sem_hydrogen_queue);
        sem_post(sem_hydrogen_queue);
        *hydrogens -= 2;
        sem_post(sem_oxygen_queue);
        *oxygens -= 1;
    }
    else {
        sem_post(sem_mutex);
    }

    sem_wait(sem_actions);
    *count_actions += 1;
    fprintf(*file, QUEUE_H, *count_actions, idH);
    fflush(*file);
    sem_post(sem_actions);

    if (args[0] < 1 || args[1] < 2) {
        sem_wait(sem_actions);
        *count_actions += 1;
        fprintf(*file, NOT_ENOUGH_H, *count_actions, idH);
        fflush(*file);
        sem_post(sem_actions);
    }
    else {
        sem_wait(sem_hydrogen_queue);
        if (*num_of_ox >= 1 && *num_of_hyd >= 2) {
            hydrogen_create_molecule(idH, *count_molecules);
            sem_post(sem_barier);
        }
        else {
            sem_wait(sem_actions);
            *count_actions += 1;
            fprintf(*file, NOT_ENOUGH_H, *count_actions, idH);
            fflush(*file);
            sem_post(sem_actions);
        }
    }

    delete_semaphores(sem_barier);
    delete_semaphores(sem_oxygen_queue);
    delete_semaphores(sem_hydrogen_queue);
    delete_semaphores(sem_mutex);
    delete_semaphores(sem_actions);
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
    delete_sem_files();
    remove("proj2.out");
    *file = fopen("proj2.out", "w");

    *num_of_ox = (int)args[0];
    *num_of_hyd = (int)args[1];

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

    fclose(*file);
    delete_sem_files();
    delete_shared_memory();

    return EXIT_SUCCESS;
}