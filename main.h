/**
 * @name main.h
 * @brief Interface of h2o-builder program
 * @authors Marián Tarageľ
 * @date 24.4.2022
 */

#ifndef _MAIN_H
#define _MAIN_H

const char usage[] = "Usage: ./proj2 NO NH TI TB\n"
    "   - NO: number of oxygens\n"
    "   - NH: number of hydrogens\n"
    "   - TI: time of process sleeping\n"
    "   - TB: time of molecule cretion\n";

#define NOT_A_NUMBER "NaN found in argument %d\n"
#define OUT_OF_RANGE "Argument %d out of range\n"
#define NEGATIVE_NUMBER "Argument can not be negative number\n"

#define SEM_ACTIONS "xtarag01_sem_count_actions"
#define SEM_MUTEX "xtarag01_sem_mutex"
#define SEM_OXY_QUEUE "xtarag01_sem_oxygen_queue"
#define SEM_HYDRO_QUEUE "xtarag01_sem_hydrogen_queue"
#define SEM_CREATE_MOL_O "xtarag01_sem_create_molecule_o"
#define SEM_CREATE_MOL_H "xtarag01_sem_create_molecule_h"
#define SEM_BARIER "xtarag01_sem_barier"

#define STARTED_O "%d: O %d: started\n"
#define QUEUE_O "%d: O %d: going to queue\n"
#define CREATING_MOL_O "%d: O %d: creating molecule %d\n"
#define CREATED_MOL_O "%d: O %d: molecule %d created\n"
#define NOT_ENOUGH_O "%d: O %d: not enough H\n"
#define ENDED_O "%d: O %d: ended\n"

#define STARTED_H "%d: H %d: started\n"
#define QUEUE_H "%d: H %d: going to queue\n"
#define CREATING_MOL_H "%d: H %d: creating molecule %d\n"
#define CREATED_MOL_H "%d: H %d: molecule %d created\n"
#define NOT_ENOUGH_H "%d: H %d: not enough O or H\n"

/**
 * @brief Intialize file and semaphores
 */
void init();

/**
 * @brief Clean opened file and semaphores
 */
void clean();

/**
 * @brief Handle error messages
 * 
 * @param error Name of function that failed
 */
void handle_error(char *error);

/**
 * @brief Create a shared memory
 */
void create_shared_memory();

/**
 * @brief Delete a shared memory
 */
void delete_shared_memory();

/**
 * @brief Generate random number
 * 
 * @param max Maximum value of random number
 * @return Random number
 */
int generate_random_number(int max);

/**
 * @brief Create a semaphores
 * 
 * @param name Name of a semaphore
 * @param value Intial value of semaphore
 * @return Semaphore
 */
sem_t* create_semaphores(char *name, int value);

/**
 * @brief Delete a semaphore files
 */
void delete_sem_files();

/**
 * @brief Function to determin, how many molecules can be created
 * 
 * @param oxy Number of oxygens
 * @param hyd Number of hydrogens
 * @return Number of molecules, that can be created
 */
int count_num_of_mol(int oxy, int hyd);

/**
 * @brief Function where hydrogen creates molecule
 * 
 * @param idO Identifier of oxygen
 * @param molecule_num Number of molecule that will be created
 * @param max_time_of_creation Maximum value of 
 */
void oxygen_create_molecule(int idO,int molecule_num, int max_time_of_creation);

/**
 * @brief Function where hydrogen creates molecule
 * 
 * @param idH Identifier of hydrogen
 * @param molecule_num Number of molecule that will be created
 */
void hydrogen_create_molecule(int idH, int molecule_num);

/**
 * @brief Main process of oxygen
 * 
 * @param idO Identifier of oxygen
 * @param args Arguments of command line
 */
void oxygen(int idO, long args[]);

/**
 * @brief Main process of hydrogen
 * 
 * @param idH Identifier of hydrogen
 * @param args Arguments of command line
 */
void hydrogen(int idH, long args[]);

/**
 * @brief Create a process
 * 
 * @param number Number of processes to create
 * @param function Function where process will countinue
 * @param args Arguments of command line
 */
void create_process(int number, void (*function)(int, long *), long args[]);

#endif