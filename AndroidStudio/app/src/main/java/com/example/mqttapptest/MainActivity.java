package com.example.mqttapptest;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.widget.TextView;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.io.UnsupportedEncodingException;

public class MainActivity extends AppCompatActivity implements SensorEventListener, View.OnClickListener {

    private SensorManager manager;
    private Sensor accelerometer;
    private double xAcceleration;
    private double yAcceleration;
    private double zAcceleration;
    private double xgravity;
    private double ygravity;
    private double zgravity;
    private int lijeviklik;
    private int desniklik;
    private Button buttonStart, buttonStop;
    private Button buttonLeft, buttonRight;
    private boolean nStopLoop;

    MqttAndroidClient client;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        String clientId = MqttClient.generateClientId();
        client = new MqttAndroidClient(this.getApplicationContext(), "tcp://broker.hivemq.com:1883", clientId);
        MqttConnectOptions options = new MqttConnectOptions();

        manager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        accelerometer = manager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);

        buttonStart = findViewById(R.id.button);
        buttonStop = findViewById(R.id.button2);

        buttonStart.setOnClickListener(this);
        buttonStop.setOnClickListener(this);

        buttonLeft = findViewById(R.id.button3);
        buttonRight = findViewById(R.id.button4);

        buttonLeft.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if(event.getAction() == MotionEvent.ACTION_DOWN) {
                    lijeviklik = 1;
                } else if (event.getAction() == MotionEvent.ACTION_UP) {
                    lijeviklik = 0;
                }
                return false;
            }
        });

        buttonRight.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if(event.getAction() == MotionEvent.ACTION_DOWN) {
                    desniklik = 1;
                } else if (event.getAction() == MotionEvent.ACTION_UP) {
                    desniklik = 0;
                }
                return false;
            }
        });

        try {
            IMqttToken token = client.connect();
            token.setActionCallback(new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    Toast.makeText(MainActivity.this, "Konekcija uspostavljena", Toast.LENGTH_LONG).show();
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Toast.makeText(MainActivity.this, "Konekcija prekinuta", Toast.LENGTH_LONG).show();

                }
            });
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {

            final double alpha = 0.8;

            xgravity = alpha * xgravity + (1 - alpha) * event.values[0];
            ygravity = alpha * ygravity + (1 - alpha) * event.values[1];
            zgravity = alpha * zgravity + (1 - alpha) * event.values[2];

            xAcceleration = event.values[0] - xgravity;
            yAcceleration = event.values[1] - ygravity;
            zAcceleration = event.values[2] - zgravity;

        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }

    @Override
    protected void onStart() {
        super.onStart();
        manager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_FASTEST);
    }

    @Override
    protected void onPause() {
        super.onPause();
        manager.unregisterListener(this);
    }

    public void onClick(View view) {
        switch (view.getId()){
            case R.id.button:
                nStopLoop = true;

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        while(nStopLoop) {
                                String topic = "hajde";
                                String s = Double.toString(xAcceleration) + ", " + Double.toString(yAcceleration) + ", " + Integer.toString(lijeviklik) + ", " + Integer.toString(desniklik);
                                try {
                                    client.publish(topic, s.getBytes(), 0, false);
                                } catch (MqttException e) {
                                    e.printStackTrace();
                                }
                                try {
                                    Thread.sleep(100);
                                } catch (InterruptedException e) {
                                    e.printStackTrace();
                                }
                        }

                    }
                }).start();
                break;
            case R.id.button2:
                nStopLoop = false;
                break;
        }
    }
    @Override
    public void onPointerCaptureChanged(boolean hasCapture) {

    }
}