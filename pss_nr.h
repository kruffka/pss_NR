#include "defs.h"


#define  LENGTH_PSS_NR                (127)

#define  IQ_SIZE                      (sizeof(int16_t) * 2)        /* I and Q are alternatively stored into buffers */

#define DEFINE_VARIABLES_PSS_NR_H
// #define INIT_VARIABLES_PSS_NR_H


#ifdef DEFINE_VARIABLES_PSS_NR_H
#define EXTERN
#else
#define EXTERN  extern
#endif


EXTERN int16_t *primary_synchro_time_nr[NUMBER_PSS_SEQUENCE]
#ifdef INIT_VARIABLES_PSS_NR_H
= { NULL, NULL, NULL}
#endif
;

EXTERN int16_t *primary_synchro_nr[NUMBER_PSS_SEQUENCE]
#ifdef INIT_VARIABLES_PSS_NR_H
= { NULL, NULL, NULL}
#endif
;
EXTERN int16_t *primary_synchro_nr2[NUMBER_PSS_SEQUENCE]
#ifdef INIT_VARIABLES_PSS_NR_H
= { NULL, NULL, NULL}
#endif
;

EXTERN int64_t *pss_corr_ue[NUMBER_PSS_SEQUENCE]
#ifdef INIT_VARIABLES_PSS_NR_H
= { NULL, NULL, NULL}
#endif
;

/* profiling structure */
// EXTERN time_stats_t generic_time[TIME_LAST];

void init_context_synchro_nr(NR_DL_FRAME_PARMS *frame_parms_ue);
void free_context_synchro_nr(void);
void init_context_pss_nr(NR_DL_FRAME_PARMS *frame_parms_ue);
void free_context_pss_nr(void);
int set_pss_nr(int ofdm_symbol_size);
int pss_synchro_nr(PHY_VARS_NR_UE *PHY_vars_UE, int is, int rate_change);
int pss_search_time_nr(int **rxdata, ///rx data in time domain
                       NR_DL_FRAME_PARMS *frame_parms,
		               int fo_flag,
                       int is,
                       int *eNB_id,
		               int *f_off);
