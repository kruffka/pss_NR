#include "defs.h"


/*******************************************************************
*
* NAME :         pss_synchro_nr
*
* PARAMETERS :   int rate_change
*
* RETURN :       position of detected pss
*
* DESCRIPTION :  pss search can be done with sampling decimation.*
*
*********************************************************************/

int pss_synchro_nr(PHY_VARS_NR_UE *PHY_vars_UE, int is, int rate_change)
{
  NR_DL_FRAME_PARMS *frame_parms = &(PHY_vars_UE->frame_parms);
  int synchro_position;
  int **rxdata = NULL;
  int fo_flag = PHY_vars_UE->UE_fo_compensation;  // flag to enable freq <offset> estimation and compensation

#ifdef DBG_PSS_NR

  LOG_M("rxdata0_rand.m","rxd0_rand", &PHY_vars_UE->common_vars.rxdata[0][0], frame_parms->samples_per_frame, 1, 1);

#endif

// У нас 1, поэтому функции в этом условии не переносил
  if (rate_change != 1) {

//     rxdata = (int32_t**)malloc16(frame_parms->nb_antennas_rx*sizeof(int32_t*));

//     for (int aa=0; aa < frame_parms->nb_antennas_rx; aa++) {
//       rxdata[aa] = (int32_t*) malloc16_clear( (frame_parms->samples_per_frame+8192)*sizeof(int32_t));
//     }

// #ifdef SYNCHRO_DECIMAT

//     decimation_synchro_nr(PHY_vars_UE, rate_change, rxdata);

// #endif

  }
  else {

    rxdata = PHY_vars_UE->common_vars.rxdata;
  }

#ifdef DBG_PSS_NR

  LOG_M("rxdata0_des.m","rxd0_des", &rxdata[0][0], frame_parms->samples_per_frame,1,1);

#endif


#if TEST_SYNCHRO_TIMING_PSS

  start_meas(&generic_time[TIME_PSS]);

#endif

  synchro_position = pss_search_time_nr(rxdata,
                                        frame_parms,
					                    fo_flag,
                                        is,
                                        (int *)&PHY_vars_UE->common_vars.gNb_id,
					                    (int *)&PHY_vars_UE->common_vars.freq_offset);

#if TEST_SYNCHRO_TIMING_PSS

  stop_meas(&generic_time[TIME_PSS]);

  int duration_ms = generic_time[TIME_PSS].p_time/(cpuf*1000.0);

  #ifndef NR_UNIT_TEST

    printf("PSS execution duration %4d microseconds \n", duration_ms);

  #endif

#endif

#ifdef SYNCHRO_DECIMAT

  if (rate_change != 1) {

    if (rxdata[0] != NULL) {

      for (int aa=0;aa<frame_parms->nb_antennas_rx;aa++) {
        free(rxdata[aa]);
      }

      free(rxdata);
    }

    restore_frame_context_pss_nr(frame_parms, rate_change);  
  }
#endif

  return synchro_position;
}





static inline int abs32(int x)
{
  return (((int)((short*)&x)[0])*((int)((short*)&x)[0]) + ((int)((short*)&x)[1])*((int)((short*)&x)[1]));
}

static inline int64_t abs64(int64_t x)
{
  return (((int64_t)((int32_t*)&x)[0])*((int64_t)((int32_t*)&x)[0]) + ((int64_t)((int32_t*)&x)[1])*((int64_t)((int32_t*)&x)[1]));
}

static inline double angle64(int64_t x)
{

  double re=((int32_t*)&x)[0];
  double im=((int32_t*)&x)[1];
  return (atan2(im,re));
}


