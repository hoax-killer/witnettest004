package net.ddns.koshka.witnettest004;

import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.util.SparseArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.math.BigInteger;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.time.Instant;
import java.util.*;
import java.util.concurrent.ArrayBlockingQueue;

/**
 * Created by siralex on 19-Mar-18.
 */

public class TestLogElement{

    private ArrayBlockingQueue<DiagDataPacket> fifo;
    private int fifoTotalCapacity       = 0;
    private volatile long fifoPktTimeShift  = 0;
    private String fifoLastPaketTimestamp  = "";
    private SparseArray knownDiagMsgs;
    DiagRevealerControl DRC;

    public TestLogElement ( ArrayBlockingQueue<DiagDataPacket> f, CfgMessage[] knownMsgs, DiagRevealerControl drc){
        fifo    = f;
        fifoTotalCapacity = fifo.remainingCapacity();
        DRC = drc;

        knownDiagMsgs = new SparseArray();
        for(CfgMessage msg : knownMsgs){
            knownDiagMsgs.put(msg.getTypeId(), msg.getTypeName());
        }
    }


    public void runTest(){
        new Thread(new Runnable() {
            @Override
            public void run() {
                DiagDataPacket diagPkt;
                DateFormat formatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss:SSS");
                try {
                    while(true){

                        diagPkt = fifo.take();
                        fifoPktTimeShift = System.currentTimeMillis() - diagPkt.getEpochMs();
                        fifoLastPaketTimestamp = diagPkt.getTimestamp();

                        String json_packet = DRC.decodePacket(diagPkt.getPayload());
                        String outLogPacketStr = "\n\nincoming pkt.\n";

                        JSONObject pktJsonObj;
                        try {
                            pktJsonObj = new JSONObject(json_packet);
                            JSONObject header  = pktJsonObj.getJSONObject("header");
                            int type_id = header.getInt("type_id");
                            outLogPacketStr += "type_id: " + knownDiagMsgs.get(type_id) + "\n";
                            long timestamp = header.getLong("timestamp");
                            Date date = new Date(timestamp);
                            outLogPacketStr += "pkt time: " + formatter.format(date) + "\n";
                            int pktsz = header.getInt("log_msg_len");
                            outLogPacketStr += "pkt size: " + pktsz + "\n";

                            int spacesToIndentEachLevel = 2;
                            JSONObject payloadObj = pktJsonObj.getJSONObject("payload");
                            outLogPacketStr += payloadObj.toString(spacesToIndentEachLevel);


                        } catch (JSONException e) {
                            Log.e("witnettest", "Json parsing error: " + e.getMessage());
                        }


                        GuiMessenger.getInstance().sendMessage(GuiMessenger.guiLogMessage, " " + outLogPacketStr   );

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

