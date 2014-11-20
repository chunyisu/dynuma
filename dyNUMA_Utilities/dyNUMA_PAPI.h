#ifndef DYNUMA_PAPI_H
#define DYNUMA_PAPI_H
#include "../global.h"
#include "papi.h"
#include "omp.h"

#define PAPI_PRINT_ERR( rc, arg )			\
  fprintf( stderr, "%s:%d -- " #arg ": %s %s\n",	\
	   __FILE__, __LINE__,				\
	   PAPI_strerror(rc), PAPI_descr_error(rc) )

#define PAPI_CHECK( rc, arg )		     \
  if ( (rc = arg) != PAPI_OK ) {             \
    PAPI_PRINT_ERR( rc, arg );		     \
  }

#define PAPI_CHECK_EXEC( rc, arg, cmd )		\
  if ( (rc = arg) != PAPI_OK ) {		\
    PAPI_PRINT_ERR( rc, arg );			\
    cmd;					\
  }


INT dyNUMA_PAPI_init();
INT dyNUMA_PAPI_start(REGION rid);
INT dyNUMA_PAPI_read(REGION rid);
INT dyNUMA_PAPI_stop(REGION rid);
INT  dyNUMA_PAPI_th_add_events(CHAR *str_event, INT event_set);
INT dyNUMA_PAPI_print_region(REGION rid);
#endif




