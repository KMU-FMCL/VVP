#ifndef VVP_VISUALIZATION_INPUT_IMAGE_LAYER_H_
#define VVP_VISUALIZATION_INPUT_IMAGE_LAYER_H_

#include "vvp/visualization/layers/i_drawable_layer.h"
#include "vvp/visualization/visualization_context.h"  // For VisualizationContext

namespace vv::visualization {

class InputImageLayer : public IDrawableLayer {
 public:
  InputImageLayer();
  ~InputImageLayer() override = default;

  void draw(VisualizationContext& context) override;
};

}  // namespace vv::visualization

#endif  // VVP_VISUALIZATION_INPUT_IMAGE_LAYER_H_
