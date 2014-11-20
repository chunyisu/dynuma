/*
dyNUMA_Collector.h 
*/

#ifndef DYNUMA_COLLECTOR_H
#define DYNUMA_COLLECTOR_H

#include <time.h>
#include <sys/time.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include "../global.h"
#include "../dyNUMA_Utilities/dyNUMA_PAPI.h"
#include "../dyNUMA_Utilities/dyNUMA_FuncPointer.h"


INT dyNUMA_cltr_init(void);
INT dyNUMA_cltr_ld_collect_patterns(void);
INT dyNUMA_cltr_start(REGION);
INT dyNUMA_cltr_stop(REGION);
INT dyNUMA_cltr_data_postprocess();
INT dyNUMA_cltr_time(struct timeval *t);
INT dyNUMA_cltr_time_diff(struct timeval *start, struct timeval* end,DOUBLE *diff);
INT dyNUMA_cltr_get_region_time(INT lid, REGION rid, DOUBLE *time );
INT dyNUMA_cltr_print_region_time(INT lid, REGION rid);
INT dyNUMA_cltr_data_postprocess();
INT dyNUMA_cltr_get_threads_position(INT *thread_pos);
INT count_events();	
#endif
