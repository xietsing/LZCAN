/*
 * Copyright 2009-2011 Cedric Priscal
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <jni.h>

#include     <errno.h>
#include <pthread.h>

#include "SerialPort.h"

#include "android/log.h"
#include "usbcan.h"

static const char *TAG="serial_port";
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)





static speed_t getBaudrate(jint baudrate)
{
	switch(baudrate) {
	case 0: return B0;
	case 50: return B50;
	case 75: return B75;
	case 110: return B110;
	case 134: return B134;
	case 150: return B150;
	case 200: return B200;
	case 300: return B300;
	case 600: return B600;
	case 1200: return B1200;
	case 1800: return B1800;
	case 2400: return B2400;
	case 4800: return B4800;
	case 9600: return B9600;
	case 19200: return B19200;
	case 38400: return B38400;
	case 57600: return B57600;
	case 115200: return B115200;
	case 230400: return B230400;
	case 460800: return B460800;
	case 500000: return B500000;
	case 576000: return B576000;
	case 921600: return B921600;
	case 1000000: return B1000000;
	case 1152000: return B1152000;
	case 1500000: return B1500000;
	case 2000000: return B2000000;
	case 2500000: return B2500000;
	case 3000000: return B3000000;
	case 3500000: return B3500000;
	case 4000000: return B4000000;
	default: return -1;
	}
}

pthread_t ntid;
int fd;



char *cmdopencan1="020100030001000303";//打开CAN1
char *cmd_can_syn="02030001000203";//获取Can1
char *cmdclosecan1="02040001000503";//关闭can1
char *cmdclosecan2="02040001010403";//关闭can2
char *cmdbeep="020a000220200803";




//==========定义互斥锁
pthread_mutex_t mutex;

#define STX 						0x02	//协议头字段
#define ETX 						0x03	//协议尾字段

typedef struct _T_CAN_INFO_RAW_{
    uint8_t canIndex;				//0：can1 ,	1： can2
    uint8_t nPack;				//0：can1 ,	1： can2
    uint16_t  lenData;
    char  cData[4096];
} T_CAN_INFO_RAW;

T_CAN_INFO_RAW canRawArray[100] = {0};
int canRawArraySize =0;

typedef struct _T_CAN_INFO_ {		//存储CAN信息结构体
    uint32_t CanID;				//canID
    uint8_t CanIndex;				//0：can1 ,	1： can2
    uint8_t IDE;					//0标准帧，1扩展帧
    uint8_t ucCanData[8];		//can数据
} T_CAN_INFO;

//发送命令
void sentCMD(char * cmd)
{
    char W_BUF[50]={0};
    hex_str_to_byte(cmd,strlen(cmd),(unsigned char*)W_BUF);
    int wv = write(fd, W_BUF,strlen(cmd)/2) ;
    if(wv < 0)
    {
        LOGD("Write() error:%s\n",strerror(errno)) ;
    }
    sleep(1);
}

//发送命令
void sentCMDByte(char * cmd,int len)
{

    int wv = write(fd, cmd,len) ;
    if(wv < 0)
    {
        LOGD("Write() error:%s\n",strerror(errno)) ;
    }
    sleep(1);
}

//xor校验和
char getxor(char *strS,int len)
{
    char xorstr=0x00;
    for(int i=0;i<len-3;i++)
    {
        xorstr ^= strS[i+1];
    }
    return  xorstr;
}

void clearBuff(){

    canRawArraySize = 0;
    memset(canRawArray,0,sizeof(T_CAN_INFO_RAW)*100);
}

///检查校验返回帧
bool checkResponse(char * strResponse,int nLen,char idCMD) //
{
    if(nLen>=10)//正常的CAN
    {
        if(strResponse[0]!=STX) {
            LOGD("帧头error");
            return false; //帧头
        }
        if(strResponse[nLen-1]!=ETX)
        {
            LOGD("帧尾error");
            return false; //帧尾
        }
        if(strResponse[1]!=0xfe)
        {
            LOGD("应答error");
            return false;//应答
        }
        int nLenPack = strResponse[2]*256+strResponse[3]; //包长度
        unsigned char IDNumber = strResponse[4]; //处理的ID消息 =======
        if(IDNumber != 0x03) {
            LOGD("IDNumber=%d",IDNumber);
            return false;
        }
        int nCan =  strResponse[5];   // 第几路CAN           ===========
        int isSuccess = strResponse[6]; //是否成功
        //
        int nCanInfo =strResponse[7];  //Can包数量           ===========
//        T_CAN_INFO canInfo[64] = {0};            //CANINFO信息================
//        memcpy(canInfo,&strResponse[8],sizeof(T_CAN_INFO)*nCanInfo);
        if((nCanInfo*sizeof(T_CAN_INFO) +  4 )!=nLenPack)
        {
            LOGD("nLenPack error");
            return false;
        }
        if(nLen != nLenPack +6 )
        {
            LOGD("nLen error");
            return false;
        }
        if(strResponse[7+sizeof(T_CAN_INFO)*nCanInfo+1] !=getxor(strResponse,nLen))
        {
            LOGD("sizeof(T_CAN_INFO)*nCanInfo error");
            return false;
        }

        //=============加锁
        pthread_mutex_lock(&mutex);

        canRawArray[canRawArraySize].canIndex = nCan;
        canRawArray[canRawArraySize].nPack = nCanInfo;
        canRawArray[canRawArraySize].lenData = nCanInfo*sizeof(T_CAN_INFO);
        memcpy(canRawArray[canRawArraySize].cData,&strResponse[8],sizeof(T_CAN_INFO)*nCanInfo);
        canRawArraySize++;
        if(canRawArraySize > 100)clearBuff();

        //============解锁
        pthread_mutex_unlock(&mutex);

        return true;


    } else
    {
        LOGD("other error");
        return false;
    }


}

///读USB线程
void *thr_read(void *arg)
{
//    int fd = (int)&arg;
    int     rv = -1 ;
    fd_set rset;
    char    r_buf[1024] ;
    struct timeval timeout;
    timeout.tv_sec=2;
    timeout.tv_usec=0;
//    LOGD("Read from fd===%d",fd) ;

    //Init 初始化锁
    pthread_mutex_init(&mutex,NULL);

    while(1)
    {
        FD_ZERO(&rset) ;
        FD_SET(fd, &rset) ;

        rv = select(fd+1, &rset, NULL, NULL, &timeout) ;
        timeout.tv_sec=2;
        timeout.tv_usec=0;
        if(rv < 0)
        {
            LOGD("select() failed: %s\n", strerror(errno)) ;
            continue;
        }
        if(rv == 0)
        {
            LOGD("select() time out!\n") ;
            sentCMD(cmd_can_syn);

        }
        memset(r_buf, 0, sizeof(r_buf)) ;
        rv = read(fd, r_buf, sizeof(r_buf)) ;
        if(rv < 0)
        {
            LOGD("Read() error:%s\n",strerror(errno)) ;
            continue;
        }
        if(checkResponse(r_buf,rv,0x03)==false)
        {
            uint8_t  out[200]={0};
            array2hex((uint8_t*)r_buf,rv,out);
            LOGD("rvbuff=%s",out);
//            LOGD("checkResponse() error:%s\n",strerror(errno)) ;
            continue;
        }
//        LOGD("Read from tty: %d\n", rv) ;
    }


}
//开启线程
void openReadTh()
{
    int		err;
    //创建线程，主线程把新线程ID存放在ntid中，新线程去执行thr_fn函数
    err = pthread_create(&ntid, NULL, thr_read, NULL);
    if (err != 0){
        close(fd);
    }

}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_xmlenz_lzcan_LZCan_SerialPortOpen(JNIEnv *env, jobject thiz, jstring path, jint baudrate, jint flags) {
//    int fd;
    speed_t speed;
    jobject mFileDescriptor;



    /* Check arguments */
    {
        speed = getBaudrate(baudrate);
        if (speed == -1) {
            /* TODO: throw an exception */
            LOGE("Invalid baudrate");
            return NULL;
        }
    }

    /* Opening device */
    {
        jboolean iscopy;
        const char *path_utf = env->GetStringUTFChars( path, &iscopy);
        LOGD("Opening serial port %s with flags 0x%x", path_utf, O_RDWR | flags);
        fd = open(path_utf, O_RDWR | flags);
        LOGD("open() fd = %d", fd);
        env->ReleaseStringUTFChars( path, path_utf);
        if (fd == -1)
        {
            /* Throw an exception */
            LOGE("Cannot open port");
            /* TODO: throw an exception */
            return NULL;
        }
    }

    /* Configure device */
    {
        struct termios cfg;
        LOGD("Configuring serial port");
        if (tcgetattr(fd, &cfg))
        {
            LOGE("tcgetattr() failed");
            close(fd);
            /* TODO: throw an exception */
            return NULL;
        }

//        cfmakeraw(&cfg);
//        cfsetispeed(&cfg, speed);
//        cfsetospeed(&cfg, speed);
        cfmakeraw(&cfg);
//        cfg.c_cflag|=(CLOCAL|CREAD ); // CREAD 开启串行数据接收，CLOCAL并打开本地连接模式
//        cfg.c_cflag &=~CSIZE;// 先使用CSIZE做位屏蔽
//        cfg.c_cflag |= CS8; //设置8位数据位
//        cfg.c_cflag &= ~PARENB; //无校验位
//        /* 设置115200波特率  */

        cfsetispeed(&cfg, speed);
        cfsetospeed(&cfg, speed);

        cfg.c_cflag &= ~CSTOPB;/* 设置一位停止位; */
        cfg.c_cc[VTIME] = 0;/* 非规范模式读取时的超时时间；*/
        cfg.c_cc[VMIN]  = 0; /* 非规范模式读取时的最小字符数*/
        tcflush(fd ,TCIFLUSH);/* tcflush清空终端未完成的输入/输出请求及数据；TCIFLUSH表示清空正收到的数据，且不读取出来 */


        if (tcsetattr(fd, TCSANOW, &cfg))
        {
            LOGE("tcsetattr() failed");
            close(fd);
            /* TODO: throw an exception */
            return NULL;
        }
    }


    //=========打开读线程
    openReadTh();
    //=======打开can1
    sentCMD(cmdopencan1);


    /* Create a corresponding file descriptor */
    {
        jclass cFileDescriptor = env->FindClass( "java/io/FileDescriptor");
        jmethodID iFileDescriptor = env->GetMethodID( cFileDescriptor, "<init>", "()V");
        jfieldID descriptorID = env->GetFieldID( cFileDescriptor, "descriptor", "I");
        mFileDescriptor = env->NewObject( cFileDescriptor, iFileDescriptor);
        env->SetIntField( mFileDescriptor, descriptorID, (jint)fd);
    }

    return mFileDescriptor;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_xmlenz_lzcan_LZCan_SerialPortClose(JNIEnv * env, jobject thiz) {
    // TODO: implement SerialPortClose()
    jclass SerialPortClass = env->GetObjectClass( thiz);
    jclass FileDescriptorClass = env->FindClass( "java/io/FileDescriptor");

    jfieldID mFdID = env->GetFieldID( SerialPortClass, "mFd", "Ljava/io/FileDescriptor;");
    jfieldID descriptorID = env->GetFieldID( FileDescriptorClass, "descriptor", "I");

    jobject mFd = env->GetObjectField( thiz, mFdID);
    jint descriptor = env->GetIntField( mFd, descriptorID);

    LOGD("close(fd = %d)", descriptor);
    close(descriptor);
}


