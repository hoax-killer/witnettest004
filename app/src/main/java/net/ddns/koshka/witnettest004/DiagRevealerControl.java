package net.ddns.koshka.witnettest004;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

/**
 * Created by siralex on 21-Mar-18.
 */

public class DiagRevealerControl {

    MyFIFO fifo;
    Handler h;
    Boolean revialerIsRunning = false;

    static {
        System.loadLibrary("diagRevealer");
    }

    DiagRevealerControl(MyFIFO f, Handler hh){
        fifo = f;
        h = hh;
    }

    public void runRevealer(){
        if(revialerIsRunning) return;
        setRevealerRunState(true);
        new Thread(new Runnable() {
            @Override
            public void run() {
                readDiag();
                setRevealerRunState(false);
            }
        }).start();
     }

    public Boolean getRevealerRunStatus(){
       return  revialerIsRunning;
    }

    private void setRevealerRunState( boolean boo ){
        Message msg;
        revialerIsRunning = boo;
        msg = h.obtainMessage(1, 0,  0, boo?"Stop":"Run");
        h.sendMessage(msg);
    }

    public void logRevealer(byte[] bb){
        //Log.v("DEBUGNET", "logRevealer called with array size " + bb.length + "with data" + _print_hex(bb));
        fifo.putDataBytes(bb);
    }

    public void stopRevealer(){
        if(revialerIsRunning) stopDiag();
    }

    public String[] getKnownMessageTypes(){
        // TODO
        // later this list should be taken from c++ code, from constants
        String[] str = { "_1xEV_Connection_Attempt",  "_1xEV_Connection_Release", "_1xEV_Signaling_Control_Channel_Broadcast", "_1xEV_Connected_State_Search_Info",
                "GSM_Surround_Cell_BA_List", "GSM_RR_Cell_Selection_Parameters", "GSM_RR_Cell_Information", "GSM_RR_Cell_Reselection_Meas",
                "LTE_MAC_Configuration", "LTE_MAC_Rach_Trigger", "LTE_MAC_Rach_Attempt", "LTE_MAC_DL_Transport_Block", "LTE_MAC_UL_Transport_Block", "LTE_MAC_UL_Buffer_Status_Internal",
                "LTE_PUCCH_Power_Control", "LTE_PUSCH_Power_Control", "LTE_PDCCH_PHICH_Indication_Report", "LTE_PDSCH_Stat_Indication"};
        return str;
    }

    public boolean writeNewDiagCfg(String[] msgTypes){
        return true;
    }


    private native void readDiag();
    private native void stopDiag();
}
