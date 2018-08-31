#include "pattern_serializer.h"
#include <cassert>
#include <string>
#include <cstdlib>
#include <cassert>

MP7PatternSerializer::MP7PatternSerializer(const std::string &fname, unsigned int nmux, int nempty, unsigned int nlinks, const std::string &boardName) :
    fname_(fname), nlinks_(nlinks ? nlinks : MP7_NCHANN/nmux), nmux_(nmux), nchann_(MP7_NCHANN/nmux), nempty_(std::abs(nempty)), fillmagic_(nempty<0), file_(nullptr), ipattern_(0) 
{
    if (!fname.empty()) {
        file_ = fopen(fname.c_str(), "w");
        fprintf(file_, "Board %s\n", boardName.c_str());
        fprintf(file_, " Quad/Chan :    ");
        for (unsigned int i = 0; i < nlinks_; ++i) fprintf(file_, "q%02dc%1d      ", i/4, i % 4);
        fprintf(file_, "\n      Link :     ");
        for (unsigned int i = 0; i < nlinks_; ++i) fprintf(file_, "%02d         ", i);
        fprintf(file_, "\n");
    }
    if (nmux_ > 1) {
        assert(MP7_NCHANN % nmux_ == 0);
        buffer_.resize(nmux_);
        zero();
    }
}

MP7PatternSerializer::~MP7PatternSerializer() 
{
    if (file_) {
        if (nmux_ > 1 && (ipattern_ % nmux_ != 0)) flush();
        fclose(file_); file_ = nullptr;
        printf("Saved %u MP7 patterns to %s.\n", ipattern_, fname_.c_str());
    }
}

