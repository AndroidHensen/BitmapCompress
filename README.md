## Ч����ʾ

![����дͼƬ����](http://img.blog.csdn.net/20171224123647498?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMzAzNzk2ODk=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

## ����ѹ��

����ѹ�������ݴ��ݽ�ȥ��������С������ϵͳ�Դ���ѹ���㷨����ͼƬѹ����JPEG��ʽ

```
/**
 * ����ѹ��
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

## �ߴ�ѹ��

�ߴ�ѹ��������ͼƬ�����ű������еȱȴ�С����С�ߴ磬�Ӷ��ﵽѹ����Ч��

```
/**
 * �ߴ�ѹ��
 *
 * @param bitmap
 * @param file
 */
public static void compressSize(Bitmap bitmap, File file) {
    int ratio = 8;//�ߴ�ѹ������
    Bitmap result = Bitmap.createBitmap(bitmap.getWidth() / ratio, bitmap.getHeight() / ratio, Bitmap.Config.ARGB_8888);
    Canvas canvas = new Canvas(result);
    Rect rect = new Rect(0, 0, bitmap.getWidth() / ratio, bitmap.getHeight() / ratio);
    canvas.drawBitmap(bitmap, null, rect, null);

    compressQuality(result, 100, file);
}
```

## ������ѹ��

������ѹ��������ͼƬ�Ĳ����ʴ�С����ѹ��

```
/**
 * ������ѹ��
 *
 * @param filePath
 * @param file
 */
public static void compressSample(String filePath, File file) {
    int inSampleSize = 8;//����������
    BitmapFactory.Options options = new BitmapFactory.Options();
    options.inJustDecodeBounds = false;
    options.inSampleSize = inSampleSize;
    Bitmap bitmap = BitmapFactory.decodeFile(filePath, options);

    compressQuality(bitmap, 100, file);
}
```

## LibJpegѹ��

LibJpegѹ����ͨ��Ndk����LibJpeg�����ѹ��������ԭ�е����أ������ȸ�

### ����LibJpeg

1����Github�Ͽ��������Ѿ�д�ñ���ű�����Ŀ��https://github.com/Zelex/libjpeg-turbo-android���������ϴ���Linux��������ĳ��Ŀ¼

![����дͼƬ����](http://img.blog.csdn.net/20171224115545476?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMzAzNzk2ODk=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

2����������Ŀ¼Ȩ��

```
chmod 777 -R libjpeg-turbo-android-master
```

3������libjpegĿ¼��ʹ������ָ����б��룬ǰ������ķ������Ѿ����ndk-build�������˻�������

```
ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk APP_ABI=armeabi-v7a obj/local/armeabi-v7a/libjpeg.a LOCAL_ARM_MODE=arm LOCAL_ARM_NEON=true ARCH_ARM_HAVE_NEON=true
```

4�����ű���ɹ��󣬻��� obj/local Ŀ¼������������Ҫ�� libjpeg.a

![����дͼƬ����](http://img.blog.csdn.net/20171224115511186?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMzAzNzk2ODk=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

### ��������

1������һ���µ���Ŀ����ѡ����C++����ѡC++11��C++��������

![����дͼƬ����](http://img.blog.csdn.net/20171224121527507?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMzAzNzk2ODk=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

2�������ɵ� libjpeg.a��ͷ�ļ����뵽���ǵ���Ŀ��

![����дͼƬ����](http://img.blog.csdn.net/20171224121424198?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcXFfMzAzNzk2ODk=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

3������gradle

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
                //֧�ֵ�CPU����
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
    //�޸�Libs���·��
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

4������CMake

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

5������Ȩ��

```
<uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE"/>
<uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>
```

### ʹ��LibJpeg

1������ѡ���ļ���Intent

```
/**
 * ѡ���ļ�
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

2���Է��صĽ������ѹ��

```
/**
 * ���ؽ��
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
 * ѹ��ͼƬ
 * ע�⣺�ǵ��ֶ�����Ȩ��
 *
 * @param uri
 */
public void compressImage(Uri uri) {
    try {
        File saveFile = new File(getExternalCacheDir(), "NDKѹ��.jpg");
        Bitmap bitmap = MediaStore.Images.Media.getBitmap(getContentResolver(), uri);
        int code = CompressUtils.compressBitmap(bitmap, 20, saveFile.getAbsolutePath().getBytes(), true);

        File saveFile1 = new File(getExternalCacheDir(), "����ѹ��.jpg");
        CompressUtils.compressQuality(bitmap, 20, saveFile1);

        File saveFile2 = new File(getExternalCacheDir(), "�ߴ�ѹ��.jpg");
        CompressUtils.compressSize(bitmap, saveFile2);

		//�����ʱȽ����⣬��Ҫ�����ļ���Ŀ¼���������ֱ��ָ��Ŀ¼���ļ�
        File saveFile3 = new File(getExternalCacheDir(), "������ѹ��.jpg");
        File LocalFile = new File("/storage/emulated/0/DCIM/Camera/IMG_20171216_171956.jpg");
        if (LocalFile.exists()) {
            CompressUtils.compressSample(LocalFile.getAbsolutePath(), saveFile3);
        }

    } catch (IOException e) {
        e.printStackTrace();
    }
}
```

3�����ر��ؿ������LibJpegѹ������

```
public class CompressUtils {

    static {
        System.loadLibrary("native-lib");
    }

	public static native int compressBitmap(Bitmap bitmap, int quality, byte[] fileNameBytes,
	                                            boolean optimize);
}
```

4����дLibJpeg�ı����ļ�

* ��ȡͼƬ��ARGBͨ����RGBͨ��
* ����LibJpeg��API����ѹ��
* ������д�뵽�ļ���

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
 * ����Libjpegѹ��
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

    //jpeg�Ľṹ�壬����ı�����ߡ�λ�ͼƬ��ʽ����Ϣ
    struct jpeg_compress_struct jcs;
    //�����������ļ���ʱ��ͻ�ص�my_error_exit
    struct my_error_mgr jem;
    jcs.err = jpeg_std_error(&jem.pub);
    jem.pub.error_exit = my_error_exit;
    if (setjmp(jem.setjmp_buffer)) {
        return 0;
    }
    //��ʼ��jsc�ṹ��
    jpeg_create_compress(&jcs);
    //������ļ�
    FILE* f = fopen(outfilename, "wb");
    if (f == NULL) {
        return 0;
    }
    //���ýṹ����ļ�·��
    jpeg_stdio_dest(&jcs, f);
    jcs.image_width = w;//���ÿ��
    jcs.image_height = h;
    //���ù��������룬TRUE=arithmetic coding, FALSE=Huffman
	if (optimize) {
        jcs.arith_code = false;
	} else {
        jcs.arith_code = true;
	}
    //��ɫͨ������
    int nComponent = 3;
    jcs.input_components = nComponent;
    //���ýṹ�����ɫ�ռ�ΪRGB
    jcs.in_color_space = JCS_RGB;
    //ȫ������Ĭ�ϲ���
    jpeg_set_defaults(&jcs);
    //�Ƿ���ù����������ݼ��� Ʒ�����5-10��
    jcs.optimize_coding = optimize;
    //��������
    jpeg_set_quality(&jcs, quality, true);
    //��ʼѹ����(�Ƿ�д��ȫ������)
    jpeg_start_compress(&jcs, TRUE);

    JSAMPROW row_pointer[1];
    int row_stride;
    //һ�е�RGB����
    row_stride = jcs.image_width * nComponent;
    //һ��һ�б���
    while (jcs.next_scanline < jcs.image_height) {
        //�õ�һ�е��׵�ַ
        row_pointer[0] = &data[jcs.next_scanline * row_stride];
        //�˷����Ὣjcs.next_scanline��1
        jpeg_write_scanlines(&jcs, row_pointer, 1);//row_pointer����һ�е��׵�ַ��1��д�������
    }
    jpeg_finish_compress(&jcs);
    jpeg_destroy_compress(&jcs);
    fclose(f);
    return 1;
}

/**
 * byte����תC���ַ���
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
    //��ȡBitmap��Ϣ
    AndroidBitmapInfo android_bitmap_info;
    AndroidBitmap_getInfo(env, bitmap, &android_bitmap_info);
    //��ȡbitmap�� ���ߣ�format
    int w = android_bitmap_info.width;
    int h = android_bitmap_info.height;
    int format = android_bitmap_info.format;

    if (format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        return -1;
    }

    //�洢ARGB�������ص�
    BYTE *pixelsColor;
    //1����ȡBitmap����������Ϣ
    AndroidBitmap_lockPixels(env, bitmap, (void **) &pixelsColor);
    //2������ÿ�����أ�ȥ��Aͨ����ȡ��RGBͨ��
    int i = 0, j = 0;
    BYTE a, r, g, b;
    //�洢RGB�������ص�
    BYTE *data;
    data = (BYTE *) malloc(w * h * 3);
    //�洢RGB�׵�ַ
    BYTE *tempData = data;

    int color;
    for (i = 0; i < h; ++i) {
        for (j = 0; j < w; ++j) {
            //��8λͨ��ת��32λͨ��
            color = *((int *) pixelsColor);
            //ȡֵ
            a = ((color & 0xFF000000) >> 24);
            r = ((color & 0x00FF0000) >> 16);
            g = ((color & 0x0000FF00) >> 8);
            b = ((color & 0x000000FF));
            //��ֵ
            *data = b;
            *(data + 1) = g;
            *(data + 2) = r;
            //ָ��������
            data += 3;
            pixelsColor += 4;
        }
    }

    //3����ȡ���ص����
    AndroidBitmap_unlockPixels(env, bitmap);
    char *fileName = jstrinTostring(env, fileNameBytes_);
    //4������Libjpeg����ѹ��
    int resultCode = generateJPEG(tempData, w, h, quality, fileName, optimize);
    if (resultCode == 0) {
        return 0;
    }
    return 1;
}
```

## Դ������

[Դ������](https://github.com/AndroidHensen/BitmapCompress)