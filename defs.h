#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define NO_RATE_CHANGE (1)

typedef struct {

  /// \brief Holds the received data in time domain.
  /// Should point to the same memory as PHY_vars->rx_vars[a].RX_DMA_BUFFER.
  /// - first index: rx antenna [0..nb_antennas_rx[
  /// - second index: sample [0..2*FRAME_LENGTH_COMPLEX_SAMPLES+2048[
  int32_t **rxdata;

} NR_UE_COMMON;

typedef struct NR_DL_FRAME_PARMS NR_DL_FRAME_PARMS;

struct NR_DL_FRAME_PARMS {

/// Number of samples in a radio frame
  uint32_t samples_per_frame;

  /// Number of Receive antennas in node
  uint8_t nb_antennas_rx;
};

/// Top-level PHY Data Structure for UE
typedef struct {

    // ...

    /// \brief Indicator that UE should enable estimation and compensation of frequency offset
    int UE_fo_compensation;

    NR_DL_FRAME_PARMS frame_parms;
    NR_UE_COMMON    common_vars;

    // ...

} PHY_VARS_NR_UE;



int pss_synchro_nr(PHY_VARS_NR_UE *PHY_vars_UE, int is, int rate_change);