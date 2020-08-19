const FACTOR_AXIS_HEIGHT = 200;
const MAX_GLYPH_RADIUS = 70;
const MARGIN_HEIGHT_FOR_GLYPH = 40;
const GLYPH_CENTER_TO_BAR = 15;
const FACTOR_TYPE_COLORMAP = [ "#84a7b2","#0047B6", "#CD6600","#3690c0", "#EEAD0E", "#996699"];
const MIN_OPACITY = 0.4;
const MAX_OPACITY = 1;
const FACTOR_TYPE_COLORMAP_MIN_OPACITY = [
      "rgb(132, 167, 178, " + MIN_OPACITY+")",
      "rgb(0, 71, 182, " + MIN_OPACITY+")", 
      "rgba(205, 102, 0, " + MIN_OPACITY+")", 
      "rgb(54, 144, 192, " + MIN_OPACITY+")", 
      "rgb(238, 173, 14, " + MIN_OPACITY+")", 
      "rgb(153, 102, 153, " + MIN_OPACITY+")",
      ];
const FRAME_HEADER_HEIGHT = 52;

export default{
  FACTOR_AXIS_HEIGHT,
  MAX_GLYPH_RADIUS,
  GLYPH_CENTER_TO_BAR,
  MARGIN_HEIGHT_FOR_GLYPH,
  FACTOR_TYPE_COLORMAP,
  MIN_OPACITY,
  MAX_OPACITY,
  FACTOR_TYPE_COLORMAP_MIN_OPACITY,
  FRAME_HEADER_HEIGHT,
}
