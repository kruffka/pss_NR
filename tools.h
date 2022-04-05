#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define max(a,b) cmax(a,b)
#define cmax(a,b)  ((a>b) ? (a) : (b))

#define SHRT_MAX 32767

typedef enum DFT_size_idx {
	DFT_12,    DFT_24,    DFT_36,   DFT_48,     DFT_60,   DFT_72,   DFT_96,
	DFT_108,   DFT_120,   DFT_128,  DFT_144,    DFT_180,  DFT_192,  DFT_216,   DFT_240,
	DFT_256,   DFT_288,   DFT_300,  DFT_324,    DFT_360,  DFT_384,  DFT_432,   DFT_480,
	DFT_512,   DFT_540,   DFT_576,  DFT_600,    DFT_648,  DFT_720,  DFT_768,   DFT_864,
	DFT_900,   DFT_960,   DFT_972,  DFT_1024,   DFT_1080, DFT_1152, DFT_1200,  DFT_1296,
	DFT_1440,  DFT_1500,  DFT_1536, DFT_1620,   DFT_1728, DFT_1800, DFT_1920,  DFT_1944,
	DFT_2048,  DFT_2160,  DFT_2304, DFT_2400,   DFT_2592, DFT_2700, DFT_2880,  DFT_2916,
	DFT_3000,  DFT_3072,  DFT_3240, DFT_4096,   DFT_6144, DFT_8192, DFT_9216,  DFT_12288,
	DFT_18432, DFT_24576, DFT_36864, DFT_49152, DFT_73728, DFT_98304,
	DFT_SIZE_IDXTABLESIZE
} dft_size_idx_t;

typedef enum idft_size_idx {
	IDFT_128,   IDFT_256,  IDFT_512,   IDFT_1024,  IDFT_1536,  IDFT_2048,  IDFT_3072,  IDFT_4096,
	IDFT_6144,  IDFT_8192, IDFT_9216,  IDFT_12288, IDFT_18432, IDFT_24576, IDFT_36864, IDFT_49152, 
	IDFT_73728, IDFT_98304, 
	IDFT_SIZE_IDXTABLESIZE
} idft_size_idx_t;

#ifndef malloc16
#  ifdef __AVX2__
#    define malloc16(x) memalign(32,x+32)
#  else
#    define malloc16(x) aligned_alloc(16,x+16) // memalign был здесь
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


/* The first occurrence of EXPR is not evaluated due to the sizeof,
   but will trigger any pedantic warnings masked by the __extension__
   for the second occurrence.  The ternary operator is required to
   support function pointers and bit fields in this context, and to
   suppress the evaluation of variable length arrays.  */
#  define assert(expr)							\
  ((void) sizeof ((expr) ? 1 : 0), __extension__ ({			\
      if (expr)								\
        ; /* empty */							\
      else								\
        __assert_fail (#expr, __FILE__, __LINE__, __ASSERT_FUNCTION);	\
    }))
