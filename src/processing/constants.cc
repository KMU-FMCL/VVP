#include "vvp/processing/constants.h"

namespace vv {

// 상수 정의
cv::Scalar const ImageConstants::Colors::kWhite =
    cv::Scalar(ImageConstants::kMaxPixelValue, ImageConstants::kMaxPixelValue,
               ImageConstants::kMaxPixelValue);
cv::Scalar const ImageConstants::Colors::kBlack =
    cv::Scalar(ImageConstants::kMinPixelValue, ImageConstants::kMinPixelValue,
               ImageConstants::kMinPixelValue);
cv::Scalar const ImageConstants::Colors::kGreen =
    cv::Scalar(ImageConstants::kMinPixelValue, ImageConstants::kMaxPixelValue,
               ImageConstants::kMinPixelValue);
cv::Scalar const ImageConstants::Colors::kRed =
    cv::Scalar(ImageConstants::kMinPixelValue, ImageConstants::kMinPixelValue,
               ImageConstants::kMaxPixelValue);

}  // namespace vv
