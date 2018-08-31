#include <cstdio>
#include "firmware/simple_tkobjalgo.h"
#include "utils/random_inputs.h"
#include "utils/DiscretePFInputs_IO.h"
#include "utils/pattern_serializer.h"
#include "utils/test_utils.h"

#define NTEST 250


int main() {

    // input format: could be random or coming from simulation
    //RandomPFInputs inputs(37); // 37 is a good random number
    //DiscretePFInputs inputs("regions_TTbar_PU140.dump");
    DiscretePFInputs inputs("barrel_alltracks_sectors_1x12_TTbar_PU140.dump");
    
    // input TP objects
    TkObj track[NTRACK]; z0_t pv_gen; z0_t pv_cmssw;

    // output track objects
    VtxObj outvtx[NVTX], outvtx_ref[NVTX];
#if defined(TESTMP7)
    MP7PatternSerializer serInPatterns( "mp7_input_patterns.txt", HLS_pipeline_II,HLS_pipeline_II-1); // mux each event into HLS_pipeline_II frames
    MP7PatternSerializer serOutPatterns("mp7_output_patterns.txt",HLS_pipeline_II,HLS_pipeline_II-1); // assume only one tkobj core running per chip,
    MP7PatternSerializer serInPatterns2( "mp7_input_patterns_magic.txt", HLS_pipeline_II,-HLS_pipeline_II+1); // mux each event into HLS_pipeline_II frames
    MP7PatternSerializer serOutPatterns2("mp7_output_patterns_magic.txt",HLS_pipeline_II,-HLS_pipeline_II+1); // assume only one tkobj core running per chip,
    MP7PatternSerializer serInPatterns3( "mp7_input_patterns_nomux.txt");
    MP7PatternSerializer serOutPatterns3("mp7_output_patterns_nomux.txt");
#endif
#if defined(TESTCTP7)
    CTP7PatternSerializer serInPatterns4( "ctp7_input_patterns_nomux.txt",CTP7_NCHANN_IN, true);  // 
    CTP7PatternSerializer serOutPatterns4("ctp7_output_patterns_nomux.txt",CTP7_NCHANN_OUT, false); // fill the rest of the lines with empty events for now
#endif
#if defined(TESTVCU118)
    VCU118PatternSerializer serInPatterns5( "vcu118_input_patterns.coe", HLS_pipeline_II,HLS_pipeline_II-1,N_IN_SECTORS); // mux each event into HLS_pipeline_II frames
    VCU118PatternSerializer serOutPatterns5("vcu118_output_patterns.coe",HLS_pipeline_II,HLS_pipeline_II-1,N_IN_SECTORS); // assume only one tkobj core running per chip,
    VCU118PatternSerializer serInPatterns6( "vcu118_input_patterns_magic.coe", HLS_pipeline_II,-HLS_pipeline_II+1,N_IN_SECTORS); // mux each event into HLS_pipeline_II frames
    VCU118PatternSerializer serOutPatterns6("vcu118_output_patterns_magic.coe",HLS_pipeline_II,-HLS_pipeline_II+1,N_IN_SECTORS); // assume only one tkobj core running per chip,
    VCU118PatternSerializer serInPatterns7( "vcu118_input_patterns_nomux.coe",1,0,N_IN_SECTORS);
    VCU118PatternSerializer serOutPatterns7("vcu118_output_patterns_nomux.coe",1,0,N_IN_SECTORS);
#endif
    HumanReadablePatternSerializer serHR("human_readable_patterns.txt");
    HumanReadablePatternSerializer debugHR("-"); // this will print on stdout, we'll use it for errors

    // -----------------------------------------
    // variables necessary for tracking the efficiency over multiple tests
    int errors_tot = 0, nagree = 0, ntot_tot = 0, nvtx_tot = 0, ncmssw_vtx = 0;
    double resol = 0, cmssw_resol = 0;

    // run multiple tests
    for (int test = 1; test <= NTEST; ++test) {
        // initialize TP objects
        for (int i = 0; i < NTRACK; ++i) { clear(track[i]); }

        // get the inputs from the input object
        //if (!inputs.nextRegion(track, hwZPV)) break;
        if (!inputs.nextEvent(track, pv_gen, pv_cmssw, true, VERBOSE)) {std::cout<<"BREAK!"<<std::endl;break;}
        
#if defined(TESTMP7) // Full L1TkObj, with MP7 wrapping 
        MP7DataWord data_in[MP7_NCHANN], data_out[MP7_NCHANN];

        // initialize
        for (int i = 0; i < MP7_NCHANN; ++i) {
            data_in[i] = 0;
            data_out[i] = 0;
        }
        mp7wrapped_pack_in(track, data_in);
        TOP_FUNC(data_in, data_out);
        mp7wrapped_unpack_out(data_out, outvtx);
        REF_FUNC(track, outvtx_ref);

        // write out patterns for MP7 board hardware or simulator test
        serInPatterns(data_in); serOutPatterns(data_out);
        serInPatterns2(data_in); serOutPatterns2(data_out);
        serInPatterns3(data_in); serOutPatterns3(data_out);

#elif defined(TESTCTP7) // Full L1TkObj, with CTP7 wrapping
        MP7DataWord data_in[CTP7_NCHANN_IN], data_out[CTP7_NCHANN_OUT];

        // initialize
        for (int i = 0; i < CTP7_NCHANN_IN; ++i) { data_in[i] = 0; }
        for (int i = 0; i < CTP7_NCHANN_OUT; ++i) { data_out[i] = 0; }
        mp7wrapped_pack_in(track, data_in);
        TOP_FUNC(data_in, data_out);
        mp7wrapped_unpack_out(data_out, outvtx);
        REF_FUNC(track, outvtx_ref);

        // write out patterns for CTP7 board hardware or simulator test
        serInPatterns4(data_in,CTP7_NCHANN_IN); serOutPatterns4(data_out,CTP7_NCHANN_OUT);

#elif defined(TESTVCU118) // Full L1TkObj, with VCU118 wrapping
        // create the necessary data structures
        MP7DataWord data_in[NTRACK];
        MP7DataWord data_out[NVTX];
        hls::stream<MP7DataWordVecIn>  data_in_stream;
        hls::stream<MP7DataWordVecOut> data_out_stream;

        // initialize the input array
        for (int i = 0; i < NTRACK; ++i) data_in[i] = 0;
        // initialize the output array
        for (int i = 0; i < NVTX; ++i) data_out[i] = 0;
        
        mp7wrapped_pack_in(track, data_in, data_in_stream);
        TOP_FUNC(data_in_stream, data_out_stream);
        mp7wrapped_unpack_out(data_out_stream, data_out, outvtx);
        REF_FUNC(track, outvtx_ref);

        // write out patterns for VCU118 board hardware or simulator test
        serInPatterns5(data_in/*, (i==23) ? true : false*/); serOutPatterns5(data_out/*, (i==23) ? true : false*/);
        serInPatterns6(data_in/*, (i==23) ? true : false*/); serOutPatterns6(data_out/*, (i==23) ? true : false*/);
        serInPatterns7(data_in/*, (i==23) ? true : false*/); serOutPatterns7(data_out/*, (i==23) ? true : false*/);

#else // standard TKObjAlgo test without MP7 packing
        tkobjalgo_ref(track, outvtx_ref);
        tkobjalgo(track, outvtx);
#endif

        // write out human-readable patterns
        serHR(track, outvtx);

#ifdef TESTMP7
        if (!MP7_VALIDATE) continue;
#endif
#ifdef TESTCTP7
        if (!CTP7_VALIDATE) continue;
#endif
#ifdef TESTVCU118
        if (!VCU118_VALIDATE) continue;
#endif

        // -----------------------------------------
        // validation against the reference algorithm
        int errors = 0, ntot = 0, nvtx = 0;

        // check all vertices for a given test and over all tests
        for (int i = 0; i < NVTX; ++i) {
            if (!vtx_equals(outvtx_ref[i], outvtx[i], "Vertices", i)) { errors_tot; errors++;}
            if (outvtx_ref[i].hwSumPt > 0) { 
                bool match_gen = abs(int(outvtx_ref[i].hwZ0-pv_gen)) <= 10;
                ntot++; ntot_tot++;
                if (abs(int(outvtx_ref[i].hwZ0-pv_gen)) <= 10) { nvtx_tot++; nvtx++; resol += std::pow(double(pv_gen - outvtx_ref[i].hwZ0), 2); }
                if (abs(int(pv_gen-pv_cmssw)) <= 10) { ncmssw_vtx++; cmssw_resol += std::pow(double(pv_gen - pv_cmssw), 2); }
                if (abs(int(outvtx_ref[i].hwZ0-pv_cmssw)) <= 10) nagree++;
                printf("%sGEN PV %+4d    CMSSW PV %+4d  bin %3d :  REF %+4d  bin %3d, ptsum %8d, diff %+4d :  HW %+4d  bin %3d, ptsum %8d diff %+4d  :  MATCH %+1d%s\n",
                       (!match_gen)?"\e[1;31m":"\e[32m", int(pv_gen), int(pv_cmssw), int(tkobjalgo_fetch_bin(pv_cmssw).bin), int(outvtx_ref[i].hwZ0), int(outvtx_ref[i].hwBin),
                       int(outvtx_ref[i].hwSumPt), int(outvtx_ref[i].hwZ0-pv_gen),int(outvtx[i].hwZ0), int(outvtx[i].hwBin), int(outvtx[i].hwSumPt), int(outvtx[i].hwZ0-pv_gen),
                       int(outvtx[i].hwZ0-outvtx_ref[i].hwZ0),(!match_gen)?"\e[0m":"\e[0m");
            }
        }

        // messages per test
        if (errors != 0) {
            printf("Error in computing test %d (%d)\n", test, errors);
            printf("Inputs: \n"); debugHR.dump_inputs(track);
            printf("Reference output: \n"); debugHR.dump_outputs(outvtx_ref);
            printf("Current output: \n"); debugHR.dump_outputs(outvtx);
            return 1;
        } else if (VERBOSE){
            printf("Passed test %d (%d, %d)\n", test, ntot, nvtx);
        }
    }

    // summary of all tests
    printf("\n\n\e[1m\e[33m%s\n",std::string(110,'*').c_str());
    printf("Good matches: CMSSW %4d/%4d = %.3f  REF %4d/%4d = %.3f  (REF vs CMSSW: %4d/%4d = %.3f). ERRORS: %d\n",
           ncmssw_vtx, ntot_tot, float(ncmssw_vtx)/(ntot_tot), nvtx_tot, ntot_tot, float(nvtx_tot)/(ntot_tot), nagree, ntot_tot, float(nagree)/(ntot_tot), errors_tot);
    printf("Resolution: CMSSW: %5.3f mm   REF %5.3f mm\n", 0.5*sqrt(cmssw_resol/(ncmssw_vtx)), 0.5*sqrt(resol/(nvtx_tot)));
    printf("%s\e[0m\e[21m\n\n\n\n",std::string(110,'*').c_str());
    return 0;
}
