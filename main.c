#include "pss_nr.h"

void main() {

    int32_t sync_pos;
    int frame = 307200;
    int nb_rx = 1;
    int n_frames = 2;
    int is;

    PHY_VARS_NR_UE *ue = (PHY_VARS_NR_UE *)malloc(sizeof(PHY_VARS_NR_UE));

    // rxdata
    ue->common_vars.rxdata = (int32_t **)malloc(nb_rx * sizeof(int32_t *));
    for(int i = 0; i < nb_rx; i++) {
        ue->common_vars.rxdata[i] = (int32_t *)malloc((frame*2+2048) * sizeof(int32_t));
    }

    ue->frame_parms.nb_antennas_rx = nb_rx;
    ue->frame_parms.samples_per_frame = frame;
    ue->frame_parms.subcarrier_spacing = 15000;
    ue->frame_parms.first_carrier_offset = 1412;
    ue->frame_parms.ssb_start_subcarrier = 516;
    ue->frame_parms.ofdm_symbol_size = 2048;
    ue->common_vars.gNb_id = 0;
    

    init_context_synchro_nr(&ue->frame_parms); // init_context_synchro_nr


    // ...

    FILE *file = fopen("input/rxdata238272.pcm", "r");
    if (file == NULL) {
        printf("error opening file\n");
        exit(0);
    }

    fread(&ue->common_vars.rxdata[0][0], 307200*2*sizeof(int32_t), 1, file);

    fclose(file);

    // ...
     
    // initial sync performed on two successive frames, if pbch passes on first frame, no need to process second frame 
    // only one frame is used for symulation tools
    for(is=0; is<n_frames;is++) {
        /* process pss search on received buffer */
        sync_pos = pss_synchro_nr(ue, is, NO_RATE_CHANGE);


        // ...
    }

    // ...


    free_context_synchro_nr();
    for(int i = 0; i < nb_rx; i++){
        free(ue->common_vars.rxdata[i]);
    }
    free(ue->common_vars.rxdata);

    free(ue);

}