


rgb rgb_blend(rgb a, rgb b, f32 ratio){
  f32 inv_ratio = 1.0f - ratio;
  return (rgb){
    .r = a.r * ratio + b.r * inv_ratio,
      .g = a.g * ratio + b.g * inv_ratio,
      .b = a.b * ratio + b.b * inv_ratio};
}

hsv hsv_blend(hsv a, hsv b, f32 ratio){
  f32 inv_ratio = 1.0f - ratio;
  f32 d1 = fabs(a.h - b.h);
  if(d1 > 180){
    b.h -= 360;
  }
  
  hsv result = {
    .h = a.h * ratio + b.h * inv_ratio,
    .s = a.s * ratio + b.s * inv_ratio,
    .v = a.v * ratio + b.v * inv_ratio};
  if(result.h < 0)
    result.h += 360;
  return result;
}

rgb rgb_add(rgb a, rgb b){
  a.r += b.r;
  a.g += b.g;
  a.b += b.b;
  return a;
}


f32 fract(f32 x){
  return x - floor(x);
}


f32 noise(vec2 coords) {
   return fract(sin(dot(coords, vec2_new(12.9898,78.233))) * 43758.5453);
}
