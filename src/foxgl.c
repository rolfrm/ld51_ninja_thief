
#include <iron/full.h>
#include <iron/gl.h>
#include <iron/utf8.h>
#include "foxlisp.h"
#include <GL/gl.h>
#include <math.h>
#undef POP

lisp_value math_pow(lisp_value a, lisp_value b){
  type_assert(a, LISP_RATIONAL);
  type_assert(b, LISP_RATIONAL);
  return rational(pow(a.rational, b.rational));
}

lisp_value math_sqrt(lisp_value a){
  type_assert(a, LISP_RATIONAL);
  return rational(sqrt(a.rational));
}

lisp_value math_sqrtf(lisp_value a){
  return float32(sqrtf(lisp_float32(a).rational));
}

lisp_value math_mod(lisp_value a, lisp_value b){
  if(is_integer(a) && is_integer(b))
    return integer_lisp_value(lisp_value_integer(a) % lisp_value_integer(b));
  return rational_lisp_value(fmod(lisp_value_as_rational(a), lisp_value_as_rational(b)));
}

lisp_value math_round(lisp_value a, lisp_value dec){
  if(is_integer(a)) return a;
  if(is_float(a)){
    var v = lisp_value_as_rational(a);
    if(is_nil(dec)){
      v = round(v);
    }else{
      var decimals = lisp_value_integer(dec);
      var amount = pow(10, decimals);
      
      v = round(v * amount) / amount;
    }
    if(is_float32(a)) return float32_lisp_value(v);
    return rational_lisp_value(v);
  }
  raise_string("Not roundable");
  return nil;
}

lisp_value math_atan(lisp_value a, lisp_value b){
  if(!is_nil(b))
    return rational_lisp_value(atan2(lisp_value_as_rational(a), lisp_value_as_rational(b)));
  return rational_lisp_value(atan(lisp_value_as_rational(a)));
}
lisp_value math_tan(lisp_value a){
  return rational_lisp_value(atan(lisp_value_as_rational(a)));
}



lisp_value math_random(lisp_value range){
  if(range.type == LISP_INTEGER){
    return integer(randu32((u32)range.integer));
  }
  if(range.type == LISP_RATIONAL || range.type == LISP_FLOAT32)
    return rational(randf64() * range.rational);
  return nil;
}

lisp_value foxgl_load_font(lisp_value str, lisp_value size){
  const char * fontfile_init = str.string;
  //printf("Load font: %s %i\n", str.string, size.integer);
  var fnt0 =  blit_load_font_file(fontfile_init,  size.integer);
  blit_set_current_font(fnt0);
  return nil;
}

lisp_value foxgl_load_texture_from_path(lisp_value str){
  image img2 = image_from_file(str.string);
  if(img2.source == NULL) return nil;

  texture duck_tex = texture_from_image(&img2);
  image_delete(&img2);
  texture * t = lisp_malloc(sizeof(duck_tex));
  *t = duck_tex;
  return (lisp_value){.type = LISP_NATIVE_POINTER, .native_pointer = t};
}

lisp_value math_mul_in_place(lisp_value target, lisp_value a, lisp_value b){
  type_assert(target, LISP_VECTOR);
  type_assert(a, LISP_VECTOR);
  type_assert(b, LISP_VECTOR);
  type_assert(a.vector->default_value, LISP_FLOAT32);
  type_assert(b.vector->default_value, LISP_FLOAT32);
  if(a.vector->count == 16){
	 mat4 * m1 = a.vector->data;
	 if(b.vector->count == 16){
		mat4 * m2 = b.vector->data;
		
		mat4 * m3 = target.vector->data;
      ASSERT(target.vector->count == 16);
      
      *m3 = mat4_mul(*m1, *m2);
		return nil;
	 }
    if(b.vector->count == 3){
      vec3 * m2 = b.vector->data;
      vec3 *m3 = ((vec3 *) target.vector->data);
      
      *m3 = mat4_mul_vec3(*m1, *m2);
      return nil;
    }
	 raise_string("Invalid number of rows and columns");	
  }else if(a.vector->count == 9){
    ASSERT(target.vector->count == 9);
    
	 mat3 * m1 = a.vector->data;
	 if(b.vector->count == 9){
		mat3 * m2 = b.vector->data;
		mat3 * m3 = target.vector->data;
		*m3 = mat3_mul(*m1, *m2);
		return nil;
	 }
  }

  raise_string("Invalid number of rows and columns");
  return a;
}


