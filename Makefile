# UNIX Makefile
# 
CC=g++
PAPI_INC=${HOME}/workspace/tools/papi/include
C_FLAGS= -g -fopenmp -I${PAPI_INC} -I${PWD}

#C_FLAGS= -g -Wall -Werror -fopenmp -I${PAPI_INC} -I${PWD}
C_LDFLAGS= -g


All: libdyNUMA.a

libdyNUMA.a: dyNUMA.o dyNUMA_Collector.o dyNUMA_PAPI.o dyNUMA_Utilities.o dyNUMA_Trainer.o dyNUMA_ANN.o dyNUMA_Optimizer.o dyNUMA_Concurrency_Predictor.o dyNUMA_Mapping_Predictor.o global.o dyNUMA_FuncPointer.o
	ar rcs $@ dyNUMA.o dyNUMA_Collector.o dyNUMA_PAPI.o dyNUMA_Utilities.o dyNUMA_Trainer.o dyNUMA_ANN.o dyNUMA_Optimizer.o dyNUMA_Concurrency_Predictor.o dyNUMA_Mapping_Predictor.o global.o dyNUMA_FuncPointer.o

dyNUMA.o:
	${CC} ${C_FLAGS} -c dyNUMA.c
global.o:
	${CC} ${C_FLAGS} -c global.c
dyNUMA_Collector.o:
	${CC} ${C_FLAGS} -c dyNUMA_Collector/dyNUMA_Collector.c

dyNUMA_PAPI.o:
	${CC} ${C_FLAGS} -c dyNUMA_Utilities/dyNUMA_PAPI.c

dyNUMA_Utilities.o:
	${CC} ${C_FLAGS} -c dyNUMA_Utilities/dyNUMA_Utilities.c
	
dyNUMA_Trainer.o:
	${CC} ${C_FLAGS} -c dyNUMA_Trainer/dyNUMA_Trainer.c

dyNUMA_ANN.o:
	${CC} ${C_FLAGS} -c dyNUMA_Trainer/dyNUMA_ANN.c

dyNUMA_Optimizer.o:
	${CC} ${C_FLAGS} -c dyNUMA_Optimizer/dyNUMA_Optimizer.c

dyNUMA_Concurrency_Predictor.o:
	${CC} ${C_FLAGS} -c dyNUMA_Predictor/dyNUMA_Concurrency_Predictor.c

dyNUMA_Mapping_Predictor.o:
	${CC} ${C_FLAGS} -c dyNUMA_Predictor/dyNUMA_Mapping_Predictor.c

dyNUMA_FuncPointer.o:
	${CC} ${C_FLAGS} -c dyNUMA_Utilities/dyNUMA_FuncPointer.c

clean:
	${RM} *.exe *.core *.o *.a *~

