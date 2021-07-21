package com.xmlenz.lzcan;

public class CanInfoRaw {

    private int canID=0;//canID
    private int canIndex=0;//0：can1 ,	1： can2
    private int nPack=0;//pack数量
    private byte[] bContent ; //包内容

    public CanInfoRaw(int canID, int canIndex, int nPack, byte[] bContent) {
        this.canID = canID;
        this.canIndex = canIndex;
        this.nPack = nPack;
        this.bContent = bContent;
    }

    public int getCanID() {
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

    public int getnPack() {
        return nPack;
    }

    public void setnPack(int nPack) {
        this.nPack = nPack;
    }

    public byte[] getbContent() {
        return bContent;
    }

    public void setbContent(byte[] bContent) {
        this.bContent = bContent;
    }
}
