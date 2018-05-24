#include "simple_tkobjalgo.h"
#include "mp7pf_encoding.h"
#include <cmath>
#include <cassert>
#ifndef __SYNTHESIS__
#include <cstdio>
#endif

template<typename T, int NIn, int NOut>
void ptsort_hwopt(T in[NIn], T out[NOut]) {
    T tmp[NOut];
    #pragma HLS ARRAY_PARTITION variable=tmp complete

    for (int iout = 0; iout < NOut; ++iout) {
        #pragma HLS unroll
        tmp[iout].hwPt = 0;
    }

    for (int it = 0; it < NIn; ++it) {
        for (int iout = NOut-1; iout >= 0; --iout) {
            if (tmp[iout].hwPt <= in[it].hwPt) {
                if (iout == 0 || tmp[iout-1].hwPt > in[it].hwPt) {
                    tmp[iout] = in[it];
                } else {
                    tmp[iout] = tmp[iout-1];
                }
            }
        }

    }
    for (int iout = 0; iout < NOut; ++iout) {
        out[iout] = tmp[iout];
    }

}

//-------------------------------------------------------
// Tracking Algos
//-------------------------------------------------------

void tkobjalgo(TkObj track[NTRACK], VtxObj outvtx[NVTX]) {
    
    #pragma HLS ARRAY_PARTITION variable=track complete
    #pragma HLS ARRAY_PARTITION variable=outvtx complete
    #pragma HLS pipeline II=HLS_pipeline_II

    // ---------------------------------------------------------------
    // Vertexing
    //tk2calo_caloalgo(hadcalo_sub, sumtk, sumtkerr2, outne_all);
    //ptsort_hwopt<PFNeutralObj,NCALO,NSELCALO>(outne_all, outne);
    printf("In tkobjalgo function\n");
    for(int i=0; i<NVTX; i++) {
        outvtx[i].hwSumPt = 0;
        outvtx[i].hwZ0    = 0;
        outvtx[i].mult    = 0;
        outvtx[i].hwBin   = 0;
    }
}


void mp7wrapped_pack_in(TkObj track[NTRACK], MP7DataWord data[MP7_NCHANN]) {
    #pragma HLS ARRAY_PARTITION variable=data complete
    #pragma HLS ARRAY_PARTITION variable=track complete
    // pack inputs
    assert(2*NTRACK <= MP7_NCHANN);
    mp7_pack<NTRACK,0>(track, data);
}

void mp7wrapped_unpack_in(MP7DataWord data[MP7_NCHANN], TkObj track[NTRACK]) {
    #pragma HLS ARRAY_PARTITION variable=data complete
    #pragma HLS ARRAY_PARTITION variable=track complete
    // unpack inputs
    assert(2*NTRACK <= MP7_NCHANN);
    mp7_unpack<NTRACK,0>(data, track);
}

void mp7wrapped_pack_out( VtxObj outvtx[NVTX], MP7DataWord data[MP7_NCHANN]) {
    #pragma HLS ARRAY_PARTITION variable=data complete
    #pragma HLS ARRAY_PARTITION variable=outvtx complete
    // pack outputs
    assert(2*NVTX <= MP7_NCHANN);
    for (unsigned int i = 0; i < NVTX; ++i) {
        data[2*i+0] = ( outvtx[i].hwZ0, outvtx[i].hwSumPt );
        data[2*i+1] = ( outvtx[i].mult, outvtx[i].hwBin );
    }
}
void mp7wrapped_unpack_out( MP7DataWord data[MP7_NCHANN], VtxObj outvtx[NVTX]) {
    #pragma HLS ARRAY_PARTITION variable=data complete
    #pragma HLS ARRAY_PARTITION variable=outvtx complete
    // unpack outputs
    assert(2*NVTX <= MP7_NCHANN);
    for (unsigned int i = 0; i < NTRACK; ++i) {
        outvtx[i].hwSumPt = data[2*i+0](15, 0);
        outvtx[i].hwZ0    = data[2*i+0](25,16);
        outvtx[i].hwBin   = data[2*i+1](9, 0);
        outvtx[i].mult    = data[2*i+1](14,10);
    }
}

void mp7wrapped_tkobjalgo(MP7DataWord input[MP7_NCHANN], MP7DataWord output[MP7_NCHANN]) {
    
    #pragma HLS ARRAY_PARTITION variable=input complete
    #pragma HLS ARRAY_PARTITION variable=output complete
    #pragma HLS INTERFACE ap_none port=output

    #pragma HLS pipeline II=HLS_pipeline_II

    TkObj track[NTRACK];
    #pragma HLS ARRAY_PARTITION variable=track complete

    VtxObj outvtx[NVTX];
    #pragma HLS ARRAY_PARTITION variable=outvtx complete

    printf("In mp7wrapped_tkobjalgo function\n");
    mp7wrapped_unpack_in(input, track);
    tkobjalgo(track, outvtx);
    mp7wrapped_pack_out(outvtx, output);
}

