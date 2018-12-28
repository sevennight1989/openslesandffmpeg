package aa.opensles;

import android.Manifest;
import android.content.res.AssetManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import com.hujiang.permissiondispatcher.CheckPermission;
import com.hujiang.permissiondispatcher.PermissionItem;
import com.hujiang.permissiondispatcher.PermissionListener;

public class MainActivity extends AppCompatActivity {

    public static final String TAG = "PengLog";

    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        PermissionItem permissionItem = new PermissionItem(Manifest.permission.WRITE_EXTERNAL_STORAGE,Manifest.permission.RECORD_AUDIO);
        permissionItem.needGotoSetting(true)
                .settingText("进入设置")
                .deniedButton("取消")
                .deniedMessage("你可以进入设置->权限管理界面去重新开启该权限");
        CheckPermission.instance(this).check(permissionItem, new PermissionListener() {
            @Override
            public void permissionGranted() {
                //获取到权限时执行正常业务逻辑
            }

            @Override
            public void permissionDenied() {
                //权限被拒绝时给用户友好提示
            }
        });
        avcodeConfig();
        avfilterinfo();
    }

    public void onClick(View v){
        switch (v.getId()){
            case R.id.run:
                run();
                break;
            case R.id.quit:
                quit();
                break;
            case R.id.callback:
                callback();
                break;

            case R.id.playAsset:
                playAssetResource(getAssets(),"rd111.3gpp");
                break;

            case R.id.stopAsset:
                stopAssetResource();
                break;
        }
    }

    public native void run();
    public native void quit();
    public native void callback();
    public native void playAssetResource(AssetManager assetManager,String fileName);
    public native void stopAssetResource();
    public native String avcodeConfig();
    public native String avfilterinfo();
    public native String protocolinfo();
    public native String avformatinfo();
    public native String avcodecinfo();
    public void onError(int code,String msg){
        Log.d(TAG,"[ code = " + code +"    msg = " + msg +" ]");
    }

}
