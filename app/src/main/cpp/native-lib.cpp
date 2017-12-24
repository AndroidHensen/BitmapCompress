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
