#include "dyNUMA_Utilities.h"
#include <omp.h>
extern INT concurrency_mapping[8];
INT dyNUMA_Utilities_read_env_vars()
{

	CHAR* string;

	//Read the environment variables
	string=getenv("DYNUMA_VERSION");
	env.DYNUMA_VERSION	= strtol(string, NULL,	10);
	env.DYNUMA_VERSION=atoi(string);
	
	string=getenv("PLATFORM");
	env.PLATFORM=string;
	string=getenv("OPT_CONCURRENCY");
	env.OPT_CONCURRENCY=atoi(string);
	string=getenv("OPT_TMA");
	env.OPT_TMA=atoi(string);	
	string=getenv("OPT_SCHEDULE");
	env.OPT_SCHEDULE=atoi(string);
	string=getenv("NUM_REGIONS");
	env.NUM_REGIONS=atoi(string);
	string=getenv("NUM_THREADS");
	env.NUM_THREADS=atoi(string);
	string=getenv("NUM_LOOPS");
	env.NUM_LOOPS=atoi(string);	
	string=getenv("PAPI_NUM_EVENT");
	env.PAPI_NUM_EVENT=atoi(string);
	string=getenv("PAPI_EVENT_1");
	env.CHAR_PAPI_EVENT[0]=string;
	string=getenv("PAPI_EVENT_2");
	env.CHAR_PAPI_EVENT[1]=string;
	string=getenv("PAPI_EVENT_3");
	env.CHAR_PAPI_EVENT[2]=string;
	string=getenv("PAPI_EVENT_4");
	env.CHAR_PAPI_EVENT[3]=string;
	string=getenv("PREDICT_METRIC");
	env.PREDICT_METRIC=string;
	string=getenv("TRAINING_FILE");
	env.TRAINING_FILE=string;
	string=getenv("PLATFORM_MAX_NUM_THREAD");
	env.PLATFORM_MAX_NUM_THREAD=atoi(string);
	string=getenv("ANN_CONCURRENCY_KNOWLEDGE_FILE");
	env.ANN_CONCURRENCY_KNOWLEDGE_FILE=string;
	string=getenv("ANN_MAPPING_KNOWLEDGE_FILE");
	env.ANN_MAPPING_KNOWLEDGE_FILE=string;
	string=getenv("NUM_PROFILE_ITERATIONS");
	env.NUM_PROFILE_ITERATIONS=atoi(string);
	string=getenv("NUM_EXE_SIGNATURE");
	env.NUM_EXE_SIGNATURE=atoi(string);
	string=getenv("TRAINING_MODE");
	env.TRAINING_MODE=atoi(string);

	fprintf(stdout,SEPS,"Environment Variable Configuration");
	fprintf(stdout,"DYNUMA_VERSION=%d\n",env.DYNUMA_VERSION);
	fprintf(stdout,"PLATFORM_MAX_NUM_THREAD=%d\n",env.PLATFORM_MAX_NUM_THREAD);
	fprintf(stdout,"PLATFORM=%s\n",env.PLATFORM);
	fprintf(stdout,"OPT_CONCURRENCY=%d\n",env.OPT_CONCURRENCY);
	fprintf(stdout,"OPT_TMA=%d\n",env.OPT_TMA);
	fprintf(stdout,"OPT_SCHEDULE=%d\n",env.OPT_SCHEDULE);
	fprintf(stdout,"NUM_LOOPS=%d\n",env.NUM_LOOPS);
	fprintf(stdout,"NUM_REGIONS=%d\n",env.NUM_REGIONS);
	fprintf(stdout,"NUM_THREADS=%d\n",env.NUM_THREADS);
	fprintf(stdout,"PAPI_NUM_EVENT=%d\n",env.PAPI_NUM_EVENT);
	fprintf(stdout,"PAPI_EVENT_1=%s\n",env.CHAR_PAPI_EVENT[0]);
	fprintf(stdout,"PAPI_EVENT_2=%s\n",env.CHAR_PAPI_EVENT[1]);
	fprintf(stdout,"PAPI_EVENT_3=%s\n",env.CHAR_PAPI_EVENT[2]);
	fprintf(stdout,"PAPI_EVENT_4=%s\n",env.CHAR_PAPI_EVENT[3]);
	fprintf(stdout,"PREDICT_METRIC=%s\n",env.PREDICT_METRIC);
	fprintf(stdout,"TRAINING_FILE=%s\n",env.TRAINING_FILE);
	fprintf(stdout,"ANN_CONCURRENCY_KNOWLEDGE_FILE=%s\n",env.ANN_CONCURRENCY_KNOWLEDGE_FILE);
	fprintf(stdout,"ANN_MAPPING_KNOWLEDGE_FILE=%s\n",env.ANN_MAPPING_KNOWLEDGE_FILE);
	fprintf(stdout,"NUM_PROFILE_ITERATIONS=%d\n",env.NUM_PROFILE_ITERATIONS);
	fprintf(stdout,"NUM_EXE_SIGNATURE=%d\n",env.NUM_EXE_SIGNATURE);
	fprintf(stdout,"TRAINING_MODE=%d\n",env.TRAINING_MODE);
	fprintf(stdout,SEP);
	fflush(stdout);	
	return SUCCESS;
}

