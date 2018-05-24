#ifndef FASTPUPPI_NTUPLERPRODUCER_DISCRETEPFINPUTS_IO_H
#define FASTPUPPI_NTUPLERPRODUCER_DISCRETEPFINPUTS_IO_H

#include <vector>
#include <cassert>
#include "../firmware/data.h"
#include "DiscretePFInputs.h"
#include "DiscretePF2Firmware.h"
#include <iostream>

typedef l1tpf_int::InputRegion Region;

struct Event {
	uint32_t run, lumi; uint64_t event;
	float z0, genZ0;
	float alphaCMed, alphaCRms, alphaFMed, alphaFRms;
	std::vector<Region> regions;
	
	Event() : run(0), lumi(0), event(0), z0(0.), alphaCMed(0.), alphaCRms(0.), alphaFMed(0.), alphaFRms(0.), regions() {}
	bool readFromFile(FILE *fRegionDump) {
		if (!fread(&run, sizeof(uint32_t), 1, fRegionDump)) return false;
		fread(&lumi, sizeof(uint32_t), 1, fRegionDump);
		fread(&event, sizeof(uint64_t), 1, fRegionDump);
		l1tpf_int::readManyFromFile(regions, fRegionDump); 
		fread(&z0, sizeof(float), 1, fRegionDump);
		fread(&genZ0, sizeof(float), 1, fRegionDump);
		fread(&alphaCMed, sizeof(float), 1, fRegionDump);
		fread(&alphaCRms, sizeof(float), 1, fRegionDump);
		fread(&alphaFMed, sizeof(float), 1, fRegionDump);
		fread(&alphaFRms, sizeof(float), 1, fRegionDump);
		return true;
	}
};

class DiscretePFInputs {
	public:
		DiscretePFInputs(const char *fileName) : file_(fopen(fileName,"rb")), iregion_(0) {
                    if (!file_) { std::cout << "ERROR: cannot read '" << fileName << "'" << std::endl; }
                    assert(file_);
                }
		~DiscretePFInputs() { fclose(file_); }
        
        // for region-by-region approach
		bool nextRegion(TkObj track[NTRACKS_PER_SECTOR], z0_t & hwZPV) {
			if (!nextRegion()) return false;
		    	const Region &r = event_.regions[iregion_];

                        dpf2fw::convert<NTRACKS_PER_SECTOR>(r.track, track);

#ifdef __GXX_EXPERIMENTAL_CXX0X__
			hwZPV = event_.z0 * l1tpf_int::InputTrack::Z0_SCALE;
#else
			hwZPV = event_.z0 * 20;
#endif

			printf("Read region %u with %lu tracks\n", iregion_, r.track.size());
			iregion_++;
			return true;
		}

        // for full event approach (don't mix with the above)
		bool nextEvent() {
			if (feof(file_)) return false;
			if (!event_.readFromFile(file_)) return false;
			return true;
		}
		bool nextEvent(TkObj track[NTRACK], z0_t & pv_gen, z0_t & pv_cmssw) {
			if (feof(file_)) return false;
			if (!event_.readFromFile(file_)) return false;
			if (event_.regions.size() != N_IN_SECTORS) {
            	printf("ERROR: Mismatching number of input regions: %lu\n", event_.regions.size());
            	return false;
        	}

        	TkObj track_tmp[2*NTRACKS_PER_SECTOR];
        	for (int is = 0; is < N_IN_SECTORS; ++is) {
        		const Region & r = event_.regions[is];
        		dpf2fw::convert<2*NTRACKS_PER_SECTOR>(r.track, track_tmp);
        		for (int i = 0, t = 0; i < NTRACKS_PER_SECTOR; ++i) {
        			//track[2*is+0][i] = track_tmp[t++];
        			//track[2*is+1][i] = track_tmp[t++];
        			track[((2*is+0)*NTRACKS_PER_SECTOR)+i] = track_tmp[t++];
        			track[((2*is+1)*NTRACKS_PER_SECTOR)+i] = track_tmp[t++];
        		}
        	}

#ifdef __GXX_EXPERIMENTAL_CXX0X__
        	pv_gen   = round(event_.genZ0 * l1tpf_int::InputTrack::Z0_SCALE);
        	pv_cmssw = round(event_.z0    * l1tpf_int::InputTrack::Z0_SCALE);
#else
        	pv_gen   = round(event_.genZ0 * 20);
        	pv_cmssw = round(event_.z0    * 20);
#endif
        	
			return true;
		}
		const Event & event() { return event_; }

	private:
		bool nextRegion() {
			while(true) {
				if (event_.event == 0 || iregion_ == event_.regions.size()) {
					if (feof(file_)) return false;
					if (!event_.readFromFile(file_)) return false;
					printf("Beginning of run %u, lumi %u, event %lu \n", event_.run, event_.lumi, event_.event);
					iregion_ = 0;
				}
				const Region &r = event_.regions[iregion_];
				if (fabs(r.etaCenter) > 1.5) {
					iregion_++;
					continue; // use only regions in the barrel for now
				}
				return true;
			}
		}

		FILE *file_;
		Event event_;
		unsigned int iregion_;
};
#endif
