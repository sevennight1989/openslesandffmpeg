//
// Created by Administrator on 2018/12/20/020.
//

#include "WlListener.h"


WlListener::WlListener(JavaVM *vm, _JNIEnv *env, jobject obj) {
    jvm = vm;
    jenv = env;
    jobj = obj;
    jclass clz = jenv->GetObjectClass(jobj);
    if (!clz) {
        return;
    }

    jmid = jenv->GetMethodID(clz, "onError", "(ILjava/lang/String;)V");
    if (!jmid) {
        return;
    }
}

void WlListener::onError(int type, int code, const char *msg) {
    if (type == 0) {
        jstring jmsg = jenv->NewStringUTF(msg);
        jenv->CallVoidMethod(jobj, jmid, code, jmsg);
        jenv->DeleteLocalRef(jmsg);
    } else if (type == 1) {
        JNIEnv *env;
        jvm->AttachCurrentThread(&env, 0);
        jstring jmsg = env->NewStringUTF(msg);
        env->CallVoidMethod(jobj, jmid, code, jmsg);
        env->DeleteLocalRef(jmsg);
        jvm->DetachCurrentThread();
    }
}
