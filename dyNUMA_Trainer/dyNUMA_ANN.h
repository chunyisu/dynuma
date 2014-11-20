#ifndef DYNUMA_ANN_H
#define DYNUMA_ANN_H
#include <math.h>

#include "../global.h"

/******************************************************************************
               A P P L I C A T I O N - S P E C I F I C   C O D E
 ******************************************************************************/


#define NUM_LAYERS    6
#define N             4 // {IPC,LLC_MISS_PER_CYC_MISSES,BRANCH_MISS_PER_CYC}*32 cores 
#define IL            10  //INNER LAYER UNIT , 4 memory node  
#define M             1
#define TRAIN_LWB     (0)
#define TRAIN_UPB     (800)//(1546)//
#define TRAIN_YEARS   (TRAIN_UPB - TRAIN_LWB + 1)
#define TEST_LWB      (801)//(1547)//
#define TEST_UPB      (900)//(1626)//
#define TEST_YEARS    (TEST_UPB - TEST_LWB + 1)
#define EVAL_LWB      (901)//(1627)//
#define EVAL_UPB      (1079)//(1695)
#define EVAL_YEARS    (EVAL_UPB - EVAL_LWB + 1)
#define sqr(x)        ((x)*(x))

#define LO            0.1
#define HI            0.9
#define BIAS          1
#define MAX_EVENT     5
typedef struct {                     /* A LAYER OF A NET:                     */
	INT             Id;            /* - layer id                            */
        INT             Units;         /* - number of units in this layer       */
        DOUBLE*         Output;        /* - output of ith unit                  */
        DOUBLE*         Error;         /* - error term of ith unit              */
        DOUBLE**        Weight;        /* - connection weights to ith unit      */
        DOUBLE**        WeightSave;    /* - saved weights for stopped training  */
        DOUBLE**        dWeight;       /* - last weight deltas for momentum     */
} LAYER;

typedef struct {                     /* A NET:                                */
        LAYER**       Layer;         /* - layers of this net                  */
        LAYER*        InputLayer;    /* - input layer                         */
        LAYER*        OutputLayer;   /* - output layer                        */
        DOUBLE          Alpha;         /* - momentum factor                     */
        DOUBLE          Eta;           /* - learning rate                       */
        DOUBLE          Gain;          /* - gain of sigmoid function            */
        DOUBLE          Error;         /* - total net error                     */
	DOUBLE Mean[M];
	DOUBLE Min_Mertic[M];
	DOUBLE Max_Mertic[M];
	DOUBLE Min_EVENT[MAX_EVENT];
	DOUBLE Max_EVENT[MAX_EVENT];
} NET;
extern DOUBLE Min_Mertic[M];
extern DOUBLE Max_Mertic[M];
extern DOUBLE Min_EVENT[MAX_EVENT];
extern DOUBLE Max_EVENT[MAX_EVENT];

void dyNUMA_ANN_InitializeApplication(NET* Net);
void dyNUMA_ANN_GenerateNetwork(NET* Net);
void dyNUMA_ANN_load_knowledge(NET* Net, CHAR* WeightFile);
void dyNUMA_ANN_start();
void PropagateNet(NET* Net);
void GetOutput(NET* Net, DOUBLE* Output);
void ComputeOutputError(NET* Net, DOUBLE* Target);
DOUBLE cint(DOUBLE x);
#endif
