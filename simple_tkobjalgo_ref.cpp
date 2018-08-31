#include "firmware/data.h"
#include "firmware/simple_tkobjalgo.h"
#include "utils/DiscretePFInputs.h"
#include "utils/Firmware2DiscretePF.h"
#include <cmath>
#include <algorithm>

//-------------------------------------------------------
// Hold-over Algos
//-------------------------------------------------------
// ------------------------------------------------------
template<typename T, int NIn, int NOut>
void ptsort_ref(T in[NIn], T out[NOut]) {
    for (int iout = 0; iout < NOut; ++iout) {
        out[iout].hwPt = 0;
    }
    for (int it = 0; it < NIn; ++it) {
        for (int iout = 0; iout < NOut; ++iout) {
            if (in[it].hwPt >= out[iout].hwPt) {
                for (int i2 = NOut-1; i2 > iout; --i2) {
                    out[i2] = out[i2-1];
                }
                out[iout] = in[it];
                break;
            }
        }
    }
}

//-------------------------------------------------------
// Vertexing Algos
//-------------------------------------------------------
// ------------------------------------------------------
// -- Make Histogram FUNCTION
void tkobjalgo_make_histogram_ref(TkObj track[NTRACK], histogram_t hist[NBINS]) {
    for (unsigned int it = 0; it < NTRACK; ++it) {
        pt_t tkpt = (track[it].hwPt >> 1);
        if ((QUALITY && tkobjalgo_track_quality_check(track[it])) || (!QUALITY)) {
            //
            // Saturation or truncation
            //
            if (tkpt > MAXPT) {
                tkpt = (TRUNCSAT) ? MAXPT : 0;
            }

            //
            // Check bin validity of bin found for the current track
            //
            zbin_vt bin = tkobjalgo_fetch_bin(track[it].hwZ0);
            assert(bin.bin >= 0 && bin.bin < NBINS);

            //
            // If the bin is valid then sum the tracks
            //
            if (bin.valid) {
                int newsum = int(hist[bin.bin].pt) + tkpt;
                hist[bin.bin].pt = ptsum_t(newsum > MAXBIN ? MAXBIN : newsum);
                hist[bin.bin].multiplicity++;
                #if(DEBUG==3)
                if (tkpt>0 && (bin.bin==45))
                    std::cout << "REF: bin=" << bin.bin << " tack=" << it << " pt=" << tkpt << std::endl;
                if (bin.bin==37) {
                    std::cout<<"bhv_find_pv_ref::is="<<is<<"\tcurrent track pT=" << tkpt << "\tcurrent sum=" <<hist[bin.bin].pt << std::endl;
                }
                #endif
            }
        }
    }
}

// ------------------------------------------------------
// -- Find Vertex FUNCTION
void tkobjalgo_find_vtx_ref(histogram_t hist[NBINS], VtxObj outvtx[NVTX]) {
    //
    // Setup the intermediate and final storage objects
    //
    unsigned int b_max = 0, sigma_max = 0, mult_max = 0;
    histogram_t binpt[WINDOWSIZE], binpt_max[WINDOWSIZE];

    //
    // Loop through all bins, taking into account the fact that the last bin is
    //  nbins-window_width+1
    // Save the small amount of intermediate information if the sum pT (sigma) of
    //  the current bin is the highest so far
    //
    for (unsigned int b = 0; b < NBINS-WINDOWSIZE+1; ++b) {
        int sigma = 0;
        int mult = 0;
        for (unsigned int w = 0; w < WINDOWSIZE;  ++w) {
            binpt[w] = hist[b+w];
            sigma += binpt[w].pt;
            mult  += binpt[w].multiplicity;
        }
        #if (defined(__GXX_EXPERIMENTAL_CXX0X__) or defined(CMSSW)) && (DEBUG == 3)
            std::cout << "bin = " << b << std::endl;
            show<ptsum_t,WINDOWSIZE>(binpt);
        #endif
        if (sigma > sigma_max) {
            sigma_max = sigma;
            b_max = b;
            mult_max = mult;
            std::copy(std::begin(binpt), std::end(binpt), std::begin(binpt_max));
        }
    }
    //
    //Find the weighted position only for the highest sum pT window
    //
    //vivado_hls is smart enough to inline this function right here
    //That is why we can split off this simple line of code
    outvtx[0].hwZ0 = tkobjalgo_weighted_position(b_max,binpt_max,sigma_max);
    outvtx[0].hwSumPt = sigma_max;
    outvtx[0].mult = mult_max;
    tkobjalgo_bin_plus_half_window<unsigned int,zbin_t>(b_max,outvtx[0].hwBin);
}

//-------------------------------------------------------
// REFERENCE FUNCTION
//-------------------------------------------------------
void tkobjalgo_ref(TkObj track[NTRACK], VtxObj outvtx[NVTX]) {

    ////////////////////////////////////////////////////
    // Vertexing

    //Initialization
    for(int i=0; i<NVTX; i++) {
        clear(outvtx[i]);
    }
    histogram_t hist[NBINS];
    for (unsigned int b = 0; b < NBINS; ++b) {
        clear(hist[b]);
    }
    
    // Make the histogram and find vertex using the histogram
    tkobjalgo_make_histogram_ref(track, hist);
    tkobjalgo_find_vtx_ref(hist,outvtx);
}
