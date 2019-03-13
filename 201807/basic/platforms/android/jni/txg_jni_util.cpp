#include "txg_jni_util.h"
#include "txg_log.h"
#include <pthread.h>

static pthread_key_t txg_jni_key;

jclass _getClassID(const char *className) {
    if (nullptr == className) {
        return nullptr;
    }

    JNIEnv *env = TXCJNIUtil::getEnv();

    jstring _jstrClassName = env->NewStringUTF(className);

    jclass _clazz = (jclass) env->CallObjectMethod(TXCJNIUtil::classloader,
                                                   TXCJNIUtil::loadclassMethod_methodID,
                                                   _jstrClassName);

    if (nullptr == _clazz) {
        LOGE("Classloader failed to find class of %s", className);
        env->ExceptionClear();
    }

    env->DeleteLocalRef(_jstrClassName);

    return _clazz;
}

void _detachCurrentThread(void *a) {
    TXCJNIUtil::getJavaVM()->DetachCurrentThread();

    pthread_t thisthread = pthread_self();
    LOGD("JniHelper::_detachCurrentThread(), pthread_self() = %ld", thisthread);
}

//copy from AOSP: dalvik/vm/Checkjni.cpp
static char checkUtfBytes(const char *bytes, const char **errorKind) {
    while (*bytes != '\0') {
        char utf8 = *(bytes++);
        // Switch on the high four bits.
        switch (utf8 >> 4) {
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
                // Bit pattern 0xxx. No need for any extra bytes.
                break;
            case 0x08:
            case 0x09:
            case 0x0a:
            case 0x0b:
            case 0x0f:
                /*
                 * Bit pattern 10xx or 1111, which are illegal start bytes.
                 * Note: 1111 is valid for normal UTF-8, but not the
                 * modified UTF-8 used here.
                 */
                *errorKind = "start";
                return utf8;
            case 0x0e:
                // Bit pattern 1110, so there are two additional bytes.
                utf8 = *(bytes++);
                if ((utf8 & 0xc0) != 0x80) {
                    *errorKind = "continuation";
                    return utf8;
                }
                // Fall through to take care of the final byte.
            case 0x0c:
            case 0x0d:
                // Bit pattern 110x, so there is one additional byte.
                utf8 = *(bytes++);
                if ((utf8 & 0xc0) != 0x80) {
                    *errorKind = "continuation";
                    return utf8;
                }
                break;
        }
    }
    return 0;
}

jmethodID TXCJNIUtil::getMethodByName(jclass jCls, const char *methodName, const char *signature) {
    if (jCls == NULL || TXCJNIUtil::getEnv() == NULL) return NULL;

    return TXCJNIUtil::getEnv()->GetMethodID(jCls, methodName, signature);
}

jmethodID
TXCJNIUtil::getStaticMethodByName(jclass jCls, const char *methodName, const char *signature) {
    if (jCls == NULL || TXCJNIUtil::getEnv() == NULL) return NULL;

    return TXCJNIUtil::getEnv()->GetStaticMethodID(jCls, methodName, signature);
}

jmethodID TXCJNIUtil::getMethodByObj(jobject jObj, const char *methodName, const char *signature) {
    if (jObj == NULL || TXCJNIUtil::getEnv() == NULL) return NULL;

    jclass cls = TXCJNIUtil::getEnv()->GetObjectClass(jObj);

    jmethodID method = getMethodByName(cls, methodName, signature);

    TXCJNIUtil::getEnv()->DeleteLocalRef(cls);

    return method;
}

jmethodID
TXCJNIUtil::getStaticMethodByObj(jobject jObj, const char *methodName, const char *signature) {
    if (jObj == NULL || TXCJNIUtil::getEnv() == NULL) return NULL;

    jclass cls = TXCJNIUtil::getEnv()->GetObjectClass(jObj);

    jmethodID method = getStaticMethodByName(cls, methodName, signature);

    TXCJNIUtil::getEnv()->DeleteLocalRef(cls);

    return method;
}

void TXCJNIUtil::CheckUTF8(const char *str) {
    if (str == NULL) {
        return;
    }

    const char *errorKind = NULL;
    checkUtfBytes(str, &errorKind);
    if (errorKind != NULL) {
        __android_log_print(ANDROID_LOG_WARN, "JNIUTL", "string %s contain no utf8 char", str);
    }
}

