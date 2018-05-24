#ifndef RANDOM_INPUTS_H
#define RANDOM_INPUTS_H

class RandomPFInputs {
public:
	RandomPFInputs(int seed) { srand(seed); }
	~RandomPFInputs() { }
	bool nextRegion(TkObj track[NTRACK], z0_t & hwZPV) {
		const float PT_SCALE = 4.0;
		const float ETAPHI_SCALE = (4*180/M_PI);
		const float Z0_SCALE = 20.;

		int ncharged = (rand() % NTRACK/2) + NTRACK/2;

		float zPV = (rand()/float(RAND_MAX))*20-10;
		hwZPV = zPV * Z0_SCALE;

		for (int i = 0; i < ncharged && i < NTRACK; ++i) {
			float pt = (rand()/float(RAND_MAX))*50+2, eta = (rand()/float(RAND_MAX))*2.0-1.0, phi = (rand()/float(RAND_MAX))*2.0-1.0;
			float z = (i % 2 == 0) ? (zPV + (rand()/float(RAND_MAX))*0.7-.35) : ((rand()/float(RAND_MAX))*30-15);
			track[i].hwPt    = pt * PT_SCALE;
			track[i].hwPtErr = (0.2*pt+4) * PT_SCALE; 
			track[i].hwEta = eta * ETAPHI_SCALE;
			track[i].hwPhi = phi * ETAPHI_SCALE;
			track[i].hwZ0  = z * Z0_SCALE;
		}
		
		return true;
	}
};

#endif
