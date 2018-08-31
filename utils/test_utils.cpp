#include "test_utils.h"
#include <cstdio>

bool track_equals(const TkObj &out_ref, const TkObj &out, const char *what, int idx) {
    bool ret;
    if (out_ref.hwPt == 0) {
        ret = (out.hwPt == 0);
    } else {
        ret = (out_ref.hwPt == out.hwPt && out_ref.hwEta == out.hwEta && out_ref.hwPhi == out.hwPhi && out_ref.hwZ0  == out.hwZ0);
    }
    if  (!ret) {
        printf("Mismatch at %s[%d] ref vs test, hwPt % 7d % 7d   hwEta %+7d %+7d   hwPhi %+7d %+7d   hwZ0 %+7d %+7d\n", what, idx,
                int(out_ref.hwPt), int(out.hwPt), int(out_ref.hwEta), int(out.hwEta), int(out_ref.hwPhi), int(out.hwPhi), int(out_ref.hwZ0), int(out.hwZ0));
    }
    return ret;
}

bool vtx_equals(const VtxObj &out_ref, const VtxObj &out, const char *what, int idx) {
    bool ret;
    if (out_ref.hwSumPt == 0) {
        ret = (out.hwSumPt == 0);
    } else {
        ret = (out_ref.hwSumPt == out.hwSumPt && out_ref.hwZ0 == out.hwZ0 && out_ref.hwBin == out.hwBin && out_ref.mult == out.mult);
    }
    if  (!ret) {
        printf("Mismatch at %s[%d] ref vs test, hwSumPt % 7d % 7d   hwZ0 %+7d %+7d   mult %+7d %+7d   hwBin %2d %2d   \n", what, idx,
                int(out_ref.hwSumPt), int(out.hwSumPt),
                int(out_ref.hwZ0), int(out.hwZ0),
                int(out_ref.mult), int(out.mult),
                int(out_ref.hwBin), int(out.hwBin));
    }
    return ret;
}
