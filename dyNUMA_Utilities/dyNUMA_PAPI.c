#include "dyNUMA_PAPI.h"


INT dyNUMA_PAPI_init()
{
	INT num_ctrs,rcs ;
	INT i, rc, tid; 	
	// Initialize the library //
	
	if ( PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT ) {
    	fprintf(stderr, "PAPI_library_init failed\n"); 
    	return -1; 
  	}
  	// Enable the PAPI threads //
  	PAPI_CHECK_EXEC(rcs, PAPI_thread_init( (long unsigned int (*)(void)) omp_get_thread_num ), return -1); 
	
	num_ctrs = PAPI_num_hwctrs();
	fprintf(stdout,"Number of hardware counters: %d\n", num_ctrs);

 
/* Begin parallel region */ 
#pragma omp parallel					\
  private(i, tid, rc)			\
  shared( glb)
{
	tid = omp_get_thread_num(); 
    
    	/* Create the event set */ 
    	glb.event_set[tid] = PAPI_NULL; 
    	PAPI_CHECK( rc, PAPI_create_eventset( &glb.event_set[tid] ) );
    
    	/* Add events to it */ 
    	if ( env.PAPI_NUM_EVENT > 0 ) 
      		for (i = 0; i < env.PAPI_NUM_EVENT; i++)
			dyNUMA_PAPI_th_add_events(env.CHAR_PAPI_EVENT[i], glb.event_set[tid]); 
    
/* Wait until shared arrays are allocated */ 
#pragma omp barrier
}	
	return SUCCESS;
}

INT  dyNUMA_PAPI_th_add_events(CHAR *str_event, INT event_set)
{
	INT count, tid, event, rc; 
	PAPI_event_info_t info; 
	count = 0;     
	tid = omp_get_thread_num();   
  
  	if ( PAPI_event_name_to_code(str_event, &event) == PAPI_OK ) 
	{
    		if ( PAPI_get_event_info(event, &info) == PAPI_OK ) 
		{ 
			if ( (rc = PAPI_add_event(event_set, event)) != PAPI_OK ) 
			{
				fprintf(stderr, "tid %0d: could not add event %s: %s %s\n", 
				tid, str_event, PAPI_strerror(rc), PAPI_descr_error(rc)); 
			} 
			else 
			{
				count++; 
				if (tid == 0) 
				{
	  				printf("tid %2d: Add evnet 0x%x %s\n", tid, event, str_event); 
				}
      			}
    		}
  	}

  	return count; 
}



INT dyNUMA_PAPI_start(REGION rid)
{
	INT rc, tid;
#pragma omp parallel					\
  private(tid, rc)			\
  shared( glb)
{
	tid = omp_get_thread_num(); 
    	PAPI_CHECK( rc, PAPI_start(glb.event_set[tid]) );
}

 
/* end of parallel region */ 
  return SUCCESS; 
}

INT dyNUMA_PAPI_read(REGION rid)
{

	return SUCCESS;
}

INT dyNUMA_PAPI_stop(REGION rid)
{
	INT i, j;
	INT  rc;
	INT tid;
 	INT lid=glb.lid;

#pragma omp parallel private(rc,tid) shared(glb)
{
	tid = omp_get_thread_num(); 
	PAPI_CHECK( rc, PAPI_stop(glb.event_set[tid], glb.dummy[lid][rid][tid]) );
}
	for(i=0;i<env.NUM_THREADS;i++)
	{
		for(j=0;j<env.PAPI_NUM_EVENT;j++)
		{		
			glb.profile_data[lid][rid][i][j]=(DOUBLE)glb.dummy[lid][rid][i][j];
			
		}
		 
	}
#if DEBUG_ENABLE_PRINT_REGION
	dyNUMA_PAPI_print_region(rid);
#endif	
	return SUCCESS;
}
INT dyNUMA_PAPI_print_region(REGION rid)
{
	int i,j;
	INT lid=glb.lid;
	fprintf(stdout,SEPSD,"Print Event data - Region:", rid );
	for(i=0;i<env.NUM_THREADS;i++)
	{
		fprintf(stdout,"LOOP %3d REGION %3d THREAD %3d (",lid, rid, i);
		for(j=0;j<env.PAPI_NUM_EVENT;j++)
			fprintf(stdout,"%14.0f, ", glb.profile_data[lid][rid][i][j]);  
		fprintf(stdout, ")\n");	       
		 
	}
	return SUCCESS;
}