// ************* User need to implement their own function here **************//

INT dyNUMA_Utilities_process_training_input()
{	
	//Process input data //
	// Store threads positon, lid, rid
		
	INT lid,rid,tid;
	for(lid=0;lid<env.NUM_LOOPS;lid++)
		for(rid=0;rid<env.NUM_REGIONS;rid++)
		{
			glb.re[lid][rid].lid = lid;
			glb.re[lid][rid].rid = rid;
			
			for(tid=0;tid<env.NUM_THREADS;tid++)
			{
				glb.re[lid][rid].thread_pos[tid]=glb.thread_pos[lid][rid][tid];
			}
		}

	for(lid=0;lid<env.NUM_LOOPS;lid++)
		for(rid=0;rid<env.NUM_REGIONS;rid++)
		{
			DOUBLE max_cycles=DBL_MIN;
			
			// get max num cycles from all thread execution // 
			for(tid=0;tid<env.NUM_THREADS;tid++)
			{
				if(max_cycles<glb.profile_data[lid][rid][tid][3])
				max_cycles=glb.profile_data[lid][rid][tid][3];
			}
#if DEBUG_POST_PROCESSING
			fprintf(stdout, "max_cycles= %f\n",max_cycles);
#endif
			
			for(tid=0;tid<env.NUM_THREADS;tid++)
			{
				glb.re[lid][rid].input[tid][0] = 
				(glb.profile_data[lid][rid][tid][0]) 
				/ max_cycles; /* ON-CHIP IPC */
				glb.re[lid][rid].input[tid][1] = (glb.profile_data[lid][rid][tid][1]*1000)/max_cycles; /* LLC_MISS_PER_CYC */
				glb.re[lid][rid].input[tid][2] = (glb.profile_data[lid][rid][tid][2])/max_cycles; /* BRANCH_MISS_PER_CYC */
#if DEBUG_POST_PROCESSING
				fprintf(stdout, "LOOP %2d REGION %2d THREAD %2d (%2.6f , %2.6f , %2.6f )\n",
						lid, rid, tid,
						glb.re[lid][rid].input[tid][0],
						glb.re[lid][rid].input[tid][1],  
						glb.re[lid][rid].input[tid][2]
						);		
#endif			 
			}
			
		}
	return SUCCESS;
}	
INT dyNUMA_Utilities_process_training_mertic()
{	
	
	//Process metric data
	INT lid,rid,tid;
	long long int sum_insturctions=0;	
	if(strcmp(env.PREDICT_METRIC,"IPC") == 0)
	{
		for(lid=0;lid<env.NUM_LOOPS;lid++)
			for(rid=0;rid<env.NUM_REGIONS;rid++)
			{
				DOUBLE max_cycles=DBL_MIN;
				sum_insturctions=0;
				for(tid=0;tid<env.NUM_THREADS;tid++)
				{
					if(max_cycles<glb.profile_data[lid][rid][tid][3])
					max_cycles=glb.profile_data[lid][rid][tid][3];
					sum_insturctions +=  glb.profile_data[lid][rid][tid][0]; //IPC
				}
				sum_insturctions=sum_insturctions/env.NUM_THREADS;
				glb.re[lid][rid].target = (double)sum_insturctions/max_cycles;
#if DEBUG_POST_PROCESSING
				fprintf(stdout," LID_%d RID_%d SUM_INS %13llu MAX_CYC %13.0f MERTIC IPC %2.6f\n", lid, rid, sum_insturctions, max_cycles, glb.re[lid][rid].target);
#endif
			}
	}	

	if(strcmp(env.PREDICT_METRIC,"TIME") == 0)
	{

		for(lid=0;lid<env.NUM_LOOPS;lid++)
		{
			for(rid=0;rid<env.NUM_REGIONS;rid++)
			{
				glb.re[lid][rid].target = glb.region_elapsed_time[lid][rid];
#if DEBUG_POST_PROCESSING
				fprintf(stdout, "LID_%d RID_%d MERTIC TIME(SEC) %2.6f\n",lid, rid, glb.re[lid][rid].target);
#endif
			}
		}				

	}
/*#if DEBUG_POST_PROCESSING
	fprintf(stdout,"env.PREDICT_METRIC=%s\n",env.PREDICT_METRIC);
#endif
*/ 
	if(strcmp(env.PREDICT_METRIC,"EDP") == 0)
	{
		for(lid=0;lid<env.NUM_LOOPS;lid++)
		{
			for(rid=0;rid<env.NUM_REGIONS;rid++)
			{
				glb.re[lid][rid].target = glb.region_elapsed_time[lid][rid]*env.NUM_THREADS*env.NUM_THREADS;
#if DEBUG_POST_PROCESSING
				fprintf(stdout, "LID_%d RID_%d MERTIC EDP(Joul.SEC) %2.6f\n",lid, rid, glb.re[lid][rid].target);
#endif
			}
		}		
	}
	
	return SUCCESS;
}

