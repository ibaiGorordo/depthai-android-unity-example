#include <chrono>
#include <string>
#include <android/log.h>

#include <libusb/libusb/libusb.h>
#include <jni.h>
#include "depthai/depthai.hpp"

#include "utils.h"

#define LOG_TAG "depthaiAndroid"
#define log(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG, __VA_ARGS__)

JNIEnv* jni_env = NULL;
JavaVM* JVM;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    // Ref: https://stackoverflow.com/a/49947762/13706271
    JVM = vm;
    if (vm->GetEnv((void**)&jni_env, JNI_VERSION_1_6) != JNI_OK)
        log("ERROR: GetEnv failed");

    return JNI_VERSION_1_6;
}

extern "C"
{

    std::shared_ptr<dai::Device> device;
    std::shared_ptr<dai::DataOutputQueue> qRgb, qDisparity, qDepth;

    std::vector<u_char> rgbImageBuffer, colorDisparityBuffer;

    const int disparityWidth = 640;
    const int disparityHeight = 400;

    // Closer-in minimum depth, disparity range is doubled (from 95 to 190):
    std::atomic<bool> extended_disparity{true};
    auto maxDisparity = extended_disparity ? 190.0f :95.0f;

    // Better accuracy for longer distance, fractional disparity 32-levels:
    std::atomic<bool> subpixel{false};
    // Better handling for occlusions:
    std::atomic<bool> lr_check{false};

    void api_start_device(int rgbWidth, int rgbHeight)
    {
        // libusb
        auto r = libusb_set_option(nullptr, LIBUSB_OPTION_ANDROID_JNIENV, jni_env);
        log("libusb_set_option ANDROID_JAVAVM: %s", libusb_strerror(r));

        // Create pipeline
        dai::Pipeline pipeline;

        // Define source and output
        auto camRgb = pipeline.create<dai::node::ColorCamera>();
        auto monoLeft = pipeline.create<dai::node::MonoCamera>();
        auto monoRight = pipeline.create<dai::node::MonoCamera>();
        auto stereo = pipeline.create<dai::node::StereoDepth>();

        auto xoutRgb = pipeline.create<dai::node::XLinkOut>();
        auto xoutDisparity = pipeline.create<dai::node::XLinkOut>();
        auto xoutDepth = pipeline.create<dai::node::XLinkOut>();

        xoutRgb->setStreamName("rgb");
        xoutDisparity->setStreamName("disparity");
        xoutDepth->setStreamName("depth");

        // Properties
        camRgb->setPreviewSize(rgbWidth, rgbHeight);
        camRgb->setBoardSocket(dai::CameraBoardSocket::RGB);
        camRgb->setResolution(dai::ColorCameraProperties::SensorResolution::THE_1080_P);
        camRgb->setInterleaved(true);
        camRgb->setColorOrder(dai::ColorCameraProperties::ColorOrder::RGB);

        monoLeft->setResolution(dai::MonoCameraProperties::SensorResolution::THE_400_P);
        monoLeft->setBoardSocket(dai::CameraBoardSocket::LEFT);
        monoRight->setResolution(dai::MonoCameraProperties::SensorResolution::THE_400_P);
        monoRight->setBoardSocket(dai::CameraBoardSocket::RIGHT);

        stereo->initialConfig.setConfidenceThreshold(245);
        // Options: MEDIAN_OFF, KERNEL_3x3, KERNEL_5x5, KERNEL_7x7 (default)
        stereo->initialConfig.setMedianFilter(dai::MedianFilter::KERNEL_7x7);
        stereo->setLeftRightCheck(lr_check);
        stereo->setExtendedDisparity(extended_disparity);
        stereo->setSubpixel(subpixel);

        // Linking
        camRgb->preview.link(xoutRgb->input);
        monoLeft->out.link(stereo->left);
        monoRight->out.link(stereo->right);
        stereo->disparity.link(xoutDisparity->input);
        stereo->depth.link(xoutDepth->input);

        // Connect to device and start pipeline
        device = std::make_shared<dai::Device>(pipeline, dai::UsbSpeed::SUPER);

        auto device_info = device->getDeviceInfo();
        log("%s",device_info.toString().c_str());

        // Output queue will be used to get the rgb frames from the output defined above
        qRgb = device->getOutputQueue("rgb", 4, false);
        qDisparity = device->getOutputQueue("disparity", 4, false);
        qDepth = device->getOutputQueue("depth", 4, false);

        // Resize image buffers
        rgbImageBuffer.resize(rgbWidth*rgbHeight*4);
        colorDisparityBuffer.resize(disparityWidth*disparityHeight*4);

        log("Device Connected!");
    }

    unsigned int api_get_rgb_image(unsigned char* unityImageBuffer)
    {
        auto inRgb = qRgb->get<dai::ImgFrame>();
        auto imgData = inRgb->getData();

        // Convert from RGB to RGBA for Unity
        int rgb_index = 0;
        int argb_index = 0;
        for (int y = 0; y < inRgb->getHeight(); y ++)
        {
            for (int x = 0; x < inRgb->getWidth(); x ++)
            {
                rgbImageBuffer[argb_index++] = imgData[rgb_index++]; // red
                rgbImageBuffer[argb_index++] = imgData[rgb_index++]; // green
                rgbImageBuffer[argb_index++] = imgData[rgb_index++]; // blue
                rgbImageBuffer[argb_index++] = 255; // alpha

            }
        }

        // Copy the new image to the Unity image buffer
        std::copy(rgbImageBuffer.begin(), rgbImageBuffer.end(), unityImageBuffer);

        // Return the image number
        return inRgb->getSequenceNum();
    }

    unsigned int api_get_color_disparity_image(unsigned char* unityImageBuffer)
    {
        auto inDisparity = qDisparity->get<dai::ImgFrame>();;
        auto disparityData = inDisparity->getData();
        uint8_t colorPixel[3];

        // Convert Disparity to RGBA format for Unity
        int argb_index = 0;
        for (int i = 0; i < disparityWidth*disparityHeight; i++)
        {
            // Convert the disparity to color
            colorDisparity(colorPixel, disparityData[i], maxDisparity);

            colorDisparityBuffer[argb_index++] = colorPixel[0]; // red
            colorDisparityBuffer[argb_index++] = colorPixel[1]; // green
            colorDisparityBuffer[argb_index++] = colorPixel[2]; // blue
            colorDisparityBuffer[argb_index++] = 255; // alpha
        }

        // Copy the new image to the Unity image buffer
        std::copy(colorDisparityBuffer.begin(), colorDisparityBuffer.end(), unityImageBuffer);

        // Return the image number
        return inDisparity->getSequenceNum();
    }
}