void
TXCJNIUtil::SetJavaObjStringField(JNIEnv *env, jclass pCls, jobject pObj, const char *pFieldName,
                                  const char *pValue) {
    jfieldID jFieldID = env->GetFieldID(pCls, pFieldName, "Ljava/lang/String;");
    if (jFieldID != NULL) {
        CheckUTF8(pValue);
        jstring jStr = env->NewStringUTF(pValue);
        env->SetObjectField(pObj, jFieldID, jStr);
//		env->ReleaseStringUTFChars(jStr, pValue);
    }
}

JavaVM *TXCJNIUtil::_psJavaVM = nullptr;
jmethodID TXCJNIUtil::loadclassMethod_methodID = nullptr;
jobject TXCJNIUtil::classloader = nullptr;

void TXCJNIUtil::setJavaVM(JavaVM *javaVM) {
    pthread_t thisthread = pthread_self();
    LOGD("JniHelper::setJavaVM(%p), pthread_self() = %ld", javaVM, thisthread);
    _psJavaVM = javaVM;

    pthread_key_create(&txg_jni_key, _detachCurrentThread);
}

JavaVM *TXCJNIUtil::getJavaVM() {
    pthread_t thisthread = pthread_self();
    LOGD("JniHelper::getJavaVM(), pthread_self() = %ld", thisthread);
    return _psJavaVM;
}

JNIEnv *TXCJNIUtil::getEnv() {
    if (_psJavaVM == nullptr) {
        return nullptr;
    }
    JNIEnv *_env = (JNIEnv *) pthread_getspecific(txg_jni_key);
    if (_env == nullptr) {
        LOGD("JniHelper::getEnv() env == null ptr");
        _env = TXCJNIUtil::cacheEnv(_psJavaVM);
    }
    return _env;
}

bool TXCJNIUtil::setClassLoaderFrom(jobject activityInstance) {
    TXCJniMethodInfo _getclassloaderMethod;
    if (!TXCJNIUtil::getMethodInfo_DefaultClassLoader(_getclassloaderMethod,
                                                      "android/content/Context", "getClassLoader",
                                                      "()Ljava/lang/ClassLoader;")) {
        LOGD("setClassLoaderFrom getMethodInfo_DefaultClassLoader return false");
        return false;
    }

    jobject _c = TXCJNIUtil::getEnv()->CallObjectMethod(activityInstance,
                                                        _getclassloaderMethod.methodID);

    if (nullptr == _c) {
        LOGD("setClassLoaderFrom CallObjectMethod return  false");
        return false;
    }

    TXCJniMethodInfo _m;
    if (!TXCJNIUtil::getMethodInfo_DefaultClassLoader(_m,
                                                      "java/lang/ClassLoader", "loadClass",
                                                      "(Ljava/lang/String;)Ljava/lang/Class;")) {
        LOGD("setClassLoaderFrom getMethodInfo_DefaultClassLoader return  false");
        return false;
    }

    TXCJNIUtil::classloader = TXCJNIUtil::getEnv()->NewGlobalRef(_c);
    TXCJNIUtil::loadclassMethod_methodID = _m.methodID;

    return true;
}

bool TXCJNIUtil::getStaticMethodInfo(TXCJniMethodInfo &methodinfo, const char *className,
                                     const char *methodName, const char *paramCode) {
    if ((nullptr == className) || (nullptr == methodName)
        || (nullptr == paramCode)) {
        return false;
    }

    JNIEnv *env = TXCJNIUtil::getEnv();
    if (!env) {
        LOGE("Failed to get JNIEnv");
        return false;
    }

    jclass classID = _getClassID(className);
    if (!classID) {
        LOGE("Failed to find class %s", className);
        env->ExceptionClear();
        return false;
    }

    jmethodID methodID = env->GetStaticMethodID(classID, methodName, paramCode);
    if (!methodID) {
        LOGE("Failed to find static method id of %s", methodName);
        env->ExceptionClear();
        return false;
    }

    methodinfo.classID = classID;
    methodinfo.env = env;
    methodinfo.methodID = methodID;
    return true;
}

