#ifndef MAIN_H
#define MAIN_H

#include "helper.h"
void openSem();
void createNurses(clinic *biontech);
void createVaccinators(clinic *biontech);
void createCitizens(clinic *biontech);
void createSignalHandlers();
void cleanProcess();
void removeAll();
clinic *getSharedMemory(args givenArgs);
#endif