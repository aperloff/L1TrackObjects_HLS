#ifndef FASTPUPPI_NTUPLERPRODUCER_DISCRETEPF2FIRMWARE_H
#define FASTPUPPI_NTUPLERPRODUCER_DISCRETEPF2FIRMWARE_H

/// NOTE: this include is not standalone, since the path to DiscretePFInputs is different in CMSSW & Vivado_HLS

#include "../firmware/data.h"
#include <vector>

namespace dpf2fw {

    // convert inputs from discrete to firmware
    void convert(const l1tpf_int::PropagatedTrack & in, TkObj &out) {
        out.hwPt = in.hwPt;
        out.hwPtErr = in.hwCaloPtErr;
        out.hwEta = in.hwEta; // @calo
        out.hwPhi = in.hwPhi; // @calo
        out.hwZ0 = in.hwZ0;
        out.hwTightQuality = (in.hwStubs >= 6 && in.hwChi2 < 500);
        out.hwStubs = in.hwStubs;
        out.hwChi2 = in.hwChi2;
    }

    template<unsigned int NMAX, typename In, typename Out>
    void convert(const std::vector<In> & in, Out out[NMAX]) {
        for (unsigned int i = 0, n = std::min<unsigned int>(NMAX, in.size()); i < n; ++i) {
            convert(in[i], out[i]);
        }
        for (unsigned int i = in.size(); i < NMAX; ++i) {
            clear(out[i]);
        }
    }


} // namespace

#endif
