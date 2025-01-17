#ifndef DATA_STRUCT_CINT_H
#define DATA_STRUCT_CINT_H
#include "TObject.h"
#include <vector>
#include <cstdint>
struct entry_new : public TObject{
  uint32_t event_id;
  std::vector<uint8_t> fec_ids;
  std::vector<uint8_t> hit_channel_nos;
  float time_stamp;
  std::vector<uint16_t> global_ids;
  std::vector<std::vector<uint16_t>> adcs;

public:
  ClassDef(entry_new,1)
};

struct cluster : public TObject{
  std::pair<int,int> range;
  std::vector<uint16_t> amp;
  std::vector<int> amp_time;
  std::size_t strips;
  uint16_t sum_amp;

public:
  //cluster() = default;
  //~cluster() noexcept = default;
public:

  ClassDef(cluster,1)
};

#endif // DATA_STRUCT_CINT_H
