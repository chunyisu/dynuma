#include "dyNUMA_Mapping_Predictor.h"
NET MappingTrainedNet;

void Mapping_NormalizeExecutionSignature(REGION rid)
{
	INT k,tid;
  	for(tid=0;tid<env.NUM_THREADS;tid++)
  	{
		for(k=0;k<env.NUM_EXE_SIGNATURE;k++)
		{
		glb.execution_signature[rid][tid][k] = (glb.execution_signature[rid][tid][k] -MappingTrainedNet.Min_EVENT[k]) /(MappingTrainedNet.Max_EVENT[k] - MappingTrainedNet.Min_EVENT[k]) *(HI-LO) + LO;
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

void Mapping_SetPredictInput(NET* Net, INT rid)
{
	INT i;
	/*FILE * temp;
	CHAR line[128];
	temp = fopen("temp.txt","r");
	DOUBLE val;
	for (i=1; i<=Net->InputLayer->Units; i++) 
	{
		fgets ( line, sizeof(line), temp );
		sscanf(line, "%lf\n", &val);
    		Net->InputLayer->Output[i] = val;
 	}
	fclose(temp);
	*/
  	for (i=1; i<=Net->InputLayer->Units; i++) 
	{
    		Net->InputLayer->Output[i] = glb.execution_signature[rid][(i-1)/env.NUM_EXE_SIGNATURE][(i-1)%env.NUM_EXE_SIGNATURE];
		
 	}

  	//Net->InputLayer->Output[Net->InputLayer->Units]=glb.region_elapsed_time[0][rid];// Set up wctime as the training 
	
}

DOUBLE Predict_Mapping(NET* Net,REGION rid)
{
	DOUBLE new_mapping[1];
	new_mapping[0]=env.NUM_THREADS;
	//Normalize ANN input
	Mapping_NormalizeExecutionSignature(rid);
	Mapping_SetPredictInput(Net, rid);
	PropagateNet(Net);
	GetOutput(Net, &new_mapping[0]);
	//printf("sonicat val %2.6f",new_mapping[0]);
	new_mapping[0] = (new_mapping[0] - LO)/(HI-LO)*(Net->Max_Mertic[0]-Net->Min_Mertic[0])+Net->Min_Mertic[0];
	//printf(" sonicat val %2.6f\n",new_mapping[0]);
	return cint(new_mapping[0]);
}
DOUBLE  dyNUMA_mapping_predictor(REGION rid)
{
	return Predict_Mapping(&MappingTrainedNet, rid);
}

void dyNUMA_predictor_init_ann_mapping(CHAR * ANN_knowledge_file)
{
	/*************ANN INIT*****************/
	dyNUMA_ANN_GenerateNetwork(&MappingTrainedNet);
	dyNUMA_ANN_load_knowledge(&MappingTrainedNet, ANN_knowledge_file);
	//fprintf(stdout,"%s finished\n",__func__);
	
}
