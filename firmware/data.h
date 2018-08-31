#ifndef SIMPLE_TKOBJ_DATA_H
#define SIMPLE_TKOBJ_DATA_H

#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_stream.h"

//////////////
// -- TYPEDEFS
typedef ap_int<16>  pt_t;
typedef ap_int<12>  eta_t;
typedef ap_int<17>  phi_t;
typedef ap_int<10>  vtx_t;
typedef ap_uint<3>  particleid_t;
typedef ap_int<12>  z0_t;  // 40cm / 0.1
typedef ap_int<10>  d0_t;
typedef ap_uint<10> tkchi2_t;
typedef ap_uint<5>  tkstubcon_t;
typedef ap_uint<15> tkstubmask_t;
typedef ap_uint<2>  tkspare_t;
typedef ap_uint<11> ptsum_t;
typedef ap_uint<10> zbin_t;
typedef ap_uint<11> slidingsum_t;
typedef ap_ufixed<15,1,AP_RND,AP_SAT> inverse_t;
typedef ap_fixed<20,10,AP_RND,AP_SAT> zsliding_t;

/////////////
// -- STRUCTS
struct TkObj {
   #ifdef TESTVCU118
      pt_t         hwPt;
      bool         hwCharge;
      eta_t        hwEta;
      phi_t        hwPhi; // relative to the region center, at calo
      z0_t         hwZ0;
      d0_t         hwD0;
      tkchi2_t     hwChi2;
      tkstubcon_t  hwStubPtConsistency;
      tkstubmask_t hwStubs;
      tkspare_t    hwSpare;
   #else
      pt_t         hwPt;
      pt_t         hwPtErr;
      eta_t        hwEta;
      phi_t        hwPhi; // relative to the region center, at calo
      z0_t         hwZ0;
      bool         hwTightQuality;
   #endif
};
inline void clear(TkObj & c) {
   #ifdef TESTVCU118
      c.hwPt = 0;
      c.hwCharge = 0;
      c.hwEta = 0;
      c.hwPhi = 0;
      c.hwZ0 = 0;
      c.hwD0 = 0;
      c.hwChi2 = 0;
      c.hwStubPtConsistency = 0;
      c.hwStubs = 0;
      c.hwSpare = 0;
   #else
      c.hwPt = 0;
      c.hwPtErr = 0;
      c.hwEta = 0;
      c.hwPhi = 0;
      c.hwZ0 = 0;
      c.hwTightQuality = 0;
   #endif
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

struct histogram_t {
   ptsum_t pt;
   vtx_t   multiplicity;

   histogram_t() : pt(0), multiplicity(0) {}
   histogram_t(ptsum_t pt_, vtx_t multiplicity_) : pt(pt_), multiplicity(multiplicity_) {}
   histogram_t& operator=(const histogram_t& rhs) {
      pt           = rhs.pt;
      multiplicity = rhs.multiplicity;
      return *this;
   }
   histogram_t& operator=(const int& rhs) {
      pt           = ptsum_t(rhs);
      multiplicity = vtx_t(rhs);
      return *this;
   }
   histogram_t& operator+=(const histogram_t& rhs) {
      pt           += rhs.pt;
      multiplicity += rhs.multiplicity;
      return *this;
   }
   histogram_t operator+(const histogram_t& rhs) const {
      return histogram_t(pt+rhs.pt,multiplicity+rhs.multiplicity);
   }
   bool operator==(const histogram_t& rhs) const {
      return (pt == rhs.pt && multiplicity == rhs.multiplicity);
   }
   bool operator>=(const histogram_t& rhs) const {
      return (pt >= rhs.pt);
   }
};
inline void clear(histogram_t & c) {
   c.pt = 0; c.multiplicity = 0;
}

struct zbin_vt {
    zbin_t bin;
    bool   valid;
};

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
   #define NSECTORS N_IN_SECTORS//2*N_IN_SECTORS
   #define NTRACKS_PER_SECTOR 6//15
#elif TESTCTP7  // reduced input size to fit in a board
   #define N_IN_SECTORS 12
   #define NSECTORS N_IN_SECTORS//2*N_IN_SECTORS
   #define NTRACKS_PER_SECTOR 6//7
#elif TESTVCU118
   #define N_IN_SECTORS 12//27
   #define NSECTORS N_IN_SECTORS
   #define NTRACKS_PER_SECTOR 24
#else
   #define N_IN_SECTORS 12
   #define NSECTORS 2*N_IN_SECTORS
   #define NTRACKS_PER_SECTOR 18
#endif
#define NTRACK NSECTORS*NTRACKS_PER_SECTOR
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

#define MP7_NCHANN 144
#define CTP7_NCHANN_IN 67
#define CTP7_NCHANN_OUT 48
#define CTP7_MEMORY_DEPTH 1023
#define VCU118_NCHANN NSECTORS //NTRACK
#define VCU118_MEMORY_DEPTH 1023
#ifdef TESTMP7
   #define WORDS_PER_OBJ 2
   #define MP7DataWordWidth 32
   #define NCHANN MP7_NCHANN
#elif TESTCTP7
   #define WORDS_PER_OBJ 2
   #define MP7DataWordWidth 32
   #define NCHANN MP7_NCHANN
#elif TESTVCU118
   #define WORDS_PER_OBJ 1
   #define MP7DataWordWidth 100
   #define NCHANN VCU118_NCHANN
#else
   #define WORDS_PER_OBJ 2
   #define MP7DataWordWidth 32
   #define NCHANN MP7_NCHANN
#endif
typedef ap_uint<MP7DataWordWidth> MP7DataWord;
typedef struct { MP7DataWord data[NCHANN]; } MP7DataWordVecIn;
typedef struct { TkObj       data[NCHANN]; } TkObjVec;
typedef struct { histogram_t data[NBINS]; }  HistogramVec;
typedef struct { MP7DataWord data[NVTX]; }   MP7DataWordVecOut;

#endif