INT dyNUMA_Utilities_process_profiled_exe_signature()
{
	INT lid, rid, tid, record_id;
//Save profiled to execution signature.
	for(lid=0;lid<env.NUM_PROFILE_ITERATIONS;lid++)
	{
		for(rid=0;rid<env.NUM_REGIONS;rid++)
		{
			for(tid=0;tid<env.NUM_THREADS;tid++)
			{
				for(record_id=0;record_id<env.PAPI_NUM_EVENT;record_id++)
				{		
					glb.execution_signature[rid][tid][record_id]+=glb.profile_data[lid][rid][tid][record_id];
			
				}
			}
		} 
	}

	for(rid=0;rid<env.NUM_REGIONS;rid++)
	{
		for(tid=0;tid<env.NUM_THREADS;tid++)
		{
#if DEBUG
		fprintf(stdout,"REGION %d THREAD %d ",rid,tid);
#endif
			glb.execution_signature[rid][tid][0]=glb.execution_signature[rid][tid][0]/glb.execution_signature[rid][tid][3]; // Calculate IPC
			glb.execution_signature[rid][tid][1]=glb.execution_signature[rid][tid][1]*1000/glb.execution_signature[rid][tid][3]; // Calculate * LLC_MISS_PER_CYC */
			glb.execution_signature[rid][tid][2]=glb.execution_signature[rid][tid][2]/glb.execution_signature[rid][tid][3]; // Calculate * BRANCH_MISS_PER_CYC */

#if DEBUG
			fprintf(stdout, "EXE_SIGNATURE( %2.6f ,%2.6f %2.6f)\n",
			glb.execution_signature[rid][tid][0],
			glb.execution_signature[rid][tid][1],
			glb.execution_signature[rid][tid][2]
			);			
#endif
		}
	}
 	
	return SUCCESS;
}


