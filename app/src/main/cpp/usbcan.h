//
// Created by xq on 2021/7/7.
//
#include <jni.h>

#ifndef LZCAN_USBCAN_H
#define LZCAN_USBCAN_H


extern "C" JNIEXPORT jstring JNICALL Java_com_xmlenz_lzcan_LZCan_stringFromJNI(JNIEnv *, jobject);

void toCanInfo();
void hex_str_to_byte(char *hex_str, int length, unsigned char *result);
void array2hex(uint8_t* in, int inlen, uint8_t* out);

#endif //LZCAN_USBCAN_H