//====最多不能超过100个CANINFO数据
static char sendcmd[1406]={0};
//====定义一个100个canid的缓存
static char caninfos[1400]={0};

char* getCMDCan(int nCMD, int nCan,int size)
{
    switch (nCMD){
        case 2:
        {
            sendcmd[0] = 0x02;
            sendcmd[1] = 0x02;
            sendcmd[2] = size*sizeof(T_CAN_INFO)/256;
            sendcmd[3] = size*sizeof(T_CAN_INFO)%256;
            memcpy(&sendcmd[4],caninfos,8*size );
            char x = getxor(sendcmd,4+size*8);
            sendcmd[4+8*size] = x;
            sendcmd[5+8*size] = 0x03;
            int len = 6 + 8*size;
            sentCMDByte(sendcmd,len);
            sentCMDByte(sendcmd,len);
        }
        default:
            break;

    }
    return sendcmd;


}

extern "C"
JNIEXPORT void JNICALL
Java_com_xmlenz_lzcan_LZCan_sendCmdCan(JNIEnv *env, jobject thiz, jint n_can, jint canid,
                                       jbyteArray data) {

    memset(caninfos,0,1400);
    int canindex = n_can;
    jbyte *bytes = env->GetByteArrayElements(data, 0);
    memcpy(caninfos, bytes, 8);

    getCMDCan(2,canindex,1);
    env->ReleaseByteArrayElements(data, bytes, 0);


}

