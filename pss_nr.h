#include "defs.h"


/* PSS parameters */
#define  NUMBER_PSS_SEQUENCE          (3)
#define  INVALID_PSS_SEQUENCE         (NUMBER_PSS_SEQUENCE)
#define  LENGTH_PSS_NR                (127)
#define  N_SC_RB                      (12)     /* Resource block size in frequency domain expressed as a number if subcarriers */
#define  SCALING_PSS_NR               (3)
#define  SCALING_CE_PSS_NR            (13)     /* scaling channel estimation based on ps */
#define  PSS_IFFT_SIZE                (256)

#define  PSS_SC_START_NR              (52)     /* see from TS 38.211 table 7.4.3.1-1: Resources within an SS/PBCH block for PSS... */


#define  IQ_SIZE                      (sizeof(int16_t) * 2)        /* I and Q are alternatively stored into buffers */

#define DEFINE_VARIABLES_PSS_NR_H
// #define INIT_VARIABLES_PSS_NR_H


#ifdef DEFINE_VARIABLES_PSS_NR_H
#define EXTERN
#else
#define EXTERN  extern
#endif

#define Maxneighbor NUMBER_OF_UE_MAX
	#ifndef NB_ANTENNAS_RX
		#define NB_ANTENNAS_RX  4
	#endif

#define SYNCHRO_FFT_SIZE_MAX           (8192)                       /* maximum size of fft for synchronisation */
#define SYNC_TMP_SIZE                  (NB_ANTENNAS_RX*SYNCHRO_FFT_SIZE_MAX*IQ_SIZE) /* to be aligned with existing lte synchro */
#define SYNCF_TMP_SIZE                 (SYNCHRO_FFT_SIZE_MAX*IQ_SIZE)

//#define STATIC_SYNC_BUFFER

#ifdef STATIC_SYNC_BUFFER
/* buffer defined in file lte_sync_time */
EXTERN int16_t synchro_tmp[SYNC_TMP_SIZE]   __attribute__((aligned(32)));
EXTERN int16_t synchroF_tmp[SYNCF_TMP_SIZE] __attribute__((aligned(32)));
#else
EXTERN int16_t *synchro_tmp;
EXTERN int16_t *synchroF_tmp;
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
