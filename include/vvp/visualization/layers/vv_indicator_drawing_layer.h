#ifndef VVP_VISUALIZATION_LAYERS_VV_INDICATOR_DRAWING_LAYER_H_
#define VVP_VISUALIZATION_LAYERS_VV_INDICATOR_DRAWING_LAYER_H_

#include "vvp/visualization/layers/i_drawable_layer.h"
#include "vvp/visualization/visualization_context.h"

namespace vv {
namespace VvIndicatorDrawingLayer {

class VvIndicatorDrawingLayer : public vv::visualization::IDrawableLayer {
 public:
  VvIndicatorDrawingLayer() = default;
  ~VvIndicatorDrawingLayer() override = default;

  void draw(vv::visualization::VisualizationContext& context) override;
};

}  // namespace VvIndicatorDrawingLayer
}  // namespace vv

#endif  // VVP_VISUALIZATION_LAYERS_VV_INDICATOR_DRAWING_LAYER_H_