lisp_value math_mul(lisp_value a, lisp_value b){
  type_assert(a, LISP_VECTOR);
  type_assert(b, LISP_VECTOR);
  type_assert(a.vector->default_value, LISP_FLOAT32);
  type_assert(b.vector->default_value, LISP_FLOAT32);
  if(a.vector->count == 16){
	 mat4 * m1 = a.vector->data;
	 if(b.vector->count == 16){
		mat4 * m2 = b.vector->data;
		lisp_value r = vector_copy(a);
		mat4 * m3 = r.vector->data;
		*m3 = mat4_mul(*m1, *m2);
		return r;
	 }
    if(b.vector->count == 3){
      vec3 * m2 = b.vector->data;
      var c = vector_copy(b);
      ((vec3 *) c.vector->data)[0] = mat4_mul_vec3(*m1, *m2);
      return c;
    }
	 raise_string("Invalid number of rows and columns");	
  }else if(a.vector->count == 9){
	 mat3 * m1 = a.vector->data;
	 if(b.vector->count == 9){
		mat3 * m2 = b.vector->data;
		lisp_value r = vector_copy(a);
		mat3 * m3 = r.vector->data;
		*m3 = mat3_mul(*m1, *m2);
		return r;
	 }
  }

  raise_string("Invalid number of rows and columns");
  return a;
}
lisp_value mat4_to_lisp (mat4 a);
lisp_value math_rotate_in_place(lisp_value m, lisp_value x, lisp_value y, lisp_value z){
  var m1 = (mat4 *) m.vector->data;
  *m1 = mat4_rotate_X(*m1, as_rational(x));
  *m1 = mat4_rotate_Y(*m1, as_rational(y));
  *m1 = mat4_rotate_Z(*m1, as_rational(z));
  
  //mat4_print(m1);printf(" %f %f %f \n", x.rational, y.rational, z.rational);
  return nil;
}


lisp_value math_mat4_rotate(lisp_value x, lisp_value y, lisp_value z){
  var m1 = mat4_identity();
  
  m1 = mat4_rotate_X(m1, x.rational);
  m1 = mat4_rotate_Y(m1, y.rational);
  m1 = mat4_rotate_Z(m1, z.rational);
  
  return mat4_to_lisp(m1);
}

lisp_value math_skew_in_place(lisp_value m, lisp_value values){
  mat4 m0 = mat4_identity();
  var m1 = (mat4 *) m.vector->data;
  float * pts = (f32 *) &m0;
  while(!is_nil(values)){
    int index = lisp_value_integer(car(values));
    values = cdr(values);
    f32 value = lisp_value_as_rational(car(values));
    values = cdr(values);
    if(index <0 || index >= 16)
      return nil;
    pts[index] = value;
  }
  *m1 = mat4_mul(*m1, m0);
  
  return nil;
}


mat4 lisp_value_mat4(lisp_value m){
  mat4 nil = {0};
  TYPE_ASSERT(m, LISP_VECTOR);

  var m1 = (mat4 *) m.vector->data;
  return *m1;
}

lisp_value math_translate_in_place(lisp_value m, lisp_value x, lisp_value y, lisp_value z){
  TYPE_ASSERT(m, LISP_VECTOR);

  var m1 = (mat4 *) m.vector->data;
  *m1 = mat4_mul(*m1, mat4_translate(as_rational(x), as_rational(y), as_rational(z)));
  return nil;
}

lisp_value math_scale_in_place(lisp_value m, lisp_value x, lisp_value y, lisp_value z){
  var m1 = (mat4 *) m.vector->data;
  *m1 = mat4_mul(*m1, mat4_scaled(as_rational(x), as_rational(y), as_rational(z)));
  return nil;
}

