package com.xmlenz.lzcan;

import android.os.Parcel;
import android.os.Parcelable;

public class CanInfo implements Parcelable {
    private int canID;//canID
    private String scanID;//canID
    private int canIndex;//0：can1 ,	1： can2
    private int canType;//0标准帧，1扩展帧
    private byte[] ucCanData=new byte[8]; //can数据

    protected CanInfo(Parcel in) {
        canID = in.readInt();
        scanID = in.readString();
        canIndex = in.readInt();
        canType = in.readInt();
        ucCanData = in.createByteArray();
    }
    public  CanInfo(){

    }

    public static final Creator<CanInfo> CREATOR = new Creator<CanInfo>() {
        @Override
        public CanInfo createFromParcel(Parcel in) {
            return new CanInfo(in);
        }

        @Override
        public CanInfo[] newArray(int size) {
            return new CanInfo[size];
        }
    };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel parcel, int i) {
        parcel.writeInt(canID);
        parcel.writeString(scanID);
        parcel.writeInt(canIndex);
        parcel.writeInt(canType);
        parcel.writeByteArray(ucCanData);
    }


    public void readFromParcel(Parcel reply) {
        canID = reply.readInt();
        scanID = reply.readString();
        canIndex = reply.readInt();
        reply.readByteArray(ucCanData);
    }

    public int getCanID(int canid) {
        return canID;
    }

    public void setCanID(int canID) {
        this.canID = canID;
    }

    public int getCanIndex() {
        return canIndex;
    }

    public void setCanIndex(int canIndex) {
        this.canIndex = canIndex;
    }

    public int getCanType() {
        return canType;
    }

    public void setCanType(int canType) {
        this.canType = canType;
    }

    public byte[] getUcCanData() {
        return ucCanData;
    }

    public void setUcCanData(byte[] ucCanData) {
        this.ucCanData = ucCanData;
    }

    public String getScanID() {
        return scanID;
    }

    public void setScanID(String scanID) {
        this.scanID = scanID;
    }


}




