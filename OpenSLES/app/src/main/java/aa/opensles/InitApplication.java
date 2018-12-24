package aa.opensles;

import android.Manifest;
import android.app.Application;

import com.firefly1126.permissionaspect.CheckPermissionItem;
import com.firefly1126.permissionaspect.PermissionCheckSDK;

public class InitApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        PermissionCheckSDK.init(this);
        String[] permissions = {Manifest.permission.WRITE_EXTERNAL_STORAGE};

    }
}