lisp_value math_mat4_identity_in_place(lisp_value m){
  var m1 = (mat4 *) m.vector->data;
  *m1 = mat4_identity();
  return nil;
}


lisp_value math_mat4_perspective(lisp_value fov, lisp_value aspect, lisp_value near, lisp_value far){
  TYPE_ASSERT(fov, LISP_RATIONAL);
  TYPE_ASSERT(aspect, LISP_RATIONAL);
  TYPE_ASSERT(near, LISP_RATIONAL);
  TYPE_ASSERT(far, LISP_RATIONAL);
  mat4 p = mat4_perspective(fov.rational, aspect.rational, near.rational, far.rational);
  return mat4_to_lisp(p);
}
           
lisp_value math_mat4_orthographic(lisp_value w, lisp_value h, lisp_value z){
  TYPE_ASSERT(w, LISP_RATIONAL);
  TYPE_ASSERT(h, LISP_RATIONAL);
  TYPE_ASSERT(z, LISP_RATIONAL);
  mat4 p = mat4_ortho(-w.rational, w.rational, -h.rational, h.rational, -z.rational, z.rational);
  return mat4_to_lisp(p);
}

mat4 lisp_to_mat4(lisp_value a){
  type_assert(a, LISP_VECTOR);
  type_assert(a.vector->default_value, LISP_FLOAT32);
  if(a.vector->count == 16){
	 mat4 * m1 = a.vector->data;
	 return *m1;
  }
  raise_string("Invalid number of rows and columns");
  return mat4_identity();
}


lisp_value mat4_to_lisp (mat4 a){
  lisp_value vec = make_vector(integer(16), (lisp_value){.type = LISP_FLOAT32, .rational = 0.0f});
  mat4 * v = vec.vector->data;
  *v = a;
  return vec;
}

lisp_value math_mat4_print(lisp_value a){
  type_assert(a, LISP_VECTOR);
  type_assert(a.vector->default_value, LISP_FLOAT32);
  if(a.vector->count == 16){
	 mat4 * m1 = a.vector->data;
	 mat4_print(*m1);
	 return a;
  }else if(a.vector->count == 9){
	 mat3 * m1 = a.vector->data;
	 mat3_print(*m1);
	 return a;
  }

  raise_string("Invalid number of rows and columns");
  return a;
}

float float32_2(lisp_value c, float def){
  if(c.type == LISP_RATIONAL || c.type == LISP_FLOAT32)
	 return c.rational;
  if(c.type == LISP_INTEGER || c.type == LISP_BYTE)
	 return (float)c.integer;
  return def;

}

vec4 color_from_u32(u32 integer){
  union {
	 u32 color1;
	 u8 colors[4];
  }x;
  x.color1 = integer;
  vec4 r;
  for(int i = 0; i < 4; i++)
	 r.data[i] = ((f32)x.colors[i])  / 256.0f;
  return r;
}

vec4 lisp_to_color(lisp_value color){
  vec4 r = {0};
  switch(color.type){
  case LISP_CONS:
	 r.x = float32_2(car(color), 1.0);
	 r.y = float32_2(cadr(color), 1.0);
	 r.z = float32_2(caddr(color), 1.0);
	 r.w = float32_2(cadddr(color), 1.0);
	 return r;
	 
  case LISP_INTEGER:
	 return color_from_u32(color.integer);
	 
  default:
	 break;
  }
  return r;
}

lisp_value foxgl_create_window(lisp_value w, lisp_value h){
  TYPE_ASSERT(w, LISP_INTEGER);
  TYPE_ASSERT(h, LISP_INTEGER);
  return native_pointer(gl_window_open(w.integer, h.integer));
}
lisp_value foxgl_make_current(lisp_value win){
  TYPE_ASSERT(win, LISP_NATIVE_POINTER);
  gl_window_make_current(win.native_pointer);
  return nil;
}
lisp_value foxgl_set_title(lisp_value win, lisp_value title){
  TYPE_ASSERT(win, LISP_NATIVE_POINTER);
  gl_window_set_title(win.native_pointer, title.string);
  return nil;
}

