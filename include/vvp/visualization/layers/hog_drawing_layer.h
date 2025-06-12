#ifndef VVP_VISUALIZATION_LAYERS_HOG_DRAWING_LAYER_H_
#define VVP_VISUALIZATION_LAYERS_HOG_DRAWING_LAYER_H_

#include "vvp/visualization/layers/i_drawable_layer.h"
#include "vvp/visualization/visualization_context.h"
#include <opencv2/core.hpp>

namespace vv {
namespace visualization {
namespace layers {

class HogDrawingLayer : public IDrawableLayer {
public:
    HogDrawingLayer();
    virtual ~HogDrawingLayer() = default;

    void draw(cv::Mat& output_image, const VisualizationContext& context) const override;

private:
    // Helper methods for drawing HOG, if needed, can be added here.
    // For example:
    // void draw_hog_cell(cv::Mat& image, const cv::Point& cell_origin, 
    //                    const std::vector<float>& histogram, 
    //                    int cell_size, float scale_factor) const;
};

} // namespace layers
} // namespace visualization
} // namespace vv

#endif // VVP_VISUALIZATION_LAYERS_HOG_DRAWING_LAYER_H_
