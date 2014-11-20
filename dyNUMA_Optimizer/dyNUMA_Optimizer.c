#include "dyNUMA_Optimizer.h"
INT concurrency_mapping[8]={4,8,12,16,20,24,28,32};
int thread_scatter_mapping[32]={0,8,1,9,2,10,3,11,4,12,5, 13,6, 14,7,15,16,24,17,25,18,26,19,27,20,28,21,29,22,30,23,31};
void set_thread_mapping(int mapping)
{
	if(mapping==1)
	{
		#pragma omp parallel
		{
			int ret,i;
			pid_t pid =	syscall(186);
			//unsigned long mask=0;
			//cpu_set_t	mask;
			unsigned int len = sizeof( unsigned long );
			//mask =	 0x11111111;
			cpu_set_t  mask;
			for(i=0;i<32;i++)
			{
				CPU_SET(i, &mask);
			}
			ret = sched_setaffinity(	pid , len	, &mask );
			if (ret <	0) 
			{
				printf("errno = EPERM\n");
				exit(-1);
			}
	
			
		}
	}
	if(mapping==2)
	{
		#pragma omp parallel
		{
			int ret;
			int tnum = omp_get_thread_num();
			pid_t pid =	syscall(186);
			//unsigned long mask=0;
			cpu_set_t	mask;
			CPU_SET(tnum, &mask);
			unsigned int len = sizeof( unsigned long );
			//mask =	1UL <<tnum;
			ret = sched_setaffinity(	pid , len	, &mask );
			if (ret <	0) 
			{
				printf("errno = EPERM\n");
				exit(-1);
			}
	
		}
	
	}
	if(mapping==3)
	{
		#pragma omp parallel
		{
			int ret;
			int tnum = omp_get_thread_num();
			pid_t pid =	syscall(186);
			//unsigned long mask=0;
			cpu_set_t	mask;
			unsigned int len = sizeof( unsigned long );
			//mask =	1UL <<thread_scatter_mapping[tnum];
			CPU_SET(thread_scatter_mapping[tnum], &mask);
			ret = sched_setaffinity(	pid , len	, &mask );
			if (ret <	0) 
			{
				printf("errno = EPERM\n");
				exit(-1);
			}
	
			
		}
	}
}
INT dyNUMA_optmzr_init()
{
	INT lid,rid,tid,record_id;
	//init PAPI
	dyNUMA_PAPI_init();
	//reset global variables
	for(lid=0;lid<MAX_NUM_LOOP;lid++)
	{
		for(rid=0;rid<MAX_NUM_REGION;rid++)
		{
			for(tid=0;tid<MAX_NUM_THREAD;tid++)
			{
				for(record_id=0;record_id<MAX_NUM_RECORD;record_id++)
				{
					glb.region_elapsed_time[lid][rid]=0;
					glb.profile_data[lid][rid][tid][record_id]=0;
					glb.execution_signature[rid][tid][record_id]=0;
				}
			}
		}
	}
	if(env.OPT_CONCURRENCY)
	dyNUMA_predictor_init_ann_concurrency(env.ANN_CONCURRENCY_KNOWLEDGE_FILE);
	if(env.OPT_TMA)
	dyNUMA_predictor_init_ann_mapping(env.ANN_MAPPING_KNOWLEDGE_FILE);
	;
	if(env.OPT_SCHEDULE)
	;
	return SUCCESS;
}
INT dyNUMA_optmzr_init_learners()
{

	return SUCCESS;
}

INT dyNUMA_optmzr_start(REGION rid)
{
	if(glb.lid<env.NUM_PROFILE_ITERATIONS)
	dyNUMA_optmzr_start_profile(rid);
	else
	dyNUMA_optmzr_start_optimize(rid);
	return SUCCESS;
}
INT dyNUMA_optmzr_stop(REGION rid)
{
	if(glb.lid<env.NUM_PROFILE_ITERATIONS)
	dyNUMA_optmzr_stop_profile(rid);
	else
	dyNUMA_optmzr_stop_optimize(rid);
	return SUCCESS;
}
INT dyNUMA_optmzr_start_profile(REGION rid)
{
	//profile execution signature of a region.
	dyNUMA_cltr_start(rid);
	return SUCCESS;
}
INT dyNUMA_optmzr_stop_profile(REGION rid)
{
	INT lid;
        lid=glb.lid;
	dyNUMA_cltr_stop(rid);
#if DEBUG_ENABLE_PRINT_REGION
	fprintf(stdout,"REGION %d Elapsed Time %2.6f\n",rid, glb.region_elapsed_time[lid][rid]);
#endif
	return SUCCESS;
}
INT dyNUMA_optmzr_start_optimize(REGION rid)
{
	if(env.OPT_CONCURRENCY)
	dyNUMA_optmzer_opt_concurrency(rid);
	if(env.OPT_TMA)
	dyNUMA_optmzer_opt_tma(rid);
	if(env.OPT_SCHEDULE)
	dyNUMA_optmzer_opt_schedule(rid);
	dyNUMA_cltr_start(rid);
	return SUCCESS;
}
INT dyNUMA_optmzr_stop_optimize(REGION rid)
{
	INT lid;
	lid=glb.lid;
	dyNUMA_cltr_stop(rid);
#if DEBUG_ENABLE_PRINT_REGION
	fprintf(stdout,"REGION %d After Opt Elapsed Time %2.6f\n",rid, glb.region_elapsed_time[lid][rid]);
#endif
	return SUCCESS;
}
INT dyNUMA_optmzr_read_opt_config(REGION rid)
{

	return SUCCESS;
}

INT dyNUMA_optmzer_opt_concurrency(REGION rid)
{
	INT lid=glb.lid;
	DOUBLE new_concurrency = dyNUMA_concurrency_predictor(rid);
	glb.re[lid][rid].predicted_concurrency=concurrency_mapping[(INT)(new_concurrency)];

#if DEBUG_ENABLE_PRINT_REGION

	fprintf(stdout," REGION %d PREDICT %f CONCURRENCY %d\n",rid,new_concurrency,concurrency_mapping[(INT)(new_concurrency)]);
#endif
	omp_set_num_threads(concurrency_mapping[(INT)(new_concurrency)]);
	return SUCCESS;
}
INT dyNUMA_optmzer_opt_tma(REGION rid)
{
	INT lid=glb.lid;
	DOUBLE new_mapping = dyNUMA_mapping_predictor(rid);
	set_thread_mapping((INT)new_mapping);
	glb.re[lid][rid].predicted_mapping=(INT)(new_mapping);
#if DEBUG_ENABLE_PRINT_REGION
	fprintf(stdout," REGION %d PREDICT MAPPING %f\n",rid, new_mapping);
#endif
	return SUCCESS;
}
INT dyNUMA_optmzer_opt_schedule(REGION rid)
{
	return SUCCESS;
}
