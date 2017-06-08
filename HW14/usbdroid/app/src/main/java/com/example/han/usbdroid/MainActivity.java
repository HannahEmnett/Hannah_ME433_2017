package com.example.han.usbdroid;


import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.WindowManager;

import com.hoho.android.usbserial.driver.CdcAcmSerialDriver;
import com.hoho.android.usbserial.driver.ProbeTable;
import com.hoho.android.usbserial.driver.UsbSerialDriver;
import com.hoho.android.usbserial.driver.UsbSerialPort;
import com.hoho.android.usbserial.driver.UsbSerialProber;
import com.hoho.android.usbserial.util.SerialInputOutputManager;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import static android.graphics.Color.blue;
import static android.graphics.Color.green;
import static android.graphics.Color.red;
import static android.graphics.Color.rgb;

public class MainActivity extends Activity implements TextureView.SurfaceTextureListener {
    private SeekBar myControl;
    private SeekBar myControl2;
    //TextView myTextView;
    //Button button;
    //TextView myTextView2;
    ScrollView myScrollView;
    TextView myTextView3;
    private UsbManager manager;
    private UsbSerialPort sPort;
    private final ExecutorService mExecutor = Executors.newSingleThreadExecutor();
    private SerialInputOutputManager mSerialIoManager;
    private Camera mCamera;
    private TextureView mTextureView;
    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private Bitmap bmp = Bitmap.createBitmap(640, 480, Bitmap.Config.ARGB_8888);
    private Canvas canvas = new Canvas(bmp);
    private Paint paint1 = new Paint();
    private TextView mTextView;
    private TextView location2;
    private TextView location3;

