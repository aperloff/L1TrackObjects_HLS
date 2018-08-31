template<unsigned int N, unsigned int OFFS> 
//inline void mp7_pack(TkObj track[N], MP7DataWord data[]) {
inline void mp7_pack(TkObj track[NTRACK], MP7DataWord data_array[NCHANN], hls::stream<MP7DataWordVecIn> &data) {
    #pragma HLS inline
    MP7DataWordVecIn secvec;
    unsigned int index = 0;
    mp7_pack_tracks: for(unsigned int t = 0; t < NTRACK/N; ++t) {
        mp7_pack_sectors: for (unsigned int i = 0; i < N; ++i) {
            index = t*N+i;
            #ifdef TESTVCU118
                data_array[WORDS_PER_OBJ*i+OFFS]  = ( track[index].hwSpare, track[index].hwStubs, track[index].hwStubPtConsistency, 
                                                      track[index].hwChi2, track[index].hwEta, track[index].hwZ0, track[index].hwD0, track[index].hwPhi,
                                                      track[index].hwCharge, track[index].hwPt );
                secvec.data[WORDS_PER_OBJ*i+OFFS] = ( track[index].hwSpare, track[index].hwStubs, track[index].hwStubPtConsistency, 
                                                      track[index].hwChi2, track[index].hwEta, track[index].hwZ0, track[index].hwD0, track[index].hwPhi,
                                                      track[index].hwCharge, track[index].hwPt );
            #else
                data_array[WORDS_PER_OBJ*i+0+OFFS]      = ( track[index].hwPtErr, track[index].hwPt );
                data_array[WORDS_PER_OBJ*i+1+OFFS]      = ( track[index].hwZ0, track[index].hwPhi, track[index].hwEta );
                data_array[WORDS_PER_OBJ*i+1+OFFS][30]  = track[index].hwTightQuality;
                secvec.data[WORDS_PER_OBJ*i+0+OFFS]     = ( track[index].hwPtErr, track[index].hwPt );
                secvec.data[WORDS_PER_OBJ*i+1+OFFS]     = ( track[index].hwZ0, track[index].hwPhi, track[index].hwEta );
                secvec.data[WORDS_PER_OBJ*i+1+OFFS][30] = track[index].hwTightQuality;
            #endif
        }
        data.write(secvec);
    }
}

template<unsigned int N, unsigned int OFFS> 
inline void mp7_unpack(hls::stream<MP7DataWordVecIn> &data, hls::stream<TkObjVec> &track) {
    #pragma HLS inline
    TkObjVec tracks;
    mp7_unpack_tracks: for (unsigned int i = 0; i < NTRACK/N; ++i) {
        #pragma HLS pipeline II=HLS_pipeline_II
        MP7DataWordVecIn sectors = data.read();
        mp7_unpack_sectors: for (unsigned int j = 0; j<N; ++j) {
            #ifdef TESTVCU118
                tracks.data[j].hwPt                = sectors.data[WORDS_PER_OBJ*j+OFFS](15, 0);
                tracks.data[j].hwCharge            = sectors.data[WORDS_PER_OBJ*j+OFFS][16];
                tracks.data[j].hwPhi               = sectors.data[WORDS_PER_OBJ*j+OFFS](33, 17);
                tracks.data[j].hwD0                = sectors.data[WORDS_PER_OBJ*j+OFFS](43,34);
                tracks.data[j].hwZ0                = sectors.data[WORDS_PER_OBJ*j+OFFS](55,44);
                tracks.data[j].hwEta               = sectors.data[WORDS_PER_OBJ*j+OFFS](67,56);
                tracks.data[j].hwChi2              = sectors.data[WORDS_PER_OBJ*j+OFFS](77,68);
                tracks.data[j].hwStubPtConsistency = sectors.data[WORDS_PER_OBJ*j+OFFS](82,78);
                tracks.data[j].hwStubs             = sectors.data[WORDS_PER_OBJ*j+OFFS](97,83);
                tracks.data[j].hwSpare             = sectors.data[WORDS_PER_OBJ*j+OFFS](99,98);
            #else
                tracks.data[j].hwPt           = sectors.data[WORDS_PER_OBJ*j+0+OFFS](15, 0);
                tracks.data[j].hwPtErr        = sectors.data[WORDS_PER_OBJ*j+0+OFFS](31,16);
                tracks.data[j].hwEta          = sectors.data[WORDS_PER_OBJ*j+1+OFFS](9, 0);
                tracks.data[j].hwPhi          = sectors.data[WORDS_PER_OBJ*j+1+OFFS](19,10);
                tracks.data[j].hwZ0           = sectors.data[WORDS_PER_OBJ*j+1+OFFS](29,20);
                tracks.data[j].hwTightQuality = sectors.data[WORDS_PER_OBJ*j+1+OFFS][30];
            #endif
        }
        track.write(tracks);
    }
}