/*******************************************************************
*
* NAME :         pss_search_time_nr
*
* PARAMETERS :   received buffer
*                frame parameters
*
* RETURN :       position of detected pss
*
* DESCRIPTION :  Synchronisation on pss sequence is based on a time domain correlation between received samples and pss sequence
*                A maximum likelihood detector finds the timing offset (position) that corresponds to the maximum correlation
*                Length of received buffer should be a minimum of 2 frames (see TS 38.213 4.1 Cell search)
*                Search pss in the received buffer is done each 4 samples which ensures a memory alignment to 128 bits (32 bits x 4).
*                This is required by SIMD (single instruction Multiple Data) Extensions of Intel processors
*                Correlation computation is based on a a dot product which is realized thank to SIMS extensions
*
*                                    (x frames)
*     <--------------------------------------------------------------------------->
*
*
*     -----------------------------------------------------------------------------
*     |                      Received UE data buffer                              |
*     ----------------------------------------------------------------------------
*                -------------
*     <--------->|    pss    |
*      position  -------------
*                ^
*                |
*            peak position
*            given by maximum of correlation result
*            position matches beginning of first ofdm symbol of pss sequence
*
*     Remark: memory position should be aligned on a multiple of 4 due to I & Q samples of int16
*             An OFDM symbol is composed of x number of received samples depending of Rf front end sample rate.
*
*     I & Q storage in memory
*
*             First samples       Second  samples
*     ------------------------- -------------------------  ...
*     |     I1     |     Q1    |     I2     |     Q2    |
*     ---------------------------------------------------  ...
*     ^    16  bits   16 bits  ^
*     |                        |
*     ---------------------------------------------------  ...
*     |         sample 1       |    sample   2          |
*    ----------------------------------------------------  ...
*     ^
*
*********************************************************************/

#define DOT_PRODUCT_SCALING_SHIFT    (17)

