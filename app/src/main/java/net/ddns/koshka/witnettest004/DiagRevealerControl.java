package net.ddns.koshka.witnettest004;

import android.util.Log;

/**
 * Created by siralex on 21-Mar-18.
 */

public class DiagRevealerControl {
    static {
        System.loadLibrary("diagRevealer");
    }

    DiagRevealerControl(){

    }

    public void runRevealer(){
        readDiag();
    }

    public void logRevealer(){
        Log.v("DEBUGNET", "logRevealer called");
    }

    public native void readDiag();
}
