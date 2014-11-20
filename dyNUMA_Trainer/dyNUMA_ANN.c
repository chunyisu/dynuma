#include "dyNUMA_ANN.h"

//Variables used for ANN
INT	Units[NUM_LAYERS] = {N, IL, IL, IL, IL, M};

DOUBLE TrainError;
DOUBLE TrainErrorPredictingMean;
DOUBLE TestError;
DOUBLE TestErrorPredictingMean;
DOUBLE AvgError;

FILE *pWeightFile;
void InitializeRandoms()
{
	srand(1477);
}
/******************************************************************************
        R A N D O M S   D R A W N   F R O M   D I S T R I B U T I O N S
 ******************************************************************************/
INT RandomEqualINT(INT Low, INT High)
{
  return rand() % (High-Low+1) + Low;
}      


DOUBLE RandomEqualDOUBLE(DOUBLE Low, DOUBLE High)
{
	return ((DOUBLE) rand() / RAND_MAX) * (High-Low) + Low;
}  

void RandomWeights(NET* Net) //SONICAT NEED TO MODIFY HERE TO FIT THE TOPOLOGY
{
	INT l,i,j;
   
	for (l=1; l<NUM_LAYERS; l++) 
  	{
    		for (i=1; i<=Net->Layer[l]->Units; i++) 
    		{
      			for (j=0; j<=Net->Layer[l-1]->Units; j++) 
      			{
     			   	Net->Layer[l]->Weight[i][j] = RandomEqualDOUBLE(-0.5, 0.5);
      			}
    		}
  	}
}
/******************************************************************************
                          I N I T I A L I Z A T I O N
 ******************************************************************************/



void Normalize_input_target(NET* Net)
{

	INT i,j,k, tid;
	for(i=0;i<MAX_EVENT;i++)
	{
		Net->Min_EVENT[i] = MAX_DOUBLE;
		Net->Max_EVENT[i] = MIN_DOUBLE;

	}
	for(i=0;i<M;i++)
  	{
  		Net->Min_Mertic[i] = MAX_DOUBLE;
  		Net->Max_Mertic[i] = MIN_DOUBLE;
		Net->Mean[i] = 0;
  	}  
 
//Normalize Inputs and targets
//Find Min Max		  
	for(i=0;i<=EVAL_UPB;i++)
	{
  		for(tid=0;tid<env.NUM_THREADS;tid++)
  		{
			for(k=0;k<env.NUM_EXE_SIGNATURE;k++)
			{
  				Net->Min_EVENT[k] = _MIN(Net->Min_EVENT[k], glb.te[i].input[tid][k]);
				Net->Max_EVENT[k] = _MAX(Net->Max_EVENT[k], glb.te[i].input[tid][k]);
			}			
  		}

	}
	
	for(i=0;i<=EVAL_UPB;i++)
	{
  		for(j=0;j<M;j++)
  		{
			Net->Min_Mertic[0] = _MIN(Net->Min_Mertic[0], glb.te[i].select[j]);
			Net->Max_Mertic[0] = _MAX(Net->Max_Mertic[0], glb.te[i].select[j]);
		}
	}
#if DEBUG
	for(k=0;k<env.NUM_EXE_SIGNATURE;k++)
	{
		fprintf(stdout, "Min_EVENT[%d] = %2.8f ,Max_EVENT[%d] = %2.8f \n"
				,k, Net->Min_EVENT[k],k,Net->Max_EVENT[k]);
  	}
	for(i=0;i<M;i++)
  	{
  		fprintf(stdout,"Net->Min_Mertic[0]=%f , Max_Mertic[0]=%f\n",i,Net->Min_Mertic[0],i,Net->Max_Mertic[0]);
  	}
#endif
  
//Normalize to [LO,HI]

  	for(i=0;i<=EVAL_UPB;i++)
  	{
  		for(tid=0;tid<env.NUM_THREADS;tid++)
  		{
#if DEBUG
			if(i==0)
			{
				printf("sonicat 0 EXE_SIG(%2.6f %2.6f %2.6f)",glb.te[i].input[tid][0],glb.te[i].input[tid][1],glb.te[i].input[tid][2]);
			}
#endif
			for(k=0;k<env.NUM_EXE_SIGNATURE;k++)
			{
				glb.te[i].input[tid][k] = (glb.te[i].input[tid][k] -Net->Min_EVENT[k]) /(Net->Max_EVENT[k] - Net->Min_EVENT[k]) *(HI-LO) + LO;
			}
#if DEBUG			
			if(i==0)
			{
				printf("after EXE_SIG(%2.6f %2.6f %2.6f)\n",glb.te[i].input[tid][0],glb.te[i].input[tid][1],glb.te[i].input[tid][2]);
			}
#endif
  		}
		for(j=0;j<M;j++)
  		{
			glb.te[i].select[j]=((glb.te[i].select[j]- Net->Min_Mertic[0]) / (Net->Max_Mertic[0]-Net->Min_Mertic[0])) * (HI-LO) + LO;
  			Net->Mean[j] += (glb.te[i].select[j]/(EVAL_UPB+1));
  		}
  	}

}
void dyNUMA_ANN_InitializeApplication(NET* Net)
{
	INT  index, i;
	DOUBLE Out, Err;
	Normalize_input_target(Net);
  	TrainErrorPredictingMean = 0;
  	for (index=TRAIN_LWB; index<=TRAIN_UPB; index++) 
	{
    		for (i=0; i<M; i++) 
		{
	      		Out = glb.te[index].select[i];
	      		Err = Net->Mean[i] - Out;
	      		TrainErrorPredictingMean += 0.5 * sqr(Err);  
    		}
  	}
  	
	TestErrorPredictingMean = 0;
  	for (index=TEST_LWB; index<=TEST_UPB; index++)
	{
    		for (i=0; i<M; i++) 
		{
      			Out = glb.te[index].select[i];
			Err = Net->Mean[i] - Out;
      			TestErrorPredictingMean += 0.5 * sqr(Err);
    		}
  	}
}

