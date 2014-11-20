/*
dyNUMA_Collector collects the execution signiture of OMP phases.
*/
#include "dyNUMA_Collector.h"
struct timeval time_start, time_end; /* Record the time stamps for each regions */


INT dyNUMA_cltr_init()
{
	INT lid, rid, tid, record_id;
	INT retval;
	//Initialize the PAPI interface.
	retval=dyNUMA_PAPI_init();
	if(retval==SUCCESS){
		fprintf(stdout,"Initialized the PAPI library\n");	
	}
	else{
		exit(1);
	}

	// Reset the global variable
	for(lid=0;lid<MAX_NUM_LOOP;lid++)
	{
		for(rid=0;rid<MAX_NUM_REGION;rid++)
		{
			
			for(tid=0;tid<MAX_NUM_THREAD;tid++)
			{
				glb.thread_pos[lid][rid][tid]=0;
				for(record_id=0;record_id<MAX_NUM_RECORD;record_id++)
				{
					glb.profile_data[lid][rid][tid][record_id]=0;
					glb.dummy[lid][rid][tid][record_id]=0;
							
				}
			}
		}
	}

	return SUCCESS;
}

INT dyNUMA_cltr_ld_collect_patterns()
{
	//Read the Pattern definition file
	
	return SUCCESS;
}


INT dyNUMA_cltr_start(REGION rid)
{
	INT lid=glb.lid;
#if DEBUG_ENABLE_PRINT_REGION	
	fprintf(stdout,SEPSD,"Start to collect region",rid);
#endif	
	/* assign current monitored region id */
	glb.rid=rid;
	// start to get threads position//
	dyNUMA_cltr_get_threads_position(glb.thread_pos[lid][rid]);
	
	/* start papi profiling */
	dyNUMA_PAPI_start(rid);
	//dyNUMA_PAPI_start();
	/* get timestamp  */
	dyNUMA_cltr_time(&time_start);
	return SUCCESS;
}

INT dyNUMA_cltr_stop(REGION rid)
{
	INT lid=glb.lid;
	dyNUMA_cltr_time(&time_end);
	//dyNUMA_PAPI_read(rid);
	dyNUMA_PAPI_stop(rid);

	/* Elapse time */
	DOUBLE diff = (double)(time_end.tv_sec - time_start.tv_sec) +(double)(time_end.tv_usec -time_start.tv_usec)/USEC_UNIT;
	glb.region_elapsed_time[lid][rid]=diff;
#if DEBUG_ENABLE_PRINT_REGION	
	fprintf(stdout,SEPSD,"Stop to collect region",rid);
#endif
		
	return SUCCESS;
}

INT dyNUMA_cltr_time(struct timeval *t)
{
	gettimeofday(t,NULL );
	return SUCCESS;
}

INT dyNUMA_cltr_time_diff(struct timeval *start, struct timeval *end, DOUBLE *diff)
{
	*diff = (double)(end->tv_sec - start->tv_sec) +(double)(end->tv_usec - start->tv_usec)/USEC_UNIT;
	return SUCCESS;
}
INT dyNUMA_cltr_get_region_time(INT lid, REGION rid, DOUBLE *time )
{
	*time = glb.region_elapsed_time[lid][rid];
	return SUCCESS;
}
INT dyNUMA_cltr_print_region_time(INT lid, REGION rid)
{
	DOUBLE time;
	dyNUMA_cltr_get_region_time(lid, rid, &time );

	fprintf(stdout,"Region %d Elapsed time %f\n",rid, time);
	fprintf(stdout,SEP);
//#endif
	return SUCCESS;
}

INT dyNUMA_cltr_get_threads_position(INT *thread_pos)
{
	int tid;
#pragma omp parallel private(tid)
	{
		tid = omp_get_thread_num();
		thread_pos[tid]=(sched_getcpu());
	}

	/*
for(i=0;i<env.NUM_THREADS;i++)
	{
		fprintf(stdout," TP %2d ",thread_pos[i]);
	}

	fprintf(stdout,"\n");
*/
	return SUCCESS;
}

INT dyNUMA_cltr_data_postprocess()
{
	// The folloing are funion pointers, we need to assign the real function implementations in dyNUMA.c //
	dyNUMA_fp_process_training_input();
	dyNUMA_fp_process_training_mertic();
	dyNUMA_fp_save_training_element_to_file();
	return SUCCESS;
}

/* This function need to use a function pointer for user to define their own post process of data */


//This file only define the high-level function abstraction.

