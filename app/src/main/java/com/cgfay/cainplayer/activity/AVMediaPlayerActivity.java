package com.cgfay.cainplayer.activity;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
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

import com.cgfay.media.CainMediaMetadataRetriever;
import com.cgfay.media.CainMediaPlayer;
import com.cgfay.cainplayer.R;
import com.cgfay.cainplayer.widget.AspectRatioLayout;
import com.cgfay.media.CainMetadata;
import com.cgfay.media.IMediaPlayer;
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

    private CainMediaPlayer mCainMediaPlayer;


    private CainMediaMetadataRetriever mMetadataRetriever;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_media_player);
        mPath = getIntent().getStringExtra(PATH);
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
        mCainMediaPlayer = new CainMediaPlayer();
        try {
            mCainMediaPlayer.setDataSource(mPath);
        } catch (Exception e) {

        }
        mCainMediaPlayer.setOnPreparedListener(new IMediaPlayer.OnPreparedListener() {
            @Override
            public void onPrepared(IMediaPlayer mp) {
                Log.d(TAG, "onPrepared: ");
                mCainMediaPlayer.start();
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mLayoutAspectRatio.setAspectRatio(
                                mCainMediaPlayer.getVideoWidth() * 1.0f / (mCainMediaPlayer.getVideoHeight() * 1.0f));
                        mTvCurrentPosition.setText(StringUtils.generateStandardTime(Math.max(mCainMediaPlayer.getCurrentPosition(), 0)));
                        mTvDuration.setText(StringUtils.generateStandardTime(Math.max(mCainMediaPlayer.getDuration(), 0)));
                        mSeekBar.setMax((int) Math.max(mCainMediaPlayer.getDuration(), 0));
                        mSeekBar.setProgress((int)Math.max(mCainMediaPlayer.getCurrentPosition(), 0));
                    }
                });
            }
        });
        mCainMediaPlayer.setOnErrorListener(new IMediaPlayer.OnErrorListener() {
            @Override
            public boolean onError(IMediaPlayer mp, int what, int extra) {
                Log.d(TAG, "onError: what = " + what + ", msg = " + extra);
                return false;
            }
        });
        mCainMediaPlayer.setOnCompletionListener(new IMediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(IMediaPlayer mp) {

            }
        });
        mCainMediaPlayer.setOnCurrentPositionListener(new CainMediaPlayer.OnCurrentPositionListener() {
            @Override
            public void onCurrentPosition(final long current, final long duration) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mTvCurrentPosition.setText(StringUtils.generateStandardTime(current));
                        mTvDuration.setText(StringUtils.generateStandardTime(duration));
                    }
                });
            }
        });
        try {
            mCainMediaPlayer.setOption(CainMediaPlayer.OPT_CATEGORY_PLAYER, "vcodec", "h264_mediacodec");
            mCainMediaPlayer.prepareAsync();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void initMediaMetadataRetriever() {
        if (TextUtils.isEmpty(mPath)) {
            return;
        }
        new Thread(new Runnable() {
            @Override
            public void run() {
                // 异步截屏回调
                mMetadataRetriever = new CainMediaMetadataRetriever();
                mMetadataRetriever.setDataSource(mPath);
                byte[] data = mMetadataRetriever.getEmbeddedPicture();
                if (data != null) {
                    final Bitmap bitmap = BitmapFactory.decodeByteArray(data, 0, data.length);
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mImageCover.setImageBitmap(bitmap);
                        }
                    });
                }

                final CainMetadata metadata = mMetadataRetriever.getMetadata();
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (metadata != null) {
                            mTextMetadata.setText(metadata.toString());
                        }
                    }
                });
            }
        }).start();

    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mCainMediaPlayer != null) {
            mCainMediaPlayer.pause();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mCainMediaPlayer != null) {
            mCainMediaPlayer.resume();
        }
    }

    @Override
    protected void onDestroy() {
        if (mCainMediaPlayer != null) {
            mCainMediaPlayer.stop();
            mCainMediaPlayer.release();
            mCainMediaPlayer = null;
        }
        if (mMetadataRetriever != null) {
            mMetadataRetriever.release();
            mMetadataRetriever = null;
        }
        super.onDestroy();
    }

    @Override
    public void onClick(View v) {
        int id = v.getId();
        if (id == R.id.btn_pause_play) {
            if (mCainMediaPlayer.isPlaying()) {
                mCainMediaPlayer.pause();
                mBtnPause.setBackgroundResource(R.drawable.ic_player_play);
            } else {
                mCainMediaPlayer.resume();
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
        if (mCainMediaPlayer != null) {
            mCainMediaPlayer.seekTo(mProgress);
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        if (mCainMediaPlayer != null) {
            mCainMediaPlayer.setDisplay(holder);
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }



}
