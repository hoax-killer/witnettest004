import java.util.LinkedList;
import java.util.Queue;

/**
 * Created by siralex on 22-Mar-18.
 */

public class MyFIFO {

    Queue<Byte> buffer;

    MyFIFO(){
        buffer = new LinkedList<Byte>();
    }

    public void add(byte b){
        buffer.add(b);
    }

    public byte remove(){
        return buffer.remove();
    }
}
