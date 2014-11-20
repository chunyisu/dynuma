#ifndef DYNUMA_PREDICTOR_MAPPING
#define DYNUMA_PREDICTOR_MAPPING
#include "../global.h"
#include "../dyNUMA_Trainer/dyNUMA_ANN.h"
void   dyNUMA_predictor_init_ann_mapping(CHAR * ANN_knowledge_file);
DOUBLE dyNUMA_mapping_predictor(REGION rid);
#endif
