//
// Created by Administrator on 2018/12/20/020.
//

#include <jni.h>

#ifndef OPENSLES_WLLISTENER_H
#define OPENSLES_WLLISTENER_H


class WlListener {

public:
    JavaVM *jvm;
    _JNIEnv *jenv;
    jobject jobj;
    jmethodID jmid;

    WlListener(JavaVM *vm, _JNIEnv *env, jobject obj);

    ~WlListener();

    void onError(int type, int code, const char *msg);

};


#endif //OPENSLES_WLLISTENER_H
