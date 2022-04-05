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

//   VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_PSS_SYNCHRO_NR, VCD_FUNCTION_IN);
#ifdef DBG_PSS_NR

  LOG_M("rxdata0_rand.m","rxd0_rand", &PHY_vars_UE->common_vars.rxdata[0][0], frame_parms->samples_per_frame, 1, 1);

#endif

  if (rate_change != 1) {

    rxdata = (int32_t**)malloc16(frame_parms->nb_antennas_rx*sizeof(int32_t*));

    for (int aa=0; aa < frame_parms->nb_antennas_rx; aa++) {
      rxdata[aa] = (int32_t*) malloc16_clear( (frame_parms->samples_per_frame+8192)*sizeof(int32_t));
    }
    
#ifdef SYNCHRO_DECIMAT
    decimation_synchro_nr(PHY_vars_UE, rate_change, rxdata);
#endif

  }
  else {

    rxdata = PHY_vars_UE->common_vars.rxdata;
  }

#ifdef DBG_PSS_NR

  LOG_M("rxdata0_des.m","rxd0_des", &rxdata[0][0], frame_parms->samples_per_frame,1,1);

#endif

#if TEST_SYNCHRO_TIMING_PSS

  opp_enabled = 1;

  start_meas(&generic_time[TIME_PSS]);

#endif

  VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_PSS_SEARCH_TIME_NR, VCD_FUNCTION_IN);
  synchro_position = pss_search_time_nr(rxdata,
                                        frame_parms,
					fo_flag,
                                        is,
                                        (int *)&PHY_vars_UE->common_vars.eNb_id,
					(int *)&PHY_vars_UE->common_vars.freq_offset);

  VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_PSS_SEARCH_TIME_NR, VCD_FUNCTION_OUT);

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

  VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_PSS_SYNCHRO_NR, VCD_FUNCTION_OUT);
  return synchro_position;
}