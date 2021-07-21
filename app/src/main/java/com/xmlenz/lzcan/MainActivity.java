package com.xmlenz.lzcan;

import androidx.appcompat.app.AppCompatActivity;
import androidx.databinding.DataBindingUtil;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;
import android.widget.EditText;
import android.widget.TextView;

import com.xmlenz.lzcan.databinding.ActivityMainBinding;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.StringTokenizer;

import com.xmlenz.lzcan.UtilityCommon;

public class MainActivity extends AppCompatActivity {


    private TextView can1V;
    private TextView can2V;
    private TextView can3V;
    private EditText editText;
    public   LZCan lzCan;
    private int count = 0;
    private int can1count = 0;
    private Handler mHandler = new Handler();
    //=========定义 CanInfo 队列

    private CanPackagesNum canPackagesNum = new CanPackagesNum("0");
    private  String can1number;
    private  ActivityMainBinding binding;

//    public   Queue<CanInfo> queueCanInfo = new LinkedList<CanInfo>();

    UtilityCommon utilityCommon;


    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("wccan");
    }
    private TextWatcher textWatcher1 = new TextWatcher() {

        @Override
        public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {
        }
        @Override
        public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
        }
        @Override
        public void afterTextChanged(Editable editable) {
        }
    };

    public static final int UPDATE = 0x1;
    private Handler handler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what){
                case UPDATE:
                    if(editText.getLineCount()>100)
//                        editText.
                        editText.setText("");
                    editText.getText().append(( String )msg.obj);
//                    editText.setText(( String )msg.obj );
                    break;
            }
        }
    };





    public void setUSBHost()
    {
        try{
            /* Missing read/write permission, trying to chmod the file */
            Process su;
//            su =Runtime.getRuntime().exec("echo host >/sys/class/usb_role/ci_hdrc.1-role-switch/device/role");
//            String cmd="echo host >/sys/class/usb_role/ci_hdrc.1-role-switch/device/role";
//            su.getOutputStream().write(cmd.getBytes());

        } catch (Exception e) {
            e.printStackTrace();
            throw new SecurityException();
        }

    }



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        this.requestWindowFeature(Window.FEATURE_NO_TITLE);


        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);



        binding = DataBindingUtil.setContentView(this, R.layout.activity_main);
        // Example of a call to a native method
        editText = findViewById(R.id.editText);
        editText.setKeyListener(null);//不可编辑
        can1V = findViewById(R.id.CanView1);
        can2V = findViewById(R.id.CanView2);
        can3V = findViewById(R.id.CanView3);


        setUSBHost();


        try {
            lzCan = new LZCan("/dev/ttyUSB4",1500000, 0);
            if(lzCan.getInputStream()==null)
                Log.d("No file","No file");

        } catch (IOException e) {
            e.printStackTrace();
        }



        //计算总消息数量
        try {
            startCalculate();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

    }


    @Override
    public void finish() {
        lzCan.closeCan(0);
        Log.d("Close","Close");
        super.onDestroy();
    }



    private Runnable runnable = new Runnable() {
        @Override
        public void run() {
            int size1=0;
            try {
                CanInfoRaw canInfoRaw[] = lzCan.getCanRawArray(0);
                if(canInfoRaw==null) return;
                size1 =  canInfoRaw.length;
                for(int i=0;i<size1;i++){
                    int nPack = canInfoRaw[i].getnPack();
                    byte bData[] = canInfoRaw[i].getbContent();
                    for(int j=0;j<nPack;)
                    {

                        int canid = bData[j+3]*256*256*256 + bData[j+2]*256*256 + bData[j+1]*256 +bData[j];
                        byte bCanid[] = new byte[4];
                        bCanid[0] = bData[j+3] ;
                        bCanid[1] = bData[j+2];
                        bCanid[2] = bData[j+1];
                        bCanid[3] = bData[j+0];
                        String sCanid = utilityCommon.bytesToHex(bCanid);
                        int canidex = bData[j+4];
                        int cantype = bData[j+5];
                        byte bDataTemp[] = new byte[8];
                        System.arraycopy(bData, 6,bDataTemp,0,8);
                        j=j+14;
                        CanInfo canInfo =new CanInfo();
                        canInfo.setCanIndex(canidex);
                        canInfo.setScanID(sCanid);
                        canInfo.setCanID(canid);
                        canInfo.setCanType(cantype);
                        canInfo.setUcCanData(bDataTemp);
                        String lineStr = "canid="+sCanid+"  data="+ utilityCommon.bytesToHex(bDataTemp)+  "       queue size = "+utilityCommon.queueCanInfo.size()+"\r\n";
                        Log.d("========%s",lineStr);
                        Message msg = new Message();
                        msg.what = UPDATE;
                        msg.obj = lineStr;
                        handler.sendMessage(msg);
                        UtilityCommon.queueCanInfo.offer(canInfo);
                        if(UtilityCommon.queueCanInfo.size()> 500)
//                            queueCanInfo.peek(); //取不删
                            UtilityCommon.queueCanInfo.poll();//移除第一个
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            }

            count ++;
            can1number = String.valueOf(count);
            canPackagesNum.setCanName("Can1:");
            can1count += size1;
            if(can1count > 200000000||count> 200000000)
            {
                can1count = 0;
                count=0;
            }
            canPackagesNum.setPackNumber1(String.valueOf(can1count));
            canPackagesNum.setPackNumber2(can1number);
            canPackagesNum.setPackNumber3(can1number);
            binding.setCan(canPackagesNum);
            try {
                Thread.sleep(10);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            mHandler.postDelayed(runnable,100); // 执行后延迟1000毫秒再次执行，count已++
        }
    };

    private void startCalculate() throws InterruptedException {
        mHandler.post(runnable);
    }


}