int pss_search_time_nr(int **rxdata, ///rx data in time domain
                       NR_DL_FRAME_PARMS *frame_parms,
		       int fo_flag,
                       int is,
                       int *eNB_id,
		       int *f_off)
{
  unsigned int n, ar, peak_position, pss_source;
  int64_t peak_value;
  int64_t result;
  int64_t avg[NUMBER_PSS_SEQUENCE];
  double ffo_est=0;

  // performing the correlation on a frame length plus one symbol for the first of the two frame
  // to take into account the possibility of PSS in between the two frames 
  unsigned int length;
  if (is==0)
    length = frame_parms->samples_per_frame + (2*frame_parms->ofdm_symbol_size);
  else
    length = frame_parms->samples_per_frame;

  AssertFatal(length>0,"illegal length %d\n",length);
  for (int i = 0; i < NUMBER_PSS_SEQUENCE; i++) AssertFatal(pss_corr_ue[i] != NULL,"pss_corr_ue[%d] not yet allocated! Exiting.\n", i);

  peak_value = 0;
  peak_position = 0;
  pss_source = 0;

  int maxval=0;
  for (int i=0;i<2*(frame_parms->ofdm_symbol_size);i++) {
    maxval = max(maxval,primary_synchro_time_nr[0][i]);
    maxval = max(maxval,-primary_synchro_time_nr[0][i]);
    maxval = max(maxval,primary_synchro_time_nr[1][i]);
    maxval = max(maxval,-primary_synchro_time_nr[1][i]);
    maxval = max(maxval,primary_synchro_time_nr[2][i]);
    maxval = max(maxval,-primary_synchro_time_nr[2][i]);
  }
  int shift = log2_approx(maxval);//*(frame_parms->ofdm_symbol_size+frame_parms->nb_prefix_samples)*2);

  /* Search pss in the received buffer each 4 samples which ensures a memory alignment on 128 bits (32 bits x 4 ) */
  /* This is required by SIMD (single instruction Multiple Data) Extensions of Intel processors. */
  /* Correlation computation is based on a a dot product which is realized thank to SIMS extensions */

  for (int pss_index = 0; pss_index < NUMBER_PSS_SEQUENCE; pss_index++) {
    avg[pss_index]=0;
    memset(pss_corr_ue[pss_index],0,length*sizeof(int64_t)); 
  }

  for (n=0; n < length; n+=4) { //

    for (int pss_index = 0; pss_index < NUMBER_PSS_SEQUENCE; pss_index++) {

      if ( n < (length - frame_parms->ofdm_symbol_size)) {

        /* calculate dot product of primary_synchro_time_nr and rxdata[ar][n] (ar=0..nb_ant_rx) and store the sum in temp[n]; */
        for (ar=0; ar<frame_parms->nb_antennas_rx; ar++) {

          /* perform correlation of rx data and pss sequence ie it is a dot product */
          result  = dot_product64((short*)primary_synchro_time_nr[pss_index], 
				  (short*) &(rxdata[ar][n+is*frame_parms->samples_per_frame]), 
				  frame_parms->ofdm_symbol_size, 
				  shift);
	  pss_corr_ue[pss_index][n] += abs64(result);
          //((short*)pss_corr_ue[pss_index])[2*n] += ((short*) &result)[0];   /* real part */
          //((short*)pss_corr_ue[pss_index])[2*n+1] += ((short*) &result)[1]; /* imaginary part */
          //((short*)&synchro_out)[0] += ((int*) &result)[0];               /* real part */
          //((short*)&synchro_out)[1] += ((int*) &result)[1];               /* imaginary part */

        }
      }
 
      /* calculate the absolute value of sync_corr[n] */
      avg[pss_index]+=pss_corr_ue[pss_index][n];
      if (pss_corr_ue[pss_index][n] > peak_value) {
        peak_value = pss_corr_ue[pss_index][n];
        peak_position = n;
        pss_source = pss_index;

#ifdef DEBUG_PSS_NR
        printf("pss_index %d: n %6u peak_value %15llu\n", pss_index, n, (unsigned long long)pss_corr_ue[pss_index][n]);
#endif
      }
    }
  }

  if (fo_flag){

	  // fractional frequency offset computation according to Cross-correlation Synchronization Algorithm Using PSS
	  // Shoujun Huang, Yongtao Su, Ying He and Shan Tang, "Joint time and frequency offset estimation in LTE downlink," 7th International Conference on Communications and Networking in China, 2012.

	  int64_t result1,result2;
	  // Computing cross-correlation at peak on half the symbol size for first half of data
	  result1  = dot_product64((short*)primary_synchro_time_nr[pss_source], 
				  (short*) &(rxdata[0][peak_position+is*frame_parms->samples_per_frame]), 
				  frame_parms->ofdm_symbol_size>>1, 
				  shift);
	  // Computing cross-correlation at peak on half the symbol size for data shifted by half symbol size 
	  // as it is real and complex it is necessary to shift by a value equal to symbol size to obtain such shift
	  result2  = dot_product64((short*)primary_synchro_time_nr[pss_source]+(frame_parms->ofdm_symbol_size), 
				  (short*) &(rxdata[0][peak_position+is*frame_parms->samples_per_frame])+frame_parms->ofdm_symbol_size, 
				  frame_parms->ofdm_symbol_size>>1, 
				  shift);

	  int64_t re1,re2,im1,im2;
	  re1=((int*) &result1)[0];
	  re2=((int*) &result2)[0];
	  im1=((int*) &result1)[1];
	  im2=((int*) &result2)[1];

 	  // estimation of fractional frequency offset: angle[(result1)'*(result2)]/pi
	  ffo_est=atan2(re1*im2-re2*im1,re1*re2+im1*im2)/M_PI;

    #ifdef DBG_PSS_NR
      printf("ffo %lf\n",ffo_est);
    #endif
  }

  // computing absolute value of frequency offset
  *f_off = ffo_est*frame_parms->subcarrier_spacing;  
  	
  printf("ffo %d Hz ffo_est %lf scs %d\n", *f_off, ffo_est, frame_parms->subcarrier_spacing);


  for (int pss_index = 0; pss_index < NUMBER_PSS_SEQUENCE; pss_index++) avg[pss_index]/=(length/4);

  *eNB_id = pss_source;

  LOG_I(PHY,"[UE] nr_synchro_time: Sync source = %d, Peak found at pos %d, val = %llu (%d dB) avg %d dB, ffo %lf\n", pss_source, peak_position, (unsigned long long)peak_value, dB_fixed64(peak_value),dB_fixed64(avg[pss_source]),ffo_est);

  if (peak_value < 5*avg[pss_source])
    return(-1);


#ifdef DBG_PSS_NR

  static int debug_cnt = 0;

  if (debug_cnt == 0) {
    LOG_M("pss_corr_ue0.m","pss_corr_ue0",pss_corr_ue[0],length,1,6);
    LOG_M("pss_corr_ue1.m","pss_corr_ue1",pss_corr_ue[1],length,1,6);
    LOG_M("pss_corr_ue2.m","pss_corr_ue2",pss_corr_ue[2],length,1,6);
    if (is)
      LOG_M("rxdata1.m","rxd0",rxdata[frame_parms->samples_per_frame],length,1,1); 
    else
      LOG_M("rxdata0.m","rxd0",rxdata[0],length,1,1);
  } else {
    debug_cnt++;
  }

#endif

  return(peak_position);
}
