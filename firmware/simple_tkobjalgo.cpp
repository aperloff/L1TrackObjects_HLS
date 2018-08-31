#include "simple_tkobjalgo.h"
#include "mp7pf_encoding.h"
#include <cmath>
#include <cassert>
#ifndef __SYNTHESIS__
#include <cstdio>
#endif
#if (defined(__GXX_EXPERIMENTAL_CXX0X__) and defined(CMSSW))
    #include "utility.h"
#endif

//-------------------------------------------------------
// Hold-over Algos
//-------------------------------------------------------
// ------------------------------------------------------
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
// Vertexing Algos
//-------------------------------------------------------
// ------------------------------------------------------
// -- Inversion FUNCTION
template<class data_T, int N_TABLE>
void init_inversion_table(data_T table_out[N_TABLE]) {
    #pragma HLS inline
    // Inversion function:
    //   result = 1/x
    INIT_INVERSION_TABLE: for (int ii = 0; ii < N_TABLE; ii++) {
        // First, convert from table index to X-value (unsigned 8-bit, range 0 to +1533)
        float in_val = 1533.0*ii/float(N_TABLE);
        // Next, compute lookup table function
        table_out[ii] = (in_val>0) ? 1.0/in_val : 0.0;
        #if (DEBUG==2)
            std::cout << "table_out[" << ii << "] = " << table_out[ii] << std::endl;
        #endif
    }
    return;
}
template void init_inversion_table<slidingsum_t, N_TABLE_SIZE>(slidingsum_t[N_TABLE_SIZE]);

template<class data_T, class res_T, int TABLE_SIZE>
void inversion(data_T &data_den, res_T &res) {
    #pragma HLS inline
    // Initialize the lookup table
    res_T inversion_table[TABLE_SIZE];
    init_inversion_table<res_T, TABLE_SIZE>(inversion_table);

    // Index into the lookup table based on data
    int index;

    //#pragma HLS PIPELINE

    if (data_den < 0) data_den = 0;
    if (data_den > TABLE_SIZE-1) data_den = TABLE_SIZE-1;
    index = data_den;
    res = inversion_table[index];
    #if (DEBUG==2)
        std::cout << "res = " << res << std::endl;
    #endif
    return;
}
template void inversion<slidingsum_t, inverse_t, N_TABLE_SIZE>(slidingsum_t&, inverse_t&);

template<class data_T, class res_T>
void inversion(data_T &data_den, res_T &res) {
    #pragma HLS inline
    /* Get the inversion value from the LUT */
    if(data_den==0) {
        printf("WARNING::inversion::data_den==0\n");
        return;
    }
    inversion<data_T, res_T, N_TABLE_SIZE>(data_den, res);
    return;
}
template void inversion<slidingsum_t, inverse_t>(slidingsum_t&, inverse_t&);

// ------------------------------------------------------
// -- PARALLEL_FIND_MAX FUNCTION
//    A parallel search algorithm for finding the index
//     of an array with the maximum value.
//    Both the maximum value and the bin index are returned
//     by reference.
//    Also in this section are the associated helper functions
template<class array_t, int SIZE>
void initialize_array(array_t arr[]) {
    #pragma HLS inline
    INITIALIZE_ARRAY_INDICES: for (unsigned int b = 0; b < SIZE; ++b) {
        #pragma HLS UNROLL
        arr[b] = 0;
    }
}
template void initialize_array<histogram_t,MAXIMUM_SEARCH_SIZE>(histogram_t[]);
template void initialize_array<zbin_t,MAXIMUM_SEARCH_SIZE>(zbin_t[]);

template<class array_t, class array2_t, int SIZE_SRC, int SIZE_DST>
void copy_to_p2_array(array_t input_array[SIZE_SRC], array_t output_array[SIZE_DST], array2_t output_array_index[SIZE_DST]) {
    #pragma HLS inline
    COPYP2LOOP1: for(unsigned int i=0; i<SIZE_SRC; ++i) {
        #pragma HLS UNROLL
        output_array[i]       = input_array[i];
        output_array_index[i] = i;
    }
    COPYP2LOOP2: for(unsigned int i=SIZE_SRC; i<SIZE_DST; ++i) {
        #pragma HLS UNROLL
        output_array[i]       = 0;
        output_array_index[i] = MAXIMUM_SEARCH_SIZE;
    }
}
template void copy_to_p2_array<histogram_t,zbin_t,NSUMS,MAXIMUM_SEARCH_SIZE>(histogram_t[NSUMS],histogram_t[MAXIMUM_SEARCH_SIZE],zbin_t[MAXIMUM_SEARCH_SIZE]);

