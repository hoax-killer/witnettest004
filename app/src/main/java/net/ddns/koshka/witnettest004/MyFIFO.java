package net.ddns.koshka.witnettest004;

import java.util.concurrent.ArrayBlockingQueue;

/**
 * Created by siralex on 22-Mar-18.
 */

public class MyFIFO {
    ArrayBlockingQueue<Byte> buffer;

    MyFIFO( int size){
        buffer = new ArrayBlockingQueue<Byte>(size,true);
    }

    public void putDataByte( byte b){

    }

    public void putDataBytes( byte[] bb){

    }
}
