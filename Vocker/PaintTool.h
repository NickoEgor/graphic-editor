#pragma once

#include "resource.h"

enum class PaintTool {
  RECTANGLE = ID_RECTANGLE_NEW,
  ELLIPSE = ID_ELLIPSE_NEW,
  TRIANGLE = ID_TRIANGLE_NEW,
  LINE = ID_LINE_NEW,
  ONEWAYLINE = ID_ONEWAYLINE_NEW,
  TWOWAYLINE = ID_TWOWAYLINE_NEW,
  MOVESHAPE = ID_MOVESHAPE,
  RESIZESHAPE = ID_RESIZESHAPE,
  ROTATESHAPE = ID_ROTATESHAPE
};