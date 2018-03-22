package net.ddns.koshka.witnettest004;

import android.os.Handler;
import android.os.Message;

import java.util.LinkedList;
import java.util.List;

/**
 * Created by siralex on 19-Mar-18.
 */

public class TestLogElement{


    int strnum  = 0;
    int timeout = 0;
    Handler h;

    public TestLogElement (int nstrings, int tout, Handler hh){
        strnum = nstrings;
        timeout = tout;
        h = hh;
    }


    public void runTest(){
        new Thread(new Runnable() {

            @Override
            public void run() {
                Message msg;
                String str;
                for(int i=0; i<strnum; i++){
                    str = "this is a row number " + i ;
                    msg = h.obtainMessage(0, 0,  0, str);
                    h.sendMessage(msg);
                    try {
                        Thread.sleep(timeout);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        }).start();


    }

}

