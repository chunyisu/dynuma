#ifndef DYNUMA_H
#define DYNUMA_H
#include "global.h"
INT dyNUMA_init(INT mode);
INT dyNUMA_start(INT mode, REGION rid);
INT dyNUMA_stop(INT mode, REGION rid);
INT dyNUMA_finish(INT mode);
INT dyNUMA_init_fp();
INT dyNUMA_next_iter();
INT dyNUMA_report();
#endif  
