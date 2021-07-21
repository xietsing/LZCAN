package com.xmlenz.lzcan;

import android.util.Log;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

public  class UtilityCommon {



    public static Queue<CanInfo> queueCanInfo = new LinkedList<CanInfo>();

    public static List<CanInfo> queryQueueCanInfo(List<String> canidList)
    {
        List<CanInfo> canList = new ArrayList<CanInfo>();

//        if(canidList.size()>0 )
//            if(canidList.get(0)!=null)
//                Log.d("==##############",canidList.get(0));
        for(CanInfo ci :UtilityCommon.queueCanInfo)
        {
            for(String str: canidList) {
                if (ci.getScanID().equals(str)) {
                    canList.add(ci);
                    Log.d("==##############",ci.getScanID());
                }
            }
        }
        return canList;
    }


    //====================发送CAN指令
    public static int sendCmdCanInfo(CanInfo canInfo)
    {


        return 0;
    }

    //====================发送CAN指令
    public static int sendCmdCanInfos(List<CanInfo> canInfos)
    {


        return 0;
    }

    /**
     * 字节转十六进制
     * @param b 需要进行转换的byte字节
     * @return  转换后的Hex字符串
     */
    public static String byteToHex(byte b){
        String hex = Integer.toHexString(b & 0xFF);
        if(hex.length() < 2){
            hex = "0" + hex;
        }
        return hex;
    }

    /**
     * 字节数组转16进制
     * @param bytes 需要转换的byte数组
     * @return 转换后的Hex字符串
     */
    public static String bytesToHex(byte[] bytes) {
        StringBuffer sb = new StringBuffer();
        for (int i = 0; i < bytes.length; i++) {
            String hex = Integer.toHexString(bytes[i] & 0xFF);
            if (hex.length() < 2) {
                sb.append(0);
            }
            sb.append(hex);
        }
        return sb.toString();
    }

    /**
     * Hex字符串转byte
     *
     * @param inHex 待转换的Hex字符串
     * @return 转换后的byte
     */
    public static byte hexToByte(String inHex) {
        return (byte) Integer.parseInt(inHex, 16);
    }

    /**
     * hex字符串转byte数组
     *
     * @param inHex 待转换的Hex字符串
     * @return 转换后的byte数组结果
     */
    public static byte[] hexToByteArray(String inHex) {
        int hexlen = inHex.length();
        byte[] result;
        if (hexlen % 2 == 1) {
            //奇数
            hexlen++;
            result = new byte[(hexlen / 2)];
            inHex = "0" + inHex;
        } else {
            //偶数
            result = new byte[(hexlen / 2)];
        }
        int j = 0;
        for (int i = 0; i < hexlen; i += 2) {
            result[j] = hexToByte(inHex.substring(i, i + 2));
            j++;
        }
        return result;
    }

}
