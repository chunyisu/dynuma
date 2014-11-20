#ifndef GLOBAL_H
#define GLOBAL_H



#include <stdlib.h>
#include <stdio.h>

#define ENABLE_DYNUMA 1
#define ENABLE_PAPI 1
// Debugging flags //

#define DEBUG 0
#define DEBUG_GENERAL 1
#define DEBUG_ENABLE_PRINT_REGION 0
#define DEBUG_POST_PROCESSING 0

#define SUCCESS 0
#define FAIL 1
#define MAX_NUM_LOOP   3000
#define MAX_NUM_REGION 100
#define MAX_NUM_THREAD 64
#define MAX_NUM_RECORD 4 /*The MAX_NUM_RECORD=3 environment PAPI_EVENTS + 1 PAPI_TOT_CYC*/

/* dyNUMA MODE */
#define TRAINING      1
#define COLLECTION    2
#define OPTIMIZATION  3

/* Training Mode */
#define CONCURRENCY 1
#define MAPPING     2
#define SCHEDULING  3
/* OTHERS  */
#define USEC_UNIT 1000000
#define MAX_CHAR_PERLINE 20000
#define MAX_ELEMENT 20000

#define FALSE         0
#define TRUE          1
#define NOT           !
#define AND           &&
#define OR            ||

#define MIN_DOUBLE      -HUGE_VAL
#define MAX_DOUBLE      +HUGE_VAL
#define _MIN(x,y)      ((x)<(y) ? (x) : (y))
#define _MAX(x,y)      ((x)>(y) ? (x) : (y))

// Print seperation //
#define SEPSD  "------------------------------- %36s %3d ---------------------------------------\n"
#define SEPS  "------------------------------- %40s ---------------------------------------\n"
#define SEP  "----------------------------------------------------------------------------------------------------------------\n"

typedef int REGION;
typedef int BOOL;
typedef int INT;
typedef double DOUBLE;
typedef char CHAR;

//Environment variables data struct.
typedef struct
{
	INT DYNUMA_VERSION;
	CHAR* PLATFORM;
	INT OPT_CONCURRENCY;
	INT OPT_TMA;
	INT OPT_SCHEDULE;
	INT NUM_REGIONS;
	INT NUM_THREADS;
	INT NUM_LOOPS;
	INT PLATFORM_MAX_NUM_THREAD;
	CHAR* CHAR_PAPI_EVENT[4];
	CHAR* PREDICT_METRIC;
	CHAR* TRAINING_FILE;
	CHAR* ANN_CONCURRENCY_KNOWLEDGE_FILE;
	CHAR* ANN_MAPPING_KNOWLEDGE_FILE;
	INT PAPI_NUM_EVENT;
	INT PAPI_EVENT[4]; /* First 3 events are assigned by environment variables: PAPI_EVENT_1, PAPI_EVENT_2 and PAPI_EVENT_3. The max number of events is 3. The 4th event is PAPI_TOT_CYCLES */
	INT NUM_PROFILE_ITERATIONS;
	INT NUM_EXE_SIGNATURE;
	INT TRAINING_MODE;
 
} ENV;
extern ENV env;

/* TRAINING ELEMENTS*/
typedef struct
{
	INT lid;
	INT rid;
	INT thread_pos[MAX_NUM_THREAD];
	DOUBLE input[MAX_NUM_THREAD][MAX_NUM_RECORD];
	DOUBLE target;
	INT predicted_concurrency;
	INT predicted_mapping;
}RECORDING_ELEMENT;

typedef struct
{
	INT lid;
	INT rid;
	CHAR mode;
	INT thread_pos[MAX_NUM_THREAD];
	DOUBLE input[MAX_NUM_THREAD][MAX_NUM_RECORD];
	DOUBLE target[MAX_NUM_THREAD];
	DOUBLE select[1];
}TRAINING_ELEMENT;


typedef struct
{
	INT    lid; //Current loop id
	REGION rid; //Current region id
	DOUBLE profile_data[MAX_NUM_LOOP][MAX_NUM_REGION][MAX_NUM_THREAD][MAX_NUM_RECORD];
	DOUBLE region_elapsed_time[MAX_NUM_LOOP][MAX_NUM_REGION]; /*Collected elapsed time for each region */
	INT thread_pos[MAX_NUM_LOOP][MAX_NUM_REGION][MAX_NUM_THREAD];
	RECORDING_ELEMENT re[MAX_NUM_LOOP][MAX_NUM_REGION];
	INT event_set[MAX_NUM_THREAD];
	long long int dummy[MAX_NUM_LOOP][MAX_NUM_REGION][MAX_NUM_THREAD][MAX_NUM_RECORD];
	DOUBLE execution_signature[MAX_NUM_REGION][MAX_NUM_THREAD][MAX_NUM_RECORD];//execution signature collected by optimizer
	FILE * training_file;
	TRAINING_ELEMENT te[MAX_ELEMENT];
	INT num_te;//total number of training elements
}GLOBAL;
extern GLOBAL glb;

#endif