INT dyNUMA_Utilities_save_training_element_to_file()
{
	INT lid,rid,tid,event_id;
	lid=glb.lid;
	
	//fprintf(stdout,"max num thread=%d\n",max_num_thread);
	//remove file
/*	if( remove( env.TRAINING_FILE ) != 0 )
    	{
		fprintf(stderr, "Error deleting file %s\n",env.TRAINING_FILE );
		exit(1);
	}
*/
	//OPEN the training file with append option
	glb.training_file=fopen(env.TRAINING_FILE, "a");

	//Write binding method to file
	CHAR* string;	
	string=getenv("KMP_AFFINITY");
	//fprintf(stdout,"KMP_AFFINITY=%s env.NUM_THREADS=%d\n",string,env.NUM_THREADS);
if(env.NUM_THREADS==4) 
{
	if(string==NULL)
	{
#if DEBUG_POST_PROCESSING
		fprintf(glb.training_file,"binding( ");
#endif
		fprintf(glb.training_file,"n,");
#if DEBUG_POST_PROCESSING
			fprintf(glb.training_file,") \n");
#endif
	}
	else if(strcmp(string, "granularity=fine,scatter") == 0)
	{
#if DEBUG_POST_PROCESSING
		fprintf(glb.training_file,"binding( ");
#endif
		fprintf(glb.training_file,"s,");
#if DEBUG_POST_PROCESSING
			fprintf(glb.training_file,") \n");
#endif
	}
	else if (strcmp(string, "granularity=fine,compact")== 0)
	{
#if DEBUG_POST_PROCESSING
		fprintf(glb.training_file,"binding( ");
#endif
		fprintf(glb.training_file,"c,");
#if DEBUG_POST_PROCESSING
			fprintf(glb.training_file,") \n");
#endif
	}
	
}
	if(env.NUM_THREADS<env.PLATFORM_MAX_NUM_THREAD)
	{
//		if(env.PAPI_NUM_THREAD==1)
		
		for(rid=0;rid<env.NUM_REGIONS;rid++)
		{
#if DEBUG_POST_PROCESSING
			fprintf(glb.training_file,"rid_%d target( ", rid);
#endif
			fprintf(glb.training_file,"%2.6f,",glb.re[lid][rid].target);
#if DEBUG_POST_PROCESSING
			fprintf(glb.training_file,") \n");
#endif
		}	
	}
	else
	{
		for(rid=0;rid<env.NUM_REGIONS;rid++)
		{
#if DEBUG_POST_PROCESSING
			fprintf(glb.training_file,"rid_%d target( " ,rid);
#endif
			fprintf(glb.training_file,"%2.6f,",glb.re[lid][rid].target);
#if DEBUG_POST_PROCESSING
			fprintf(glb.training_file,") \n");
#endif
		}
		//print thread position
		
		for(tid=0;tid<env.PLATFORM_MAX_NUM_THREAD;tid++)
		{
#if DEBUG_POST_PROCESSING
			fprintf(glb.training_file,"thread_pos( ");
#endif
			fprintf(glb.training_file,"%d,",glb.thread_pos[0][0][tid]);
				
#if DEBUG_POST_PROCESSING
			fprintf(glb.training_file,") \n");
#endif
		}	
		
		//print PAPI events
		for(rid=0;rid<env.NUM_REGIONS;rid++)
		{
			for(tid=0;tid<env.PLATFORM_MAX_NUM_THREAD;tid++)
			{
#if DEBUG_POST_PROCESSING
				fprintf(glb.training_file,"rid_%d tid_%d event( ",rid, tid);
#endif
				for(event_id=0;event_id<env.NUM_EXE_SIGNATURE;event_id++)
				{
					fprintf(glb.training_file,"%2.6f,",glb.re[lid][rid].input[tid][event_id]);
				}			
#if DEBUG_POST_PROCESSING
				fprintf(glb.training_file,") \n");
#endif
			}	
		}
		fprintf(glb.training_file,"\n");
	}

		return SUCCESS;
}
void parse( CHAR *record, CHAR *delim, CHAR arr[][MAXFLDSIZE],INT *fldcnt)
{
	CHAR*p=strtok(record,delim);
	INT fld=0;
	while( p!=NULL)
	{
		strcpy(arr[fld],p);
		fld++;
		p=strtok(NULL,delim);
	}	
	*fldcnt=fld-1;
}

