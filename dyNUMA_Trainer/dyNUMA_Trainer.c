#include "dyNUMA_Trainer.h"
INT dyNUMA_trainer_init()
{
	//RESET training element.
	INT i,tid,record_id;
	for(i=0;i<MAX_ELEMENT;i++)
	{
		glb.te[i].lid=0;
		glb.te[i].rid=0;
		glb.te[i].mode='x';
		glb.te[i].select[0]=-1;
		for(tid=0;tid<MAX_NUM_THREAD;tid++)
		{
				glb.te[i].thread_pos[tid]=0;
				glb.te[i].target[tid]=0;
				for(record_id=0;record_id<MAX_NUM_RECORD;record_id++)
				{
					glb.te[i].input[tid][record_id]=0;
				}	
		}
	}
	return SUCCESS;
}
INT dyNUMA_trainer_start()
{
	dyNUMA_trainer_begin();
	dyNUMA_trainer_end();
	return SUCCESS;
}
INT dyNUMA_trainer_process_training_data()
{
	switch(env.TRAINING_MODE)
	{
		case CONCURRENCY:
			dyNUMA_fp_read_concurrency_predictor_training_elements();
		break;
			
		case MAPPING:
			dyNUMA_fp_read_mapping_predictor_training_elements();
		break;
	
		case SCHEDULING:
			dyNUMA_fp_read_scheduling_predictor_training_elements();

		break;
		
		default:
		fprintf(stderr,"%s: Not a valid TRAINING_MODE %d\n",__func__,env.TRAINING_MODE);
		exit(1);
	}
	return SUCCESS;
}
INT dyNUMA_trainer_begin()
{
	switch(env.TRAINING_MODE)
	{
		case CONCURRENCY:
			dyNUMA_ANN_start();
		break;

		case MAPPING:
			dyNUMA_ANN_start();
		break;
		
		case SCHEDULING:
		break;
		
		default:
		fprintf(stderr,"%s: Not a valid TRAINING_MODE %d\n",__func__,env.TRAINING_MODE);
		exit(1);
	}
	
	return SUCCESS;
}
INT dyNUMA_trainer_end()
{
	return SUCCESS;
}
INT dyNUMA_trainer_write_training_cofficients()
{
	return SUCCESS;
}
