
static inline bool is_integer_type(lisp_type t){
  switch(t){
  case LISP_INTEGER:
  case LISP_BYTE:
  case LISP_INTEGER32:
    return true;
  default:
    break;
  }
  return false;  
}

static inline bool is_float_type(lisp_type t){
  switch(t){
  case LISP_RATIONAL:
  case LISP_FLOAT32:
    return true;
  default:
    break;
  }
  return false;  
}

inline bool is_nil(lisp_value v){
  return v.type == LISP_NIL;
}

inline bool is_t(lisp_value v){
  return v.type == LISP_T;
}

inline bool is_cons(lisp_value v){
  return v.type == LISP_CONS;
}

inline bool is_integer(lisp_value v){
  return v.type == LISP_INTEGER;
}

inline bool is_float(lisp_value a){
  return a.type == LISP_RATIONAL || a.type == LISP_FLOAT32;
}

inline bool is_float32(lisp_value a){
  return a.type == LISP_FLOAT32;
}

inline bool is_string(lisp_value a){
  return a.type == LISP_STRING;
}

inline bool is_symbol(lisp_value a){
  return a.type == LISP_SYMBOL;
}

inline bool is_function(lisp_value a){
  return a.type == LISP_FUNCTION;
}

inline bool is_function_macro(lisp_value a){
  return a.type == LISP_FUNCTION_MACRO;
}

inline bool is_function_native(lisp_value a){
  return a.type == LISP_FUNCTION_NATIVE;
}

inline bool is_macro_builtin(lisp_value a){
  return a.type == LISP_MACRO_BUILTIN;
}

inline f64 lisp_value_rational(lisp_value v){
  return v.rational;
}

inline lisp_value rational_lisp_value(double o){
  return (lisp_value){.type = LISP_RATIONAL, .rational = o};
}

inline int64_t lisp_value_as_integer(lisp_value v){
  if(is_float(v))
    return (int64_t)lisp_value_rational(v);
  return v.integer;
}

inline f64 lisp_value_as_rational(lisp_value v){
  if(is_float(v))
    return lisp_value_rational(v);
  return (f64) v.integer;
}

inline lisp_value string_lisp_value(const char * str){
  return (lisp_value){.type = LISP_STRING, .string = (char *) str};
}

inline char * lisp_value_string(lisp_value v){
  return v.string;
}

inline lisp_value integer_lisp_value(i64 i){
  return (lisp_value){.type = LISP_INTEGER, .integer = i};
}

inline int64_t lisp_value_integer(lisp_value v){
  return v.integer;
}

inline lisp_value byte_lisp_value(u8 i){
  return (lisp_value){.type = LISP_BYTE, .integer = i};
}

inline u8 lisp_value_byte(lisp_value v){
  return (u8)v.integer;
}

inline void * lisp_value_pointer(lisp_value val){
  return val.pointer;
}

inline lisp_vector * lisp_value_vector(lisp_value val){
  return lisp_value_pointer(val);
}

inline lisp_type lisp_value_type(lisp_value val){
  return val.type;
}

inline lisp_value symbol_lisp_value(lisp_symbol sym){
  return (lisp_value){.type = LISP_SYMBOL, .symbol = sym };
}

inline lisp_symbol lisp_value_symbol(lisp_value val){
  return val.symbol; 
}

inline cons * lisp_value_cons(lisp_value val){
  return val.cons;
}

inline lisp_value function_lisp_value(lisp_function * f){
  return (lisp_value){.type = LISP_FUNCTION, .function = f};
}

inline lisp_function * lisp_value_function(lisp_value val){
  return val.function;
}

inline lisp_value float32_lisp_value(f32 v){
  return (lisp_value){.type = LISP_FLOAT32, .rational = v };
}