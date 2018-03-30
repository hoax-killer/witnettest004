package net.ddns.koshka.witnettest004;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

/**
 * Created by siralex on 21-Mar-18.
 */

public class DiagRevealerControl {

    MyFIFO fifo;
    Boolean revealerIsRunning = false;

    static {
        System.loadLibrary("diagRevealer");
    }

    DiagRevealerControl(MyFIFO f){
        fifo = f;
    }

    public void runRevealer(){
        if(revealerIsRunning) return;
        _setRevealerRunState(true);
        new Thread(new Runnable() {
            @Override
            public void run() {
                readDiag();
                _setRevealerRunState(false);
            }
        }).start();
     }

    public Boolean getRevealerRunStatus(){
       return  revealerIsRunning;
    }

    private void _setRevealerRunState( boolean boo ){
        revealerIsRunning = boo;
        GuiMessenger.getInstance().sendMessage(GuiMessenger.runButtonText, boo?"Stop":"Run" );
    }
//TODO there is no point in this method. Native can write directly to FIFO
    public void logRevealer(byte[] bb){
        //Log.v("DEBUGNET", "logRevealer called with array size " + bb.length + "with data" + _print_hex(bb));
        fifo.putDataBytes(bb);
    }

    public void stopRevealer(){
        if(revealerIsRunning) stopDiag();
    }

    public String[] getKnownMessageTypes(){
        String[] str = (String[]) getTypeNames();
        return str;
    }

    public boolean writeNewDiagCfg(String[] msgTypes, String filePath){
        GuiMessenger.getInstance().sendMessage(GuiMessenger.guiLogMessage, "Trying to write new config file " + filePath + "/Diag.cfg");
        GuiMessenger.getInstance().sendMessage(GuiMessenger.guiLogMessage, "Enabled log messages count:  " + msgTypes.length);
        for(String str:msgTypes) GuiMessenger.getInstance().sendMessage(GuiMessenger.guiLogMessage, str);

        Object[] resp = writeDiagCfg( msgTypes,  filePath+"/Diag.cfg");
        GuiMessenger.getInstance().sendMessage(GuiMessenger.guiLogMessage, "response code " + (Integer)resp[0] +"; explanation: " + (String)resp[1] );

        if((Integer)resp[0] == 0){
            return true;
        }else{
            return false;
        }
    }


    private native void     readDiag();
    private native void     stopDiag();
    private native Object[] getTypeNames();
    private native Object[]  writeDiagCfg(String[] msgTypes, String fileName);
}
