package net.ddns.koshka.witnettest004;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Handler;
import android.support.annotation.IntegerRes;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ArrayBlockingQueue;


public class MainActivity extends AppCompatActivity {

    TextView tv, buf_sz_tv, last_pkt_ts_tv, queue_timeshift_tv;
    Button rbtn, exitbtn, cfgbtn, txtclrbtn, test1btn, test2btn, test3btn, test4btn, test5btn, test6btn, test7btn, test8btn, test9btn;
    Handler h;
    DiagRevealerControl DRC;
    GuiMessenger GuiMsngr;
    CfgMessage[] cfgMsgList;
    Boolean readyToRun = false;
    ArrayBlockingQueue<DiagDataPacket> diagLogBuffer;
    private List<String> msgLog = new ArrayList<String>();
    private static final int MAX_LOG_LINES = 500;
    enum ChipsetType { QUALCOMM, MTK, UNKNOWN};
    int textLogMessagesNum = 0;

    @SuppressLint("HandlerLeak")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

// initializing GUI elements
        tv = (TextView) findViewById(R.id.sample_text);

        tv.setTextIsSelectable(true);
        tv.setFocusable(true);
        //tv.setFocusableInTouchMode(true);
        buf_sz_tv = (TextView) findViewById(R.id.textView6);
        last_pkt_ts_tv = (TextView) findViewById(R.id.textView4);
        queue_timeshift_tv = (TextView) findViewById(R.id.textViewX);
        //tv.setMovementMethod(new ScrollingMovementMethod());
        rbtn        = (Button)findViewById(R.id.button);
        exitbtn     = (Button)findViewById(R.id.button2);
        cfgbtn      = (Button)findViewById(R.id.button3);
        txtclrbtn   = (Button)findViewById(R.id.button4);
        test1btn     = (Button)findViewById(R.id.button5);
        test2btn     = (Button)findViewById(R.id.button7);
        test3btn     = (Button)findViewById(R.id.button8);
        test4btn     = (Button)findViewById(R.id.button9);
        test5btn     = (Button)findViewById(R.id.button10);
        test6btn     = (Button)findViewById(R.id.button11);
        test7btn     = (Button)findViewById(R.id.button12);
        test8btn     = (Button)findViewById(R.id.button13);
        test9btn     = (Button)findViewById(R.id.button14);
        rbtn.setEnabled(false);

// preparing OS queue handler to change GUI elements from other threads
        h = new Handler(){
            @Override
            public void handleMessage(android.os.Message msg) {
                switch(msg.what) {
                    case 0: // write to log
                        //if(++textLogMessagesNum > 10){
                            //tv.setText("");
                            //textLogMessagesNum = 0;
                        //}
                        tv.append(msg.obj + "\n");
                        break;
                    case 1: // change button text
                        rbtn.setText((CharSequence) msg.obj);
                        break;
                    case 2: // buffer capacity
                        buf_sz_tv.setText((CharSequence) msg.obj);
                        break;
                    case 3: // timestamp of the last packet taken from fifo
                        last_pkt_ts_tv.setText((CharSequence) msg.obj);
                        break;
                    case 4: // delay of current packet in queue
                        queue_timeshift_tv.setText((CharSequence) msg.obj);
                        break;
                }
            };
        };
        if(_init_sequence()){

            rbtn.setEnabled(true);
            readyToRun = true;
// initializing singleton which will deliver messages to the Handler from other threads
            GuiMsngr = GuiMessenger.getInstance();
            GuiMsngr.init(h);

// initializing FIFO buffer for DIAG data
            diagLogBuffer = new ArrayBlockingQueue<DiagDataPacket>(65535,true);

// initializing class that will directly interact with native part of the project
            DRC = new DiagRevealerControl(diagLogBuffer, getApplicationContext().getApplicationInfo().dataDir+"/Diag.cfg");

// initializing the list of all diagnostic messages that can be requested via Diag.cgf
            cfgMsgList = new CfgMessage[DRC.getKnownMessageTypes().length];
            int i = 0;
            for(Object mt : DRC.getKnownMessageTypes()){

                    String msgTypeName = (String) ((Object[]) mt)[0];
                    Integer msgTypeId = (Integer) ((Object[]) mt)[1];

                    cfgMsgList[i] = new CfgMessage(msgTypeName, msgTypeId, false);

                    i++;

            }

// initializing test FIFO consumer
            TestLogElement testlog = new TestLogElement(diagLogBuffer, cfgMsgList, DRC);
            testlog.runTest();
            testlog.updateBufStats();

            tv.append("Init sequence completed successfully. \n");

        }

// CLR button OnClik listener
        txtclrbtn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                tv.setText("");
            }
        });

