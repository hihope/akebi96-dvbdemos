//
// Created by 张立军 on 2019-07-17.
//

#include "dvb_jni.h"
#include "DvbDevice.h"

static DvbDevice *sDvbDevice = NULL;

JNIEXPORT jint JNICALL
Java_tv_danmaku_ijk_media_example_dvb_DvbDevice_nativeCreate(JNIEnv *env, jobject thiz) {
    sDvbDevice = new DvbDevice();
    return 0;
}

JNIEXPORT jint JNICALL
Java_tv_danmaku_ijk_media_example_dvb_DvbDevice_nativeSetDeviceId(JNIEnv *env, jobject thiz, jint idx) {
    return sDvbDevice->setDeviceId(idx);
}

JNIEXPORT jint JNICALL
Java_tv_danmaku_ijk_media_example_dvb_DvbDevice_nativeReadData(JNIEnv *env, jobject thiz) {
    return 0;
}


