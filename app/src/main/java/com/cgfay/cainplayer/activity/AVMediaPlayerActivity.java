package com.cgfay.cainplayer.activity;

import android.graphics.Bitmap;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.TextView;

import com.cgfay.media.Medadata.AVMediaMetadataRetriever;
import com.cgfay.media.MediaPlayer.AVMediaPlayer;
import com.cgfay.cainplayer.R;
import com.cgfay.cainplayer.widget.AspectRatioLayout;
import com.cgfay.utilslibrary.utils.StringUtils;

import java.lang.ref.WeakReference;

public class AVMediaPlayerActivity extends AppCompatActivity implements View.OnClickListener,
        SurfaceHolder.Callback, SeekBar.OnSeekBarChangeListener {

    private static final String TAG = "AVMediaPlayerActivity";

    private static final int MSG_UPDATE_POSITON = 0x01;

    public static final String PATH = "path";

    private String mPath;

    private boolean visible = false;
    private LinearLayout mLayoutOperation;
    private Button mBtnPause;
    private TextView mTvCurrentPosition;
    private TextView mTvDuration;
    private SeekBar mSeekBar;
    private ImageView mImageCover;
    private TextView mTextMetadata;

    private AspectRatioLayout mLayoutAspectRatio;
    private SurfaceView mSurfaceView;

    private AVMediaPlayer mMediaPlayer;

    private EventHandler mEventHandler;

    private AVMediaMetadataRetriever mMetadataRetriever;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_media_player);
        mPath = getIntent().getStringExtra(PATH);
        mEventHandler = new EventHandler(this, Looper.getMainLooper());
        initView();
        initPlayer();
        initMediaMetadataRetriever();
    }

    private void initView() {
        mLayoutAspectRatio = (AspectRatioLayout) findViewById(R.id.layout_aspect_ratio);
        mLayoutOperation = (LinearLayout) findViewById(R.id.layout_operation);
        mBtnPause = (Button) findViewById(R.id.btn_pause_play);
        mBtnPause.setOnClickListener(this);

        mTvCurrentPosition = findViewById(R.id.tv_current_position);
        mTvDuration = findViewById(R.id.tv_duration);
        mSeekBar = findViewById(R.id.seekbar);
        mSeekBar.setOnSeekBarChangeListener(this);

        mImageCover = findViewById(R.id.iv_cover);
        mTextMetadata = findViewById(R.id.tv_metadata);

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
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mLayoutAspectRatio.setAspectRatio(
                                mMediaPlayer.getVideoWidth() / (mMediaPlayer.getVideoHeight() * 1.0f));
                        mTvCurrentPosition.setText(StringUtils.generateStandardTime(Math.max(mMediaPlayer.getCurrentPosition(), 0)));
                        mTvDuration.setText(StringUtils.generateStandardTime(Math.max(mMediaPlayer.getDuration(), 0)));
                        mSeekBar.setMax((int) Math.max(mMediaPlayer.getDuration(), 0));
                        mSeekBar.setProgress((int)Math.max(mMediaPlayer.getCurrentPosition(), 0));
                    }
                });
                mEventHandler.sendEmptyMessage(MSG_UPDATE_POSITON);
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

    private void initMediaMetadataRetriever() {
        if (TextUtils.isEmpty(mPath)) {
            return;
        }
        new Thread(new Runnable() {
            @Override
            public void run() {
                // 异步截屏回调
                mMetadataRetriever = new AVMediaMetadataRetriever();
                mMetadataRetriever.setDataSource(mPath);
                final Bitmap bitmap = mMetadataRetriever.getCoverPicture();
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mImageCover.setImageBitmap(bitmap);
                        mTextMetadata.setText(mMetadataRetriever.getMetadata().toString());
                    }
                });
            }
        }).start();

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
        if (mMediaPlayer != null) {
            mMediaPlayer.stop();
            mMediaPlayer.release();
            mMediaPlayer = null;
        }
        mEventHandler.removeCallbacksAndMessages(null);
        mEventHandler = null;
        super.onDestroy();
    }

    @Override
    public void onClick(View v) {
        int id = v.getId();
        if (id == R.id.btn_pause_play) {
            if (mMediaPlayer.isPlaying()) {
                mMediaPlayer.pause();
                mBtnPause.setBackgroundResource(R.drawable.ic_player_play);
            } else {
                mMediaPlayer.resume();
                mBtnPause.setBackgroundResource(R.drawable.ic_player_pause);
            }
        }
    }

    private int mProgress;
    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        if (fromUser) {
            mProgress = progress;
            Log.d(TAG, "onProgressChanged: progress = " + progress);
        }
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        if (mMediaPlayer != null) {
            mMediaPlayer.seekTo(mProgress);
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        if (mMediaPlayer != null) {
            mMediaPlayer.setDisplay(holder);
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        if (mMediaPlayer != null) {
            mMediaPlayer.surfaceChanged(width, height);
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        if (mMediaPlayer != null) {
            mMediaPlayer.surfaceDestroyed();
        }
    }

    /**
     * process Event at Main Looper
     */
    private class EventHandler extends Handler {

        private WeakReference<AVMediaPlayerActivity> mWeakActivity;

        public EventHandler(AVMediaPlayerActivity activity, Looper looper) {
            super(looper);
            mWeakActivity = new WeakReference<>(activity);
        }

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_UPDATE_POSITON: {
                    final long currentPosition;
                    final long duration;
                    if (mWeakActivity.get() != null && mWeakActivity.get().mMediaPlayer != null) {
                        currentPosition = mWeakActivity.get().mMediaPlayer.getCurrentPosition();
                        duration = mWeakActivity.get().mMediaPlayer.getDuration();
                    } else {
                        currentPosition = 0;
                        duration = 0;
                    }

                    if (getLooper() == Looper.getMainLooper()) {
                        if (mWeakActivity.get() != null) {
                            mWeakActivity.get().mTvCurrentPosition.setText(StringUtils.generateStandardTime(currentPosition));
                            mWeakActivity.get().mSeekBar.setProgress((int) currentPosition);
                        }
                    } else {
                        if (mWeakActivity.get() != null) {
                            mWeakActivity.get().runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    if (mWeakActivity.get() != null) {
                                        mWeakActivity.get().mTvCurrentPosition.setText(StringUtils.generateStandardTime(currentPosition));
                                        mWeakActivity.get().mSeekBar.setProgress((int) currentPosition);
                                    }
                                }
                            });
                        }
                    }
                    sendEmptyMessageDelayed(MSG_UPDATE_POSITON, 300);
                    break;
                }

                default: {
                    break;
                }
            }
        }
    }

}
