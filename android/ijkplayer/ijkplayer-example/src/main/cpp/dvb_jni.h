//
// Created by 张立军 on 2019-07-17.
//

#ifndef IJKPLAYER_DVB_JNI_H
#define IJKPLAYER_DVB_JNI_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL
Java_tv_danmaku_ijk_media_example_dvb_DvbDevice_nativeCreate(JNIEnv *env, jobject thiz);

JNIEXPORT jint JNICALL
Java_tv_danmaku_ijk_media_example_dvb_DvbDevice_nativeSetDeviceId(JNIEnv *env, jobject thiz, jint idx);

JNIEXPORT jint JNICALL
Java_tv_danmaku_ijk_media_example_dvb_DvbDevice_nativeReadData(JNIEnv *env, jobject thiz);

#ifdef __cplusplus
}
#endif

#endif //IJKPLAYER_DVB_JNI_H
