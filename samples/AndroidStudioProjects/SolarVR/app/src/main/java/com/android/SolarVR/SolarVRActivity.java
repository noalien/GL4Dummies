/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.SolarVR;

import android.app.Activity;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.view.KeyEvent;

import java.util.ArrayDeque;


public class SolarVRActivity extends Activity implements SensorEventListener {
    private SolarVRView mView;
    private SensorManager mSensorManager;
    private Sensor mAccelerometer;
    private Sensor mMagnetometer;
    private float[] mGravity;
    private float[] mGeomagnetic;
    private OrientationSmoother mOS;
    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        mView = new SolarVRView(getApplication());
	    setContentView(mView);
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mMagnetometer = mSensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
        mOS = new OrientationSmoother();
    }

    @Override protected void onPause() {
        super.onPause();
        mView.onPause();
        mSensorManager.unregisterListener(this);
    }

    @Override protected void onResume() {
        super.onResume();
        mView.onResume();
        mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_FASTEST);
        mSensorManager.registerListener(this, mMagnetometer, SensorManager.SENSOR_DELAY_FASTEST);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        //System.out.println("Hello Key : " + keyCode);
        return SolarVRLib.key(keyCode, 0) != 0 ? true : super.onKeyDown(keyCode, event);
    }
    protected class OrientationSmoother {
        private final int mMaxOrientations = 25;
        private ArrayDeque<float[]> mOrientations = new ArrayDeque<float[]>();
        private float[] mWeights = new float[mMaxOrientations];
        private float mWeightsSum;
        public OrientationSmoother() {
            int i;
            float d, si = 2.0f;
            for(i = 0, mWeightsSum = 0.0f; i < mWeights.length; i++) {
                d = 4.0f * i / (float)mWeights.length;
                mWeights[i] = (float)((1.0 / (si * Math.sqrt(2.0 * Math.PI))) * Math.exp(-d * d / (2.0 * si * si)));
                mWeightsSum += mWeights[i];
            }
        }
        protected void addAndSmoothOrientations(float[] orientation) {
            int i;
            double dx[] = new double[3], dy[] = new double[3];
            mOrientations.addLast(orientation.clone());
            if(mOrientations.size() > mMaxOrientations)
                mOrientations.removeFirst();
            i = 0;
            for(float[] e:mOrientations) {
                dx[0] += mWeights[i] * Math.cos(e[0]);
                dx[1] += mWeights[i] * Math.cos(e[1]);
                dx[2] += mWeights[i] * Math.cos(e[2]);
                dy[0] += mWeights[i] * Math.sin(e[0]);
                dy[1] += mWeights[i] * Math.sin(e[1]);
                dy[2] += mWeights[i] * Math.sin(e[2]);
            }
            dx[0] /= mWeightsSum; dx[1] /= mWeightsSum; dx[2] /= mWeightsSum;
            dy[0] /= mWeightsSum; dy[1] /= mWeightsSum; dy[2] /= mWeightsSum;
            orientation[0] = (float)Math.atan2(dy[0], dx[0]);
            orientation[1] = (float)Math.atan2(dy[1], dx[1]);
            orientation[2] = (float)Math.atan2(dy[2], dx[2]);
        }
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            mGravity = event.values.clone();
        }
        if (event.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD) {
            mGeomagnetic = event.values.clone();
        }
        if (mGravity != null && mGeomagnetic != null) {
            //System.out.println(mGravity.length);
            //System.out.println(mGeomagnetic.length);
            float R[] = new float[9];
            float I[] = new float[9];
            boolean success = SensorManager.getRotationMatrix(R, I, mGravity, mGeomagnetic);
            if (success) {
                float orientation[] = new float[3];
                SensorManager.getOrientation(R, orientation);
                mOS.addAndSmoothOrientations(orientation);
                SolarVRLib.updateOrientation((float)-Math.PI / 2.0f - orientation[2], (float) Math.PI + orientation[0], (float)-orientation[1]);
            }
        }

    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }
}
