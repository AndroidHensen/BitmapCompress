## 效果演示

![这里写图片描述](http://img.blog.csdn.net/20171224123647498?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMzAzNzk2ODk=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

## 质量压缩

质量压缩：根据传递进去的质量大小，采用系统自带的压缩算法，将图片压缩成JPEG格式

```
/**
 * 质量压缩
 *
 * @param bitmap
 * @param quality
 * @param file
 */
public static void compressQuality(Bitmap bitmap, int quality, File file) {
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    bitmap.compress(Bitmap.CompressFormat.JPEG, quality, baos);
    try {
        FileOutputStream fos = new FileOutputStream(file);
        fos.write(baos.toByteArray());
        fos.flush();
        fos.close();
    } catch (Exception e) {
        e.printStackTrace();
    }
}
```

## 尺寸压缩

尺寸压缩：根据图片的缩放比例进行等比大小的缩小尺寸，从而达到压缩的效果

```
/**
 * 尺寸压缩
 *
 * @param bitmap
 * @param file
 */
public static void compressSize(Bitmap bitmap, File file) {
    int ratio = 8;//尺寸压缩比例
    Bitmap result = Bitmap.createBitmap(bitmap.getWidth() / ratio, bitmap.getHeight() / ratio, Bitmap.Config.ARGB_8888);
    Canvas canvas = new Canvas(result);
    Rect rect = new Rect(0, 0, bitmap.getWidth() / ratio, bitmap.getHeight() / ratio);
    canvas.drawBitmap(bitmap, null, rect, null);

    compressQuality(result, 100, file);
}
```

## 采样率压缩

采样率压缩：根据图片的采样率大小进行压缩

```
/**
 * 采样率压缩
 *
 * @param filePath
 * @param file
 */
public static void compressSample(String filePath, File file) {
    int inSampleSize = 8;//采样率设置
    BitmapFactory.Options options = new BitmapFactory.Options();
    options.inJustDecodeBounds = false;
    options.inSampleSize = inSampleSize;
    Bitmap bitmap = BitmapFactory.decodeFile(filePath, options);

    compressQuality(bitmap, 100, file);
}
```

## LibJpeg压缩

LibJpeg压缩：通过Ndk调用LibJpeg库进行压缩，保留原有的像素，清晰度高

### 编译LibJpeg

1、从Github上可以下载已经写好编译脚本的项目：https://github.com/Zelex/libjpeg-turbo-android，并将其上传到Linux服务器的某个目录

![这里写图片描述](http://img.blog.csdn.net/20171224115545476?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMzAzNzk2ODk=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

2、授予整个目录权限

```
chmod 777 -R libjpeg-turbo-android-master
```

3、进入libjpeg目录，使用下面指令进行编译，前提是你的服务器已经搭建了ndk-build和配置了环境变量

```
ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk APP_ABI=armeabi-v7a obj/local/armeabi-v7a/libjpeg.a LOCAL_ARM_MODE=arm LOCAL_ARM_NEON=true ARCH_ARM_HAVE_NEON=true
```

4、接着编译成功后，会在 obj/local 目录下生成我们需要的 libjpeg.a

![这里写图片描述](http://img.blog.csdn.net/20171224115511186?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMzAzNzk2ODk=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

### 创建工程

1、创建一个新的项目，勾选包含C++，勾选C++11和C++的依赖库

![这里写图片描述](http://img.blog.csdn.net/20171224121527507?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMzAzNzk2ODk=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

2、将生成的 libjpeg.a和头文件导入到我们的项目中

![这里写图片描述](http://img.blog.csdn.net/20171224121424198?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMzAzNzk2ODk=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

3、配置gradle

```
android {
    compileSdkVersion 25
    buildToolsVersion "25.0.3"
    defaultConfig {
        applicationId "com.handsome.bitmapcompress"
        minSdkVersion 16
        targetSdkVersion 25
        versionCode 1
        versionName "1.0"
        testInstrumentationRunner "android.support.test.runner.AndroidJUnitRunner"
        externalNativeBuild {
            cmake {
                cppFlags "-std=c++11 -frtti -fexceptions"
                //支持的CPU类型
                abiFilters "armeabi", "armeabi-v7a"
            }
        }
    }
    buildTypes {
        release {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android.txt'), 'proguard-rules.pro'
        }
    }
    //修改Libs库的路径
    sourceSets.main {
        jniLibs.srcDirs = ['libs']
        jni.srcDirs = []
    }
    externalNativeBuild {
        cmake {
            path "CMakeLists.txt"
        }
    }
}
```

4、配置CMake

```
cmake_minimum_required(VERSION 3.4.1)

include_directories(./libs/jpeg)
link_directories(./libs/${ANDROID_ABI})

find_library(log-lib
             log)
find_library(android-lib
             android)
find_library(bitmap-lib
             jnigraphics)


add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.cpp )

target_link_libraries( native-lib
                       ${log-lib}
                       ${android-lib}
                       ${bitmap-lib}
                       jpeg )
```

5、声明权限

```
<uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE"/>
<uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>
```

### 使用LibJpeg

1、启动选择文件的Intent

```
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
```

2、对返回的结果进行压缩

```
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
```

3、加载本地库和声明LibJpeg压缩方法

```
public class CompressUtils {

    static {
        System.loadLibrary("native-lib");
    }

	public static native int compressBitmap(Bitmap bitmap, int quality, byte[] fileNameBytes,
	                                            boolean optimize);
}
```

4、编写LibJpeg的本地文件

* 提取图片的ARGB通量的RGB通量
* 采用LibJpeg的API进行压缩
* 将数据写入到文件中

```
#include <jni.h>
#include <string>
#include <android/bitmap.h>
#include <android/log.h>
#include <setjmp.h>

extern "C" {
#include "jpeglib.h"
#include "cdjpeg.h"
}

#define LOG_TAG "jni"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

typedef uint8_t BYTE;

typedef struct my_error_mgr *my_error_ptr;
struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

METHODDEF(void)
my_error_exit(j_common_ptr cinfo) {
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    LOGE("jpeg_message_table[%d]:%s", myerr->pub.msg_code,
         myerr->pub.jpeg_message_table[myerr->pub.msg_code]);
    longjmp(myerr->setjmp_buffer, 1);
}

/**
 * 采用Libjpeg压缩
 * @param data
 * @param w
 * @param h
 * @param quality
 * @param outfilename
 * @param optimize
 * @return
 */
int generateJPEG(BYTE *data, int w, int h, int quality,
                 const char *outfilename, jboolean optimize) {

    //jpeg的结构体，保存的比如宽、高、位深、图片格式等信息
    struct jpeg_compress_struct jcs;
    //当读完整个文件的时候就会回调my_error_exit
    struct my_error_mgr jem;
    jcs.err = jpeg_std_error(&jem.pub);
    jem.pub.error_exit = my_error_exit;
    if (setjmp(jem.setjmp_buffer)) {
        return 0;
    }
    //初始化jsc结构体
    jpeg_create_compress(&jcs);
    //打开输出文件
    FILE* f = fopen(outfilename, "wb");
    if (f == NULL) {
        return 0;
    }
    //设置结构体的文件路径
    jpeg_stdio_dest(&jcs, f);
    jcs.image_width = w;//设置宽高
    jcs.image_height = h;
    //设置哈夫曼编码，TRUE=arithmetic coding, FALSE=Huffman
	if (optimize) {
        jcs.arith_code = false;
	} else {
        jcs.arith_code = true;
	}
    //颜色通道数量
    int nComponent = 3;
    jcs.input_components = nComponent;
    //设置结构体的颜色空间为RGB
    jcs.in_color_space = JCS_RGB;
    //全部设置默认参数
    jpeg_set_defaults(&jcs);
    //是否采用哈弗曼表数据计算 品质相差5-10倍
    jcs.optimize_coding = optimize;
    //设置质量
    jpeg_set_quality(&jcs, quality, true);
    //开始压缩，(是否写入全部像素)
    jpeg_start_compress(&jcs, TRUE);

    JSAMPROW row_pointer[1];
    int row_stride;
    //一行的RGB数量
    row_stride = jcs.image_width * nComponent;
    //一行一行遍历
    while (jcs.next_scanline < jcs.image_height) {
        //得到一行的首地址
        row_pointer[0] = &data[jcs.next_scanline * row_stride];
        //此方法会将jcs.next_scanline加1
        jpeg_write_scanlines(&jcs, row_pointer, 1);//row_pointer就是一行的首地址，1：写入的行数
    }
    jpeg_finish_compress(&jcs);
    jpeg_destroy_compress(&jcs);
    fclose(f);
    return 1;
}

/**
 * byte数组转C的字符串
 */
char *jstrinTostring(JNIEnv *env, jbyteArray barr) {
    char *rtn = NULL;
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, 0);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_handsome_bitmapcompress_CompressUtils_compressBitmap(JNIEnv *env, jclass type,
                                                              jobject bitmap,
                                                              jint quality,
                                                              jbyteArray fileNameBytes_,
                                                              jboolean optimize) {
    //获取Bitmap信息
    AndroidBitmapInfo android_bitmap_info;
    AndroidBitmap_getInfo(env, bitmap, &android_bitmap_info);
    //获取bitmap的 宽，高，format
    int w = android_bitmap_info.width;
    int h = android_bitmap_info.height;
    int format = android_bitmap_info.format;

    if (format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        return -1;
    }

    //存储ARGB所有像素点
    BYTE *pixelsColor;
    //1、读取Bitmap所有像素信息
    AndroidBitmap_lockPixels(env, bitmap, (void **) &pixelsColor);
    //2、解析每个像素，去除A通量，取出RGB通量
    int i = 0, j = 0;
    BYTE a, r, g, b;
    //存储RGB所有像素点
    BYTE *data;
    data = (BYTE *) malloc(w * h * 3);
    //存储RGB首地址
    BYTE *tempData = data;

    int color;
    for (i = 0; i < h; ++i) {
        for (j = 0; j < w; ++j) {
            //将8位通道转成32位通道
            color = *((int *) pixelsColor);
            //取值
            a = ((color & 0xFF000000) >> 24);
            r = ((color & 0x00FF0000) >> 16);
            g = ((color & 0x0000FF00) >> 8);
            b = ((color & 0x000000FF));
            //赋值
            *data = b;
            *(data + 1) = g;
            *(data + 2) = r;
            //指针往后移
            data += 3;
            pixelsColor += 4;
        }
    }

    //3、读取像素点完毕
    AndroidBitmap_unlockPixels(env, bitmap);
    char *fileName = jstrinTostring(env, fileNameBytes_);
    //4、采用Libjpeg进行压缩
    int resultCode = generateJPEG(tempData, w, h, quality, fileName, optimize);
    if (resultCode == 0) {
        return 0;
    }
    return 1;
}
```

## 源码下载

[源码下载](https://github.com/AndroidHensen/BitmapCompress)