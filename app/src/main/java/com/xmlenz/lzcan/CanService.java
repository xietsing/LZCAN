package com.xmlenz.lzcan;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

import java.util.List;


////Service 接口共享给其他应用
public class CanService extends Service {

    private ICANConnect.Stub icanConnect =new ICANConnect.Stub() {
        @Override
        public void basicTypes(int anInt, long aLong, boolean aBoolean, float aFloat, double aDouble, String aString) throws RemoteException {

        }

        @Override
        public int openCan(int iCan,int nBaudrate) throws RemoteException {
            Log.d("==##############","openCan");
            return 123456;
        }


        //关闭Can
        @Override
        public int closeCan(int iCan) throws RemoteException {
            return 1;
        }

        //蜂鸣
        @Override
        public int iBeep() throws RemoteException {
            return 1;
        }

        @Override
        public List<CanInfo> queryQueueCanInfo(List<String> canidList) throws RemoteException {
//            Log.d("==##############","queryQueueCanInfo");
            return UtilityCommon.queryQueueCanInfo(canidList);
        }

        @Override
        public int sendCmdCanInfos(List<CanInfo> canInfos) throws RemoteException {
            UtilityCommon.sendCmdCanInfos(canInfos);
            return 0;
        }

        @Override
        public int sendCmdCanInfo(CanInfo canInfo) throws RemoteException {
            UtilityCommon.sendCmdCanInfo(canInfo);
            return 0;
        }


        //关机
        @Override
        public int ShutDown() throws RemoteException {
            return 1;
        }

        @Override
        public int HeartBeat() throws RemoteException {
            return 1;
        }

        //软件版本
        @Override
        public int GetSoftVersion() throws RemoteException {
            return 1;
        }

        //升级
        @Override
        public int updateMCU() throws RemoteException {
            return 1;
        }



        @Override
        public IBinder asBinder() {
            return null;
        }
    };

    public CanService() {
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
//        throw new UnsupportedOperationException("Not yet implemented");

        System.out.println("执行了onBind()");
        return icanConnect;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        System.out.println("执行了onDestory()");
    }


    @Override
    public boolean onUnbind(Intent intent) {
        System.out.println("执行了onUnbind()");
        return super.onUnbind(intent);
    }

}
