package net.ddns.koshka.witnettest004;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.util.Arrays;
import java.util.concurrent.ArrayBlockingQueue;

/**
 * Created by siralex on 21-Mar-18.
 */

public class DiagRevealerControl {

    ArrayBlockingQueue<DiagDataPacket> fifo;
    Boolean revealerIsRunning = false;
    String diagConfigFilePath = "";

    static {
        System.loadLibrary("diagRevealer");
    }

    DiagRevealerControl(ArrayBlockingQueue<DiagDataPacket> f, String filepath){
        fifo = f;
        diagConfigFilePath = filepath;
    }

    public void runRevealer(){
        if(revealerIsRunning) return;
        _setRevealerRunState(true);
        new Thread(new Runnable() {
            @Override
            public void run() {
                Object[] resp =readDiag(diagConfigFilePath);
                _setRevealerRunState(false);
                if((Integer)resp[0]!=0){
                    GuiMessenger.getInstance().sendMessage(GuiMessenger.guiLogMessage, "DIAG thread ended with error: " + (String)resp[1] );
                }else{
                    GuiMessenger.getInstance().sendMessage(GuiMessenger.guiLogMessage, "DIAG thread ended gracefully: " + (String)resp[1] );
                }
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

    public void logRevealer(byte[] header_buf, byte[] data_buf){
        if((header_buf.length < 12) || (data_buf.length == 0)){
            GuiMessenger.getInstance().sendMessage(GuiMessenger.guiLogMessage, "Short packet dropped: header "+header_buf.length+"; data "+data_buf.length );
            return;
        }

        DiagDataPacket diagPkt = new DiagDataPacket(header_buf, data_buf);
        fifo.add(diagPkt);

    }

    public void stopRevealer(){
        if(revealerIsRunning) stopDiag();
    }

    public Object[] getKnownMessageTypes(){
        return  getTypeNames();
    }

    public String decodePacket(byte[] bb){
        return processLogChunk(bb);
    }

    public boolean writeNewDiagCfg(String[] msgTypes){
        GuiMessenger.getInstance().sendMessage(GuiMessenger.guiLogMessage, "Trying to write new config file " + diagConfigFilePath);
        GuiMessenger.getInstance().sendMessage(GuiMessenger.guiLogMessage, "Enabled log messages count:  " + msgTypes.length);
        for(String str:msgTypes) GuiMessenger.getInstance().sendMessage(GuiMessenger.guiLogMessage, str);

        Object[] resp = writeDiagCfg( msgTypes,  diagConfigFilePath);
        GuiMessenger.getInstance().sendMessage(GuiMessenger.guiLogMessage, "response code " + (Integer)resp[0] +"; explanation: " + (String)resp[1] );

        if((Integer)resp[0] == 0){
            return true;
        }else{
            return false;
        }
    }

    private native String       processLogChunk(byte[] bb);
    private native Object[]     readDiag(String diagConfigFilePath);
    private native void         stopDiag();
    private native Object[]     getTypeNames();
    private native Object[]     writeDiagCfg(String[] msgTypes, String fileName);

}