    static long prevtime = 0; // for FPS calculation
    static int progressupdate=0;
    static int progressupdate2=0;
    static int dif=0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON); // keeps the screen from turning off

        //myTextView = (TextView) findViewById(R.id.textView01);
        //myTextView.setText("Enter whatever you Like!");
        mTextView = (TextView) findViewById(R.id.cameraStatus);
        location2= (TextView) findViewById(R.id.location);
        location3 = (TextView) findViewById(R.id.location2);

        //myTextView2 = (TextView) findViewById(R.id.textView02);
        myScrollView = (ScrollView) findViewById(R.id.ScrollView01);
        myTextView3 = (TextView) findViewById(R.id.textView03);
        //button = (Button) findViewById(R.id.button1);

        // see if the app has permission to use the camera
        //ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.CAMERA}, 1);
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED) {
            mSurfaceView = (SurfaceView) findViewById(R.id.surfaceview);
            mSurfaceHolder = mSurfaceView.getHolder();

            mTextureView = (TextureView) findViewById(R.id.textureview);
            mTextureView.setSurfaceTextureListener(this);

            // set the paintbrush for writing text on the image
            paint1.setColor(0xffff0000); // red
            paint1.setTextSize(24);
            myControl = (SeekBar) findViewById(R.id.seek1);
            myControl2 = (SeekBar) findViewById(R.id.seek2);
            setMyControlListener();
            setMyControl2Listener();
            mTextView.setText("started camera");
        } else {
            mTextView.setText("no camera permissions");
        }

        setMyControlListener();
        /*button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //myTextView2.setText("value on click is "+myControl.getProgress());
                String sendString = String.valueOf(dif) + '\n';
                try {
                    sPort.write(sendString.getBytes(), 10); // 10 is the timeout
                } catch (IOException e) { }
            }
        });*/
        manager = (UsbManager) getSystemService(Context.USB_SERVICE);
    }

    private void setMyControlListener() {
        myControl.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {

            int progressChanged = 0;

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                progressChanged = progress;
                progressupdate=progressChanged;
                location2.setText("The value is: "+progress);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }
    private void setMyControl2Listener() {
        myControl2.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            int progressChanged2 = 0;

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                progressChanged2 = progress;
                progressupdate2 = progressChanged2;
                location3.setText("The value is: " + progress);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }
    private final SerialInputOutputManager.Listener mListener =
            new SerialInputOutputManager.Listener() {
                @Override
                public void onRunError(Exception e) {

                }

                @Override
                public void onNewData(final byte[] data) {
                    MainActivity.this.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            MainActivity.this.updateReceivedData(data);
                        }
                    });
                }
            };

    @Override
    protected void onPause(){
        super.onPause();
        stopIoManager();
        if(sPort != null){
            try{
                sPort.close();
            } catch (IOException e){ }
            sPort = null;
        }
        finish();
    }

    @Override
    protected void onResume() {
        super.onResume();

        ProbeTable customTable = new ProbeTable();
        customTable.addProduct(0x04D8,0x000A, CdcAcmSerialDriver.class);
        UsbSerialProber prober = new UsbSerialProber(customTable);

        final List<UsbSerialDriver> availableDrivers = prober.findAllDrivers(manager);

        if(availableDrivers.isEmpty()) {
            //check
            return;
        }

        UsbSerialDriver driver = availableDrivers.get(0);
        sPort = driver.getPorts().get(0);

        if (sPort == null){
            //check
        }else{
            final UsbManager usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
            UsbDeviceConnection connection = usbManager.openDevice(driver.getDevice());
            if (connection == null){
                //check
                PendingIntent pi = PendingIntent.getBroadcast(this, 0, new Intent("com.android.example.USB_PERMISSION"), 0);
                usbManager.requestPermission(driver.getDevice(), pi);
                return;
            }

            try {
                sPort.open(connection);
                sPort.setParameters(9600, 8, UsbSerialPort.STOPBITS_1, UsbSerialPort.PARITY_NONE);

            }catch (IOException e) {
                //check
                try{
                    sPort.close();
                } catch (IOException e1) { }
                sPort = null;
                return;
            }
        }
        onDeviceStateChange();
    }

    private void stopIoManager(){
        if(mSerialIoManager != null) {
            mSerialIoManager.stop();
            mSerialIoManager = null;
        }
    }

    private void startIoManager() {
        if(sPort != null){
            mSerialIoManager = new SerialInputOutputManager(sPort, mListener);
            mExecutor.submit(mSerialIoManager);
        }
    }

    private void onDeviceStateChange(){
        stopIoManager();
        startIoManager();
    }

    private void updateReceivedData(byte[] data) {
        //do something with received data

        //for displaying:
        String rxString = null;
        try {
            rxString = new String(data, "UTF-8"); // put the data you got into a string
            myTextView3.append(rxString);
            myScrollView.fullScroll(View.FOCUS_DOWN);
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
    }
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mCamera = Camera.open();
        Camera.Parameters parameters = mCamera.getParameters();
        parameters.setPreviewSize(640, 480);
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_INFINITY); // no autofocusing
        parameters.setAutoExposureLock(true); // keep the white balance constant
        mCamera.setParameters(parameters);
        mCamera.setDisplayOrientation(90); // rotate to portrait mode

        try {
            mCamera.setPreviewTexture(surface);
            mCamera.startPreview();
        } catch (IOException ioe) {
            // Something bad happened
        }
    }

    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        // Ignored, Camera does all the work for us
    }

    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        mCamera.stopPreview();
        mCamera.release();
        return true;
    }
    // the important function
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
        // every time there is a new Camera preview frame
        mTextureView.getBitmap(bmp);

        final Canvas c = mSurfaceHolder.lockCanvas();
        int val=0;
        int ind=0;
        float btm= 0;
        float top=0;
        float COM=0;
        if (c != null) {
            int thresh = progressupdate; // comparison value
            int thresh2=progressupdate2;
            int[] pixels = new int[bmp.getWidth()]; // pixels[] is the RGBA data
            int sum_mr = 0; // the sum of the mass times the radius
            int sum_m = 0; // the sum of the masses
            int startY = 250; // which row in the bitmap to analyze to read

            bmp.getPixels(pixels, 0, bmp.getWidth(), 0, startY, bmp.getWidth(), 1);

            // in the row, see if there is more green than red
            for (int i = 0; i < bmp.getWidth(); i++) {
                if (((green(pixels[i]) - red(pixels[i])) > -thresh2)&&((green(pixels[i]) - red(pixels[i])) < thresh2)&&(green(pixels[i])  > thresh)) {
                    pixels[i] = rgb(1, 1, 1); // set the pixel to almost 100% black

                    sum_m = sum_m + green(pixels[i])+red(pixels[i])+blue(pixels[i]);
                    sum_mr = sum_mr + (green(pixels[i])+red(pixels[i])+blue(pixels[i]))*i;
                }
                /*if ((green(pixels[i]) +blue(pixels[i])+red(pixels[i])< 500+thresh) && (blue(pixels[i])<200) ) {
                    pixels[i] = rgb(0, 0, 0); // over write the pixel wth pure green
                    val = val + i;
                    ind++;
                }*/
            }
            if(sum_m>5){
                COM = sum_mr / sum_m;
            }
            else{
                COM = 0;
            }

            // update the row
            bmp.setPixels(pixels, 0, bmp.getWidth(), 0, startY, bmp.getWidth(), 1);
            //btm= (float) val/ind;
            /*
            startY=100;
            //val=0;
            //ind=0;
            bmp.getPixels(pixels, 0, bmp.getWidth(), 0, startY, bmp.getWidth(), 1);

            // in the row, see if there is more green than red
            for (int i = 10; i < bmp.getWidth()-10; i++) {
                /*if ((green(pixels[i]) +blue(pixels[i])+red(pixels[i])< 500+thresh) && (blue(pixels[i])<200)) {
                    pixels[i] = rgb(0, 0, 0); // over write the pixel with pure green
                    val = val + i;
                    ind++;
                }*/
                /*if (((green(pixels[i]) - red(pixels[i])) > -thresh2)&&((green(pixels[i]) - red(pixels[i])) < thresh2)&&(green(pixels[i])  > thresh)) {
                    pixels[i] = rgb(1, 1, 1); // set the pixel to almost 100% black

                    sum_m = sum_m + green(pixels[i])+red(pixels[i])+blue(pixels[i]);
                    sum_mr = sum_mr + (green(pixels[i])+red(pixels[i])+blue(pixels[i]))*i;
                }
            }
            if(sum_m>5){
                top = sum_mr / sum_m;
            }
            else{
                top = 0;
            }
            // update the row
            bmp.setPixels(pixels, 0, bmp.getWidth(), 0, startY, bmp.getWidth(), 1);
            */
            //top= (float) val/ind;
        }
        // draw a circle at some position
        //dif= Math.round(btm)- Math.round(top);
        //int pos1 = Math.round(top);
        //int pos2 = Math.round(btm);
        canvas.drawCircle(COM, 250, 5, paint1); // x position, y position, diameter, color
        //canvas.drawCircle(pos2, 400, 5, paint1); // x position, y position, diameter, color

        // write the pos as text
        canvas.drawText("COM = " + COM, 10, 200, paint1);
        c.drawBitmap(bmp, 0, 0, null);
        mSurfaceHolder.unlockCanvasAndPost(c);

        // calculate the FPS to see how fast the code is running
        long nowtime = System.currentTimeMillis();
        long diff = nowtime - prevtime;
        mTextView.setText("FPS " + 1000 / diff);
        prevtime = nowtime;


        String sendString = String.valueOf(COM) + '\n';
        try {
            sPort.write(sendString.getBytes(), 10); // 10 is the timeout
        } catch (IOException e) { }

    }

}
