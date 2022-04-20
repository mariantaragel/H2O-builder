/**
 * @name main.h
 * @brief Interface of h2o-builder program
 * @authors Marián Tarageľ
 * @date 19.4.2022
 */

#ifndef _MAIN_H
#define _MAIN_H

const char usage[] = "Usage: ./proj2 NO NH TI TB\n"
    "   - NO: number of oxygens\n"
    "   - NH: number of hydrogens\n"
    "   - TI: time of process sleeping\n"
    "   - TB: time of molecule cretion\n";

#define NO_DIGITS "No digits were found in argument %d\n"
#define OUT_OF_RANGE "Argument %d out of range\n"

#define SEM_ACTIONS "xtarag01_sem_count_actions"
#define SEM_MUTEX "xtarag01_sem_mutex"
#define SEM_OXY_QUEUE "xtarag01_sem_oxygen_queue"
#define SEM_HYDRO_QUEUE "xtarag01_sem_hydrogen_queue"
#define SEM_CREATE_MOL "xtarag01_sem_create_molecule"

#define STARTED_O "%d: O %d: started\n"
#define QUEUE_O "%d: O %d: going to queue\n"
#define CREATING_MOL_O "%d: O %d: creating molecule %d\n"
#define CREATED_MOL_O "%d: O %d: molecule %d created\n"
#define ENDED_O "%d: O %d: ended\n"

#define STARTED_H "%d: H %d: started\n"
#define QUEUE_H "%d: H %d: going to queue\n"
#define CREATING_MOL_H "%d: H %d: creating molecule %d\n"
#define CREATED_MOL_H "%d: H %d: molecule %d created\n"
#define ENDED_H "%d: H %d: ended\n"

void handle_error(char *error);
void create_shared_memory();
void delete_shared_memory();
int generate_random_number(int max);
sem_t* create_semaphores(char *name, int value);
void delete_semaphores(sem_t *semaphore, char *name);

#endif