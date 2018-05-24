# get the configuration
source config_hls_tkobjalgo.tcl

# open the project, don't forget to reset
open_project -reset proj_${l1board}_${l1tkobjAlgo}
#set_top bhv_add_track
set_top ${l1tkobjTopFunc}
add_files firmware/simple_tkobjalgo.cpp -cflags "-DTEST${l1board} -std=c++0x -DQUALITY=${l1tkQualityCuts}"
# -DCMSSW
add_files -tb simple_tkobjalgo_test.cpp -cflags "-DTEST${l1board} -DHLS_pipeline_II=2 -DTOP_FUNC=${l1tkobjTopFunc} -DREF_FUNC=${l1tkobjRefFunc} -D${l1board}_VALIDATE=${l1tkobjValidate} -DQUALITY=${l1tkQualityCuts}"
add_files -tb simple_tkobjalgo_ref.cpp -cflags "-DTEST${l1board} -std=c++0x -DQUALITY=${l1tkQualityCuts}"
add_files -tb utils/pattern_serializer.cpp -cflags "-DTEST${l1board}"
add_files -tb utils/test_utils.cpp -cflags "-DTEST${l1board}"
add_files -tb utils/DiscretePFInputs.h -cflags "-DTESTCTP7 -std=c++0x"
add_files -tb utils/DiscretePFInputs_IO.h -cflags "-DTEST${l1board} -std=c++0x"
#add_files -tb data/regions_TTbar_PU140.dump
#add_files -tb data/barrel_sectors_1x12_TTbar_PU140.dump
add_files -tb data/barrel_alltracks_sectors_1x12_TTbar_PU140.dump

# reset the solution
open_solution -reset "solution"
set_part {xcvu9p-flga2104-2-i-EVAL}
#200MHz
#create_clock -period 5 -name default
#240MHz
#create_clock -period 4.16667 -name default
#320MHz (might be a problem. Need to check the RTL implementation)
create_clock -period 3.125 -name default
set_clock_uncertainty 1.5

config_interface -trim_dangling_port
config_core DSP48 -latency 2

# do stuff
csim_design
#csynth_design
#cosim_design -trace_level all
#export_design -format ip_catalog -vendor "cern-cms" -version ${l1tkobjIPVersion} -description "${l1tkobjTopFunc}"

# exit Vivado HLS
exit
