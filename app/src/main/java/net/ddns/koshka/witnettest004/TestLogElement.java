package net.ddns.koshka.witnettest004;

import android.os.Handler;
import android.os.Message;

import java.util.LinkedList;
import java.util.List;

/**
 * Created by siralex on 19-Mar-18.
 */

public class TestLogElement{

    MyFIFO fifo;

    public TestLogElement ( MyFIFO f){
        fifo    = f;
    }


    public void runTest(){
        new Thread(new Runnable() {
            @Override
            public void run() {
                String str;
                Message msg;
                byte[] data = new byte[5];
                while(true){
                    for(int i = 0; i<5; i++) data[i] = fifo.getDataByte();
                    str = _print_hex(data);
                    try {
                        Thread.sleep(2000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        }).start();


    }

    private String _print_hex (byte[] bb){
        String str1 = "", str2 = "";
        int i = 0;

        for (i = 0; i < bb.length; i++) {
            str1 = String.format("%02x ", bb[i]);
            str2 = str2 + str1;
        }
        return str2 + "buf: " + fifo.loadPercentage()+"% "+ fifo.getStat();
    }
}

