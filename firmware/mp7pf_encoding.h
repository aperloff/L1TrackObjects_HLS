template<unsigned int N, unsigned int OFFS> 
inline void mp7_pack(TkObj track[N], MP7DataWord data[]) {
    #pragma HLS inline
    for (unsigned int i = 0; i < N; ++i) {
        data[2*i+0+OFFS] = ( track[i].hwPtErr, track[i].hwPt );
        data[2*i+1+OFFS] = ( track[i].hwZ0, track[i].hwPhi, track[i].hwEta );
        data[2*i+1+OFFS][30] = track[i].hwTightQuality;
    }
}

template<unsigned int N, unsigned int OFFS> 
inline void mp7_unpack(MP7DataWord data[], TkObj track[N]) {
    #pragma HLS inline
    for (unsigned int i = 0; i < N; ++i) {
        track[i].hwPt    = data[2*i+0+OFFS](15, 0);
        track[i].hwPtErr = data[2*i+0+OFFS](31,16);
        track[i].hwEta   = data[2*i+1+OFFS](9, 0);
        track[i].hwPhi   = data[2*i+1+OFFS](19,10);
        track[i].hwZ0    = data[2*i+1+OFFS](29,20);
        track[i].hwTightQuality = data[2*i+1+OFFS][30];
    }
}