// RUN/STOP button OnClik listener
        rbtn.setOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    if(!readyToRun){
                        tv.append("Init sequence was not completed. \n");
                        tv.append("Unable to continue. \n");
                        return;
                    }


                    if(DRC.getRevealerRunStatus()){
                        tv.append("Stopping DIAG communication \n");
                        DRC.stopRevealer();
                    }else{
                        tv.append("Starting DIAG communication \n");
                        DRC.runRevealer();
                    }

            }
        });

// TEST button OnClick listener
        test1btn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
               DRC.ws_test((byte)1);
            }
        });
// TEST button OnClick listener
        test2btn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                DRC.ws_test((byte)2);
            }
        });
        // TEST button OnClick listener
        test3btn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                DRC.ws_test((byte)3);
            }
        });
        // TEST button OnClick listener
        test4btn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                DRC.ws_test((byte)4);
            }
        });
        // TEST button OnClick listener
        test5btn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                DRC.ws_test((byte)5);
            }
        });
        // TEST button OnClick listener
        test6btn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                DRC.ws_test((byte)6);
            }
        });
        // TEST button OnClick listener
        test7btn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                DRC.ws_test((byte)7);
            }
        });
        // TEST button OnClick listener
        test8btn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                DRC.ws_test((byte)8);
            }
        });
        // TEST button OnClick listener
        test9btn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                DRC.ws_test((byte)9);
            }
        });
// EXIT button OnClick listener
        exitbtn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                if(DRC.getRevealerRunStatus()) {
                    DRC.stopRevealer();
                }
                finishAffinity();
                System.exit(0);
            }
        });

