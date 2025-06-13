#ifndef VVP_VISUALIZATION_LAYERS_HOG_DRAWING_LAYER_H_
#define VVP_VISUALIZATION_LAYERS_HOG_DRAWING_LAYER_H_

#include "vvp/visualization/layers/i_drawable_layer.h"

namespace vv::visualization {

class HogDrawingLayer : public IDrawableLayer {
 public:
  void draw(VisualizationContext& context) override;
};

}  // namespace vv::visualization

#endif  // VVP_VISUALIZATION_LAYERS_HOG_DRAWING_LAYER_H_
