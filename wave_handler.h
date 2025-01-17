#ifndef wave_handler_H
#define wave_handler_H 1 
#include <cstdint>
struct wave_char{
  uint16_t peak;
  int peak_position;
};
struct wave_handler{
  template <class _iter_t>
  void handler(_iter_t begin, _iter_t end){
  }
public:
};

struct wave_t{
  bool _is_calc = false;
  int strip_id;
  float mean=0;
  float sigma=0;
  uint16_t* wave=nullptr;
  uint16_t peak=0;
  int peak_position=0;
};

#endif

