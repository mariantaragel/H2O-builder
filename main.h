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
#define STARTED "%d: O %d: started\n"
#define ENDED "%d: O %d: ended\n"

void handle_error(char *error);
void create_shared_memory();
void delete_shared_memory();
int generate_random_number(int max);
sem_t* create_semaphores(char *name, int value);
void delete_semaphores(sem_t *semaphore, char *name);

#endif