/******************************************************************************
            S U P P O R T   F U N C T I O N S
 ******************************************************************************/

DOUBLE cint(DOUBLE x){
	DOUBLE intpart;
	if (modf(x,&intpart)>=.5)
	return ceil(x);
	else
	return floor(x);
}

void dyNUMA_ANN_save_knowledge(NET* Net, BOOL SaveToFile)
{
	INT l,i,j,k;
  	if(SaveToFile)
  	{
		if(env.TRAINING_MODE==CONCURRENCY)
  		pWeightFile = fopen(env.ANN_CONCURRENCY_KNOWLEDGE_FILE,"w");
		if(env.TRAINING_MODE==MAPPING)
		pWeightFile = fopen(env.ANN_MAPPING_KNOWLEDGE_FILE,"w");
  	}
  
  	for (l=1; l<NUM_LAYERS; l++) 
	{
	    	for (i=1; i<=Net->Layer[l]->Units; i++) 
		{
	      		for (j=0; j<=Net->Layer[l-1]->Units; j++) 
			{
				Net->Layer[l]->WeightSave[i][j] = Net->Layer[l]->Weight[i][j];
	        	    	if(SaveToFile) fprintf(pWeightFile,"%d %d %d %2.8f\n",l,i,j,Net->Layer[l]->WeightSave[i][j]);
      			}
  	  	}
  	}

	//Save min max values of input
	if(SaveToFile)
	{
		for(k=0;k<env.NUM_EXE_SIGNATURE;k++)
		{
			fprintf(pWeightFile,"%2.8f\n",Net->Min_EVENT[k] );
			fprintf(pWeightFile,"%2.8f\n",Net->Max_EVENT[k] );
		}		
		for(j=0;j<M;j++)
  		{
			fprintf(pWeightFile,"%2.8f\n",Net->Min_Mertic[0]);
			fprintf(pWeightFile,"%2.8f\n",Net->Max_Mertic[0]);
		}
	}
  	if(SaveToFile)
  	{
  		fclose(pWeightFile);
  	}
}

