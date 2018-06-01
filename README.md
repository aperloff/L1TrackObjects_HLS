# L1TrackObjects_HLS
Contains algorithms used for the finding/creation of phase 2 level 1 tracking based objects. Much of the code is will be run through Vivado HLS and put on FPGAs.

## To compile this project using g++

### Go to the vivado_hls GUI to look up the compile commands for each file
vivado_hls -p proj_CTP7_tkobjalgo/

### Run these compile commands
<details>
<summary>Vivado_HLS 2016.4</summary>
```
g++ -DTESTCTP7 -I/home/xilinx/Vivado_HLS/2016.4/include/etc -I/home/xilinx/Vivado_HLS/2016.4/lnx64/tools/systemc/include -I/home/aperloff/L1TrackObjects_HLS -I/home/xilinx/Vivado_HLS/2016.4/include/ap_sysc -I/home/xilinx/Vivado_HLS/2016.4/include -I/home/xilinx/Vivado_HLS/2016.4/lnx64/tools/auto_cc/include -O0 -g3 -Wall -std=c++0x -c -fmessage-length=0 utils/pattern_serializer.cpp
g++ -DTESTCTP7 -I/home/xilinx/Vivado_HLS/2016.4/include/etc -I/home/xilinx/Vivado_HLS/2016.4/lnx64/tools/systemc/include -I/home/aperloff/L1TrackObjects_HLS -I/home/xilinx/Vivado_HLS/2016.4/include/ap_sysc -I/home/xilinx/Vivado_HLS/2016.4/include -I/home/xilinx/Vivado_HLS/2016.4/lnx64/tools/auto_cc/include -O0 -g3 -Wall -c -fmessage-length=0 utils/test_utils.cpp
g++ -DTESTCTP7 -DHLS_pipeline_II=2 -DTOP_FUNC=mp7wrapped_tkobjalgo -DREF_FUNC=tkobjalgo_ref -DCTP7_VALIDATE=1 -DQUALITY=0 -I/home/xilinx/Vivado_HLS/2016.4/include/etc -I/home/xilinx/Vivado_HLS/2016.4/lnx64/tools/systemc/include -I/home/aperloff/L1TrackObjects_HLS -I/home/xilinx/Vivado_HLS/2016.4/include/ap_sysc -I/home/xilinx/Vivado_HLS/2016.4/include -I/home/xilinx/Vivado_HLS/2016.4/lnx64/tools/auto_cc/include -O0 -g3 -Wall -c -fmessage-length=0 simple_tkobjalgo_test.cpp
g++ -DTESTCTP7 -DQUALITY=0 -I/home/xilinx/Vivado_HLS/2016.4/include/etc -I/home/xilinx/Vivado_HLS/2016.4/lnx64/tools/systemc/include -I/home/aperloff/L1TrackObjects_HLS -I/home/xilinx/Vivado_HLS/2016.4/include/ap_sysc -I/home/xilinx/Vivado_HLS/2016.4/include -I/home/xilinx/Vivado_HLS/2016.4/lnx64/tools/auto_cc/include -O0 -g3 -Wall -std=c++0x -c -fmessage-length=0 simple_tkobjalgo_ref.cpp
g++ -DTESTCTP7 -DQUALITY=0 -I/home/xilinx/Vivado_HLS/2016.4/include/etc -I/home/xilinx/Vivado_HLS/2016.4/lnx64/tools/systemc/include -I/home/aperloff/L1TrackObjects_HLS -I/home/xilinx/Vivado_HLS/2016.4/include/ap_sysc -I/home/xilinx/Vivado_HLS/2016.4/include -I/home/xilinx/Vivado_HLS/2016.4/lnx64/tools/auto_cc/include -O0 -g3 -Wall -std=c++0x -c -fmessage-length=0 firmware/simple_tkobjalgo.cpp
g++ -o simple_tkobjalgo pattern_serializer.o test_utils.o simple_tkobjalgo_test.o simple_tkobjalgo.o simple_tkobjalgo_ref.o
```
</details>
<details>
<summary>Vivado_HLS 2018.1</summary>
```
g++ -DTESTVCU118 -I/opt/Xilinx/Vivado/2018.1/lnx64/tools/auto_cc/include -I/opt/Xilinx/Vivado/2018.1/lnx64/tools/systemc/include -I/nfs/data41/aperloff/L1TrackObjects_HLS -I/opt/Xilinx/Vivado/2018.1/include/ap_sysc -I/opt/Xilinx/Vivado/2018.1/include -I/opt/Xilinx/Vivado/2018.1/include/etc -O0 -g3 -Wall -std=c++0x -c -fmessage-length=0 utils/pattern_serializer.cpp
g++ -DTESTVCU118 -I/opt/Xilinx/Vivado/2018.1/lnx64/tools/auto_cc/include -I/opt/Xilinx/Vivado/2018.1/lnx64/tools/systemc/include -I/nfs/data41/aperloff/L1TrackObjects_HLS -I/opt/Xilinx/Vivado/2018.1/include/ap_sysc -I/opt/Xilinx/Vivado/2018.1/include -I/opt/Xilinx/Vivado/2018.1/include/etc -O0 -g3 -Wall -c -fmessage-length=0 utils/test_utils.cpp
g++ -DTESTVCU118 -DHLS_pipeline_II=2 -DTOP_FUNC=mp7wrapped_tkobjalgo -DREF_FUNC=tkobjalgo_ref -DVCU118_VALIDATE=1 -DQUALITY=0 -I/opt/Xilinx/Vivado/2018.1/lnx64/tools/auto_cc/include -I/opt/Xilinx/Vivado/2018.1/lnx64/tools/systemc/include -I/nfs/data41/aperloff/L1TrackObjects_HLS -I/opt/Xilinx/Vivado/2018.1/include/ap_sysc -I/opt/Xilinx/Vivado/2018.1/include -I/opt/Xilinx/Vivado/2018.1/include/etc -O0 -g3 -Wall -c -fmessage-length=0 simple_tkobjalgo_test.cpp
g++ -DTESTVCU118 -DQUALITY=0 -I/opt/Xilinx/Vivado/2018.1/lnx64/tools/auto_cc/include -I/opt/Xilinx/Vivado/2018.1/lnx64/tools/systemc/include -I/nfs/data41/aperloff/L1TrackObjects_HLS -I/opt/Xilinx/Vivado/2018.1/include/ap_sysc -I/opt/Xilinx/Vivado/2018.1/include -I/opt/Xilinx/Vivado/2018.1/include/etc -O0 -g3 -Wall -std=c++0x -c -fmessage-length=0 simple_tkobjalgo_ref.cpp
g++ -DTESTVCU118 -DQUALITY=0 -I/opt/Xilinx/Vivado/2018.1/lnx64/tools/auto_cc/include -I/opt/Xilinx/Vivado/2018.1/lnx64/tools/systemc/include -I/nfs/data41/aperloff/L1TrackObjects_HLS -I/opt/Xilinx/Vivado/2018.1/include/ap_sysc -I/opt/Xilinx/Vivado/2018.1/include -I/opt/Xilinx/Vivado/2018.1/include/etc -O0 -g3 -Wall -std=c++0x -c -fmessage-length=0 firmware/simple_tkobjalgo.cpp
g++ -o simple_tkobjalgo pattern_serializer.o test_utils.o simple_tkobjalgo_test.o simple_tkobjalgo.o simple_tkobjalgo_ref.o
```
</details>

### Run the executable
./simple_tkobjalgo
