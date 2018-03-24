package net.ddns.koshka.witnettest004;

import android.util.Log;

/**
 * Created by siralex on 21-Mar-18.
 */

public class DiagRevealerControl {

    MyFIFO fifo;

    static {
        System.loadLibrary("diagRevealer");
    }

    DiagRevealerControl(MyFIFO f){
        fifo = f;
    }

    public void runRevealer(){
        new Thread(new Runnable() {
            @Override
            public void run() {
                readDiag();
            }
        }).start();
    }

    public void logRevealer(byte[] bb){
        //Log.v("DEBUGNET", "logRevealer called with array size " + bb.length + "with data" + _print_hex(bb));
        fifo.putDataBytes(bb);
    }

    private String _print_hex (byte[] bb){
        String str1 = "", str2 = "";
        int i = 0;

        for (i = 0; i < bb.length; i++) {
            str1 = String.format("%02x ", bb[i]);
            str2 = str2 + str1;
        }
        return str2;
    }
    public native void readDiag();
}
