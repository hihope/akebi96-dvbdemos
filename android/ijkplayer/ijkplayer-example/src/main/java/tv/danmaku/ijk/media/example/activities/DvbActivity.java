package tv.danmaku.ijk.media.example.activities;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.widget.Toast;

import tv.danmaku.ijk.media.example.R;
import tv.danmaku.ijk.media.example.application.AppActivity;
import tv.danmaku.ijk.media.example.dvb.DvbDevice;

public class DvbActivity extends AppActivity {

    private static final int Unknow = 0;
    private static final int MSG_DVB_OPEN_FAILED = 1;
    private static final int MSG_DVB_OPEN_SUCCESS = 2;

    private DvbDevice mDvbDevice;

    private Handler mHander = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message message) {
            switch (message.what) {
                case MSG_DVB_OPEN_FAILED:
                    Toast.makeText(DvbActivity.this, "Open dvb failed", Toast.LENGTH_LONG);
                    break;
                case MSG_DVB_OPEN_SUCCESS:
                    VideoActivity.intentTo(DvbActivity.this, mDvbDevice.getDvrPath(), "dvb");
                    break;
                case Unknow:
                    break;
            }
            return true;
        }
    });


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dvb);
        initDvb();
    }

    private void initDvb() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                mDvbDevice = new DvbDevice(0);
                if (mDvbDevice.open()) {
                    mHander.sendEmptyMessage(MSG_DVB_OPEN_SUCCESS);
                } else {
                    mHander.sendEmptyMessage(MSG_DVB_OPEN_FAILED);
                };
            }
        }).start();
    }
}