void dyNUMA_ANN_load_knowledge(NET* Net, CHAR* WeightFile)
{
	//printf("%s:\n",__func__);
	pWeightFile = fopen(WeightFile,"r");
	if(pWeightFile==NULL)
	{
		fprintf(stdout,"dyNUMA: Error Loading Knowledge file.. %s \n",WeightFile);
	}
	char line [ 128 ];
	int l,i,j,k;
	int ll,jj,ii;
	double weight;
	
     	for (l=1; l<NUM_LAYERS; l++) 
	{
	    	for (i=1; i<=Net->Layer[l]->Units; i++) 
		{
	      		for (j=0; j<=Net->Layer[l-1]->Units; j++) 
			{
				if(fgets ( line, sizeof(line), pWeightFile ) != NULL)
				{ 
					sscanf(line, "%d %d %d %lf\n", &ll,&ii,&jj, &weight);
   					Net->Layer[ll]->Weight[ii][jj]=weight;
#if DEBUG
					fprintf(stdout, "LOAD COFFICIENT @(%d %d %d) = %2.8f\n", ll,ii,jj,weight );
#endif
				}
      			}
  	  	}
  	}
	for(k=0;k<env.NUM_EXE_SIGNATURE;k++)
	{
		if(fgets ( line, sizeof(line), pWeightFile ) != NULL)
		sscanf(line,"%lf\n",&Net->Min_EVENT[k]);
		if(fgets ( line, sizeof(line), pWeightFile ) != NULL)
		sscanf(line,"%lf\n",&Net->Max_EVENT[k]);
#if DEBUG_GENERAL
		fprintf(stdout, "LOAD MINMAX Min_EVENT[%d]=%2.8f, Max_EVENT[%d]=%2.8f \n",
		k,Net->Min_EVENT[k],k,Net->Max_EVENT[k]);
#endif
	}

	for(j=0;j<M;j++)
  	{
		if(fgets ( line, sizeof(line), pWeightFile ) != NULL)
		sscanf(line,"%lf\n",&Net->Min_Mertic[0]);
		if(fgets ( line, sizeof(line), pWeightFile ) != NULL)
		sscanf(line,"%lf\n",&Net->Max_Mertic[0]);
#if DEBUG_GENERAL
		fprintf(stdout, "LOAD MINMAX Min_Mertic[%d]=%2.8f, Max_Mertic[%d]=%2.8f\n",
		j,Net->Min_Mertic[0],j,Net->Max_Mertic[0]);
#endif
	}	

     	fclose ( pWeightFile );		

}

void RestoreWeights(NET* Net)
{
  	INT l,i,j;
  	for (l=1; l<NUM_LAYERS; l++) 
	{
		for (i=1; i<=Net->Layer[l]->Units; i++) 
		{  	 
			for (j=0; j<=Net->Layer[l-1]->Units; j++)
		    	{
      				Net->Layer[l]->Weight[i][j] = Net->Layer[l]->WeightSave[i][j];
      			}
    		}
  	}
}

void SetInput(NET* Net, int index, BOOL Training)
{
	INT i;
	FILE *temp;
  	for (i=1; i<=Net->InputLayer->Units; i++) 
	{
    		Net->InputLayer->Output[i] = glb.te[index].input[(i-1)/env.NUM_EXE_SIGNATURE][(i-1)%env.NUM_EXE_SIGNATURE];

  	}
}

void GetOutput(NET* Net, DOUBLE* Output)
{
  	INT i;
  	for (i=1; i<=Net->OutputLayer->Units; i++) 
	{
    		Output[i-1] = Net->OutputLayer->Output[i];
	}
}
/******************************************************************************
                     P R O P A G A T I N G   S I G N A L S
 ******************************************************************************/


void PropagateLayer(NET* Net, LAYER* Lower, LAYER* Upper)
{
  	INT  i,j;
  	DOUBLE Sum;

  	for (i=1; i<=Upper->Units; i++) 
	{
    		Sum = 0;
    		for (j=0; j<=Lower->Units; j++) 
		{
      			Sum += Upper->Weight[i][j] * Lower->Output[j];
    		}
    		Upper->Output[i] = 1 / (1 + exp(-Net->Gain * Sum)); //output (0-1)
  	}
}


void PropagateNet(NET* Net)
{
	INT l;   
  	for (l=0; l<NUM_LAYERS-1; l++) 
	{
    		PropagateLayer(Net, Net->Layer[l], Net->Layer[l+1]);
  	}
}


/******************************************************************************
                  B A C K P R O P A G A T I N G   E R R O R S
 ******************************************************************************/


void ComputeOutputError(NET* Net, DOUBLE* Target)
{
  	INT  i;
  	DOUBLE Out, Err;
   
  	Net->Error = 0;
  	for (i=1; i<=Net->OutputLayer->Units; i++) 
	{
	    	Out = Net->OutputLayer->Output[i];
    		Err = Target[i-1]-Out;
    		Net->OutputLayer->Error[i] = Net->Gain * Out * (1-Out) * Err; //Derivative of activation function *Err 
    		Net->Error += 0.5 * sqr(Err);
	}
}//Ok.