lisp_value foxgl_set_window_size(lisp_value _win, lisp_value _w, lisp_value _h){
  gl_window * win = _win.native_pointer;
  int w = lisp_value_integer(_w);
  int h = lisp_value_integer(_h);
  
  gl_window_set_size(win, w, h);
  return nil;
}

lisp_value foxgl_swap(lisp_value win){
  TYPE_ASSERT(win, LISP_NATIVE_POINTER);
  gl_window_swap(win.native_pointer);
  return nil;
}

lisp_value lisp_window_size(lisp_value win){
  TYPE_ASSERT(win, LISP_NATIVE_POINTER);
  int w, h;
  gl_window_get_size(win.native_pointer, &w, &h);
  return new_cons(integer(w), new_cons(integer(h), nil));
}

lisp_value foxgl_poll_events(){
  gl_window_poll_events();
  return nil;
}

blit3d_context * blit3d_current_context = NULL;

lisp_value foxgl_color_to_int(lisp_value r, lisp_value g, lisp_value b, lisp_value a){
  lisp_value values[] = {r, g, b, a};
  int v = 0;
  for(int i = 0; i < array_count(values); i++){
    var x = values[i];
    var l = is_nil(x) ? 255 : is_float(x) ? (int)(lisp_value_rational(x) * 255) : lisp_value_integer(x) * 255;
    v += (CLAMP(l, 0, 255) << (i * 8));
  }

  return integer_lisp_value(v);
}

lisp_value foxgl_color(lisp_value color){
  vec4 col = vec4_zero;
  if(is_integer(color)){
    col = color_from_u32(lisp_value_integer(color));   
  }else{
    col = lisp_to_color(color);
  }

  if(blit3d_current_context != NULL)
	 blit3d_color(blit3d_current_context, col);
  return nil;
}

lisp_value foxgl_transform(lisp_value tform1){
  mat4 tform = lisp_to_mat4(tform1);
  if(blit3d_current_context != NULL)
	 blit3d_view(blit3d_current_context, tform);
  return nil;
}

lisp_value foxgl_init(){
  iron_gl_debug = true;
  if(blit3d_current_context == NULL)
	 blit3d_current_context = blit3d_context_new();
  blit3d_context_load(blit3d_current_context);
  //glDisable(GL_BLEND);
  return nil;
}

lisp_value foxgl_square2(){
  static blit3d_polygon * square;
  if(blit3d_current_context == NULL) return nil;
  if(square == NULL){
	 square = blit3d_polygon_new();
	 f32 pts[] = {0, 0, 1, 0, 0, 1, 1, 1};
	 blit3d_polygon_load_data(square, pts, sizeof(pts));
	 blit3d_polygon_configure(square, 2);
  }
  blit3d_polygon_blit2(blit3d_current_context, &square, 1);
  return nil;
}

lisp_value load_polygon (lisp_value val, lisp_value dim, lisp_value opt_offset, lisp_value opt_count, lisp_value opt_type){
  if(blit3d_current_context == NULL) return nil;
  type_assert(val, LISP_VECTOR);
  type_assert(val.vector->default_value, LISP_FLOAT32);
  int dimensions = 2;
  if(dim.type != LISP_NIL){
    type_assert(dim, LISP_INTEGER);
    dimensions = dim.integer;
  }

  ssize_t offset = 0;
  if(!is_nil(opt_offset))
    offset = lisp_value_integer(opt_offset);
  size_t l = val.vector->count;
  if(!is_nil(opt_count)){
    size_t l2 = lisp_value_integer(opt_count);
    
    l = l2;
  }
  if(offset < 0){
    raise_string("offset may not be negative");
    return nil;
  }
  if(offset + l > val.vector->count){
    raise_string("Index out of range");
    return nil;
  }
  

  var pts = (float *) val.vector->data;

  var poly = blit3d_polygon_new();
  blit3d_polygon_load_data(poly, pts + offset,  l * val.vector->elem_size);
  blit3d_polygon_configure(poly, dimensions);

  return native_pointer(poly);
}

