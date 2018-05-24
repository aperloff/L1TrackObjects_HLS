#include <cstdio>
#include "firmware/simple_tkobjalgo.h"
#include "utils/random_inputs.h"
#include "utils/DiscretePFInputs_IO.h"
#include "utils/pattern_serializer.h"
#include "utils/test_utils.h"

#define NTEST 250//500


int main() {

    // input format: could be random or coming from simulation
    //RandomPFInputs inputs(37); // 37 is a good random number
    //DiscretePFInputs inputs("regions_TTbar_PU140.dump");
    DiscretePFInputs inputs("data/barrel_alltracks_sectors_1x12_TTbar_PU140.dump");
    
    // input TP objects
    TkObj track[NTRACK]; z0_t pv_gen; z0_t pv_cmssw;

    // output track objects
    VtxObj outvtx[NVTX], outvtx_ref[NVTX];
#if defined(TESTMP7)
    MP7PatternSerializer serInPatterns( "mp7_input_patterns.txt", HLS_pipeline_II,HLS_pipeline_II-1); // mux each event into HLS_pipeline_II frames
    MP7PatternSerializer serOutPatterns("mp7_output_patterns.txt",HLS_pipeline_II,HLS_pipeline_II-1); // assume only one tkobj core running per chip,
    MP7PatternSerializer serInPatterns2( "mp7_input_patterns_magic.txt", HLS_pipeline_II,-HLS_pipeline_II+1); // mux each event into HLS_pipeline_II frames
    MP7PatternSerializer serOutPatterns2("mp7_output_patterns_magic.txt",HLS_pipeline_II,-HLS_pipeline_II+1); // assume only one tkobj core running per chip,
    MP7PatternSerializer serInPatterns3( "mp7_input_patterns_nomux.txt");  // 
    MP7PatternSerializer serOutPatterns3("mp7_output_patterns_nomux.txt"); // ,
#endif
#if defined(TESTCTP7)
    CTP7PatternSerializer serInPatterns4( "ctp7_input_patterns_nomux.txt",CTP7_NCHANN_IN, true);  // 
    CTP7PatternSerializer serOutPatterns4("ctp7_output_patterns_nomux.txt",CTP7_NCHANN_OUT, false); // fill the rest of the lines with empty events for now
#endif
    HumanReadablePatternSerializer serHR("human_readable_patterns.txt");
    HumanReadablePatternSerializer debugHR("-"); // this will print on stdout, we'll use it for errors

    // -----------------------------------------
    // run multiple tests
    for (int test = 1; test <= NTEST; ++test) {
        printf("sfsg0\n");
        // initialize TP objects
        for (int i = 0; i < NTRACK; ++i) {
            track[i].hwPt = 0; track[i].hwPtErr = 0; track[i].hwEta = 0; track[i].hwPhi = 0; track[i].hwZ0 = 0; 
        }

        // get the inputs from the input object
        //if (!inputs.nextRegion(track, hwZPV)) break;
        printf("sfsg1\n");
        if (!inputs.nextEvent(track, pv_gen, pv_cmssw)) break;
        printf("sfsg2\n");
#if defined(TESTMP7) // Full PF, with MP7 wrapping 
        MP7DataWord data_in[MP7_NCHANN], data_out[MP7_NCHANN];
        // initialize
        for (int i = 0; i < MP7_NCHANN; ++i) {
            data_in[i] = 0;
            data_out[i] = 0;
        }
        mp7wrapped_pack_in(track, data_in);
        TOP_FUNC(data_in, data_out);
        mp7wrapped_unpack_out(data_out, outvtx);
		// for (int ii = 0; ii < 72; ++ii){ std::cout << ii << ", " << data_in[ii] << std::endl; }
		
        REF_FUNC(track, outvtx_ref);

        // write out patterns for MP7 board hardware or simulator test
        serInPatterns(data_in); serOutPatterns(data_out);
        serInPatterns2(data_in); serOutPatterns2(data_out);
        serInPatterns3(data_in); serOutPatterns3(data_out);

#elif defined(TESTCTP7) // Full PF, with CTP7 wrapping
        printf("sfsg3\n");
        MP7DataWord data_in[CTP7_NCHANN_IN], data_out[CTP7_NCHANN_OUT];
        // initialize
        for (int i = 0; i < CTP7_NCHANN_IN; ++i) { data_in[i] = 0; }
        for (int i = 0; i < CTP7_NCHANN_OUT; ++i) { data_out[i] = 0; }
        printf("sfsg4\n");
        mp7wrapped_pack_in(track, data_in);
        printf("sfsg5\n");
        TOP_FUNC(data_in, data_out);
        printf("sfsg6\n");
        mp7wrapped_unpack_out(data_out, outvtx);
        printf("sfsg7\n");
    
        REF_FUNC(track, outvtx_ref);
        // write out patterns for CTP7 board hardware or simulator test
        serInPatterns4(data_in,CTP7_NCHANN_IN); serOutPatterns4(data_out,CTP7_NCHANN_OUT);       

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

        // -----------------------------------------
        // validation against the reference algorithm
        int errors = 0; int ntot = 0, nvtx = 0;

        // check charged hadrons
        for (int i = 0; i < 1; ++i) {
            if (!vtx_equals(outvtx_ref[i], outvtx[i], "Vertices", i)) errors++;
            if (outvtx_ref[i].hwSumPt > 0) { ntot++; nvtx++; }
        }

        if (errors != 0) {
            printf("Error in computing test %d (%d)\n", test, errors);
            printf("Inputs: \n"); debugHR.dump_inputs(track);
            printf("Reference output: \n"); debugHR.dump_outputs(outvtx_ref);
            printf("Current output: \n"); debugHR.dump_outputs(outvtx);
            return 1;
        } else {
            printf("Passed test %d (%d, %d)\n", test, ntot, nvtx);
        }

    }
    return 0;
}