template<class array_t, int SIZE>
void copy_array(array_t input_array[SIZE], array_t output_array[SIZE]) {
    #pragma HLS inline
    COPYLOOP: for(unsigned int i=0; i<SIZE; ++i) {
        #pragma HLS UNROLL
        output_array[i] = input_array[i];
    }
}
template void copy_array<histogram_t,MAXIMUM_SEARCH_SIZE>(histogram_t[MAXIMUM_SEARCH_SIZE],histogram_t[MAXIMUM_SEARCH_SIZE]);

template<class type1_t, class type2_t>
void comparator(type1_t bin1, type1_t bin2, type2_t binindex1, type2_t binindex2, type1_t &res, type2_t &resindex) {
    #pragma HLS inline
    if (bin1 >= bin2) {
        res = bin1;
        resindex = binindex1;
    }
    else {
        res = bin2;
        resindex = binindex2;
    }
}
template void comparator<histogram_t,zbin_t>(histogram_t,histogram_t,zbin_t,zbin_t,histogram_t&,zbin_t&);

template<class value_t, class index_t, int SIZE>
void parallel_find_max(value_t input_array[SIZE], index_t &b_max, value_t &maximums) {
    #pragma HLS inline
    value_t values_array[MAXIMUM_SEARCH_SIZE];
    index_t index_array[MAXIMUM_SEARCH_SIZE];
    value_t larger_of_pair_array[MAXIMUM_SEARCH_SIZE];
    index_t larger_of_pair_index_array[MAXIMUM_SEARCH_SIZE];
    #pragma HLS array_partition variable=input_array complete dim=1
    #pragma HLS array_partition variable=values_array complete dim=1
    #pragma HLS array_partition variable=index_array complete dim=1
    #pragma HLS array_partition variable=larger_of_pair_array complete dim=1
    #pragma HLS array_partition variable=larger_of_pair_index_array complete dim=1


    copy_to_p2_array<value_t,index_t,SIZE,MAXIMUM_SEARCH_SIZE>(input_array,values_array,index_array);

    ITERATIONLOOP: for (unsigned int iteration=0; iteration<MAXIMUM_SEARCH_SIZE_ITERATIONS; ++iteration) {
        #pragma HLS UNROLL
        initialize_array<value_t,MAXIMUM_SEARCH_SIZE>(larger_of_pair_array);
        initialize_array<index_t,MAXIMUM_SEARCH_SIZE>(larger_of_pair_index_array);
        COMPARATORLOOP: for (int pair=0; pair<PWRTWO(MAXIMUM_SEARCH_SIZE_ITERATIONS-iteration); pair+=2) {
            #pragma HLS UNROLL
            comparator<value_t,index_t>(values_array[pair], values_array[pair+1], index_array[pair], index_array[pair+1], larger_of_pair_array[pair>>1],larger_of_pair_index_array[pair>>1]);
        }
        copy_array<value_t,MAXIMUM_SEARCH_SIZE>(larger_of_pair_array,values_array);
        copy_array<index_t,MAXIMUM_SEARCH_SIZE>(larger_of_pair_index_array,index_array);
    }

    b_max     = index_array[0];
    maximums  = values_array[0];
}
template void parallel_find_max<histogram_t,zbin_t,NSUMS>(histogram_t[NSUMS],zbin_t&,histogram_t&);

