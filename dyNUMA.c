#include "dyNUMA.h"
#include "dyNUMA_Collector/dyNUMA_Collector.h"
#include "dyNUMA_Utilities/dyNUMA_Utilities.h"
#include "dyNUMA_Utilities/dyNUMA_FuncPointer.h"
#include "dyNUMA_Trainer/dyNUMA_Trainer.h"
#include "dyNUMA_Optimizer/dyNUMA_Optimizer.h"

INT dyNUMA_init(INT mode){
#if ENABLE_DYNUMA
	fprintf(stdout,SEPS,"Begin to initialize dyNUMA");
	/* Read Environment variables */
	dyNUMA_Utilities_read_env_vars();
	/* Assign number of omp threads */
	omp_set_num_threads(env.NUM_THREADS);
	glb.lid=0;
	switch(mode)
	{
		case TRAINING:
		fprintf(stdout,SEPS,"MODE:TRAINING");		
		dyNUMA_trainer_init();
		break;

		case COLLECTION:
		fprintf(stdout,SEPS,"MODE:COLLECTION");
		dyNUMA_cltr_init();		
		break;

		case OPTIMIZATION:
		fprintf(stdout,SEPS,"MODE:OPTIMIZATION");
		dyNUMA_optmzr_init();
		break;

	}
	// Assign function pointer //
	dyNUMA_init_fp();
	fprintf(stdout,SEPS,"End to initialize dyNUMA");
#endif
	return SUCCESS;
}

// Assign function pointer //	
INT dyNUMA_init_fp()
{
#if ENABLE_DYNUMA	
	// data post processing
	// func pointer //                          // real implementation //
	dyNUMA_fp_process_training_input          		= dyNUMA_Utilities_process_training_input;
	//fprintf(stdout,"fp %p %p\n",dyNUMA_fp_process_training_input, dyNUMA_Utilities_process_training_input);
	dyNUMA_fp_process_training_mertic         		= dyNUMA_Utilities_process_training_mertic;
	dyNUMA_fp_process_profiled_exe_signature  		= dyNUMA_Utilities_process_profiled_exe_signature;
	dyNUMA_fp_save_training_element_to_file	  	        = dyNUMA_Utilities_save_training_element_to_file; 
	dyNUMA_fp_read_concurrency_predictor_training_elements  = dyNUMA_Utilities_read_concurrency_predictor_training_elements;
	dyNUMA_fp_read_mapping_predictor_training_elements	= dyNUMA_Utilities_read_mapping_predictor_training_elements;
	dyNUMA_fp_read_scheduling_predictor_training_elements   = dyNUMA_Utilities_read_scheduling_predictor_training_elements;
#endif
	return SUCCESS;
}
INT dyNUMA_start(INT mode, REGION rid)
{
#if ENABLE_DYNUMA
	switch(mode)
	{
		case TRAINING:
		
		break;

		case COLLECTION:
		dyNUMA_cltr_start(rid);

		break;

		case OPTIMIZATION:
		dyNUMA_optmzr_start(rid);
		break;

	}
#endif
	return SUCCESS;
}
INT dyNUMA_stop(INT mode, REGION rid)
{
#if ENABLE_DYNUMA
	switch(mode)
	{
		case TRAINING:
		
		break;

		case COLLECTION:
		dyNUMA_cltr_stop(rid);
		break;

		case OPTIMIZATION:
		dyNUMA_optmzr_stop(rid);
		break;

	}
#endif
	return SUCCESS;
}
INT dyNUMA_finish(INT mode)
{
#if ENABLE_DYNUMA
	switch(mode)
	{
		case TRAINING:
		
		break;

		case COLLECTION:
		glb.lid--;
		dyNUMA_cltr_data_postprocess();
		break;

		case OPTIMIZATION:
		dyNUMA_report();
		break;

	}
#endif
	return SUCCESS;
}
INT dyNUMA_report()
{
#if ENABLE_DYNUMA
	INT i,lid,rid;
	DOUBLE total_elapsed_profiled_time[MAX_NUM_REGION], total_elapsed_opt_time[MAX_NUM_REGION];
	DOUBLE avg_elapsed_profiled_time[MAX_NUM_REGION], avg_elapsed_opt_time[MAX_NUM_REGION];
	DOUBLE sum_profiled_time,sum_opt_time;
	sum_profiled_time=sum_opt_time=0;
	for (i=0;i<MAX_NUM_REGION;i++)
	{
		total_elapsed_profiled_time[i]=0;
		total_elapsed_opt_time[i]=0;
		avg_elapsed_profiled_time[i]=0;
		avg_elapsed_opt_time[i]=0;
	}
	for(lid=0;lid<env.NUM_PROFILE_ITERATIONS;lid++)
	{
		for (rid=0;rid<env.NUM_REGIONS;rid++)
		{
			total_elapsed_profiled_time[rid]+=glb.region_elapsed_time[lid][rid];
		}
	}
	for(lid=env.NUM_PROFILE_ITERATIONS;lid<env.NUM_LOOPS;lid++)
	{
		for (rid=0;rid<env.NUM_REGIONS;rid++)
		{
			total_elapsed_opt_time[rid]+=glb.region_elapsed_time[lid][rid];
		}
	}
	for (rid=0;rid<env.NUM_REGIONS;rid++)
	{
		avg_elapsed_profiled_time[rid]=total_elapsed_profiled_time[rid]/env.NUM_PROFILE_ITERATIONS;
		avg_elapsed_opt_time[rid]=total_elapsed_opt_time[rid]/(env.NUM_LOOPS-env.NUM_PROFILE_ITERATIONS);	
	}
#if DEBUG_GENERAL
	for (rid=0;rid<env.NUM_REGIONS;rid++)
	{
		fprintf(stdout," REGION %d AVG_PROF_TIME %2.6f AVG_OPT_TIME %2.6f SPEEDUP %2.6f PREDICTED CONCURRENCY %d \n",
												  rid,avg_elapsed_profiled_time[rid],avg_elapsed_opt_time[rid]
											            ,avg_elapsed_profiled_time[rid]/avg_elapsed_opt_time[rid]
												  ,glb.re[env.NUM_PROFILE_ITERATIONS+1][rid].predicted_concurrency );
	}
	
	for (rid=0;rid<env.NUM_REGIONS;rid++)
	{
		sum_profiled_time += avg_elapsed_profiled_time[rid];
		sum_opt_time += avg_elapsed_opt_time[rid];	
	}
	fprintf(stdout,"OVERALL SPEEDUP=%2.6f\n", sum_profiled_time/sum_opt_time);
#endif	
#endif
	return SUCCESS;
}
INT dyNUMA_next_iter()
{
#if ENABLE_DYNUMA
	glb.lid++;
	if(glb.lid==env.NUM_PROFILE_ITERATIONS) // reach the maximal number of profiled iterations 
	{
		dyNUMA_fp_process_profiled_exe_signature(); //post process the execution signature
	}
#endif
	return SUCCESS;
}
