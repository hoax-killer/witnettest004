package net.ddns.koshka.witnettest004;

import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.widget.TextView;

import java.util.Collection;
import java.util.Iterator;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.TimeUnit;

public class MainActivity extends AppCompatActivity {

    TextView tv;
    Handler h;

    // Used to load the 'native-lib' library on application startup.

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        tv = (TextView) findViewById(R.id.sample_text);
        tv.setMovementMethod(new ScrollingMovementMethod());



        //tv.setText(stringFromJNI());
        //tv.setText("some shit");
        h = new Handler(){
            @Override
            public void handleMessage(android.os.Message msg) {
                // обновляем TextView
                tv.append(msg.obj + "\n");
             };
        };

        DiagRevealerControl DC = new DiagRevealerControl();
        DC.readDiag();

        if(ExecuteAsRoot.canRunRootCommands()){
            tv.append("root is available" + "\n");
            String resp = ExecuteAsRoot.execute("ls -l /dev/diag", true);
            tv.append(resp + "\n");

            resp = ExecuteAsRoot.execute("chmod 666 /dev/diag", false);
            tv.append(resp + "\n");

            resp = ExecuteAsRoot.execute("ls -l /dev/diag",true);
            tv.append(resp + "\n");
        }else{
            tv.append("root is NOT available" + "\n");
        }
        //TestLogElement testlog = new TestLogElement(50, 1000, h);

        //testlog.runTest();
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */

}
