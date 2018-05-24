#include "../firmware/data.h"

bool track_equals(const TkObj &out_ref, const TkObj &out, const char *what, int idx) ;
bool vtx_equals(const VtxObj &out_ref, const VtxObj &out, const char *what, int idx) ;

template<typename T> 
unsigned int count_nonzero(T objs[], unsigned int NMAX) {
    unsigned int ret = 0;
    for (unsigned int i = 0; i < NMAX; ++i) ret += (objs[i].hwPt > 0);
    return ret;
}
