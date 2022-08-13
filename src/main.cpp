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

#include "../cOMS/Utils/FileUtils.h"
#include "../cOMS/Image/Skew.h"
#include "../cOMS/Image/Thresholding.h"
#include "../cOMS/Image/BillDetection.h"

const bool DEBUG = false;

int main(int argc, char** argv)
{
    if (argc != 3) {
        printf("A input image and a output image is required\n");

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