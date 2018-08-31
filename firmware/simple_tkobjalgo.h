#ifndef SIMPLE_TKOBJALGO_H
#define SIMPLE_TKOBJALGO_H

#include "data.h"

//-------------------------------------------------------
// FOR REFERENCE ALGORITHM
//-------------------------------------------------------
void    tkobjalgo_make_histogram_ref(TkObj track[NTRACK], histogram_t hist[NBINS]);
void    tkobjalgo_find_vtx_ref(histogram_t hist[NBINS], VtxObj outvtx[NVTX]);
void    tkobjalgo_ref(TkObj track[NTRACK], VtxObj outvtx[NVTX]);

//-------------------------------------------------------
// FOR HARDWARE ALGORITHM
//-------------------------------------------------------
#define N_TABLE_SIZE 1533 // size of LUT. Maximum number is 2045 for some reason (SIGSEGV otherwise)
template<class data_T, int N_TABLE>
void    init_inversion_table(data_T table_out[N_TABLE]);
template<class data_T, class res_T, int TABLE_SIZE>
void    inversion(data_T &data_den, res_T &res);
template<class data_T, class res_T>
void    inversion(data_T &data_den, res_T &res);
template<class array_t, int SIZE>
void    initialize_array(array_t arr[]);
template<class array_t, class array2_t, int SIZE_SRC, int SIZE_DST>
void    copy_to_p2_array(array_t input_array[SIZE_SRC], array_t output_array[SIZE_DST], array2_t output_array_index[SIZE_DST]);
template<class array_t, int SIZE>
void    copy_array(array_t input_array[SIZE], array_t output_array[SIZE]);
template<class type1_t, class type2_t>
void    comparator(type1_t bin1, type1_t bin2, type2_t binindex1, type2_t binindex2, type1_t &res, type2_t &resindex);
template<class value_t, class index_t, int SIZE>
void    parallel_find_max(value_t input_array[SIZE], index_t &b_max, value_t &sigma_max);
void    tkobjalgo(TkObj track[NTRACK], VtxObj outvtx[NVTX]);
void    tkobjalgo_add_track(TkObj track, histogram_t hist[NBINS]);
template<class input_t>
z0_t    tkobjalgo_bin_center(input_t iz);
template<class bin_t, class ret_t>
void    tkobjalgo_bin_plus_half_window(bin_t bin, ret_t &ret);
void    tkobjalgo_comput_sums(histogram_t hist[NBINS], histogram_t hist_window_sums[NSUMS]);
zbin_vt tkobjalgo_fetch_bin(z0_t z0);
void    tkobjalgo_find_vtx(histogram_t hist[NBINS], VtxObj outvtx[NVTX]);
void    tkobjalgo_make_histograms_per_stream(TkObj track[NTRACK], HistogramVec histos[NCHANN]);
void    tkobjalgo_merge_histograms(HistogramVec histos[NCHANN], histogram_t hist[NBINS]);
bool    tkobjalgo_track_quality_check(TkObj track);
z0_t    tkobjalgo_weighted_position(zbin_t iz, histogram_t binpt[WINDOWSIZE], slidingsum_t sigma);
void    mp7wrapped_pack_in(TkObj track[NTRACK], MP7DataWord data_array[NTRACK], hls::stream<MP7DataWordVecIn> &data_stream);
void    mp7wrapped_unpack_in(hls::stream<MP7DataWordVecIn> &data, hls::stream<TkObjVec> &track);
void    mp7wrapped_pack_out(VtxObj outvtx[NVTX], hls::stream<MP7DataWordVecOut> &data);
void    mp7wrapped_unpack_out(hls::stream<MP7DataWordVecOut> &data, MP7DataWord data_array[NVTX], VtxObj outvtx[NVTX]);
void    mp7wrapped_tkobjalgo(hls::stream<MP7DataWordVecIn> &input, hls::stream<MP7DataWordVecOut> &output);

//-------------------------------------------------------
// UNUSED
//-------------------------------------------------------
template<class STREAM_TYPE, unsigned int DEPTH_STREAM, unsigned int WIDTH_STREAM, class ARRAY_TYPE> 
inline void mp7wrapped_unpack_stream(hls::stream<STREAM_TYPE> &data_stream, ARRAY_TYPE data[DEPTH_STREAM*WIDTH_STREAM]) {
    STREAM_TYPE tmpvec;
    for(unsigned int i=0; i<DEPTH_STREAM; ++i) {
        tmpvec = data_stream.read();
        for(unsigned int j=0; i<WIDTH_STREAM; ++j) {
            data[i*WIDTH_STREAM+j] = tmpvec.data[j];
        }
    }
}
//Examples:
//mp7wrapped_unpack_stream<MP7DataWordVecIn,NTRACKS_PER_SECTOR,NCHANN,MP7DataWord>(data_in_stream,data_in);
//mp7wrapped_unpack_stream<MP7DataWordVecOut,NVTX,NVTX,MP7DataWord>(data_out_stream,data_out);

#endif
