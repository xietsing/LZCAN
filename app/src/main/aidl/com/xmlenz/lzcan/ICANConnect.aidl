// ICANConnect.aidl
package com.xmlenz.lzcan;

// Declare any non-default types here with import statements
parcelable CanInfo;

interface ICANConnect {
    /**
     * Demonstrates some basic types that you can use as parameters
     * and return values in AIDL.
     */
    void basicTypes(int anInt, long aLong, boolean aBoolean, float aFloat,
            double aDouble, String aString);

    //打开Can
    int openCan(int iCan,int nBaudrate);

    //关闭Can
    int closeCan(int iCan);

    //蜂鸣
    int iBeep();

    //========查询数据===
    List<CanInfo> queryQueueCanInfo(in List<String> canidList);


    //========下发数据===
    int sendCmdCanInfo(in CanInfo canInfo);

    //========下发数据多条===
    int sendCmdCanInfos(in List<CanInfo> canInfos);

    //关机
    int ShutDown();

    //心跳
    int HeartBeat();

    //软件版本
    int GetSoftVersion();

    //升级
    int updateMCU();
}
