package net.ddns.koshka.witnettest004;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.math.BigInteger;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.time.Instant;
import java.util.Calendar;
import java.util.Date;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.ArrayBlockingQueue;

/**
 * Created by siralex on 19-Mar-18.
 */

public class TestLogElement{

    private ArrayBlockingQueue<DiagDataPacket> fifo;
    private int fifoTotalCapacity       = 0;
    private volatile long fifoPktTimeShift  = 0;
    private String fifoLastPaketTimestamp  = "";
    DiagRevealerControl DRC;

    public TestLogElement ( ArrayBlockingQueue<DiagDataPacket> f, DiagRevealerControl drc){
        fifo    = f;
        fifoTotalCapacity = fifo.remainingCapacity();
        DRC = drc;
    }


    public void runTest(){
        new Thread(new Runnable() {
            @Override
            public void run() {
                DiagDataPacket diagPkt;
                try {
                    while(true){

                        diagPkt = fifo.take();
                        fifoPktTimeShift = System.currentTimeMillis() - diagPkt.getEpochMs();
                        fifoLastPaketTimestamp = diagPkt.getTimestamp();

                        DRC.decodePacket(diagPkt.getPayload());

                        //str = _print_hex(data);formatter.format(date
                       //GuiMessenger.getInstance().sendMessage(GuiMessenger.guiLogMessage, "msgtype: "+ msg_type + " msglen: " + msg_len + " ts: " + formatter.format(date)  );
                        //GuiMessenger.getInstance().sendMessage(GuiMessenger.guiLogMessage, " " + _print_hex(data) +" "+ String.format("0x%16X", msg_ts)   );
                    }
                }catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();


    }


    public void updateBufStats(){
        new Thread(new Runnable() {
            @Override
            public void run() {

                while(true){
                    int i = fifo.remainingCapacity() * 100 / fifoTotalCapacity;
                    GuiMessenger.getInstance().sendMessage(GuiMessenger.guiBufStatMsg, Integer.toString(i) );
                    GuiMessenger.getInstance().sendMessage(GuiMessenger.guiBufLastMsgTime, fifoLastPaketTimestamp );
                    GuiMessenger.getInstance().sendMessage(GuiMessenger.guiBufTimeshift,   Long.toString(fifoPktTimeShift) );
                    try {
                        Thread.sleep(300);
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
        return str2;
    }
}