// ------------------------------------------------------
void tkobjalgo(hls::stream<TkObjVec> &track, VtxObj outvtx[NVTX]) {
    #pragma HLS inline

    //Initialization
    #pragma HLS ARRAY_PARTITION variable=outvtx complete
    TKOBJALGO_INITIALIZE_OUTPUT: for(unsigned int i=0; i<NVTX; ++i) {
        #pragma HLS pipeline II=HLS_pipeline_II
        clear(outvtx[i]);
    }
    HistogramVec histos[NCHANN];
    #pragma HLS ARRAY_PARTITION variable=histos complete
    TKOBJALGO_INITIALIZE_STREAM_HISTOGRAMS_STREAMS: for (unsigned int c = 0; c < NCHANN; ++c) {
        #pragma HLS pipeline II=HLS_pipeline_II
        TKOBJALGO_INITIALIZE_STREAM_HISTOGRAMS_BINS: for (unsigned int b = 0; b < NBINS; ++b) {
            clear(histos[c].data[b]);
        }
    }
    histogram_t hist[NBINS];
    #pragma HLS ARRAY_PARTITION variable=hist complete
    TKOBJALGO_INITIALIZE_HISTOGRAM: for(unsigned int b=0; b<NBINS; ++b) {
        #pragma HLS pipeline II=HLS_pipeline_II
        clear(hist[b]);
    }
    
    // Make the histogram and find vertex using the histogram
    TkObjVec tracks;
    TKOBJALGO_ITERATE_THROUGH_STREAM: for(unsigned int i=0; i<NTRACKS_PER_SECTOR; i++) {
        #pragma HLS pipeline II=HLS_pipeline_II
        tracks = track.read();
        tkobjalgo_make_histograms_per_stream(tracks.data,histos);
    }
    tkobjalgo_merge_histograms(histos,hist);
    tkobjalgo_find_vtx(hist,outvtx);
}

// ------------------------------------------------------
void tkobjalgo_add_track(TkObj track, histogram_t hist[NBINS]) {
    #pragma HLS inline
    #pragma HLS pipeline II=HLS_pipeline_II
    //#pragma HLS interface ap_memory port=hist
    //#pragma HLS resource  variable=hist core=ram_1p

    pt_t tkpt = (track.hwPt >> 1);
    //
    // Saturation or truncation
    //
    if (tkpt > MAXPT) tkpt = (TRUNCSAT) ? MAXPT : 0;

    //
    // Check bin validity of bin found for the current track
    //
    zbin_vt bin = tkobjalgo_fetch_bin(track.hwZ0);
    assert(bin.bin >= 0 && bin.bin < NBINS);

    //
    // If the bin is valid then sum the tracks
    //
    if (bin.valid) {
        ptsum_t newsum = hist[bin.bin].pt + tkpt;
        hist[bin.bin].pt = newsum > (MAXBIN+1) ? ptsum_t(MAXBIN) : newsum;
        hist[bin.bin].multiplicity++;
        #if(DEBUG==3)
            std::cout<<"tkobjalgo_add_track::bin=" << bin.bin << "\ttrack pT=" << track.hwPt << "\treduced pT=" << tkpt << "\tcurrent sum=" <<hist[bin.bin].pt << std::endl;
        #endif
    }
}

// ------------------------------------------------------
// -- BIN_CENTER FUNCTIONS
//    Input: Unsigned fixed [0,71]
//    Output: Signed integer bin center [-284,284]
template<class input_t>
z0_t tkobjalgo_bin_center(input_t iz) {
    #pragma HLS inline
    input_t z = iz - NHALFBINS;
    #if (DEBUG==1)
        std::cout<<"bin_center_ref::iz = " << iz << std::endl;
        std::cout<<"bin_center_ref::NHALFBINS = " << NHALFBINS << std::endl;
        std::cout<<"bin_center_ref::(z << SHIFT) = " << (z << SHIFT) << std::endl;
        std::cout<<"bin_center_ref::( 1 << (SHIFT-1) ) = " << ( 1 << (SHIFT-1) ) << std::endl;
        std::cout<<"bin_center_ref::(z << SHIFT)+( 1 << (SHIFT-1) ) = " <<(z << SHIFT) + ( 1 << (SHIFT-1) ) << std::endl;
        std::cout<<"bin_center_ref::z0_t((z << SHIFT)+( 1 << (SHIFT-1) )) = " <<z0_t((z << SHIFT) + ( 1 << (SHIFT-1) )) << std::endl;
    #endif
    return z0_t((z << SHIFT) + ( 1 << (SHIFT-1) ));
}
template z0_t tkobjalgo_bin_center<zbin_t>(zbin_t);
template z0_t tkobjalgo_bin_center<zsliding_t>(zsliding_t);

// ------------------------------------------------------
template<class bin_t, class ret_t>
void tkobjalgo_bin_plus_half_window(bin_t bin, ret_t &ret) {
    #pragma HLS inline
    ret=bin+(WINDOWSIZE >> 1);
}
template void tkobjalgo_bin_plus_half_window<unsigned int,zbin_t>(unsigned int, zbin_t&);

