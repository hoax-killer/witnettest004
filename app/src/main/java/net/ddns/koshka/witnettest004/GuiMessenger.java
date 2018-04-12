package net.ddns.koshka.witnettest004;

import android.os.Handler;
import android.os.Message;

public class GuiMessenger {
    private Handler h = null;
    private static  GuiMessenger _instance = null;

    public static final int guiLogMessage       = 0;
    public static final int runButtonText       = 1;
    public static final int guiBufStatMsg       = 2;
    public static final int guiBufLastMsgTime   = 3;
    public static final int guiBufTimeshift     = 4;

    private GuiMessenger(){}

    public static synchronized GuiMessenger getInstance() {
        if (_instance == null)
            _instance = new GuiMessenger();
        return _instance;
    }

    public void init(Handler hh){
        h = hh;
    }

    public void sendMessage(int target, String str){
        Message msg;
        if(h != null) {
            msg = h.obtainMessage(target, 0, 0, str);
            h.sendMessage(msg);
        }
    }
}
