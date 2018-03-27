package net.ddns.koshka.witnettest004;

import java.util.concurrent.ArrayBlockingQueue;

/**
 * Created by siralex on 22-Mar-18.
 */

public class MyFIFO {
    ArrayBlockingQueue<Byte> buffer;
    int qsize = 0;

    MyFIFO( int size){
        qsize = size;
        buffer = new ArrayBlockingQueue<Byte>(size,true);
    }

    public byte loadPercentage(){
        return (byte) (buffer.remainingCapacity() * 100 / qsize);
    }

    public String getStat(){
        return "stat: " + buffer.remainingCapacity() + " " + buffer.size();
    }

    public Byte getDataByte(){
        try {
            return buffer.take();
        } catch (InterruptedException e) {
            return null;
        }
    }
    public void putDataByte( byte b){
        buffer.add(b);
    }

    //TODO use AddAll() ?
    public void putDataBytes( byte[] bb){
        for(int i =0; i< bb.length; i++){
            buffer.add(bb[i]);
        }
    }
}
