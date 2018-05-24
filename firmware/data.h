#ifndef SIMPLE_TKOBJ_DATA_H
#define SIMPLE_TKOBJ_DATA_H

#include "ap_int.h"
#include "ap_fixed.h"

//////////////
// -- TYPEDEFS
typedef ap_int<16> pt_t;
typedef ap_int<10>  etaphi_t;
typedef ap_int<5>  vtx_t;
typedef ap_uint<3>  particleid_t;
typedef ap_int<10> z0_t;  // 40cm / 0.1
typedef ap_uint<5> tkstub_t;
typedef ap_uint<16> tkpar_t;
typedef ap_uint<11> ptsum_t;
typedef ap_uint<10> zbin_t;
struct zbin_vt {
    zbin_t bin;
    bool   valid;
};
typedef ap_uint<11> slidingsum_t;
typedef ap_ufixed<15,1,AP_RND,AP_SAT> inverse_t;
typedef ap_fixed<20,10,AP_RND,AP_SAT> zsliding_t;
		
///////////////////////////
// -- PRECOMPILER FUNCTIONS
#define IS_REPRESENTIBLE_IN_D_BITS(D, N)                \
   (((unsigned long) N >= (1UL << (D - 1)) && (unsigned long) N < (1UL << D)) ? D : -1)
#define BITS_TO_REPRESENT(N)                             \
   (N == 0 ? 1 : (31                                     \
                  + IS_REPRESENTIBLE_IN_D_BITS( 1, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS( 2, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS( 3, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS( 4, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS( 5, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS( 6, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS( 7, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS( 8, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS( 9, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(10, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(11, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(12, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(13, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(14, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(15, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(16, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(17, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(18, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(19, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(20, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(21, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(22, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(23, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(24, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(25, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(26, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(27, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(28, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(29, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(30, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(31, N)    \
                  + IS_REPRESENTIBLE_IN_D_BITS(32, N)    \
                  )                                      \
      )
#define PWRTWO(x) (1 << (x))

///////////////////////
// -- DEFINE STATEMENTS
#ifdef TESTMP7  // reduced input size to fit in a board
   #define N_IN_SECTORS 12
   #define NSECTORS 2*N_IN_SECTORS
   #define NTRACKS_PER_SECTOR 15
   #define NTRACK NSECTORS*NTRACKS_PER_SECTOR
#elif TESTCTP7  // reduced input size to fit in a board
   #define N_IN_SECTORS 12
   #define NSECTORS 2*N_IN_SECTORS
   #define NTRACKS_PER_SECTOR 7
   #define NTRACK NSECTORS*NTRACKS_PER_SECTOR
#else
   #define N_IN_SECTORS 12
   #define NSECTORS 2*N_IN_SECTORS
   #define NTRACKS_PER_SECTOR 18
   #define NTRACK NSECTORS*NTRACKS_PER_SECTOR
#endif
#define NVTX 1
#define NBINS 72
#define MAXPT 100
#define MAXBIN 511
#define CHI2MAX 100
#define PTMINTRA 2
#define NSTUBSMIN 4
//0 : truncation. Tracks with PT above PTMAX are ignored
//1 : saturation . Tracks with PT above PTMAX are set to PT=PTMAX.
#define TRUNCSAT 1
#define SHIFT 3
#define NHALFBINS (NBINS/2)
#define WINDOWSIZE 3
#define NSUMS NBINS-WINDOWSIZE+1
#define BITS_TO_STORE_NSUMS BITS_TO_REPRESENT(NSUMS)+1
#define MAXIMUM_SEARCH_SIZE PWRTWO(BITS_TO_STORE_NSUMS-1)
#define MAXIMUM_SEARCH_SIZE_ITERATIONS BITS_TO_REPRESENT(MAXIMUM_SEARCH_SIZE)-1

struct TkObj {
	pt_t hwPt, hwPtErr;
	etaphi_t hwEta, hwPhi; // relative to the region center, at calo
	z0_t hwZ0;
	tkstub_t hwStubs;
	tkpar_t hwChi2;
	bool hwTightQuality;
};
inline void clear(TkObj & c) {
    c.hwPt = 0; c.hwPtErr = 0; c.hwEta = 0; c.hwPhi = 0; c.hwZ0 = 0; c.hwStubs = 0; c.hwChi2 = 0; c.hwTightQuality = 0;
}

struct VtxObj {
	pt_t   hwSumPt;
	z0_t   hwZ0;
	vtx_t  mult;
	zbin_t hwBin;
};
inline void clear(VtxObj & c) {
    c.hwSumPt = 0; c.hwZ0 = 0; c.mult = 0; c.hwBin = 0;
}

#define MP7_NCHANN 144
#define CTP7_NCHANN_IN 67
#define CTP7_NCHANN_OUT 48
typedef ap_uint<32> MP7DataWord;

#endif