void BackpropagateLayer(NET* Net, LAYER* Upper, LAYER* Lower)
{
  INT  i,j;
  DOUBLE Out, Err;
   for (i=1; i<=Lower->Units; i++) {
    Out = Lower->Output[i];
    Err = 0;
    for (j=1; j<=Upper->Units; j++) {
      Err += Upper->Weight[j][i] * Upper->Error[j]; //sum up error
    }
    Lower->Error[i] = Net->Gain * Out * (1-Out) * Err;
  }
 
}


void BackpropagateNet(NET* Net)
{
  INT l;
   
  for (l=NUM_LAYERS-1; l>1; l--) {
    BackpropagateLayer(Net, Net->Layer[l], Net->Layer[l-1]);
  }
}


void AdjustWeights(NET* Net)
{
  INT  l,i,j;
  DOUBLE Out, Err, dWeight;
   
  for (l=1; l<NUM_LAYERS; l++) {
    for (i=1; i<=Net->Layer[l]->Units; i++) {
      for (j=0; j<=Net->Layer[l-1]->Units; j++) {
        Out = Net->Layer[l-1]->Output[j];
        Err = Net->Layer[l]->Error[i];
        dWeight = Net->Layer[l]->dWeight[i][j];
        Net->Layer[l]->Weight[i][j] += Net->Eta * Err * Out + Net->Alpha * dWeight;
        Net->Layer[l]->dWeight[i][j] = Net->Eta * Err * Out;
      }
    }
  }
}


/******************************************************************************
                      S I M U L A T I N G   T H E   N E T
 ******************************************************************************/


void SimulateNet(NET* Net, int index, DOUBLE* Output, DOUBLE* Target, BOOL Training)
{
	SetInput(Net, index, Training);
	PropagateNet(Net);
	GetOutput(Net, Output);   
  	ComputeOutputError(Net, Target);
  	if (Training) 
	{
	    	BackpropagateNet(Net);
		AdjustWeights(Net);
  	}
}


void TrainNet(NET* Net, INT Epochs)
{
	INT  index, n;
  	DOUBLE Output[M];

  	for (n=0; n<Epochs*TRAIN_YEARS; n++) 
	{
    		index = RandomEqualINT(TRAIN_LWB, TRAIN_UPB);
		//printf("select = %f\n",glb.te[index].select[0]);
		SimulateNet(Net,index, Output, &(glb.te[index].select[0]), TRUE);
  	}  
}


void TestNet(NET* Net)
{
  	INT  index;
  	DOUBLE Output[M];

  	TrainError = 0;
  	for (index=TRAIN_LWB; index<=TRAIN_UPB; index++) 
	{
   		SimulateNet(Net, index, Output, &(glb.te[index].select[0]), FALSE);
    		TrainError += Net->Error;
	}
  	TestError = 0;
  	for (index=TEST_LWB; index<=TEST_UPB; index++) 
	{
    		SimulateNet(Net, index, Output, &(glb.te[index].select[0]), FALSE);
    		TestError += Net->Error;
  	}

  	fprintf(stdout, "\nNMSE is %0.13f on Training Set and %0.13f on Test Set\n",
        		TrainError / TrainErrorPredictingMean,
             		TestError / TestErrorPredictingMean);

}


void EvaluateNet(NET* Net)
{
	
	INT  index,i;
  	DOUBLE Output [M];
  	DOUBLE AvgError;
  	AvgError=0;
	DOUBLE count;
  	count=0;
  	INT best_prediction_choice = -1;
	INT best_man_choice = -1;
	DOUBLE val1 = MAX_DOUBLE;
	DOUBLE val2 = MAX_DOUBLE;
	
	fprintf(stdout, "\n\n\n");
	fprintf(stdout, "Index	OutputIndex	Region	Output Actual	Error rate\n");
  	fprintf(stdout, "\n");
  	for (index=EVAL_LWB; index<=EVAL_UPB; index++) 
	{
    		SimulateNet(Net, index, Output,  &(glb.te[index].select[0]), FALSE);
		best_prediction_choice = -1;
		best_man_choice = -1;
		val1 = MAX_DOUBLE;
		val2 = MAX_DOUBLE;
		DOUBLE c,d;
		for(i=0;i<M;i++)
		{
			c = (Output[i] - LO)/(HI-LO)*(Net->Max_Mertic[0]-Net->Min_Mertic[0])+Net->Min_Mertic[0];
			d = (glb.te[index].select[i] - LO)/(HI-LO)*(Net->Max_Mertic[0]-Net->Min_Mertic[0])+Net->Min_Mertic[0];
			DOUBLE error=fabs(Output[i]-glb.te[index].select[i])/glb.te[index].select[i];
			
			AvgError+=error;
			count++;
			if(val1>Output[i])
			{
				val1=Output[i];
				best_prediction_choice=i;	
			}
			if(val2>glb.te[index].select[i] )
			{
				val2=glb.te[index].select[i];
				best_man_choice=i;	
			}
			fprintf(stdout,"%2d	%d	RID_%d	%2.6f	%2.6f	%2.6f	%2.6f\n"
					, index, i, glb.te[index].rid
					, Output[i],glb.te[index].select[i]
					, cint(c),d);		
		}
			//fprintf(stdout,"Region_%d BP %2d BM %2d Select %f\n", glb.te[index].rid, best_prediction_choice,best_man_choice, glb.te[index].select[0]);
		
	}
	fprintf(stdout,"Average Error Rate:%2.6f\n",AvgError/count);    
   
}