extern "C"
JNIEXPORT void JNICALL
Java_com_xmlenz_lzcan_LZCan_sendCmdCans(JNIEnv *env, jobject thiz, jint n_can, jint canid,
                                        jint cmd_number, jbyteArray data) {

    memset(caninfos,0,1400);
    int canindex = n_can;
    int number = cmd_number;
    jbyte *bytes = env->GetByteArrayElements(data, 0);
    memcpy(caninfos, bytes, 8*number);
    getCMDCan(2,canindex,number);

    env->ReleaseByteArrayElements(data, bytes, 0);

}



extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_xmlenz_lzcan_LZCan_getCanRawArray(JNIEnv *env, jobject thiz, jint n_can) {

//    jclass jclassResultInfo = env->FindClass("com/xmlenz/lzcan/CanInfoRaw"); // 获取类 引用
//    //(int canID, int canIndex, int nPack, byte[] bContent)
//    jmethodID jidResultInfo =
//            env->GetMethodID(jclassResultInfo, "<init>", "(III[B)V"); // 获取类的方法，此方法是构造函数
//    // "(ILjava/lang/String;[B)V" 解释一下这个字符串，V代表返回空类型， ()号里面代表三个参数， 分解成三个分别是(我用空格隔开)
//    // I   Ljava/lang/String;  [B   分别代表整数，字符string,和byte[] 自己体会

    jobjectArray infos = NULL;	// jobjectArray 为指针类型
    jclass clsCanInfo = NULL;		// jclass 为指针类型
    jobject obj;
    jfieldID canID;
    jfieldID canIndex;
    jfieldID nPack;
    jfieldID bContent;
    jmethodID consID;
    jsize len;
    int i;

    //=============加锁
    pthread_mutex_lock(&mutex);

    clsCanInfo = env->FindClass("com/xmlenz/lzcan/CanInfoRaw");
    len = canRawArraySize;
    infos = env->NewObjectArray(len, clsCanInfo, NULL);
    canID = env->GetFieldID(clsCanInfo, "canID", "I");
    canIndex = env->GetFieldID(clsCanInfo, "canIndex", "I");
    nPack = env->GetFieldID(clsCanInfo, "nPack", "I");
    bContent = env->GetFieldID(clsCanInfo, "bContent", "[B");
    consID = env->GetMethodID(clsCanInfo, "<init>", "()V");



//    LOGD("canRawArraySize = %d",len);
    for(i = 0; i < len; i++)
    {
        obj = env->NewObject(clsCanInfo, consID);
        env->SetIntField(obj, canID, (jint)0);
        env->SetIntField(obj, canIndex, (jint) canRawArray[i].canIndex);
        env->SetIntField(obj, nPack, (jint) canRawArray[i].nPack);
        jbyteArray byteA = env->NewByteArray(canRawArray[i].lenData);
        env->SetByteArrayRegion(byteA, 0, canRawArray[i].lenData, (const jbyte*)canRawArray[i].cData);

        env->SetObjectField(obj, bContent, (jbyteArray) byteA);
        env->SetObjectArrayElement(infos, i, obj);
        env->DeleteLocalRef(byteA);  // 中间变量需要释放
    }
    //清空缓存
    clearBuff();
    //============解锁
    pthread_mutex_unlock(&mutex);
    return infos;




}

extern "C"
JNIEXPORT void JNICALL
Java_com_xmlenz_lzcan_LZCan_beep(JNIEnv *env, jobject thiz) {

    sentCMD(cmdbeep);
//    sentCMD(cmdclosecan2);
//    sentCMD(cmdclosecan3);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_xmlenz_lzcan_LZCan_closeCan(JNIEnv *env, jobject thiz, jint n_can) {

    sentCMD(cmdclosecan1);
    sentCMD(cmdclosecan2);
//    sentCMD(cmdclosecan3);
}