lisp_value blit_set_mode(lisp_value mode){
  if(lisp_value_eq(mode, get_symbol(":points"))){
    blit3d_set_mode(blit3d_current_context, BLIT3D_POINTS);
    glEnable(GL_PROGRAM_POINT_SIZE);
  }
  else if(lisp_value_eq(mode, get_symbol(":triangles"))){
    blit3d_set_mode(blit3d_current_context, BLIT3D_TRIANGLES);
    //glEnable(GL_PROGRAM_POINT_SIZE);
  }else if(lisp_value_eq(mode, get_symbol(":triangles-color"))){
    blit3d_set_mode(blit3d_current_context, BLIT3D_TRIANGLES_COLOR);
    //glEnable(GL_PROGRAM_POINT_SIZE);
  }else if(lisp_value_eq(mode, get_symbol(":triangle-strip-color"))){
    blit3d_set_mode(blit3d_current_context, BLIT3D_TRIANGLE_STRIP_COLOR);
    //glEnable(GL_PROGRAM_POINT_SIZE);
  }
  else{
    glDisable(GL_PROGRAM_POINT_SIZE);
    blit3d_set_mode(blit3d_current_context, BLIT3D_TRIANGLE_STRIP);
  }
    return nil;
}

lisp_value blit_polygon (lisp_value val){
  if(val.type == LISP_CONS){
    
    blit3d_polygon * poly2[10];
    int i = 0;
    while(is_nil(val) == false){
      poly2[i] = val.cons->car.native_pointer;
      val = val.cons->cdr;
      i++;
    }
    blit3d_polygon_blit2(blit3d_current_context, poly2, i);
  
    return nil;
  }
  type_assert(val, LISP_NATIVE_POINTER);
  blit3d_polygon * poly = val.native_pointer;
  blit3d_polygon_blit2(blit3d_current_context, &poly, 1);
  return nil;
}

lisp_value delete_polygon (lisp_value val){
  type_assert(val, LISP_NATIVE_POINTER);
  blit3d_polygon * poly = val.native_pointer;
  blit3d_polygon_destroy(&poly);
  return nil;
}

lisp_value foxgl_get_events(){
  lisp_value evts = nil;
  gl_window_event events[20];
  while(true){
    size_t cnt = gl_get_events(events, array_count(events));
    if(cnt == 0)
      break;
    for(size_t i = 0; i < cnt; i++){
      gl_window_event evt = events[i];
      gl_event_known_event_types type = evt.type;
      lisp_value levt = nil;
      switch(type){
      case EVT_MOUSE_MOVE:
        levt = new_cons(get_symbol("mouse-move"), new_cons(integer(evt.mouse_move.x), integer(evt.mouse_move.y)));
        break;
      case EVT_MOUSE_LEAVE:
        levt = new_cons(get_symbol("mouse-leave"), nil);
        break;
      case EVT_MOUSE_ENTER:
        levt = new_cons(get_symbol("mouse-enter"), nil);
        break;
      case EVT_MOUSE_BTN_DOWN:
        levt = new_cons(get_symbol("mouse-button-down"),
                        new_cons(integer(evt.mouse_btn.button), nil));
        break;
      case EVT_MOUSE_BTN_UP:
        levt = new_cons(get_symbol("mouse-button-up"),
                        new_cons(integer(evt.mouse_btn.button), nil));
        break;
      case EVT_MOUSE_SCROLL:
        levt = new_cons(get_symbol("mouse-scroll"),
                        new_cons(rational(evt.mouse_scroll.x), new_cons(rational(evt.mouse_scroll.y), nil)));
        
        break;
      case EVT_KEY_REPEAT:
        break;
      case EVT_KEY_DOWN:
      case EVT_KEY_UP:
        {
        char keystr[6] = {0};
        if(evt.key.codept != 0)
          codepoint_to_utf8(evt.key.codept, keystr, 5);
        
        
        lisp_value keyevt = nil;
        if(strlen(keystr) >0){
          levt = new_cons(get_symbol("char"), new_cons(get_symbol(keystr), nil));
          break;
        }
        else if(evt.key.key != -1){
          keyevt = new_cons(get_symbol("key"), new_cons(integer(evt.key.key), nil));
        }else if(evt.key.scancode != -1){
          keyevt = new_cons(get_symbol("scankey"), new_cons(integer(evt.key.scancode), nil));
        
        }
        
        levt = new_cons(get_symbol(type == EVT_KEY_DOWN ? "key-down" : (EVT_KEY_UP ? "key-up" : "key-repeat")), keyevt);
        }
        break;
      default:
        levt = new_cons(get_symbol("unknown"), new_cons(integer(type), nil));
      
      }
      levt = new_cons(get_symbol("event"), new_cons(get_symbol("timestamp"), new_cons(integer(evt.timestamp), levt)));
      evts = new_cons(levt, evts);
    }
  }
  return evts;
}