void dyNUMA_ANN_EvaluateNet(NET* Net)
{
	
}
void dyNUMA_ANN_GenerateNetwork(NET* Net)
{
	INT l,i;

	Net->Layer = (LAYER**) calloc(NUM_LAYERS, sizeof(LAYER*));
   
	for (l=0; l<NUM_LAYERS; l++) 
  	{
    		Net->Layer[l] = (LAYER*) malloc(sizeof(LAYER));
    		Net->Layer[l]->Id         = l; 
    		Net->Layer[l]->Units      = Units[l];
    		Net->Layer[l]->Output     = (DOUBLE*)  calloc(Units[l]+1, sizeof(DOUBLE)); //Why +1?, BIAS
    		Net->Layer[l]->Error      = (DOUBLE*)  calloc(Units[l]+1, sizeof(DOUBLE));
    		Net->Layer[l]->Weight     = (DOUBLE**) calloc(Units[l]+1, sizeof(DOUBLE*));
    		Net->Layer[l]->WeightSave = (DOUBLE**) calloc(Units[l]+1, sizeof(DOUBLE*));
    		Net->Layer[l]->dWeight    = (DOUBLE**) calloc(Units[l]+1, sizeof(DOUBLE*));
    		Net->Layer[l]->Output[0]  = BIAS;
      
   		if (l != 0) 
    		{  
	    		for (i=1; i<=Units[l]; i++) 
	    		{
	        		Net->Layer[l]->Weight[i]     = (DOUBLE*) calloc(Units[l-1]+1, sizeof(DOUBLE));
	        		Net->Layer[l]->WeightSave[i] = (DOUBLE*) calloc(Units[l-1]+1, sizeof(DOUBLE));
	        		Net->Layer[l]->dWeight[i]    = (DOUBLE*) calloc(Units[l-1]+1, sizeof(DOUBLE));
		    	}    
    		}
  	}
  
	Net->InputLayer  = Net->Layer[0];
  	Net->OutputLayer = Net->Layer[NUM_LAYERS - 1];
  	Net->Alpha       = 0.2;
  	Net->Eta         = 0.05;
  	Net->Gain        = 1;
}


void dyNUMA_ANN_start()
{

	NET  Net;
	BOOL Stop;
	DOUBLE MinTestError;

  	InitializeRandoms();
  	dyNUMA_ANN_GenerateNetwork(&Net);
  	RandomWeights(&Net);
    	dyNUMA_ANN_InitializeApplication(&Net);

  	Stop = FALSE;
  	MinTestError = MAX_DOUBLE;
  	do 
	{
    		TrainNet (&Net, 10);
    		TestNet(&Net);
    		fprintf(stdout,"\nTestError=%3.30f,MinTestError=%3.30f %5.5f\n ",TestError,MinTestError,TestError/MinTestError);
    		if ( TestError < MinTestError) 
		{
      			 
      			fprintf(stdout, " - saving Weights ...");
      			MinTestError = TestError;	
      			dyNUMA_ANN_save_knowledge(&Net,FALSE );
    		}
    		else if (TestError > (1.2)* MinTestError) 
		{
			fprintf(stdout, " - stopping Training and restoring Weights ...");
      			Stop = TRUE;
      			RestoreWeights(&Net);
    		}
    	
  	}while (NOT Stop);

	TestNet(&Net);
	EvaluateNet(&Net);
	if(AvgError<0.5)
  	{
  		dyNUMA_ANN_save_knowledge(&Net,TRUE );
  	} 

}
