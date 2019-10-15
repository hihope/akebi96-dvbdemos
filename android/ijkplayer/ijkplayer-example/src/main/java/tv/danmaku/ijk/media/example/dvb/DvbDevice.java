package tv.danmaku.ijk.media.example.dvb;

public class DvbDevice {
    static {
        System.loadLibrary("dvb");
    }

    private int mDeviceIdx;
    private boolean mIsOpened = false;

    public DvbDevice(int device) {
        mDeviceIdx = device;
        nativeCreate();
    }

    public boolean open() {
        if (nativeSetDeviceId(mDeviceIdx) == -1) {
            mIsOpened = false;
            return mIsOpened;
        }
        mIsOpened = true;
        return mIsOpened;
    }

    public boolean isOpened() {
        return mIsOpened;
    }

    public String getDvrPath() {
        return  String.format("/dev/dvb%d.dvr0", mDeviceIdx);
    }

    native int nativeCreate();
    native int nativeSetDeviceId(int idx);
    native int nativeReadData();
}