// ------------------------------------------------------
// -- COMPUTE SUMS FUNCTION
//    Computes the sum of the bins within a window and places those
//     sums in a second array.
//    The array of computed sums is returned by reference.
void tkobjalgo_compute_sums(histogram_t hist[NBINS], histogram_t hist_window_sums[NSUMS]) {
    #pragma HLS inline
    #pragma HLS pipeline II=HLS_pipeline_II
    // Loop through all bins, taking into account the fact that the last bin is
    //  nbins-window_width+1
    TKOBJALGO_COMPUT_SUMS_BINS: for (unsigned int b = 0; b < NSUMS; ++b) {
        TKOBJALGO_COMPUT_SUMS_WINDOW: for (unsigned int w = 0; w < WINDOWSIZE;  ++w) {
            hist_window_sums[b] += hist[b+w];
        }
    }
}

// ------------------------------------------------------
// -- Fetch Bin FUNCTION
zbin_vt tkobjalgo_fetch_bin(z0_t z0) {
    #pragma HLS inline
    zbin_vt ret;
    ret.bin   = (z0 >> SHIFT) + NHALFBINS;
    ret.valid = true;
    if (ret.bin < 0) { ret.bin = 0; ret.valid = false; }
    if (ret.bin > NBINS-1) { ret.bin = 0; ret.valid = false; }
    return ret;
}

// ------------------------------------------------------
void tkobjalgo_find_vtx(histogram_t hist[NBINS], VtxObj outvtx[NVTX]) {
    #pragma HLS inline
    #pragma HLS pipeline II=HLS_pipeline_II
    #pragma HLS array_partition variable=hist complete dim=1

    //
    // Setup the intermediate and final storage objects
    //
    zbin_t ibest = 0, b_max = 0;
    histogram_t maximums; clear(maximums);
    zsliding_t zvtx_sliding;
    histogram_t binpt_max[WINDOWSIZE];
    #pragma HLS array_partition variable=binpt_max complete dim=1
    histogram_t hist_window_sums[NSUMS];
    #pragma HLS array_partition variable=hist_window_sums complete dim=1
    INITIALIZELOOP: for (unsigned int b = 0; b < NBINS-WINDOWSIZE+1; ++b) clear(hist_window_sums[b]);

    #if (defined(__GXX_EXPERIMENTAL_CXX0X__) and defined(CMSSW))
        show<ptsum_t,NBINS>(hist,80,0,-1,std::cout,"","\e[92m");
    #endif
    tkobjalgo_compute_sums(hist,hist_window_sums);

    //#if (defined(__GXX_EXPERIMENTAL_CXX0X__) or defined(CMSSW))
    //    show<slidingsum_t,NBINS-WINDOWSIZE+1>(hist_window_sums);
    //#endif

    // Search through all of the bins to find the one with the highest sum pT
    //  within the window
    parallel_find_max<histogram_t,zbin_t,NSUMS>(hist_window_sums,b_max,maximums);

    //
    // Save the pT information for just the highest pT window
    //
    WINDOWSELECT: for (unsigned int w = 0; w < WINDOWSIZE;  ++w) {
        binpt_max[w] = hist[b_max+w];
    }

    outvtx[0].hwSumPt = maximums.pt;
    zvtx_sliding = tkobjalgo_weighted_position(b_max,binpt_max,maximums.pt);
    outvtx[0].hwZ0 = zvtx_sliding;
    //vivado_hls is smart enough to inline this function right here
    //That is why we can split off this simple line of code
    tkobjalgo_bin_plus_half_window<zbin_t,zbin_t>(b_max,ibest);
    outvtx[0].hwBin = ibest;
    outvtx[0].mult  = maximums.multiplicity;

    return;
}

// ------------------------------------------------------
// -- Make Histogram FUNCTION
void tkobjalgo_make_histograms_per_stream(TkObj track[NCHANN], HistogramVec histos[NCHANN]) {
    #pragma HLS inline
    #pragma HLS pipeline II=HLS_pipeline_II
    #pragma HLS ARRAY_PARTITION variable=histos complete

    TKOBJALGO_MAKE_HISTOGRAMS_PER_STREAM: for (unsigned int it = 0; it < NCHANN; ++it) {
        if ((QUALITY && tkobjalgo_track_quality_check(track[it])) || (!QUALITY)) {
            tkobjalgo_add_track(track[it], histos[it].data);
        }
    }
}

