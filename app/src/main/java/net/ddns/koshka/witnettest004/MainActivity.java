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
    DiagRevealerControl DRC;
    MyFIFO fifo;
    GuiMessenger GuiMsngr;
    CfgMessage[] cfgMsgList;

    @SuppressLint("HandlerLeak")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

// initializing GUI elements
        tv = (TextView) findViewById(R.id.sample_text);
        tv.setMovementMethod(new ScrollingMovementMethod());
        rbtn    = (Button)findViewById(R.id.button);
        exitbtn = (Button)findViewById(R.id.button2);
        cfgbtn  = (Button)findViewById(R.id.button3);

// preparing OS queue handler to change GUI elements from other threads
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

// initializing singleton which will deliver messages to the Handler from other threads
        GuiMsngr = GuiMessenger.getInstance();
        GuiMsngr.init(h);

// initializing FIFO buffer for DIAG data
        fifo = new MyFIFO(1024);

// initializing class that will directly interact with native part of the project
        DRC = new DiagRevealerControl(fifo);

// initializing the list of all diagnostic messages that can be requested via Diag.cgf
        cfgMsgList = new CfgMessage[DRC.getKnownMessageTypes().length];
        int i = 0;
        for(String mt : DRC.getKnownMessageTypes()){
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
        TestLogElement testlog = new TestLogElement(fifo);

        testlog.runTest();

// RUN/STOP button OnClik listener
        rbtn.setOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    tv.append("There is nothing to run yet... \n");
/*
                if(DRC.getRevealerRunStatus()){
                    DRC.stopRevealer();
                }else{
                    DRC.runRevealer();
                }
*/
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
                        DRC.writeNewDiagCfg(enabledMessageTypesList.toArray(new String[0]), getApplicationContext().getApplicationInfo().dataDir);
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
    public void     setTypeName(String str) { typeName = str;}
    public void     enable(boolean b)       { enabled = b;}
}