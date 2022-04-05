#include <fftw3.h>
#include <stdint.h>

#define REAL 0
#define IMAG 1


void int_to_double(int16_t *int_prachF, fftw_complex* signal, int num_ps) {

    for(int i = 0; i < num_ps; i++){
        signal[i][REAL] = ((int16_t *)&int_prachF[0])[2*i] / 32767.0f;
        signal[i][IMAG] = ((int16_t *)&int_prachF[0])[2*i + 1] / 32767.0f;
    }

}

void double_to_int(fftw_complex* result, int16_t* int_prach, int num_ps) {
    int amp_div = 100;

    for(int i = 0; i < num_ps; i++){
        ((int16_t *)&int_prach[0])[2*i] = result[i][REAL]/amp_div * 32767;
        ((int16_t *)&int_prach[0])[2*i+1] = result[i][IMAG]/amp_div * 32767;
    }

}

int idft(int length, int16_t *txdataF, int16_t *txdata, int scaling) {
    fftw_complex signal[length];
    fftw_complex result[length];

    fftw_plan plan = fftw_plan_dft_1d(length,
                                      signal,
                                      result,
                                      FFTW_BACKWARD, //idft
                                      FFTW_ESTIMATE);

    int num_ps = length;

    int_to_double(txdataF, signal, num_ps); // from txdataF (int) to signal (double)
    fftw_execute(plan);
    double_to_int(result, txdata, num_ps); // from result (double) to txdata (int)


    fftw_destroy_plan(plan);

    return 0;
}