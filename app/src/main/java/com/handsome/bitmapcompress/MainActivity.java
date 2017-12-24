package com.handsome.bitmapcompress;

import android.Manifest;
import android.annotation.TargetApi;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Build;
import android.provider.MediaStore;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

import java.io.File;
import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    private static final int REQUEST_PICK_IMAGE = 0x0011;
    private static final int REQUEST_KITKAT_PICK_IMAGE = 0x0012;
    private static final int REQUEST_PERMISSION_CODE = 0x0013;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    /**
     * 点击事件
     *
     * @param v
     */
    public void pickFromGallery(View v) {
        checkPermission();
    }

    /**
     * 申请权限
     */
    public void checkPermission() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},
                    REQUEST_PERMISSION_CODE);
        } else {
            selectFile();
        }
    }

    /**
     * 选择文件
     */
    public void selectFile() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.KITKAT) {
            startActivityForResult(new Intent(Intent.ACTION_GET_CONTENT).setType("image/*"), REQUEST_PICK_IMAGE);
        } else {
            Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
            intent.addCategory(Intent.CATEGORY_OPENABLE);
            intent.setType("image/*");
            startActivityForResult(intent, REQUEST_KITKAT_PICK_IMAGE);
        }
    }

    /**
     * 返回结果
     *
     * @param requestCode
     * @param resultCode
     * @param data
     */
    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode == RESULT_OK) {
            switch (requestCode) {
                case REQUEST_PICK_IMAGE:
                    if (data != null) {
                        Uri uri = data.getData();
                        compressImage(uri);
                    }
                    break;
                case REQUEST_KITKAT_PICK_IMAGE:
                    if (data != null) {
                        Uri uri = ensureUriPermission(this, data);
                        compressImage(uri);
                    }
                    break;
            }
        }
    }

    /**
     * 分配临时权限，仅适配机型作用
     *
     * @param context
     * @param intent
     * @return
     */
    @SuppressWarnings("ResourceType")
    @TargetApi(Build.VERSION_CODES.KITKAT)
    public static Uri ensureUriPermission(Context context, Intent intent) {
        Uri uri = intent.getData();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            final int takeFlags = intent.getFlags() & Intent.FLAG_GRANT_READ_URI_PERMISSION;
            context.getContentResolver().takePersistableUriPermission(uri, takeFlags);
        }
        return uri;
    }

    /**
     * 压缩图片
     * 注意：记得手动开启权限
     *
     * @param uri
     */
    public void compressImage(Uri uri) {
        try {
            File saveFile = new File(getExternalCacheDir(), "NDK压缩.jpg");
            Bitmap bitmap = MediaStore.Images.Media.getBitmap(getContentResolver(), uri);
            int code = CompressUtils.compressBitmap(bitmap, 20, saveFile.getAbsolutePath().getBytes(), true);

            File saveFile1 = new File(getExternalCacheDir(), "质量压缩.jpg");
            CompressUtils.compressQuality(bitmap, 20, saveFile1);

            File saveFile2 = new File(getExternalCacheDir(), "尺寸压缩.jpg");
            CompressUtils.compressSize(bitmap, saveFile2);

            //采样率比较特殊，需要传递文件的目录，这里采用直接指定目录的文件
            File saveFile3 = new File(getExternalCacheDir(), "采样率压缩.jpg");
            File LocalFile = new File("/storage/emulated/0/DCIM/Camera/IMG_20171216_171956.jpg");
            if (LocalFile.exists()) {
                CompressUtils.compressSample(LocalFile.getAbsolutePath(), saveFile3);
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * 权限回调
     *
     * @param requestCode
     * @param permissions
     * @param grantResults
     */
    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        if (requestCode == REQUEST_PERMISSION_CODE) {
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                selectFile();
            } else {
                Toast.makeText(MainActivity.this, "Permission Denied", Toast.LENGTH_SHORT).show();
            }
            return;
        }
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }
}