lisp_value foxgl_create_framebuffer (lisp_value width, lisp_value height){
  type_assert(width, LISP_INTEGER);
  type_assert(height, LISP_INTEGER);
  blit_framebuffer * b = lisp_malloc(sizeof(*b));
  b->width = width.integer;
  b->height = height.integer;
  b->channels = 4;
  b->mode = IMAGE_MODE_NONE;
 
  blit_create_framebuffer(b);
  return native_pointer(b);
}

lisp_value foxgl_bind_framebuffer(lisp_value fb){
  //printf("Use fb\n");
  type_assert(fb, LISP_NATIVE_POINTER);
  blit_use_framebuffer(fb.native_pointer);
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);
  return nil;
}

lisp_value foxgl_clear(){
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  return nil;
}

lisp_value foxgl_unbind_framebuffer(lisp_value fb){
  //printf("Unuse fb\n");
  type_assert(fb, LISP_NATIVE_POINTER);
  blit_unuse_framebuffer(fb.native_pointer);
  return nil;
}

lisp_value foxgl_destroy_framebuffer(lisp_value fb){
  type_assert(fb, LISP_NATIVE_POINTER);
  blit_delete_framebuffer(fb.native_pointer);
  return nil;
}

lisp_value foxgl_framebuffer_texture(lisp_value fb){
  type_assert(fb, LISP_NATIVE_POINTER);
  texture * tx = lisp_malloc(sizeof(*tx));
  *tx = blit_framebuffer_as_texture(fb.native_pointer);
  return native_pointer(tx);
}

//blit3d_bind_texture(blit3d_context * ctx, texture * tex){
lisp_value foxgl_bind_texture(lisp_value tex){
  if(is_nil(tex)){
    blit3d_bind_texture(blit3d_current_context, NULL);
    return nil;
  }
  type_assert(tex, LISP_NATIVE_POINTER);
  
  blit3d_bind_texture(blit3d_current_context, tex.native_pointer);
  return nil;
}

lisp_value foxgl_blit_text(lisp_value text, lisp_value matrix){
  type_assert(text, LISP_STRING);
  type_assert(matrix, LISP_VECTOR);
  mat4 * m1 = matrix.vector->data;
  blit3d_text(blit3d_current_context, mat4_identity(), *m1, text.string);

  return nil;
}

