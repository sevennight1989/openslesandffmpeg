#include <jni.h>
#include <string>
#include <android/log.h>
#include <pthread.h>
#include <unistd.h>
#include <queue>
#include "WlListener.h"
#include <stdio.h>
#include <stdlib.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#define TAG "PengLog"

extern "C" {
#include <libavcodec/avcodec.h>
}

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__)


SLObjectItf engineObject = NULL;
SLEngineItf engineEngine = NULL;
SLObjectItf outputMixObject = NULL;

SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

//assets播放器
SLObjectItf fdPlayerObject = NULL;
SLPlayItf fdPlayerPlay = NULL;
SLVolumeItf fdPlayerVolume = NULL;


void createEngine() {
    SLresult result;
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    (void) result;
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    (void) result;
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    (void) result;
}

std::queue<int> queue;

pthread_t pthread_product;
pthread_t pthread_customer;
pthread_mutex_t mutex;
pthread_cond_t cond;

int flag = 1;

void *productThread(void *data) {

    while (queue.size() < 40) {
        LOGD("生产者生产一个产品");
        pthread_mutex_lock(&mutex);
        queue.push(1);
        if (queue.size() > 0) {
            LOGD("生产者通知消费者有产品产生，产品数量为：%d", queue.size());
            pthread_cond_signal(&cond);
        }
        pthread_mutex_unlock(&mutex);
        sleep(4);
    }
    pthread_exit(&pthread_product);

}


void *customerThread(void *data) {

    while (flag) {
        pthread_mutex_lock(&mutex);
        if (queue.size() > 0) {
            queue.pop();
            LOGE("消费者消费一个产品，产品数量为：%d", queue.size());
        } else {
            LOGE("产品消费完了，等待生产者生产......");
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);
        usleep(500 * 1000);
    }
    pthread_exit(&pthread_customer);

}

void initMetex() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_create(&pthread_product, NULL, productThread, (void *) "product");
    pthread_create(&pthread_customer, NULL, customerThread, NULL);
}


extern "C" JNIEXPORT void

JNICALL
Java_aa_opensles_MainActivity_run(
        JNIEnv *env,
        jobject jobj) {
    for (int i = 0; i < 10; i++) {
        queue.push(i);
    }
    initMetex();
}

extern "C" JNIEXPORT void

JNICALL
Java_aa_opensles_MainActivity_quit(
        JNIEnv *env,
        jobject jobj) {
    flag = 0;
}


JavaVM *jvm;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    jvm = vm;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}

pthread_t callBackThread;

void *callbackT(void *data) {
    WlListener *wlListener = static_cast<WlListener *>(data);
    wlListener->onError(1, 200, "Child thread running success!");
    pthread_exit(&callBackThread);
}


extern "C" JNIEXPORT void

JNICALL
Java_aa_opensles_MainActivity_callback(
        JNIEnv *env,
        jobject jobj) {

    WlListener *wlListener = new WlListener(jvm, env, env->NewGlobalRef(jobj));
    wlListener->onError(0, 100, "JNIENV thread running success!");
    pthread_create(&callBackThread, NULL, callbackT, wlListener);
}

extern "C" JNIEXPORT void

JNICALL
Java_aa_opensles_MainActivity_playAssetResource(
        JNIEnv *env, jobject jobj, jobject assetManager, jstring fileName) {


    const char *utf8 = env->GetStringUTFChars(fileName, NULL);

    // use asset manager to open asset by filename
    AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
    AAsset *asset = AAssetManager_open(mgr, utf8, AASSET_MODE_UNKNOWN);
    env->ReleaseStringUTFChars(fileName, utf8);

    // open asset as file descriptor
    off_t start, length;
    int fd = AAsset_openFileDescriptor(asset, &start, &length);
    AAsset_close(asset);

    SLresult result;


    //第一步，创建引擎
    createEngine();

    //第二步，创建混音器
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mids, mreq);
    (void) result;
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    (void) result;
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        (void) result;
    }
    //第三步，设置播放器参数和创建播放器
    // 1、 配置 audio source
    SLDataLocator_AndroidFD loc_fd = {SL_DATALOCATOR_ANDROIDFD, fd, start, length};
    SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
    SLDataSource audioSrc = {&loc_fd, &format_mime};

    // 2、 配置 audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    // 创建播放器
    const SLInterfaceID ids[3] = {SL_IID_SEEK, SL_IID_MUTESOLO, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &fdPlayerObject, &audioSrc, &audioSnk,
                                                3, ids, req);
    (void) result;

    // 实现播放器
    result = (*fdPlayerObject)->Realize(fdPlayerObject, SL_BOOLEAN_FALSE);
    (void) result;

    // 得到播放器接口
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_PLAY, &fdPlayerPlay);
    (void) result;

    // 得到声音控制接口
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_VOLUME, &fdPlayerVolume);
    (void) result;

    // 设置播放状态
    if (NULL != fdPlayerPlay) {

        result = (*fdPlayerPlay)->SetPlayState(fdPlayerPlay, SL_PLAYSTATE_PLAYING);
        (void) result;
    }

    //设置播放音量 （100 * -50：静音 ）
    (*fdPlayerVolume)->SetVolumeLevel(fdPlayerVolume, 20 * -50);
    LOGD("!!!Play!!!");

}


extern "C" JNIEXPORT void
JNICALL
Java_aa_opensles_MainActivity_stopAssetResource(
        JNIEnv *env,
        jobject jobj) {
    if (NULL != fdPlayerPlay) {
        (*fdPlayerPlay)->SetPlayState(fdPlayerPlay, SL_PLAYSTATE_STOPPED);
    }
    LOGD("!!!Stop!!!");
}


extern "C" JNIEXPORT jstring
JNICALL
Java_aa_opensles_MainActivity_avcodeConfigFromJni(
        JNIEnv *env,
        jobject ) {
    char info[10000] = {0};
    sprintf(info, "%s\n", avcodec_configuration());
    LOGD("--> %s",info);
    return env->NewStringUTF(info);
}

