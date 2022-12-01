/**
 * Karaka
 *
 * @package   App
 * @copyright Dennis Eichhorn
 * @license   OMS License 1.0
 * @version   1.0.0
 * @link      https://karaka.app
 */
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>

#include "../cOMS/Utils/MathUtils.h"
#include "../cOMS/Utils/ArrayUtils.h"
#include "../cOMS/Utils/FileUtils.h"
#include "../cOMS/Image/Skew.h"
#include "../cOMS/Image/Thresholding.h"
#include "../cOMS/Image/BillDetection.h"
#include "../cOMS/Image/Kernel.h"

#ifndef OMS_DEMO
    #define OMS_DEMO false
#endif

void printHelp()
{
    printf("    The OCRImageOptimizer developed by jingga is a image optimizer for OCR readers in\n");
    printf("    order to improve the text recognition performance on non-optimal image sources.\n\n");
    printf("    Run: ./App -i <input_path_to_the_image> -o <output_path_for_new_image>\n\n");
    printf("    -h: Prints the help output\n");
    printf("    -v: Prints the version\n");
    printf("    -i: The input image which should get optimized\n");
    printf("    -o: The output path for the optimized image\n\n");
    printf("    If no optimization functions are defined (see below), optimizations marked\n");
    printf("    with * are automatically applied.\n\n");
    printf("    Optimization functions:\n");
    printf("    --rotate :* Tries to automatically fix the image rotation\n");
    printf("    --edges  :* Tries to automatically detect and remove irrelevant parts from the image\n");
    printf("    --binary :* Tries to turn the image purely into black and white and remove shadows\n");
    printf("    --sharpen:  Tries to sharpen (remove blurriness) from the image (not recommended)\n");
    printf("\n");
    printf("    Website: https://jingga.app\n");
    printf("    Copyright: jingga (c) Dennis Eichhorn\n");
}

void printVersion()
{
    printf("Version: 1.0.0\n");
}

int main(int argc, char **argv)
{
    bool hasHelpCmd  = Utils::ArrayUtils::has_arg("-h", argv, argc);
    if (hasHelpCmd) {
        printHelp();

        return 0;
    }

    bool hasVersionCmd  = Utils::ArrayUtils::has_arg("-v", argv, argc);
    if (hasVersionCmd) {
        printVersion();

        return 0;
    }

    char *inputImage  = Utils::ArrayUtils::get_arg("-i", argv, argc);
    char *outputImage = Utils::ArrayUtils::get_arg("-o", argv, argc);

    if (inputImage == NULL || outputImage == NULL) {
        printf("Invalid application usage:\n");

        printHelp();

        return -1;
    }

    if (!Utils::FileUtils::file_exists(inputImage)) {
        printf("Image file doesn't exist.\n");

        return -1;
    }

    bool hasRotateCmd  = Utils::ArrayUtils::has_arg("--rotate", argv, argc);
    bool hasEdgesCmd   = Utils::ArrayUtils::has_arg("--edges", argv, argc);
    bool hasBinaryCmd  = Utils::ArrayUtils::has_arg("--binary", argv, argc);
    bool hasSharpenCmd = Utils::ArrayUtils::has_arg("--sharpen", argv, argc);

    cv::Mat in;
    in = cv::imread(inputImage, cv::IMREAD_UNCHANGED);
    if (!in.data) {
        printf("Couldn't read image.\n");

        return -1;
    }

    int maxDim = oms_max(in.size().width, in.size().height);
    if (maxDim > 1500) {
        cv::resize(in, in, cv::Size(
            (int) (in.size().width * 1500 / maxDim),
            (int) (in.size().height * 1500 / maxDim)
        ), 0.0, 0.0, cv::INTER_AREA);
    }

    #if OMS_DEBUG
        cv::imshow("original", in);
    #endif

    cv::Mat out;
    cv::cvtColor(in, out, cv::COLOR_BGRA2BGR); // alternatively use CV_BGR2GRAY

    if (hasEdgesCmd ||
        (!hasEdgesCmd && !hasRotateCmd && !hasBinaryCmd && !hasSharpenCmd)
    ) {
        out = Image::BillDetection::highlightBill(out);
        #if OMS_DEBUG
            cv::imshow("bill_detection", out);
        #endif
    }

    if (hasBinaryCmd ||
        (!hasEdgesCmd && !hasRotateCmd && !hasBinaryCmd && !hasSharpenCmd)
    ) {
        out = Image::Thresholding::integralThresholding(out);
        #if OMS_DEBUG
            cv::imshow("thresholding", out);
        #endif
    }

    if (hasRotateCmd ||
        (!hasEdgesCmd && !hasRotateCmd && !hasBinaryCmd && !hasSharpenCmd)
    ) {
        out = Image::Skew::deskewHoughLines(out);
        #if OMS_DEBUG
            cv::imshow("rotation", out);
        #endif
    }

    if (hasSharpenCmd) {
        out = Image::Kernel::convolve(out, Image::KERNEL_SHARPEN);
        #if OMS_DEBUG
            cv::imshow("sharpen", out);
        #endif
    }

    #if OMS_DEMO
        cv::Size dim = out.size();
        cv::putText(out, "Demo", cv::Point(out.size().width / 2, out.size().height / 2), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 2);
    #endif

    cv::imwrite(outputImage, out);

    #if OMS_DEBUG
        cv::waitKey(0);
    #endif

    return 0;
}
