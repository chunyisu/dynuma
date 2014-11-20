#ifndef DYNUMA_Trainer_H
#define DYNUMA_Trainer_H
#include "../global.h"
#include "dyNUMA_ANN.h"
#include "../dyNUMA_Utilities/dyNUMA_FuncPointer.h"

INT dyNUMA_trainer_init();
INT dyNUMA_trainer_start();
INT dyNUMA_trainer_process_training_data();
INT dyNUMA_trainer_begin();
INT dyNUMA_trainer_end();
INT dyNUMA_trainer_write_training_cofficients();
#endif
