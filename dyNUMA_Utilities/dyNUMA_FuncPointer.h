#ifndef DYNUMA_FUNC_POINTER_H
#define DYNUMA_FUNC_POINTER_H
#include "../global.h"

//function pointer for data post process
extern INT (*dyNUMA_fp_process_training_input)(); 
extern INT (*dyNUMA_fp_process_training_mertic)();
extern INT (*dyNUMA_fp_process_profiled_exe_signature)();
extern INT (*dyNUMA_fp_save_training_element_to_file)();
extern INT (*dyNUMA_fp_read_concurrency_predictor_training_elements)();
extern INT (*dyNUMA_fp_read_mapping_predictor_training_elements)();
extern INT (*dyNUMA_fp_read_scheduling_predictor_training_elements)();
#endif

