package net.ddns.koshka.witnettest004;

import android.util.Log;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;

/**
 * Created by siralex on 20-Mar-18.
 */

public class ExecuteShellCommand {



    public static boolean canRunRootCommands()
    {
        boolean retval = false;
        Process suProcess;

        try
        {
            suProcess = Runtime.getRuntime().exec("su");

            DataOutputStream os     = new DataOutputStream(suProcess.getOutputStream());
            BufferedReader osRes    = new BufferedReader( new InputStreamReader(suProcess.getInputStream()) );
            //DataInputStream osRes = new DataInputStream(suProcess.getInputStream());

            // Getting the id of the current user to check if this is root
            os.writeBytes("id\n");
            os.flush();

            String currUid = osRes.readLine();
            boolean exitSu = false;
            if (null == currUid)
            {
                retval = false;
                exitSu = false;
                Log.d("ROOT", "Can't get root access or denied by user");
            }
            else if (true == currUid.contains("uid=0"))
            {
                retval = true;
                exitSu = true;
                Log.d("ROOT", "Root access granted");
            }
            else
            {
                retval = false;
                exitSu = true;
                Log.d("ROOT", "Root access rejected: " + currUid);
            }

            if (exitSu)
            {
                os.writeBytes("exit\n");
                os.flush();
            }
        }
        catch (Exception e)
        {
            // Can't get root !
            // Probably broken pipe exception on trying to write to output stream (os) after su failed, meaning that the device is not rooted

            retval = false;
            Log.d("ROOT", "Root access rejected [" + e.getClass().getName() + "] : " + e.getMessage());
        }

        return retval;
    }



    public static String executeAsRoot( String str, Boolean RespNeeded){
        String retval = null;
        long timeoutMillis = 5000;

        try {
            Process suProcess = Runtime.getRuntime().exec("su");

            DataOutputStream os     = new DataOutputStream(suProcess.getOutputStream());
            BufferedReader osRes    = new BufferedReader( new InputStreamReader(suProcess.getInputStream()) );

            os.writeBytes(str +"\n");
            os.flush();

            if(RespNeeded) {
                // reading with timeout because we don't know if command will return something
                long maxTimeMillis = System.currentTimeMillis() + timeoutMillis;
                boolean readOK = false;
                while (System.currentTimeMillis() < maxTimeMillis && !readOK) {
                    if (osRes.ready()) {
                        retval = osRes.readLine();
                        readOK = true;
                    }
                }
            }

            os.writeBytes("exit\n");
            os.flush();
            try
            {
                 int suProcessRetval = suProcess.waitFor();

                if (255 != suProcessRetval)
                {
                    if (retval == null || retval == "") retval = "OK";
                    // Root access granted
                }
                else
                {
                    // Root access denied
                    retval = null;
                }
            }
            catch (Exception ex)
            {
                Log.e("ROOT", "Error executing single command", ex);
            }
        }
        catch (IOException ex)
        {
            Log.w("ROOT", "Can't get root access", ex);
        }
        catch (SecurityException ex)
        {
            Log.w("ROOT", "Can't get root access", ex);
        }
        catch (Exception ex)
        {
            Log.w("ROOT", "Error executing internal operation", ex);
        }

        return retval;
    }





    public static String execute( String str, Boolean RespNeeded){
        String retval = null;
        long timeoutMillis = 5000;

        try {
            Process process = Runtime.getRuntime().exec(str);

                if(RespNeeded) {
                    // reading with timeout because we don't know if command will return something
                    long maxTimeMillis = System.currentTimeMillis() + timeoutMillis;
                    boolean readOK = false;
                    BufferedReader osRes    = new BufferedReader( new InputStreamReader(process.getInputStream()) );
                    while (System.currentTimeMillis() < maxTimeMillis && !readOK) {
                        if (osRes.ready()) {
                            retval = osRes.readLine();
                            readOK = true;
                        }
                    }
                    osRes.close();
                }

                int processRetval = process.waitFor();


        }
        catch (IOException ex)
        {
            Log.w("ROOT", "Can't get root access", ex);
        }
        catch (SecurityException ex)
        {
            Log.w("ROOT", "Can't get root access", ex);
        }
        catch (Exception ex)
        {
            Log.w("ROOT", "Error executing internal operation", ex);
        }

        return retval;
    }

}
