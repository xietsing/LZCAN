package com.xmlenz.lzcan;

import android.util.Log;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class LZCan {

    private static final String TAG = "SerialPort";

    /*
     * Do not remove or rename the field mFd: it is used by native method close();
     */
    private FileDescriptor mFd;
    private FileInputStream mFileInputStream;
    private FileOutputStream mFileOutputStream;


    public LZCan(String strpath0, int baudrate, int flags) throws SecurityException, IOException {

        /* Check access permission */
        File device = new File(strpath0);

        if(!device.exists())
            return;
        mFd = SerialPortOpen(device.getAbsolutePath(), baudrate, flags);
        if (mFd == null) {
            Log.e(TAG, "native open returns null");
            throw new IOException();
        }
        mFileInputStream = new FileInputStream(mFd);
        mFileOutputStream = new FileOutputStream(mFd);
    }

    // Getters and setters
    public InputStream getInputStream() {
        return mFileInputStream;
    }

    public OutputStream getOutputStream() {
        return mFileOutputStream;
    }




    // JNI
    static {
        System.loadLibrary("wccan");
    }

    public native FileDescriptor  SerialPortOpen(String path, int baudrate, int flags);
    public native CanInfoRaw[] getCanRawArray(int nCan);
    public native void closeCan(int nCan);
    public native void beep();

    public native void sendCmdCan(int nCan ,int canid, byte[] data);
    public native void sendCmdCans(int nCan ,int canid,int cmdNumber, byte[] data);

}
