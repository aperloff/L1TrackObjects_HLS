#include "firmware/data.h"
#include "firmware/simple_tkobjalgo.h"
#include "utils/DiscretePFInputs.h"
#include "utils/Firmware2DiscretePF.h"
#include <cmath>
#include <algorithm>

bool g_debug_ = 0;

void tkobjalgo_ref_set_debug(bool debug) { g_debug_ = debug; }

template <typename T> int sqr(const T & t) { return t*t; }

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

void tkobjalgo_ref(TkObj track[NTRACK], VtxObj outvtx[NVTX]) {

    if (g_debug_) {
#ifdef FASTPUPPI_NTUPLERPRODUCER_DISCRETEPFINPUTS_MORE
        for (int i = 0; i < NTRACK; ++i) { if (track[i].hwPt == 0) continue;
            l1tpf_int::PropagatedTrack tk; fw2dpf::convert(track[i], tk); 
            printf("FW  \t track %3d: pt %8d [ %7.2f ]  calo eta %+7d [ %+5.2f ]  calo phi %+7d [ %+5.2f ]  calo ptErr %6d [ %7.2f ] \n", 
                                i, tk.hwPt, tk.floatPt(), tk.hwEta, tk.floatEta(), tk.hwPhi, tk.floatPhi(), tk.hwCaloPtErr, tk.floatCaloPtErr());
        }
#endif
    }

    ////////////////////////////////////////////////////
    // Vertexing
    //ptsort_ref<PFNeutralObj,NCALO,NSELCALO>(outne_all, outne);
    printf("In tkobjalgo_ref function\n");
    for(int i=0; i<NVTX; i++) {
        outvtx[i].hwSumPt = 0;
        outvtx[i].hwZ0    = 0;
        outvtx[i].mult    = 0;
        outvtx[i].hwBin   = 0;
    }
}
