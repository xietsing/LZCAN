#include <jni.h>
#include <string>
#include "usbcan.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_xmlenz_lzcan_LZCan_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

void toCanInfo(){
//    while(1)
//    {
//        FD_ZERO(&rset) ;
//        FD_SET(tty_fd, &rset) ;
//        rv = select(tty_fd+1, &rset, NULL, NULL, NULL) ;
//        if(rv < 0)
//        {
//            printf("select() failed: %s\n", strerror(errno)) ;
//            goto cleanup ;
//        }
//        if(rv == 0)
//        {
//            printf("select() time out!\n") ;
//            goto cleanup ;
//        }
//        memset(r_buf, 0, sizeof(r_buf)) ;
//        rv = read(tty_fd, r_buf, sizeof(r_buf)) ;
//        if(rv < 0)
//        {
//            printf("Read() error:%s\n",strerror(errno)) ;
//            goto cleanup ;
//        }
//        printf("Read from tty: %s\n",r_buf) ;
//    }
//    cleanup:
//
//    close(tty_fd) ;

}


void hex_str_to_byte(char *hex_str, int length, unsigned char *result)
{
    char h, l;
    for(int i = 0; i < length/2; i++)
    {
        if(*hex_str < 58)
        {
            h = *hex_str - 48;
        }
        else if(*hex_str < 71)
        {
            h = *hex_str - 55;
        }
        else
        {
            h = *hex_str - 87;
        }
        hex_str++;
        if(*hex_str < 58)
        {
            l = *hex_str - 48;
        }
        else if(*hex_str < 71)
        {
            l = *hex_str - 55;
        }
        else
        {
            l = *hex_str - 87;
        }
        hex_str++;
        *result++ = h<<4|l;
    }
 }


uint8_t hex_lookup[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                  '8','9', 'a', 'b', 'c', 'd', 'e', 'f' };
void array2hex(uint8_t* in, int inlen, uint8_t* out)
{
    int i;
    for (i = 0; i < inlen; ++i)
    {
        out[2 * i] = hex_lookup[in[i] >> 4];
        out[2 * i + 1] = hex_lookup[in[i] & 0x0f];
    }
}
void hex2array(uint8_t* in, int inlen, uint8_t* out)
{
    int i;
    unsigned char r;
    for (i = 0; i < inlen; i += 2)
    {
        r = in[i] - '0';
        if (r > 9) r += '0' + 10 - 'a';
        //printf("%c(%x): %x\n", in[i], in[i], r);
        out[i / 2] = r << 4;
        r = in[i + 1] - '0';
        if (r > 9) r += '0' + 10 - 'a';
        out[i / 2] += r;

        //printf("%c(%x): %x <%x>\n", in[i+1], in[i+1], r, out[i/2]);
    }
}
