//
// Created by 张立军 on 2019-07-16.
//

#ifndef IJKPLAYER_DVBDEV_H
#define IJKPLAYER_DVBDEV_H

#include <linux/dvb/dmx.h>
#include <string>
#include <vector>
#include <map>
#include <mutex>

class DvbDevice {

    static const int NUM_POLLFDS = 1;
    static const int DVB_ERROR_RETRY_INTERVAL_MS = 100 * 1000;
    static const int DVB_TUNE_STOP_DELAY_MS = 100 * 1000;
    static const int FE_POLL_TIMEOUT_MS = 100;
    static const int PAT_PID = 0;

    int mFeFd;
    int mDvrFd;
    int mDvbIdx;
    int mPatFilterFd;
    std::map<int, int> mPidFilters;

public:
    DvbDevice();
    int reset();
    int setDeviceId(int dvbIdx);
    int readTsStream(uint8_t *tsBuffer, int tsBufferSize, int timeout_ms);

private:
    int init();
    int openDvbFe();
    int openDvbDvr();
    int openDvbDemux();
    void closeDvbFe();
    void closeDvbDvr();
    void closePatFilter();
    void closeAllDvbPidFilter();

    int tune();
    int startTsPidFilter(int pid, enum dmx_ts_pes type);
};


#endif //IJKPLAYER_DVBDEV_H
