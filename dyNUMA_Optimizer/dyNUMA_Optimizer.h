#ifndef DYNUMA_OPTIMIZER
#define DYNUMA_OPTIMIZER
#include <unistd.h> 
#include <sched.h>
#include </usr/include/sched.h>
#include "../global.h"
#include "../dyNUMA_Utilities/dyNUMA_PAPI.h"
#include "../dyNUMA_Predictor/dyNUMA_Concurrency_Predictor.h"
#include "../dyNUMA_Predictor/dyNUMA_Mapping_Predictor.h"
#include "../dyNUMA_Collector/dyNUMA_Collector.h"

INT dyNUMA_optmzr_init();
INT dyNUMA_optmzr_start(REGION rid);
INT dyNUMA_optmzr_stop(REGION rid);
INT dyNUMA_optmzr_start_profile(REGION rid);
INT dyNUMA_optmzr_stop_profile(REGION rid);
INT dyNUMA_optmzr_start_optimize(REGION rid);
INT dyNUMA_optmzr_stop_optimize(REGION rid);
INT dyNUMA_optmzr_read_opt_config(REGION rid);
INT dyNUMA_optmzer_opt_concurrency(REGION rid);
INT dyNUMA_optmzer_opt_tma(REGION rid);
INT dyNUMA_optmzer_opt_schedule(REGION rid);
#endif