// ------------------------------------------------------
void tkobjalgo_merge_histograms(HistogramVec histos[NCHANN], histogram_t hist[NBINS]) {
    #pragma HLS inline
    #pragma HLS pipeline II=HLS_pipeline_II
    #pragma HLS ARRAY_PARTITION variable=histos complete
    #pragma HLS ARRAY_PARTITION variable=hist complete

    TKOBJALGO_MERGE_HISTOGRAMS_BINS: for (unsigned int b = 0; b < NBINS; ++b) {
        histogram_t newsum;
        TKOBJALGO_MERGE_HISTOGRAMS_STREAMS: for (unsigned int istream = 0; istream < NCHANN; ++istream) {
            newsum = (hist[b] + histos[istream].data[b]);
            hist[b] = (newsum.pt > (MAXBIN+1)) ? histogram_t(ptsum_t(MAXBIN),newsum.multiplicity) : newsum;
        }
    }
}

// ------------------------------------------------------
bool tkobjalgo_track_quality_check(TkObj track) {
    // track quality cuts
    //if (fabs(z) > ZMAX ) continue; //ZMAX=25.
    if (track.hwChi2 > CHI2MAX)    return false;
    if (track.hwPt < PTMINTRA)     return false;
    if (track.hwStubs < NSTUBSMIN) return false;
    //if (nPS < nStubsPSmin) return false; //nStubsPSmin=3

    // quality cuts from Louise S, based on the pt-stub compatibility (June 20, 2014)
    //int trk_nstub  = (int) trackIter ->getStubRefs().size();
    //float chi2dof = chi2 / ((trk_nstub<<1)-4);
    //if(doPtComp)
    //float trk_consistency = trackIter ->getStubPtConsistency();
    //if (trk_nstub == 4) {
    //   if (fabs(eta)<2.2 && trk_consistency>10) return false;
    //   else if (fabs(eta)>2.2 && chi2dof>5.0) return false;
    //}
    //if(doTightChi2)
    //if (pt>10.0 && chi2dof>5.0) return false;

    return true;
}

// ------------------------------------------------------
z0_t tkobjalgo_weighted_position(zbin_t iz, histogram_t binpt[WINDOWSIZE], slidingsum_t sigma) {
    #pragma HLS array_partition variable=binpt complete dim=1
    zsliding_t zvtx_sliding = 0;
    slidingsum_t zvtx_sliding_sum = 0;
    #if (DEBUG==1)
        std::cout << "zvtx_sliding (start) = " << zvtx_sliding_sum << std::endl;
        std::cout << "weighted_position_ref::iz = " << iz << std::endl;
        std::cout << "weighted_position_ref::zvtx_sliding_sum (weighted sum) = " << std::flush;
    #endif
    WINDOWLOOP: for(ap_uint<BITS_TO_REPRESENT(NBINS)> w = 0; w < WINDOWSIZE; ++w) {
        #if (DEBUG==1)
            std::cout << "(" << binpt[w].pt << " * " << w << ")" << std::flush;
            if (w<WINDOWSIZE-1) std::cout<< " + " << std::flush;
        #endif
        //Might be a problem in future due to timing
        //Check if the RTL doesn't meet the timing requirement
        zvtx_sliding_sum += (binpt[w].pt*w);
    }
    #if (DEBUG==1)
        std::cout << " = " << zvtx_sliding_sum << std::endl;
        std::cout << "weighted_position_ref::sigma = " << sigma << std::endl;
    #endif
    if(sigma!=0) {
        inverse_t inv;
        //vivado_hls is smart enough to inline this function right here
        inversion<slidingsum_t, inverse_t>(sigma, inv);
        zvtx_sliding = zvtx_sliding_sum*inv;
    }
    else {
        zvtx_sliding = (WINDOWSIZE >> 1) + ((WINDOWSIZE%2!=0) ? 0.5 : 0.0);//bin_center_ref(iz+(BHV_WINDOWSIZE >> 1));
    }
    #if (DEBUG==1)
        std::cout << "weighted_position_ref::zvtx_sliding (final) = " << zvtx_sliding << std::endl;
    #endif
    zvtx_sliding += iz;
    //vivado_hls is smart enough to inline this function right here
    zvtx_sliding = tkobjalgo_bin_center(zvtx_sliding);
    return zvtx_sliding;
}

// ------------------------------------------------------
void mp7wrapped_pack_in(TkObj track[NTRACK], MP7DataWord data_array[NTRACK], hls::stream<MP7DataWordVecIn> &data_stream) {
    #pragma HLS ARRAY_PARTITION variable=data_array complete
    #pragma HLS ARRAY_PARTITION variable=track complete
    #pragma HLS pipeline II=HLS_pipeline_II
    // pack inputs
    assert(WORDS_PER_OBJ*NTRACK/NTRACKS_PER_SECTOR <= NCHANN);
    mp7_pack<NSECTORS,0>(track, data_array, data_stream);
}

