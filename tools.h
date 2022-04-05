#include <stdint.h>


#define max(a,b) cmax(a,b)
#define cmax(a,b)  ((a>b) ? (a) : (b))


#ifndef malloc16
#  ifdef __AVX2__
#    define malloc16(x) memalign(32,x+32)
#  else
#    define malloc16(x) memalign(16,x+16)
#  endif
#endif

int64_t dot_product64(int16_t *x,
                      int16_t *y,
                      uint32_t N, //must be a multiple of 8
                      uint8_t output_shift);

uint8_t dB_fixed64(uint64_t x);

unsigned char log2_approx(unsigned int x);



#define _Assert_Exit_                           \
    fprintf(stderr, "\nExiting execution\n");   \
    fflush(stdout);                             \
    fflush(stderr);                             \
    abort();


#define _Assert_(cOND, aCTION, fORMAT, aRGS...)             \
do {                                                        \
    if (!(cOND)) {                                          \
        fprintf(stderr, "\nAssertion ("#cOND") failed!\n"   \
                "In %s() %s:%d\n" fORMAT,                   \
                __FUNCTION__, __FILE__, __LINE__, ##aRGS);  \
        aCTION;                                             \
    }						\
} while(0)

#define AssertFatal(cOND, fORMAT, aRGS...)          _Assert_(cOND, _Assert_Exit_, fORMAT, ##aRGS)
