#include <jni.h>
#include <opencv2/opencv.hpp>
#include <android/log.h>
#include <algorithm>

using namespace cv;
using namespace std;

/*
extern "C"
JNIEXPORT void JNICALL
Java_com_example_opencv_1face_1detection_MainActivity_ConvertRGBtoGray(JNIEnv *env, jobject thiz,
                                                                       jlong mat_addr_input,
                                                                       jlong mat_addr_result) {
    // TODO: implement ConvertRGBtoGray()
    Mat &matInput = *(Mat *)mat_addr_input;
    Mat &matResult = *(Mat *)mat_addr_result;

    cvtColor(matInput, matResult, COLOR_RGBA2GRAY);

}*/

float resize(Mat img_src, Mat &img_resize, int resize_width) {
    float scale = resize_width / (float)img_src.cols ;
    if (img_src.cols > resize_width) {
        int new_height = cvRound(img_src.rows * scale);
        resize(img_src, img_resize, Size(resize_width, new_height));
    }
    else {
        img_resize = img_src;
    }
    return scale;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_opencv_1face_1detection_MainActivity_loadCascade(JNIEnv *env, jobject thiz,
                                                                  jstring cascade_file_name) {
    // TODO: implement loadCascade()
    const char *nativeFileNameString = env->GetStringUTFChars(cascade_file_name, 0);
    string baseDir("/storage/emulated/0/");
    baseDir.append(nativeFileNameString);
    const char *pathDir = baseDir.c_str();
    jlong ret = 0;
    ret = (jlong) new CascadeClassifier(pathDir);
    if (((CascadeClassifier *) ret)->empty()) {
        __android_log_print(ANDROID_LOG_DEBUG, "native-lib :: ",
                            "CascadeClassifier로 로딩 실패  %s", nativeFileNameString);
    }
    else
        __android_log_print(ANDROID_LOG_DEBUG, "native-lib :: ",
                            "CascadeClassifier로 로딩 성공 %s", nativeFileNameString);
    env->ReleaseStringUTFChars(cascade_file_name, nativeFileNameString);
    return ret;
}

extern "C"
JNIEXPORT jdouble JNICALL
Java_com_example_opencv_1face_1detection_MainActivity_detect(JNIEnv *env, jobject thiz,
                                                             jlong cascade_classifier_face,
                                                             jlong cascade_classifier_eye,
                                                             jlong mat_addr_input,
                                                             jlong mat_addr_result) {
    // TODO: implement detect()
    Mat &img_input = *(Mat *) mat_addr_input;
    Mat &img_result = *(Mat *) mat_addr_result;

    int ret = 0;
    img_result = img_input.clone();
    std::vector<Rect> faces;
    Mat img_gray;
    cvtColor(img_input, img_gray, COLOR_BGR2GRAY);
    equalizeHist(img_gray, img_gray);
    Mat img_resize;
    float resizeRatio = resize(img_gray, img_resize, 640);
    //-- Detect faces
    ((CascadeClassifier *) cascade_classifier_face)->detectMultiScale( img_resize, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );
    __android_log_print(ANDROID_LOG_DEBUG, (char *) "native-lib :: ", (char *) "face %d found ", faces.size());
    ret = faces.size();
    //for (int i = 0; i < faces.size(); i++) { 하나만 다루기 위해 잠시 주석처리함
    double_t pixel_avg = -1;
    double pi = 2 * acos(0.0);
    for (int i = 0; i < std::min(1, (int)faces.size()); i++) {
        double real_facesize_x = faces[i].x / resizeRatio;
        double real_facesize_y = faces[i].y / resizeRatio;
        double real_facesize_width = faces[i].width / resizeRatio;
        double real_facesize_height = faces[i].height / resizeRatio;
        double area = 0;
        double pixel_sum = 0;
        double dtheta = pi / 5;
        double dr = real_facesize_height / 20;
        for (double_t j = 0; j < 2 * pi; j += dtheta) {
            double_t radius = sqrt((real_facesize_width * cos(j)) * (real_facesize_width * cos(j)) + (real_facesize_height * sin(j))*(real_facesize_height * sin(j)));
            for (double_t k = 0; k < radius; k += dr) {
                area += (k * dr * dtheta);
                pixel_sum += (img_gray.at<uchar>(Point(real_facesize_x + real_facesize_width / 2 + k * cos(j), real_facesize_y + real_facesize_height / 2 + k * sin(j))) * k * dr * dtheta);
            }
        }
        pixel_avg = pixel_sum / area;
        Point center( real_facesize_x + real_facesize_width / 2, real_facesize_y + real_facesize_height/2);
        //Mat mask = ellipse(img_result, center, Size( real_facesize_width / 2, real_facesize_height / 2), 0, 0, 360, Scalar(255, 255, 255), -1); //making a mask
        //for (size_t j = )
        ellipse(img_result, center, Size( real_facesize_width / 2, real_facesize_height / 2), 0, 0, 360, Scalar(255, 0, 255), 30, 8, 0);
        Rect face_area(real_facesize_x, real_facesize_y, real_facesize_width,real_facesize_height);
        Mat faceROI = img_gray( face_area );
        std::vector<Rect> eyes;
        //-- In each face, detect eyes
        ((CascadeClassifier *) cascade_classifier_eye)->detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(30, 30) );
        for ( size_t j = 0; j < eyes.size(); j++ )
        {
            Point eye_center( real_facesize_x + eyes[j].x + eyes[j].width/2, real_facesize_y + eyes[j].y + eyes[j].height/2 );
            int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
            circle( img_result, eye_center, radius, Scalar( 255, 0, 0 ), 30, 8, 0 );
        }
    }

    return pixel_avg;
}