// CONFIG (create Diag.cfg) button OnClick listener
        cfgbtn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
                String[] knownMsgTypes = new String[cfgMsgList.length];
                boolean[] checkedKnownMsgTypes = new boolean[ cfgMsgList.length];

                for (int i=0; i<cfgMsgList.length; i++) {
                    knownMsgTypes[i]=cfgMsgList[i].getTypeName();
                    checkedKnownMsgTypes[i]=cfgMsgList[i].isEbabled();
                }


                builder.setMultiChoiceItems(knownMsgTypes, checkedKnownMsgTypes, new DialogInterface.OnMultiChoiceClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which, boolean isChecked) {
                        // Update the current focused item's checked status
                        cfgMsgList[which].enable(isChecked);
                    }
                });

                builder.setTitle("Select message types");

                builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                          ArrayList<String> enabledMessageTypesList = new ArrayList<>();

                        for (int i=0; i<cfgMsgList.length; i++) {
                            if(cfgMsgList[i].isEbabled()) enabledMessageTypesList.add(cfgMsgList[i].getTypeName());
                        }
                        DRC.writeNewDiagCfg(enabledMessageTypesList.toArray(new String[0]));
                    }
                });

                builder.setNeutralButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        // Do something when click the neutral button
                        tv.append("Message types dialog cancelled. \n");
                    }
                });

                AlertDialog dialog = builder.create();
                dialog.show();
            }
        });
    }

    private boolean _init_sequence(){

        if(!ExecuteShellCommand.canRunRootCommands()){
            tv.append("Unable to get root access." + "\n");
            tv.append("Root access is required. Unable to continue." + "\n");
            return false;
        }


        ChipsetType chipsetType = _getChipsetType();
        if(chipsetType == ChipsetType.QUALCOMM){
            tv.append("detected chipset type is QUALCOMM. Supported." + "\n");
        }else if(chipsetType == ChipsetType.MTK){
            tv.append("detected chipset type is MTK. Currently is unsupported." + "\n");
            tv.append("unable to continue \n");
            return false;
        }else{
            tv.append("detected chipset type is UNKNOWN. Unable to continue." + "\n");
            return false;
        }

        if(chipsetType == ChipsetType.QUALCOMM){
            //String res = ExecuteShellCommand.executeAsRoot("ls -l /dev/diag;",true);
            //if(res.matches("^.*system.*diag.*/dev/diag.*$")){
            if(new File("/dev/diag").exists()){
                String res;
                ExecuteShellCommand.executeAsRoot("chmod 666 /dev/diag;",false);
                res = ExecuteShellCommand.executeAsRoot("ls -l /dev/diag;",true);
                if(!res.matches("^crw-rw-rw-.*$")){
                    tv.append("Unable to change rights for diag interface\n");
                    tv.append("Unable to continue\n");
                    return false;
                }
            }else{
                tv.append("Unable to find Qualcomm diag interface\n");
                tv.append("Unable to continue\n");
                return false;
            }
        }else if(chipsetType == ChipsetType.MTK){

        }

        //ExecuteShellCommand.executeAsRoot("setprop log.redirect-stdio true",false);

        _update_security_policy();

        return true;
    }



    //MediaTek: [ro.board.platform]: [mt6735m]
    //Qualcomm: [ro.board.platform]: [msm8084]
    private ChipsetType _getChipsetType(){
        String res = ExecuteShellCommand.executeAsRoot("getprop ro.board.platform;",true);
        if(res == null) return ChipsetType.UNKNOWN;

        if(res.matches("^msm.*")){
            return ChipsetType.QUALCOMM;
        }else if(res.matches("^mt.*")){
            return ChipsetType.MTK;
        }else{
            return ChipsetType.UNKNOWN;
        }
    }

    private boolean _update_security_policy(){
        String cmd;
        cmd = "echo 0 > /sys/fs/selinux/enforce";
/*
        cmd = "setenforce 0; ";
        cmd = cmd + "supolicy --live \"allow init logd dir getattr\";";

    //Depreciated supolicies. Still keep them for backup purpose
        cmd = cmd + "supolicy --live \"allow init init process execmem\";";
        cmd = cmd + "supolicy --live \"allow atfwd diag_device chr_file {read write open ioctl}\";";
        cmd = cmd + "supolicy --live \"allow init properties_device file execute\";";
        cmd = cmd + "supolicy --live \"allow system_server diag_device chr_file {read write}\";";

    //Suspicious supolicies: MI works without them, but it seems that they SHOULD be enabled...

    //mi2log permission denied (logcat | grep denied), but no impact on log collection/analysis
        cmd = cmd + "supolicy --live \"allow untrusted_app app_data_file file {rename}\";";

    //Suspicious: why still works after disabling this command? Won't FIFO fail?
        cmd = cmd + "supolicy --live \"allow init app_data_file fifo_file {write open getattr}\";";
        cmd = cmd + "supolicy --live \"allow init diag_device chr_file {getattr write ioctl}\"; ";

    //Nexus 6 only
        cmd = cmd + "supolicy --live \"allow untrusted_app diag_device chr_file {write open getattr}\";";
        cmd = cmd + "supolicy --live \"allow system_server diag_device chr_file {read write}\";";
        cmd = cmd + "supolicy --live \"allow netmgrd diag_device chr_file {read write}\";";
        cmd = cmd + "supolicy --live \"allow rild diag_device chr_file {read write}\";";
        cmd = cmd + "supolicy --live \"allow rild debuggerd app_data_file {read open getattr}\";";
        cmd = cmd + "supolicy --live \"allow debuggerd app_data_file file {read open getattr}\";";
        cmd = cmd + "supolicy --live \"allow zygote zygote process {execmem}\";";
        cmd = cmd + "supolicy --live \"allow zygote ashmem_device chr_file {execute}\";";
        cmd = cmd + "supolicy --live \"allow zygote zygote_tmpfs file {execute}\";";
        cmd = cmd + "supolicy --live \"allow zygote activity_service service_manager {find}\";";
        cmd = cmd + "supolicy --live \"allow zygote package_service service_manager {find}\";";
        cmd = cmd + "supolicy --live \"allow zygote system_server binder {call}\";";
        cmd = cmd + "supolicy --live \"allow zygote system_server binder {transfer}\";";
        cmd = cmd + "supolicy --live \"allow system_server zygote binder {call}\";";
        cmd = cmd + "supolicy --live \"allow untrusted_app sysfs file {read open getattr}\";";

        cmd = cmd + "supolicy --live \"allow wcnss_service mnt_user_file dir {search}\";";

        cmd = cmd + "supolicy --live \"allow wcnss_service fuse dir {read open search}\";";

        cmd = cmd + "supolicy --live \"allow wcnss_service mnt_user_file lnk_file {read}\";";

        cmd = cmd + "supolicy --live \"allow wcnss_service fuse file {read append getattr}\";";
*/
//TODO при выходе надо бы возвращать состояние SELinux в то которое было при старте
        String res;
        res = ExecuteShellCommand.executeAsRoot("/system/bin/getenforce", true);
        if(res.matches("^.*Enforcing.*$")){
            res = ExecuteShellCommand.executeAsRoot(cmd,true);
            tv.append("Make SELinux permissive: "+ res + "\n");
            if(res.matches("^.*Ok.*$")){
                return true;
            }else{
                return false;
            }
        }else if(res.matches("^.*Permissive.*$")){
            tv.append("SELinux already Permissive\n");
        }



        return true;
    }

 }

class CfgMessage
{
    private String typeName;
    private boolean enabled;
    private Integer typeId;

    public CfgMessage(String key, Integer id, boolean value)
    {
        typeName    = key;
        typeId      = id;
        enabled     = value;
    }

    public String   getTypeName()           { return typeName; }
    public boolean  isEbabled()             { return enabled; }
    public Integer  getTypeId()             {return typeId;}
    public void     setTypeName(String str) { typeName = str;}
    public void     enable(boolean b)       { enabled = b;}
}