INT dyNUMA_Utilities_read_concurrency_predictor_training_elements()
{
	CHAR line[MAX_CHAR_PERLINE];
	INT fldcnt=0;
	INT index=0;
	INT elmt_index=0;
	CHAR arr[MAXFLDS][MAXFLDSIZE];
	INT thread_pos[MAX_NUM_THREAD];
	INT i,j,tid,record_id;
	FILE *training_file = fopen ( env.TRAINING_FILE, "r" );
	
	if(training_file == NULL)
	{
#if DEBUG_GENERAL
		fprintf(stderr,"Error open training file. %s\n",env.TRAINING_FILE);
#endif
		exit(1);  
	}
	/* read a line */
	while ( fgets ( line, sizeof line, training_file ) != NULL ) 
	{
		parse(line,",",arr,&fldcnt);
		/* save to elements */
		elmt_index=1;
		for(j=0;j<env.NUM_REGIONS;j++)
		{
			glb.te[index+j].lid=0;
			glb.te[index+j].rid=j;
		}
		for(i=0;i<8;i++)
		{
			for(j=0;j<env.NUM_REGIONS;j++,elmt_index++)
			{
				glb.te[index+j].mode=arr[0][0];
				glb.te[index+j].target[i]=atof(arr[elmt_index]);
				//fprintf(stdout,"Run %d Region %2d Target %2.6f\n",i,j,glb.te[index+j].target[i]);
			}
		}
		//thread pos
		
		for(tid=0;tid<env.NUM_THREADS;tid++,elmt_index++)
		{	
			thread_pos[tid]=atoi(arr[elmt_index]);
		}
		for(j=0;j<env.NUM_REGIONS;j++)
		{
			//fprintf(stdout,"Region %2d Thread( ",j);
			for(tid=0;tid<env.NUM_THREADS;tid++)
			{
				glb.te[index+j].thread_pos[tid]=thread_pos[tid];
				//fprintf(stdout," %d,",glb.te[index+j].thread_pos[tid]);
			}
			//fprintf(stdout,")\n");
		}
		for(j=0;j<env.NUM_REGIONS;j++)
		{
			//fprintf(stdout,"Region %2d Input( ",j);
			for(tid=0;tid<env.NUM_THREADS;tid++)
			{
				for(record_id=0;record_id<env.NUM_EXE_SIGNATURE;record_id++,elmt_index++)
				{
					glb.te[index+j].input[tid][record_id]=atof(arr[elmt_index]);
					
				}
				//fprintf(stdout," (%f,%f,%f),",glb.te[index+j].input[tid][0],glb.te[index+j].input[tid][1],glb.te[index+j].input[tid][2]);				
			}
			//fprintf(stdout,")\n");	
		}
		index+=env.NUM_REGIONS; 
	}
	glb.num_te = index;
	for(i=0;i<index;i++)
	{
		DOUBLE val= MAX_DOUBLE;
		DOUBLE select = 1;
		for(j=0;j<8;j++)
		{
			if(val>glb.te[i].target[j])
			{
				val=glb.te[i].target[j];
				select=j;
			}
		}
		val=glb.te[i].select[0]=select;
		//fprintf(stdout," (SONICAT select=%f)\n",glb.te[i].select[0]);
	}
	/* validate the trining element*/
	
#if DEBUG
	printf("dyNUMA: Total training elements: %d\n",index);
	for(i=0;i<index;i++)
	{
		fprintf(stdout,"lid=%d\n",glb.te[i].lid);
		fprintf(stdout,"rid=%d\n",glb.te[i].rid);
		fprintf(stdout,"mode=%c\n",glb.te[i].mode);
		fprintf(stdout,"thraead pos( ");		
		for(j=0;j<env.NUM_THREADS;j++)
		{
			fprintf(stdout,"%d,",glb.te[i].thread_pos[j]);
		}
		fprintf(stdout," )\n");
		fprintf(stdout,"input ( ");		

		for(j=0;j<env.NUM_THREADS;j++)
		{
			fprintf(stdout,"(%f,%f,%f),",glb.te[i].input[j][0],glb.te[i].input[j][1],glb.te[i].input[j][2]);
		}
		fprintf(stdout," )\n");
		fprintf(stdout,"target ( ");
		for(j=0;j<8;j++)
		{
			fprintf(stdout,"%f,",glb.te[i].target[j]);
		}
		fprintf(stdout," )\n");
		fprintf(stdout," (select=%f)\n",glb.te[i].select[0]);
	}
#endif
	fclose ( training_file );
	
	
	return SUCCESS;	
}
DOUBLE max_3(DOUBLE a, DOUBLE b, DOUBLE c)
{
	INT select =1;
	if(a>=b)
	{
		if(a>=c)
		return 1;
		else
		return 3;
	}
	if(a<b) 
	{
		if(b>=c)
		return 2;
		else
		return 3;
	}
	return select;
}
INT dyNUMA_Utilities_read_mapping_predictor_training_elements()
{
	CHAR line[MAX_CHAR_PERLINE];
	INT fldcnt=0;
	INT index=0;
	INT elmt_index=0;
	CHAR arr[MAXFLDS][MAXFLDSIZE];
	INT thread_pos[MAX_NUM_THREAD];
	INT i,j,tid,record_id;
	FILE *training_file = fopen ( env.TRAINING_FILE, "r" );
	
	if(training_file == NULL)
	{
#if DEBUG_GENERAL
		fprintf(stderr,"Error open training file. %s\n",env.TRAINING_FILE);
#endif
		exit(1);  
	}
	/* read a line */
	while ( fgets ( line, sizeof line, training_file ) != NULL ) 
	{
		parse(line,",",arr,&fldcnt);
		/* save to elements */
		elmt_index=1;
		for(j=0;j<env.NUM_REGIONS;j++)
		{
			glb.te[index+j].lid=0;
			glb.te[index+j].rid=j;
		}
		for(i=0;i<8;i++)
		{
			for(j=0;j<env.NUM_REGIONS;j++,elmt_index++)
			{
				glb.te[index+j].mode=arr[0][0];
				glb.te[index+j].target[i]=atof(arr[elmt_index]);
				//fprintf(stdout,"Run %d Region %d Target %f\n",i,j,glb.te[index+j].target[i]);
			}
		}
		//thread pos
		
		for(tid=0;tid<env.NUM_THREADS;tid++,elmt_index++)
		{	
			thread_pos[tid]=atoi(arr[elmt_index]);
		}
		for(j=0;j<env.NUM_REGIONS;j++)
		{
			//fprintf(stdout,"Region %d Thread( ",j);
			for(tid=0;tid<env.NUM_THREADS;tid++)
			{
				glb.te[index+j].thread_pos[tid]=thread_pos[tid];
				//fprintf(stdout," %d,",glb.te[index+j].thread_pos[tid]);
			}
			//fprintf(stdout,")\n");
		}
		for(j=0;j<env.NUM_REGIONS;j++)
		{
			//fprintf(stdout,"Region %d Input( ",j);
			for(tid=0;tid<env.NUM_THREADS;tid++)
			{
				for(record_id=0;record_id<env.NUM_EXE_SIGNATURE;record_id++,elmt_index++)
				{
					glb.te[index+j].input[tid][record_id]=atof(arr[elmt_index]);
					
				}
				//fprintf(stdout," (%f,%f),",glb.te[index+j].input[tid][0],glb.te[index+j].input[tid][1]);				
			}
			//fprintf(stdout,")\n");	
		}
		index+=env.NUM_REGIONS; 
	}
	glb.num_te = index;
	for(i=0;i<(index+1)/3;i++)
	{
		DOUBLE val_n= 0;
		DOUBLE val_c= 0;
		DOUBLE val_s= 0;
		
		for(j=0;j<8;j++)
		{
			val_n+=glb.te[i].target[j];
			val_c+=glb.te[i+1080].target[j];
			val_s+=glb.te[i+2160].target[j];
		}
		glb.te[i].select[0] = max_3(val_n,val_c,val_s);
	}
	/* validate the trining element*/
#if DEBUG
	printf("index=%d\n",index);
	for(i=0;i<index;i++)
	{	
		fprintf(stdout,"index=%d\n",i);
		fprintf(stdout,"lid=%d\n",glb.te[i].lid);
		fprintf(stdout,"rid=%d\n",glb.te[i].rid);
		fprintf(stdout,"mode=%c\n",glb.te[i].mode);
		fprintf(stdout,"select=%f\n",glb.te[i].select[0]);
		fprintf(stdout,"thraead pos( ");		
		for(j=0;j<env.NUM_THREADS;j++)
		{
			fprintf(stdout,"%d,",glb.te[i].thread_pos[j]);
		}
		fprintf(stdout," )\n");
		fprintf(stdout,"input ( ");		

		for(j=0;j<env.NUM_THREADS;j++)
		{
			fprintf(stdout,"(%f,%f),",glb.te[i].input[j][0],glb.te[i].input[j][1]);
		}
		fprintf(stdout," )\n");
		fprintf(stdout,"target ( ");
		for(j=0;j<8;j++)
		{
			fprintf(stdout,"%f,",glb.te[i].target[j]);
		}
		fprintf(stdout," )\n");
	
	}
#endif
	fclose ( training_file );
	

	return SUCCESS;
}
INT dyNUMA_Utilities_read_scheduling_predictor_training_elements()
{
	return SUCCESS;
}
