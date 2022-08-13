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
#include <vector>

#include "../cOMS/Utils/ArrayUtils.h"
#include "../cOMS/Utils/FileUtils.h"
#include "../cOMS/Image/Skew.h"
#include "../cOMS/Image/Thresholding.h"
#include "../cOMS/Image/BillDetection.h"

const bool DEBUG = false;

void printHelp()
{
    printf("    The OCRImageOptimizer developed by jingga is a image optimizer for OCR readers in\n");
    printf("    order to improve the text recognition performance on non-optimal image sources.\n\n");
    printf("    Run: ./App -i <input_path_to_the_image> -o <output_path_for_new_image>\n\n");
    printf("    -h: Prints the help output\n");
    printf("    -i: The input image which should get optimized\n");
    printf("    -o: The output path for the optimized image\n\n");
    printf("    If no optimization functions are defined (see below), all of them will be applied.\n\n");
    printf("    Optimization functions:\n");
    printf("    --rotate:  Tries to automatically fix the image rotation\n");
    printf("    --edges:   Tries to automatically detect and remove irrelevant parts from the image\n");
    printf("    --binary:  Tries to turn the image purely into black and white and remove shadows\n");
    printf("    --sharpen: Tries to sharpen (remove blurriness) from the image\n");
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
    const char *inputIdentifier  = "-i";
    const char *outputIdentifier = "-o";

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

    char *inputImage  = Utils::ArrayUtils::get_arg(inputIdentifier, argv, argc);
    char *outputImage = Utils::ArrayUtils::get_arg(outputIdentifier, argv, argc);

    if (inputImage == NULL || outputImage == NULL) {
        printf("Invalid application usage:\n");

        printHelp();

        return -1;
    }

    if (!Utils::FileUtils::file_exists(argv[1])) {
        printf("Image file doesn't exist.\n");

        return -1;
    }

    cv::Mat in;
    in = cv::imread(argv[1], cv::IMREAD_UNCHANGED);
    if (!in.data) {
        printf("Couldn't read image.\n");

        return -1;
    }

    cv::Mat out = in.clone();

    out = Image::BillDetection::highlightBill(out);
    if (DEBUG) cv::imshow("bill_detection", out);

    out = Image::Thresholding::integralThresholding(out);
    if (DEBUG) cv::imshow("thresholding", out);

    out = Image::Skew::deskewHoughLines(out);
    if (DEBUG) cv::imshow("rotation", out);

    if (DEBUG) cv::imshow("original", in);

    cv::imwrite(argv[2], out);

    if (DEBUG) cv::waitKey(0);

    return 0;
}