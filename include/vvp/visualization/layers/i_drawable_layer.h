#ifndef VVP_VISUALIZATION_I_DRAWABLE_LAYER_H_
#define VVP_VISUALIZATION_I_DRAWABLE_LAYER_H_

#include <opencv2/core/mat.hpp>

namespace vv {
namespace visualization {

// Forward declaration
struct VisualizationContext;

class IDrawableLayer {
 public:
  virtual ~IDrawableLayer() = default;
  virtual void draw(VisualizationContext& context) = 0;
};

}  // namespace visualization
}  // namespace vv

#endif  // VVP_VISUALIZATION_I_DRAWABLE_LAYER_H_
