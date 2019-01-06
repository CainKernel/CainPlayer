package com.cgfay.media.activity;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;

import com.cgfay.media.MediaPlayer.AVMediaPlayer;
import com.cgfay.media.R;
import com.cgfay.media.widget.AspectRatioLayout;

public class AVMediaPlayerActivity extends AppCompatActivity implements View.OnClickListener, SurfaceHolder.Callback {

    private static final String TAG = "AVMediaPlayerActivity";

    public static final String PATH = "path";

    private String mPath;

    private boolean visible = false;
    private LinearLayout mLayoutOperation;
    private Button mBtnPause;
    private Button mBtnPlay;

    private AspectRatioLayout mLayoutAspectRatio;
    private SurfaceView mSurfaceView;

    private AVMediaPlayer mMediaPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_media_player);
        mPath = getIntent().getStringExtra(PATH);
        initView();
        initPlayer();
    }

    private void initView() {
        mLayoutAspectRatio = (AspectRatioLayout) findViewById(R.id.layout_aspect_ratio);
        mLayoutOperation = (LinearLayout) findViewById(R.id.layout_operation);
        mBtnPause = (Button) findViewById(R.id.btn_pause);
        mBtnPause.setOnClickListener(this);
        mBtnPlay = (Button) findViewById(R.id.btn_play);
        mBtnPlay.setOnClickListener(this);

        findViewById(R.id.btn_seek).setOnClickListener(this);

        mSurfaceView = (SurfaceView) findViewById(R.id.surfaceView);
        mSurfaceView.getHolder().addCallback(this);
        mSurfaceView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (event.getAction()==(MotionEvent.ACTION_DOWN)) {
                    if (visible) {
                        mLayoutOperation.setVisibility(View.VISIBLE);
                    } else {
                        mLayoutOperation.setVisibility(View.GONE);
                    }
                    visible = !visible;
                }
                return true;
            }
        });
    }

    private void initPlayer() {
        if (TextUtils.isEmpty(mPath)) {
            return;
        }
        mMediaPlayer = new AVMediaPlayer();
        mMediaPlayer.setDataSource(mPath);
        mMediaPlayer.setOnPreparedListener(new AVMediaPlayer.OnPreparedListener() {
            @Override
            public void onPrepared() {
                Log.d(TAG, "onPrepared: ");
                mMediaPlayer.start();
                if (mMediaPlayer.getVideoHeight() != 0) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mLayoutAspectRatio.setAspectRatio(
                                    mMediaPlayer.getVideoWidth() / (mMediaPlayer.getVideoHeight() * 1.0f));
                        }
                    });
                }
            }
        });
        mMediaPlayer.setOnErrorListener(new AVMediaPlayer.OnErrorListener() {
            @Override
            public void onError(int code, String msg) {
                Log.d(TAG, "onError: code = " + code + ", msg = " + msg);
            }
        });
        mMediaPlayer.setOnCompletionListener(new AVMediaPlayer.OnCompletionListener() {
            @Override
            public void onCompleted() {

            }
        });
        mMediaPlayer.prepare();
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mMediaPlayer != null) {
            mMediaPlayer.pause();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mMediaPlayer != null) {
            mMediaPlayer.resume();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mMediaPlayer != null) {
            mMediaPlayer.stop();
            mMediaPlayer.release();
            mMediaPlayer = null;
        }
    }

    @Override
    public void onClick(View v) {
        int id = v.getId();
        if (id == R.id.btn_pause) {
            mMediaPlayer.pause();
        } else if (id == R.id.btn_play) {
            mMediaPlayer.start();
        } else if (id == R.id.btn_seek) {
            mMediaPlayer.seekTo(20000);
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        if (mMediaPlayer != null) {
            mMediaPlayer.setSurface(holder.getSurface());
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        if (mMediaPlayer != null) {
            mMediaPlayer.setSurface(null);
        }
    }
}
