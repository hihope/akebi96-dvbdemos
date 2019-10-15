//
// Created by 张立军 on 2019-07-16.
//

#include "DvbDevice.h"

#include <android/log.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <linux/dvb/dmx.h>
#include <linux/dvb/frontend.h>

#define LOG_TAG "zlj"

DvbDevice::DvbDevice() :
    mFeFd(-1),
    mDvrFd(-1),
    mPatFilterFd(-1) {
}

int DvbDevice::openDvbFe() {
    if (mFeFd != -1) {
        return mFeFd;
    }

    char fileName[100];
    sprintf(fileName, "/dev/dvb%d.frontend0", mDvbIdx);
    mFeFd = open(fileName, O_RDWR | O_NONBLOCK);
    if (mFeFd == -1) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Open dvb frontend %s failed: %s", fileName, strerror(errno));
        return -1;
    }

    struct dvb_frontend_info feInfo;
    if (ioctl(mFeFd, FE_GET_INFO, &feInfo) == -1) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Dvb frontend get info failed: %s", strerror(errno));
        return mFeFd;
    }

    const char *types;
    switch (feInfo.type) {
        case FE_QPSK:
            types = "DVB-S";
            break;
        case FE_QAM:
            types = "DVB-C";
            break;
        case FE_OFDM:
            types = "DVB-T";
            break;
        case FE_ATSC:
            types = "ATSC";
            break;
        default:
            types = "Unknown";
    }

    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Dvb frontend info:");
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "\tname =  %s", feInfo.name);
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "\ttypes =  %s", types);
    return mFeFd;
}

int DvbDevice::tune() {
    if (mFeFd == -1) {
        if (openDvbFe() == -1) {
            return -1;
        };
    }

    int frequency = 474000000;

    struct dtv_property deliverySystemProperty = {
            .cmd = DTV_DELIVERY_SYSTEM
    };
    deliverySystemProperty.u.data = SYS_DVBT;
    struct dtv_property frequencyProperty = {
            .cmd = DTV_FREQUENCY
    };
    frequencyProperty.u.data =frequency;

    struct dtv_property modulationProperty = { .cmd = DTV_MODULATION };
    modulationProperty.u.data = QAM_64;

    struct dtv_property bandwidthProperty = { .cmd = DTV_BANDWIDTH_HZ, .u.data = 8000000 };
    struct dtv_property tuneProperty = { .cmd = DTV_TUNE };

    struct dtv_property props[] = {
            deliverySystemProperty, frequencyProperty, modulationProperty, tuneProperty,bandwidthProperty
    };
    struct dtv_properties dtvProperty = {
            .num = 5, .props = props
    };
    if (ioctl(mFeFd, FE_SET_PROPERTY, &dtvProperty) == -1) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Can't set Frontend : %s", strerror(errno));
        return -1;
    }

    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "set frontend success");
    return 0;
}

int DvbDevice::openDvbDemux() {
    char fileName[100];
    sprintf(fileName, "/dev/dvb%d.demux0", mDvbIdx);
    int demuxFd = open(fileName, O_RDWR | O_NONBLOCK );
    if (demuxFd == -1) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Open dvb demux %s failed: %s", fileName, strerror(errno));
    }
    return demuxFd;
}

int DvbDevice::startTsPidFilter(int pid, enum dmx_ts_pes type) {
    if (mPidFilters.find(pid) != mPidFilters.end() || (mPatFilterFd != -1 && pid == PAT_PID)) {
        return 0;
    }

    int demuxFd = openDvbDemux();
    if (demuxFd == -1) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "startTsPidFilter %d failed ", pid);
        return -1;
    }

    struct dmx_pes_filter_params pesfilter;
    pesfilter.input = DMX_IN_FRONTEND;
    pesfilter.output = DMX_OUT_TS_TAP;
    pesfilter.pes_type = type;
    pesfilter.flags = DMX_IMMEDIATE_START;
    pesfilter.pid = pid;
    if (ioctl(demuxFd, DMX_SET_PES_FILTER, &pesfilter) != 0) {
        close(demuxFd);
        return -1;
    };
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "startTsPidFilter %d success ", pid);

    if (pid != PAT_PID) {
        mPidFilters.insert(std::pair<int, int>(pid, demuxFd));
    } else {
        mPatFilterFd = demuxFd;
    }
    return 0;
}

int DvbDevice::openDvbDvr() {
    if (mDvrFd != -1) {
        return mDvrFd;
    }

    char fileName[100];
    sprintf(fileName, "/dev/dvb%d.dvr0", mDvbIdx);
    mDvrFd = open(fileName, O_RDONLY | O_NONBLOCK);
    if (mDvrFd == -1) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Open dvb dvr %s failed: %s", fileName, strerror(errno));
        return -1;
    }
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Open dvb dvr ok : %s", fileName);
    return mDvrFd;
}

void DvbDevice::closeDvbFe() {
    if (mFeFd != -1) {
        close(mFeFd);
        mFeFd = -1;
    }
}

void DvbDevice::closeDvbDvr() {
    if (mDvrFd != -1) {
        close(mDvrFd);
        mDvrFd = -1;
    }
}

void DvbDevice::closePatFilter() {
    if (mPatFilterFd != -1) {
        close(mPatFilterFd);
        mPatFilterFd = -1;
    }
}

void DvbDevice::closeAllDvbPidFilter() {
    for (std::map<int, int>::iterator it(mPidFilters.begin());
         it != mPidFilters.end(); it++) {
        close(it->second);
    }
    mPidFilters.clear();
    closeDvbDvr();
}

int DvbDevice::init() {
    if (tune() == -1) {
        return -1;
    }

    startTsPidFilter(0, DMX_PES_OTHER);
    startTsPidFilter(4220, DMX_PES_OTHER);
    startTsPidFilter(17, DMX_PES_OTHER);
    startTsPidFilter(18, DMX_PES_OTHER);
    startTsPidFilter(201, DMX_PES_OTHER);
    startTsPidFilter(202, DMX_PES_OTHER);

    //    if (openDvbDvr() == -1) {
    //        return -1;
    //    };
    return 0;
}

int DvbDevice::reset() {
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "reset dvb : %d", mDvbIdx);
    closeDvbDvr();
    closeAllDvbPidFilter();
    closePatFilter();
    closeDvbFe();
}

int DvbDevice::setDeviceId(int dvbIdx) {
    mDvbIdx = dvbIdx;

    //reset();
    return init();
}

int DvbDevice::readTsStream(uint8_t *tsBuffer, int tsBufferSize, int timeout_ms) {
    if (mDvrFd == -1) {
        if (openDvbDvr() == -1) {
            return -1;
        };
    }

    struct pollfd pollFd;
    pollFd.fd = mDvrFd;
    pollFd.events = POLLIN|POLLPRI|POLLERR;
    pollFd.revents = 0;
    int poll_result = poll(&pollFd, NUM_POLLFDS, timeout_ms);
    if (poll_result == 0) {
        return 0;
    } else if (poll_result == -1 || pollFd.revents & POLLERR) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Can't read DVR : %s", strerror(errno));
        closeDvbDvr();
        usleep(DVB_ERROR_RETRY_INTERVAL_MS);
        return -1;
    }
    return read(mDvrFd, tsBuffer, tsBufferSize);
}