lisp_value foxgl_blend(lisp_value blend){
  if(!is_nil(blend)){
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  else{
    glDisable(GL_BLEND);
  }
  return nil;
}


lisp_value foxgl_depth(lisp_value blend){
  if(!is_nil(blend)){
    glEnable(GL_DEPTH_TEST);
  }
  else{
    glDisable(GL_DEPTH_TEST);
  }
  return nil;
}


lisp_value foxgl_key_down(lisp_value window, lisp_value keyid){
  if(gl_window_get_key_state(window.native_pointer, keyid.integer))
    return t;
  return nil;
}

lisp_value foxgl_mouse_down(lisp_value window, lisp_value mouseid){
  if(gl_window_get_btn_state(window.native_pointer, mouseid.integer))
    return t;
  return nil;
}

lisp_value foxgl_timestamp(){
  return integer(timestamp());
}

lisp_value foxgl_timestampf(){
  return rational(timestampf());
}

lisp_value lisp_vec2_len(lisp_value v){
  TYPE_ASSERT(v, LISP_VECTOR);
  vec2 * x = v.vector->data;
  return rational(vec2_len(*x));
}

lisp_value lisp_viewport(lisp_value w, lisp_value h){
  glViewport(0, 0, w.integer, h.integer);
  return nil;
}

lisp_value foxgl_bake_polygons(lisp_value polygons, lisp_value base_tform){

  var btform2 = lisp_value_mat4(base_tform);
  var btformi = mat4_invert(btform2);
  float * verts = NULL;
  float * colors = NULL;
  size_t vert_cnt = 0;
  int vdims = 0;
  while(!is_nil(polygons)){
    var polygon = car(polygons);
    var color = lisp_value_integer(car(polygon));
    u8 *cpointer = (u8 *)&color;
    var col2 = vec4_new(cpointer[0], cpointer[1], cpointer[2], cpointer[3]);
    col2 = vec4_scale(col2, 1.0 / 255.0);
    polygon = cdr(polygon);
    var tform = lisp_value_mat4(car(polygon));
    tform = mat4_mul(btformi, tform);
    polygon = cdr(polygon);
    var points = lisp_value_vector(car(polygon));
    var pointsf = (f32 *)points->data;;
    polygon = cdr(polygon);
    var dims = lisp_value_integer(car(polygon));
    if(vdims != dims && vdims != 0){
      // error
      return nil;
    }
    vdims = dims;
    var thiscnt = points->count / dims;

    var newcnt = vert_cnt + thiscnt + 2;
    verts = realloc(verts, newcnt * dims * sizeof(f32));
    colors = realloc(colors, newcnt * 3 * sizeof(f32));
    for(int i = 0; i < thiscnt; i++){
      var index = i + vert_cnt + 1;
      vec3 pt = vec3_new(pointsf[i * 3], pointsf[i*3 + 1], pointsf[i*3 + 2]);
      var pt2 = mat4_mul_vec3(tform, pt);
      var outv = verts + index * 3;
      outv[0] = pt2.x;
      outv[1] = pt2.y;
      outv[2] = pt2.z;
      var outc = colors + index * 3;
      outc[0] = col2.x;
      outc[1] = col2.y;
      outc[2] = col2.z;
    }
    

    for(int i = 0; i < 3; i++){
      verts[(vert_cnt) * 3 + i] = verts[(vert_cnt + 1) * 3 + i]; 
      colors[(vert_cnt) * 3 + i] = colors[(vert_cnt + 1) * 3 + i]; 
      verts[(newcnt - 1) * 3 + i] = verts[(newcnt - 2) * 3 + i];
      colors[(newcnt - 1) * 3 + i] = colors[(newcnt - 2) * 3 + i];  
    }
    
    vert_cnt = newcnt;
    polygons = cdr(polygons);
  }
  var vec = make_vector(integer(vert_cnt * 3), float32(0.0));
  f32 * verts2 = vec.vector->data;
  var vec2 = make_vector(integer(vert_cnt * 3), float32(0.0));
  f32 * colors2 = vec2.vector->data;
  memcpy(verts2, verts, vert_cnt * 3 * sizeof(f32));
  memcpy(colors2, colors, vert_cnt * 3 * sizeof(f32));
  dealloc(colors);
  dealloc(verts);
  return new_cons(vec, vec2);
}


void lrn(const char * l, int args, void * f){
  lisp_register_native(l, args, f);
}

lisp_value foxgl_web_canvas_size(){
  int w = 0, h = 0;
  gl_canvas_get_size(&w, &h);
  return new_cons(integer_lisp_value(w), integer_lisp_value(h));
}


lisp_value foxgl_detect_collision(lisp_value obj1, lisp_value obj2);
lisp_value foxgl_detect_collision_floor(lisp_value obj1, lisp_value obj2);

lisp_value sdf_poly(lisp_value f);
void tcp_register();
void foxal_register();
void foxgl_register(){
  lrn("math:pow", 2, math_pow);
  lrn("math:sqrt", 1, math_sqrt);
  lrn("math:sqrtf", 1, math_sqrtf);
  lrn("math:random", 1, math_random);
  lrn("math:mod", 2, math_mod);
  lrn("math:tan", 1, math_tan);
  lrn("math:atan", 2, math_atan);
  lrn("math:round", 2, math_round);
  lrn("foxgl:load-font", 2, foxgl_load_font);
  lrn("foxgl:load-texture-from-path", 1, foxgl_load_texture_from_path);
  lrn("foxgl:create-window", 2, foxgl_create_window);
  lrn("foxgl:window-set-size", 3, foxgl_set_window_size);
  lrn("foxgl:make-current", 1, foxgl_make_current);
  lrn("foxgl:set-title", 2, foxgl_set_title);
  lrn("foxgl:window-size", 1, lisp_window_size);
  lrn("foxgl:swap", 1, foxgl_swap);
  lrn("foxgl:viewport", 2, lisp_viewport);
  lrn("foxgl:poll-events", 0, foxgl_poll_events);
  lrn("foxgl:get-web-canvas-size", 0, foxgl_web_canvas_size);
  
  lrn("math:*", 2, math_mul);
  lrn("math:*!", 3, math_mul_in_place);
  lrn("math:rotate!", 4, math_rotate_in_place);
  lrn("math:scale!", 4, math_scale_in_place);
  lrn("math:translate!", 4, math_translate_in_place);
  lrn("math:skew!", 2, math_skew_in_place);
  lrn("mat4:rotate", 3, math_mat4_rotate);
  lrn("mat4:perspective", 4, math_mat4_perspective);
  lrn("mat4:orthographic", 3, math_mat4_orthographic);
  lrn("mat4:identity!", 1, math_mat4_identity_in_place);
  lrn("mat4:print", 1, math_mat4_print);
  lrn("vec2:len", 1, lisp_vec2_len);

  lrn("foxgl:timestamp", 0, foxgl_timestamp);
  lrn("foxgl:timestampf", 0, foxgl_timestampf);
  lrn("foxgl:clear", 0, foxgl_clear);
  lrn("foxgl:get-events", 0, foxgl_get_events);
  
  lrn("foxgl:color", 1, foxgl_color);
  lrn("foxgl:color->int", 4, foxgl_color_to_int);
  lrn("foxgl:transform", 1, foxgl_transform);
  lrn("foxgl:init", 0, foxgl_init);
  lrn("foxgl:quad", 1, foxgl_square2);
  lrn("foxgl:load-polygon", 5, load_polygon);
  lrn("foxgl:blit-polygon", 1, blit_polygon);
  lrn("foxgl:delete-polygon", 1, delete_polygon);

  
  lrn("foxgl:create-framebuffer", 2, foxgl_create_framebuffer);
  lrn("foxgl:bind-framebuffer", 1, foxgl_bind_framebuffer);
  lrn("foxgl:unbind-framebuffer", 1, foxgl_unbind_framebuffer);
  lrn("foxgl:destroy-framebuffer", 1, foxgl_destroy_framebuffer);

  lrn("foxgl:frame-buffer-texture", 1, foxgl_framebuffer_texture);
  lrn("foxgl:bind-texture", 1, foxgl_bind_texture);
  lrn("foxgl:blit-text", 2, foxgl_blit_text);
  lrn("foxgl:blend", 1, foxgl_blend);
  lrn("foxgl:depth", 1, foxgl_depth);
  lrn("foxgl:key-down?", 2, foxgl_key_down);
  lrn("foxgl:mouse-down?", 2, foxgl_mouse_down);
  lrn("foxgl:blit-mode", 1, blit_set_mode);
  lrn("foxgl:sdf-poly", 1, sdf_poly);
  lrn("foxgl:bake", 2, foxgl_bake_polygons);
  lrn("foxgl:detect-collision", 2, foxgl_detect_collision);
  lrn("foxgl:detect-collision-floor", 2, foxgl_detect_collision_floor);
  tcp_register();
  foxal_register();
}
