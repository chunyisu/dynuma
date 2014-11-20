#ifndef DYNUMA_UTILITIES_H
#define DYNUMA_UTILITIES_H
#include <float.h>
#include <string.h>
#include <math.h>
#include "../global.h"

#define MAXFLDS 200000 /* maximum possible number of fields */
#define MAXFLDSIZE 32 /* longest possible field + 1 = 31 byte field */

extern ENV env;//environment variables

INT dyNUMA_Utilities_read_env_vars();
// Data Post process
INT dyNUMA_Utilities_process_training_input();
INT dyNUMA_Utilities_process_training_mertic();
INT dyNUMA_Utilities_process_profiled_exe_signature();
INT dyNUMA_Utilities_save_training_element_to_file();
INT dyNUMA_Utilities_read_concurrency_predictor_training_elements();
INT dyNUMA_Utilities_read_mapping_predictor_training_elements();
INT dyNUMA_Utilities_read_scheduling_predictor_training_elements(); 
#endif
