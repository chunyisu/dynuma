#include "dyNUMA_Concurrency_Predictor.h"
NET TrainedNet;
BOOL IF_Normalized[MAX_NUM_REGION];
void Concurrency_NormalizeExecutionSignature(REGION rid)
{
	INT k,tid;
  	for(tid=0;tid<env.NUM_THREADS;tid++)
  	{
		for(k=0;k<env.NUM_EXE_SIGNATURE;k++)
		{
		glb.execution_signature[rid][tid][k] = (glb.execution_signature[rid][tid][k] -TrainedNet.Min_EVENT[k]) /(TrainedNet.Max_EVENT[k] - TrainedNet.Min_EVENT[k]) *(HI-LO) + LO;
		if(glb.execution_signature[rid][tid][k]<LO)
		glb.execution_signature[rid][tid][k]=LO;
		if(glb.execution_signature[rid][tid][k]>HI)
		glb.execution_signature[rid][tid][k]=HI;
		}
#if DEBUG
		fprintf(stdout, "Normalized RID %d TID %d EXE_SIGNATURE(%2.6f, %2.6f, %2.6f)\n",rid, tid, glb.execution_signature[rid][tid][0], glb.execution_signature[rid][tid][1],glb.execution_signature[rid][tid][2]);
#endif
  	}
}

void Concurrency_SetPredictInput(NET* Net, INT rid)
{
	INT i;
	
  	for (i=1; i<=Net->InputLayer->Units; i++) 
	{
    		Net->InputLayer->Output[i] = glb.execution_signature[rid][(i-1)/env.NUM_EXE_SIGNATURE][(i-1)%env.NUM_EXE_SIGNATURE];
		
 	}
	
}

DOUBLE Predict_Concurrency(NET* Net,REGION rid)
{
	DOUBLE new_concurrency[1];
	new_concurrency[0]=env.NUM_THREADS;
	//Normalize ANN input
	if(IF_Normalized[rid]==FALSE)Concurrency_NormalizeExecutionSignature(rid);
	IF_Normalized[rid]=TRUE;
	Concurrency_SetPredictInput(Net, rid);
	PropagateNet(Net);
	GetOutput(Net, &new_concurrency[0]);
	new_concurrency[0] = (new_concurrency[0] - LO)/(HI-LO)*(Net->Max_Mertic[0]-Net->Min_Mertic[0])+Net->Min_Mertic[0];
	return cint(new_concurrency[0]);
}
DOUBLE  dyNUMA_concurrency_predictor(REGION rid)
{
	return Predict_Concurrency(&TrainedNet, rid);
}

void dyNUMA_predictor_init_ann_concurrency(CHAR * ANN_knowledge_file)
{
	/*************ANN INIT*****************/
	dyNUMA_ANN_GenerateNetwork(&TrainedNet);
	dyNUMA_ANN_load_knowledge(&TrainedNet, ANN_knowledge_file);
	int i;
	for(i=0;i<MAX_NUM_REGION;i++)
	IF_Normalized[i]=FALSE;
}
