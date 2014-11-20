#ifndef DYNUMA_PREDICTOR_CONCURRENCY
#define DYNUMA_PREDICTOR_CONCURRENCY
#include "../global.h"
#include "../dyNUMA_Trainer/dyNUMA_ANN.h"
void   dyNUMA_predictor_init_ann_concurrency(CHAR * ANN_knowledge_file);
DOUBLE dyNUMA_concurrency_predictor(REGION rid);
#endif