bool TXCJNIUtil::getMethodInfo(TXCJniMethodInfo &methodinfo, const char *className,
                               const char *methodName, const char *paramCode) {
    if ((nullptr == className) || (nullptr == methodName)
        || (nullptr == paramCode)) {
        return false;
    }

    JNIEnv *env = TXCJNIUtil::getEnv();
    if (!env) {
        return false;
    }

    jclass classID = _getClassID(className);
    if (!classID) {
        LOGE("Failed to find class %s", className);
        env->ExceptionClear();
        return false;
    }

    jmethodID methodID = env->GetMethodID(classID, methodName, paramCode);
    if (!methodID) {
        LOGE("Failed to find method id of %s", methodName);
        env->ExceptionClear();
        return false;
    }

    methodinfo.classID = classID;
    methodinfo.env = env;
    methodinfo.methodID = methodID;

    return true;
}

std::string TXCJNIUtil::jstring2string(jstring str) {
    if (str == nullptr) {
        return "";
    }

    JNIEnv *env = TXCJNIUtil::getEnv();
    if (!env) {
        return nullptr;
    }

    const char *chars = env->GetStringUTFChars(str, nullptr);
    std::string ret(chars);
    env->ReleaseStringUTFChars(str, chars);

    return ret;
}

JNIEnv *TXCJNIUtil::cacheEnv(JavaVM *jvm) {
    JNIEnv *_env = nullptr;
    // get jni environment
    jint ret = jvm->GetEnv((void **) &_env, JNI_VERSION_1_6);

    switch (ret) {
        case JNI_OK:
            // Success!
            LOGD("Success cacheEnv JNI_OK");
            pthread_setspecific(txg_jni_key, _env);
            return _env;

        case JNI_EDETACHED:
            // Thread not attached
            if (jvm->AttachCurrentThread(&_env, nullptr) < 0) {
                LOGE("Failed to get the environment using AttachCurrentThread()");

                return nullptr;
            } else {
                // Success : Attached and obtained JNIEnv!
                LOGD("Success cacheEnv JNI_EDETACHED");
                pthread_setspecific(txg_jni_key, _env);
                return _env;
            }

        case JNI_EVERSION:
            // Cannot recover from this error
            LOGE("JNI interface version 1.4 not supported");
        default:
            LOGE("Failed to get the environment using GetEnv()");
            return nullptr;
    }
}

bool
TXCJNIUtil::getMethodInfo_DefaultClassLoader(TXCJniMethodInfo &methodinfo, const char *className,
                                             const char *methodName, const char *paramCode) {
    if ((nullptr == className) || (nullptr == methodName)
        || (nullptr == paramCode)) {
        return false;
    }

    JNIEnv *env = TXCJNIUtil::getEnv();
    if (!env) {
        return false;
    }

    jclass classID = env->FindClass(className);
    if (!classID) {
        LOGE("Failed to find class %s", className);
        env->ExceptionClear();
        return false;
    }

    jmethodID methodID = env->GetMethodID(classID, methodName, paramCode);
    if (!methodID) {
        LOGE("Failed to find method id of %s", methodName);
        env->ExceptionClear();
        return false;
    }

    methodinfo.classID = classID;
    methodinfo.env = env;
    methodinfo.methodID = methodID;

    return true;
}

TXCUTF8JstringHelper::TXCUTF8JstringHelper(JNIEnv *env, const char *srcStr) {
    mJNIEnv = env;
    std::string utfStr;
    if (srcStr == NULL) {
        utfStr = "";
    } else {
        utfStr = srcStr;
        const char *errorKind = NULL;
        checkUtfBytes(srcStr, &errorKind);
        if (errorKind != NULL) {
            utfStr = "";
        }
    }
    mUTF8Jstring = mJNIEnv->NewStringUTF(utfStr.c_str());
}

TXCUTF8JstringHelper::~TXCUTF8JstringHelper() {
    mJNIEnv->DeleteLocalRef(mUTF8Jstring);
}

jstring TXCUTF8JstringHelper::getUTF8Jstring() {
    return mUTF8Jstring;
}