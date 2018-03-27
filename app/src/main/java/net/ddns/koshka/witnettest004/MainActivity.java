package net.ddns.koshka.witnettest004;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


public class MainActivity extends AppCompatActivity {

    TextView tv;
    Button rbtn, exitbtn, cfgbtn;
    Handler h;
    DiagRevealerControl DC;
    MyFIFO fifo;
    CfgMessage[] cfgMsgList;

    @SuppressLint("HandlerLeak")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        tv = (TextView) findViewById(R.id.sample_text);
        tv.setMovementMethod(new ScrollingMovementMethod());
        rbtn    = (Button)findViewById(R.id.button);
        exitbtn = (Button)findViewById(R.id.button2);
        cfgbtn  = (Button)findViewById(R.id.button3);

        h = new Handler(){
            @Override
            public void handleMessage(android.os.Message msg) {
                switch(msg.what) {
                    case 0: // write to log
                        tv.append(msg.obj + "\n");
                        break;
                    case 1: // change button text
                        rbtn.setText((CharSequence) msg.obj);
                        break;
                }
            };
        };

        fifo = new MyFIFO(1024);
        DC = new DiagRevealerControl(fifo, h);

        cfgMsgList = new CfgMessage[DC.getKnownMessageTypes().length];
        int i = 0;
        for(String mt : DC.getKnownMessageTypes()){
            cfgMsgList[i] = new CfgMessage(mt, false);
            i++;
        }

        //DC.runRevealer();
/*
        if(ExecuteAsRoot.canRunRootCommands()){
            tv.append("root is available" + "\n");
            String resp = ExecuteAsRoot.execute("ls -l /dev/diag", true);
            tv.append(resp + "\n");

            resp = ExecuteAsRoot.execute("chmod 666 /dev/diag", false);
            tv.append(resp + "\n");

            resp = ExecuteAsRoot.execute("ls -l /dev/diag",true);
            tv.append(resp + "\n");
        }else{
            tv.append("root is NOT available" + "\n");
        }
*/
        TestLogElement testlog = new TestLogElement(h, fifo);

        testlog.runTest();

        rbtn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                if(DC.getRevealerRunStatus()){
                    DC.stopRevealer();
                }else{
                    DC.runRevealer();
                }
            }
        });

        exitbtn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                if(DC.getRevealerRunStatus()) {
                    DC.stopRevealer();
                }
                finishAffinity();
                System.exit(0);
            }
        });

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
                        // Do something when click positive button
                        ArrayList<String> enabledMessageTypesList = new ArrayList<>();
                        tv.append("Message types selected, writing new Diag.cfg \n");
                        for (int i=0; i<cfgMsgList.length; i++) {
                            if(cfgMsgList[i].isEbabled()) enabledMessageTypesList.add(cfgMsgList[i].getTypeName());
                        }
                        if(DC.writeNewDiagCfg(enabledMessageTypesList.toArray(new String[0]))){
                            tv.append("Write of Diag.cfg successfull\n");
                        }

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

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */

}

class CfgMessage
{
    private String typeName;
    private boolean enabled;

    public CfgMessage(String key, boolean value)
    {
        typeName    = key;
        enabled     = value;
    }

    public String   getTypeName()           { return typeName; }
    public boolean  isEbabled()             { return enabled; }
    public void     setTypeName(String str) {typeName = str;}
    public void     enable(boolean b)       {enabled = b;}
}