void MP7PatternSerializer::operator()(const MP7DataWord event[MP7_NCHANN]) 
{
    if (!file_) return;
    if (nmux_ == 1) print(ipattern_, event);
    else push(event);
    ipattern_++;
    if (nempty_ > 0) {
        MP7DataWord zero_event[MP7_NCHANN];
        for (unsigned int j = 0; j < MP7_NCHANN; ++j) zero_event[j] = 0;
        for (unsigned int iempty = 0; iempty < nempty_; ++iempty) {
            if (fillmagic_) {
                for (unsigned int j = 0; j < MP7_NCHANN; ++j) zero_event[j] = ((ipattern_ << 16) & 0xFFFF0000) | ((iempty << 8) & 0xFF00) | (j & 0xFF);
            }
            if (nmux_ == 1) print(ipattern_, zero_event);
            else push(zero_event);
            ipattern_++;
        }
    }
}
template<typename T> void MP7PatternSerializer::print(unsigned int iframe, const T & event) 
//void MP7PatternSerializer::print(const MP7DataWord event[MP7_NCHANN]) 
{
    fprintf(file_, "Frame %04u :", iframe);
    for (unsigned int i = 0; i < nlinks_; ++i) {
        fprintf(file_, " 1v%08x", unsigned(event[i]));
    }
    fprintf(file_, "\n");
}
void MP7PatternSerializer::push(const MP7DataWord event[MP7_NCHANN])
{
    int imux = (ipattern_ % nmux_), offs = imux * nchann_;
    for (unsigned int ic = 0, i = 0; ic < nchann_; ++ic) {
        for (unsigned int im = 0; im < nmux_; ++im, ++i) {
            buffer_[im][offs+ic] = event[i];
        }
    }
    if (imux == nmux_-1) flush();
}
void MP7PatternSerializer::flush() {
    for (unsigned int im = 0, iframe = ipattern_ - nmux_ + 1; im < nmux_; ++im, ++iframe) {
        print(iframe, buffer_[im]);
    }
    zero();
}
void MP7PatternSerializer::zero() {
    for (unsigned int i = 0; i < nmux_; ++i) {
        for (unsigned int j = 0; j < MP7_NCHANN; ++j) {
            buffer_[i][j] = 0;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HumanReadablePatternSerializer::HumanReadablePatternSerializer(const std::string &fname) :
    fname_(fname), file_(nullptr), ipattern_(0) 
{
    if (!fname.empty()) {
        if (fname == "-") {
            file_ = stdout;
        } else {
            file_ = fopen(fname.c_str(), "w");
        }
    }
}

HumanReadablePatternSerializer::~HumanReadablePatternSerializer() 
{
    if (file_ && (file_ != stdout)) {
        fclose(file_); 
        printf("Saved %u human readable patterns to %s.\n", ipattern_, fname_.c_str());
    }
}

void HumanReadablePatternSerializer::operator()(const TkObj track[NTRACK], const VtxObj outvtx[NVTX]) 
{
    if (!file_) return;
    fprintf(file_, "Frame %04u:\n", ipattern_);
    dump_inputs(track);
    dump_outputs(outvtx);
    fprintf(file_, "\n");
    if (file_ == stdout) fflush(file_);
    ipattern_++;
}

void HumanReadablePatternSerializer::dump_inputs(const TkObj track[NTRACK]) {
    dump_track(track);
}

void HumanReadablePatternSerializer::dump_track(const TkObj track[NTRACK], unsigned int N) {
    for (int i = 0; i < N; ++i) {
        fprintf(file_, "   track %3d, hwPt % 7d    hwEta %+7d   hwPhi %+7d     hwZ0 %+7d\n", i, int(track[i].hwPt), int(track[i].hwEta), int(track[i].hwPhi), int(track[i].hwZ0));
    }
    if (file_ == stdout) fflush(file_);
}

void HumanReadablePatternSerializer::dump_outputs(const VtxObj outvtx[NVTX]) 
{
    for (int i = 0; i < NVTX; ++i) {
        fprintf(file_, "   vtx %+7d, hwSumPt %+7d   hwZ0 %+7d   mult %+3d   hwBin %+3d\n", i,
                int(outvtx[i].hwSumPt), int(outvtx[i].hwZ0), int(outvtx[i].mult), int(outvtx[i].hwBin));
    }
    if (file_ == stdout) fflush(file_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTP7PatternSerializer::CTP7PatternSerializer(const std::string &fname, unsigned int nchann_max, bool isInput, unsigned int nmux, int nempty, const std::string &boardName) :
    fname_(fname), isInput_(isInput), nmux_(nmux), nchann_(MP7_NCHANN/nmux), nempty_(std::abs(nempty)), fillmagic_(nempty<0), file_(nullptr), ipattern_(0) 
{
    if (!fname.empty()) {
        file_ = fopen(fname.c_str(), "w");
        fprintf(file_, "========================================================================================================================\n");
        if (isInput_) fprintf(file_, "Input "); // hard-coded for CTP7 number if inputs and outputs
        else fprintf(file_, "Output ");
        for (int i = 0; i < nchann_max; ++i) fprintf(file_, "LINK_%02i ", i);
        fprintf(file_, "\n");
        fprintf(file_, "========================================================================================================================\n");
    }
}

CTP7PatternSerializer::~CTP7PatternSerializer() 
{
    if (file_) {
        // if (nmux_ > 1 && (ipattern_ % nmux_ != 0)) flush();
        fclose(file_); file_ = nullptr;
        printf("Saved %u CTP7 patterns to %s.\n", ipattern_, fname_.c_str());
    }
}

void CTP7PatternSerializer::operator()(const MP7DataWord event[MP7_NCHANN], unsigned int nchann)
{
    if (!file_) return;
    if (ipattern_ > CTP7_MEMORY_DEPTH) return; // total depth of ctp7 memory
    if (nmux_ == 1) print(ipattern_, event, nchann); // nmux is always 1
}
template<typename T> void CTP7PatternSerializer::print(unsigned int iframe, const T & event, unsigned int nchann) 
{

    std::cout << "ipattern = " << ipattern_ << std::endl;
    if (ipattern_ > CTP7_MEMORY_DEPTH) return;
    fprintf(file_, "0x%05x", iframe);
    for (unsigned int i = 0; i < nchann; ++i) {
        if (event[i] == 0) fprintf(file_, " 0x%08x", unsigned(event[i]));
        else fprintf(file_, " 0x%08x", unsigned(event[i]));
    }
    fprintf(file_, "\n");
    ipattern_++;

    // because it runs at 240 MHz, make some empties, right now code skips every 5 events
    for (unsigned int j = 0; j < 5; ++j) {
        std::cout << "ipattern = " << ipattern_ << std::endl;
        if (ipattern_ > CTP7_MEMORY_DEPTH) return;
        ipattern_++;
        iframe++;
        fprintf(file_, "0x%05x", iframe);
        for (unsigned int i = 0; i < nchann; ++i) { fprintf(file_, " 0x00000000");}
        fprintf(file_, "\n");
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
VCU118PatternSerializer::VCU118PatternSerializer(const std::string &fname, unsigned int nmux, int nempty, unsigned int nlinks, const std::string &boardName) :
    fname_(fname), nlinks_(nlinks ? nlinks : VCU118_NCHANN/nmux), nmux_(nmux), nchann_(VCU118_NCHANN/nmux), nempty_(std::abs(nempty)), fillmagic_(nempty<0), file_(nullptr), ipattern_(0)
{
    char bname[100], mux[50], depth_width[256];
    sprintf(bname,"; Board: %s",boardName.c_str());
    sprintf(mux, "; tmux: %i", nmux);
    sprintf(depth_width, "; of depth=%i, and width=%i. In this case, values are specified",NTRACKS_PER_SECTOR,MP7DataWordWidth*NSECTORS);
    std::vector<std::string> vheader;
    vheader.push_back("; Sample memory initialization file for Dual Port Block Memory,");
    vheader.push_back("; v3.0 or later.");
    vheader.push_back(std::string(bname));
    vheader.push_back(std::string(mux));
    vheader.push_back(";");
    vheader.push_back("; This .COE file specifies the contents for a block memory");
    vheader.push_back(std::string(depth_width));
    vheader.push_back("; in binary format.");
    vheader.push_back("memory_initialization_radix=2;");
    vheader.push_back("memory_initialization_vector=");

    // open the file and print the header
    if (!fname.empty()) {
        file_ = fopen(fname.c_str(), "w");
        for(unsigned int iline=0; iline<vheader.size(); ++iline) fprintf(file_, (vheader[iline]+"\n").c_str());
    }
    if (nmux_ > 1) {
        assert(VCU118_NCHANN % nmux_ == 0);
        buffer_.resize(nmux_);
        zero();
    }
}

VCU118PatternSerializer::~VCU118PatternSerializer() 
{
    if (file_) {
        if (nmux_ > 1 && (ipattern_ % nmux_ != 0)) flush();
        fclose(file_); file_ = nullptr;
        printf("Saved %u VCU118 patterns to %s.\n", ipattern_, fname_.c_str());
    }
}

void VCU118PatternSerializer::operator()(const MP7DataWord event[VCU118_NCHANN], bool last)
{
    //printf("nmux: %i\n", nmux_);
    //printf("nempty_: %i\n",nempty_);
    if (!file_) return;
    if (ipattern_ > VCU118_MEMORY_DEPTH) return; // total depth of VCU118 memory
    if (nmux_ == 1) print(event, last);
    else push(event);
    ipattern_++;
    if (nempty_ > 0) {
        MP7DataWord zero_event[VCU118_NCHANN];
        for (unsigned int j = 0; j < VCU118_NCHANN; ++j) zero_event[j] = 0;
        for (unsigned int iempty = 0; iempty < nempty_; ++iempty) {
            if (fillmagic_) {
                for (unsigned int j = 0; j < VCU118_NCHANN; ++j) zero_event[j] = ((ipattern_ << 16) & 0xFFFF0000) | ((iempty << 8) & 0xFF00) | (j & 0xFF);
            }
            if (nmux_ == 1) print(zero_event, last);
            else push(zero_event);
            ipattern_++;
        }
    }
    //printf("ipattern_: %i\n", ipattern_);
}
template<typename T> void VCU118PatternSerializer::set(const T & object)
{
    for(int b=0; b<MP7DataWordWidth; b++) {
        bset_.set(b,object[b]);
    }
}
template<typename T> void VCU118PatternSerializer::print(const T & event, bool last) 
{
    if (ipattern_ > VCU118_MEMORY_DEPTH) return;
    for (unsigned int i = 0; i < nlinks_; ++i) {
        set(event[i]);
        fprintf(file_, "%s", bset_.to_string().c_str());
        //printf("link%i: %s\n", i, bset_.to_string().c_str());
    }
    fprintf(file_, (last) ? ";\n" : ",\n");
}
void VCU118PatternSerializer::push(const MP7DataWord event[VCU118_NCHANN])
{
    int imux = (ipattern_ % nmux_), offs = imux * nchann_;
    for (unsigned int ic = 0, i = 0; ic < nchann_; ++ic) {
        for (unsigned int im = 0; im < nmux_; ++im, ++i) {
            buffer_[im][offs+ic] = event[i];
        }
    }
    if (imux == nmux_-1) flush();
}
void VCU118PatternSerializer::flush() {
    for (unsigned int im = 0, iframe = ipattern_ - nmux_ + 1; im < nmux_; ++im, ++iframe) {
        print(buffer_[im]);
    }
    zero();
}
void VCU118PatternSerializer::zero() {
    for (unsigned int i = 0; i < nmux_; ++i) {
        for (unsigned int j = 0; j < VCU118_NCHANN; ++j) {
            buffer_[i][j] = 0;
        }
    }
}