// ------------------------------------------------------
void mp7wrapped_unpack_in(hls::stream<MP7DataWordVecIn> &data, hls::stream<TkObjVec> &track) {
    //#pragma HLS ARRAY_PARTITION variable=data complete
    //#pragma HLS ARRAY_PARTITION variable=track complete
    //#pragma HLS ARRAY_PARTITION variable=data block factor=12
    //#pragma HLS ARRAY_PARTITION variable=track block factor=12
    #pragma HLS inline

    // unpack inputs
    assert(WORDS_PER_OBJ*NSECTORS <= NCHANN);
    mp7_unpack<NSECTORS,0>(data, track);
}

// ------------------------------------------------------
void mp7wrapped_pack_out( VtxObj outvtx[NVTX], hls::stream<MP7DataWordVecOut> &data) {
    //#pragma HLS ARRAY_PARTITION variable=data complete
    #pragma HLS ARRAY_PARTITION variable=outvtx complete
    #pragma HLS pipeline II=HLS_pipeline_II
    // pack outputs
    assert(WORDS_PER_OBJ*NVTX <= NCHANN);
    MP7DataWordVecOut datavec;
    MP7WRAPPED_PACK_OUT_VERTICES: for (unsigned int i = 0; i < NVTX; ++i) {
        #ifdef TESTVCU118
            datavec.data[WORDS_PER_OBJ*i]     = ( outvtx[i].mult, outvtx[i].hwBin, outvtx[i].hwZ0, outvtx[i].hwSumPt );
        #else
            datavec.data[WORDS_PER_OBJ*i+0] = ( outvtx[i].hwZ0, outvtx[i].hwSumPt );
            datavec.data[WORDS_PER_OBJ*i+1] = ( outvtx[i].mult, outvtx[i].hwBin );
        #endif
        data.write(datavec);
    }
}

// ------------------------------------------------------
void mp7wrapped_unpack_out( hls::stream<MP7DataWordVecOut> &data, MP7DataWord data_array[NVTX], VtxObj outvtx[NVTX]) {
    #pragma HLS ARRAY_PARTITION variable=data_array complete
    #pragma HLS ARRAY_PARTITION variable=outvtx complete
    #pragma HLS pipeline II=HLS_pipeline_II
    // unpack outputs
    assert(WORDS_PER_OBJ*NVTX <= NCHANN);
    MP7DataWordVecOut datavec = data.read();
    MP7WRAPPED_UNPACK_OUT_VERTICES: for (unsigned int i = 0; i < NVTX; ++i) {
        data_array[WORDS_PER_OBJ*i] = datavec.data[WORDS_PER_OBJ*i];
        #ifdef TESTVCU118
            outvtx[i].hwSumPt = datavec.data[WORDS_PER_OBJ*i](15, 0);
            outvtx[i].hwZ0    = datavec.data[WORDS_PER_OBJ*i](27,16);
            outvtx[i].hwBin   = datavec.data[WORDS_PER_OBJ*i](37,28);
            outvtx[i].mult    = datavec.data[WORDS_PER_OBJ*i](47,38);
        #else
            outvtx[i].hwSumPt = datavec.data[WORDS_PER_OBJ*i+0](15, 0);
            outvtx[i].hwZ0    = datavec.data[WORDS_PER_OBJ*i+0](27,16);
            outvtx[i].hwBin   = datavec.data[WORDS_PER_OBJ*i+1](9, 0);
            outvtx[i].mult    = datavec.data[WORDS_PER_OBJ*i+1](19,10);
        #endif
    }
}

//-------------------------------------------------------
// TOP FUNCTION
//-------------------------------------------------------
void mp7wrapped_tkobjalgo(hls::stream<MP7DataWordVecIn> &input, hls::stream<MP7DataWordVecOut> &output) {
    #pragma HLS dataflow

    static hls::stream<TkObjVec> track;
    VtxObj outvtx[NVTX];
    #pragma HLS ARRAY_PARTITION variable=outvtx complete

    mp7wrapped_unpack_in(input, track);
    tkobjalgo(track, outvtx);
    mp7wrapped_pack_out(outvtx, output);
}

