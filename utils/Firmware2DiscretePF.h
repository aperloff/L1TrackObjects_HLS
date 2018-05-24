#ifndef FASTPUPPI_NTUPLERPRODUCER_FIRMWARE2DISCRETEPF_H
#define FASTPUPPI_NTUPLERPRODUCER_FIRMWARE2DISCRETEPF_H

/// NOTE: this include is not standalone, since the path to DiscretePFInputs is different in CMSSW & Vivado_HLS

#include "../firmware/data.h"
#include <vector>
#include <cassert>

namespace fw2dpf {

    // convert inputs from discrete to firmware
    inline void convert(const VtxObj & src, const l1tpf_int::PropagatedTrack & track, std::vector<l1tpf_int::PFParticle> &out) {
        l1tpf_int::PFParticle pf;
        pf.hwPt = src.hwSumPt;
        pf.hwEta = 0;
        pf.hwPhi = 0;
        pf.hwVtxEta = 0;
        pf.hwVtxPhi = 0;
        pf.track = track; // FIXME: ok only as long as there is a 1-1 mapping
        pf.cluster.hwPt = 0;
        pf.hwId = 0;
        pf.hwStatus = 0;
        out.push_back(pf);
    }

    // convert inputs from discrete to firmware
    inline void convert(const TkObj & in, l1tpf_int::PropagatedTrack & out) {
        out.hwPt = in.hwPt;
        out.hwCaloPtErr = in.hwPtErr;
        out.hwEta = in.hwEta; // @calo
        out.hwPhi = in.hwPhi; // @calo
        out.hwZ0 = in.hwZ0;
    }


    template<unsigned int NMAX, typename In>
    void convert(const In in[NMAX], std::vector<l1tpf_int::PFParticle> &out) {
        for (unsigned int i = 0; i < NMAX; ++i) {
            if (in[i].hwSumPt > 0) convert(in[i], out);
        }
    } 
    template<unsigned int NMAX>
    void convert(const VtxObj in[NMAX], std::vector<l1tpf_int::PropagatedTrack> srctracks, std::vector<l1tpf_int::PFParticle> &out) {
        for (unsigned int i = 0; i < NMAX; ++i) {
            if (in[i].hwSumPt > 0) {
                assert(i < srctracks.size());
                convert(in[i], srctracks[i], out);
            }
        }
    }

} // namespace

#endif
