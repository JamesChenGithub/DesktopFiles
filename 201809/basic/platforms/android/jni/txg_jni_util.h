#ifndef TXCJNIUTIL_H
#define TXCJNIUTIL_H

#include <jni.h>
#include <android/log.h>
#include <string>

#ifdef JNI_VERSION_1_4
#define JNI_VER JNI_VERSION_1_4
#endif

#ifdef JNI_VERSION_1_5
#undef JNI_VER
#define JNI_VER JNI_VERSION_1_5
#endif
#ifdef JNI_VERSION_1_6
#undef JNI_VER
#define JNI_VER JNI_VERSION_1_6
#endif

typedef struct TXCJniMethodInfo_ {
    JNIEnv *env;
    jclass classID;
    jmethodID methodID;
} TXCJniMethodInfo;

class TXCJNIUtil {
public:

    static jmethodID getMethodByName(jclass pCls, const char *pMethodName, const char *pSignature);

    static jmethodID
    getStaticMethodByName(jclass pCls, const char *pMethodName, const char *pSignature);

    static jmethodID getMethodByObj(jobject pObj, const char *pMethodName, const char *pSignature);

    static jmethodID
    getStaticMethodByObj(jobject pObj, const char *pMethodName, const char *pSignature);

    static void CheckUTF8(const char *sZtr);

    static void
    SetJavaObjStringField(JNIEnv *env, jclass pCls, jobject pObj, const char *pFieldName,
                          const char *pValue);

    static void setJavaVM(JavaVM *javaVM);

    static JavaVM *getJavaVM();

    static JNIEnv *getEnv();

    static bool setClassLoaderFrom(jobject activityInstance);

    static bool getStaticMethodInfo(TXCJniMethodInfo &methodinfo,
                                    const char *className, const char *methodName,
                                    const char *paramCode);

    static bool getMethodInfo(TXCJniMethodInfo &methodinfo, const char *className,
                              const char *methodName, const char *paramCode);

    static std::string jstring2string(jstring str);

    static jmethodID loadclassMethod_methodID;
    static jobject classloader;
private:
    static JNIEnv *cacheEnv(JavaVM *jvm);

    static bool getMethodInfo_DefaultClassLoader(TXCJniMethodInfo &methodinfo,
                                                 const char *className, const char *methodName,
                                                 const char *paramCode);

    static JavaVM *_psJavaVM;
};

class TXCUTF8JstringHelper {
public:
    TXCUTF8JstringHelper(JNIEnv *env, const char *srcStr);

    jstring getUTF8Jstring();

    ~TXCUTF8JstringHelper();

private:
    JNIEnv *mJNIEnv;
    jstring mUTF8Jstring;
};

#endif	//TXCJNIUTIL_H
