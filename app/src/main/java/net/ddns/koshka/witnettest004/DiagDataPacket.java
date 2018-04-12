package net.ddns.koshka.witnettest004;

import android.util.Log;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

public class DiagDataPacket {
    private long epoch_msec;
    private String timestamp;
    private int data_length;
    private int msg_type;
    private byte[] payload;

// incoming DIAG packet structure:
// [2B msg_type] [2B (payload + timestamp) length] [8B timestamp milliseconds] [payload]

//Actually there is no information about structure of this packet.
//Is it valid HDLC packet? Is it one consistent network message?
//So in further analisys we should place all these packets in common buffer end extract messages using HDLC boundaries
// I presume that current packet is a chunk of data received from diag port. It may contain many different messages
// but I hope all of them will be HDLC encoded so where will be no problem to separate them
    DiagDataPacket(byte[] bb, byte[] data_bb){

        int idx = 0;
        // retrieve message_type; data comes in little-endian i.e. LSB comes first
        byte low_msg_type = bb[idx++];
        byte high_msg_type = bb[idx++];
        msg_type= ((high_msg_type&0xFF)<<8) | (low_msg_type&0xFF);

        //retrieve message length; data comes in little-endian i.e. LSB comes first
        byte low_msg_len = bb[idx++];
        byte high_msg_len = bb[idx++];
        data_length= ((high_msg_len&0xFF)<<8) | (low_msg_len&0xFF);
        data_length -= 8; // exclude timestamp
/*
        if(data_length == data_bb.length){
            Log.d("DiagDataPacket", "length is correct");
        }else{
            Log.d("DiagDataPacket", "length is NOT correct");
        }
*/
        // data comes in little-endian i.e. LSB comes first
        byte msg_ts0 = bb[idx++];
        byte msg_ts1 = bb[idx++];
        byte msg_ts2 = bb[idx++];
        byte msg_ts3 = bb[idx++];
        byte msg_ts4 = bb[idx++];
        byte msg_ts5 = bb[idx++];
        byte msg_ts6 = bb[idx++];
        byte msg_ts7 = bb[idx++];

        epoch_msec =  (msg_ts0&0xFF);
        epoch_msec += ((long)msg_ts1&0xFF) << 8;
        epoch_msec += ((long)msg_ts2&0xFF) << 16;
        epoch_msec += ((long)msg_ts3&0xFF) << 24;
        epoch_msec += ((long)msg_ts4&0xFF) << 32;
        epoch_msec += ((long)msg_ts5&0xFF) << 40;
        epoch_msec += ((long)msg_ts6&0xFF) << 48;
        epoch_msec += ((long)msg_ts7&0xFF) << 56;

        DateFormat formatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss:SSS");
        Date date = new Date(epoch_msec);
        timestamp = formatter.format(date);

        payload = data_bb;
    }

    public long getEpochMs(){
        return epoch_msec;
    }

    public String getTimestamp(){
        return timestamp;
    }
    // payload length
    public int getDataLength(){
        return data_length;
    }

    public byte[] getPayload(){
        return payload;
    }


    public int getMsgType(){
        return msg_type;
    }
}
