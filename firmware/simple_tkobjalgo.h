#ifndef SIMPLE_TKOBJALGO_H
#define SIMPLE_TKOBJALGO_H

#include "data.h"

void tkobjalgo_ref(TkObj track[NTRACK], VtxObj outvtx[NVTX]);
void tkobjalgo_ref_set_debug(bool debug);
void tkobjalgo(TkObj track[NTRACK], VtxObj outvtx[NVTX]);
void mp7wrapped_pack_in(TkObj track[NTRACK], MP7DataWord data[MP7_NCHANN]);
void mp7wrapped_unpack_in(MP7DataWord data[MP7_NCHANN], TkObj track[NTRACK]);
void mp7wrapped_pack_out(VtxObj outvtx[NVTX], MP7DataWord data[MP7_NCHANN]);
void mp7wrapped_unpack_out(MP7DataWord data[MP7_NCHANN], VtxObj outvtx[NVTX]);
void mp7wrapped_tkobjalgo(MP7DataWord input[MP7_NCHANN], MP7DataWord output[MP7_NCHANN]);

#endif
