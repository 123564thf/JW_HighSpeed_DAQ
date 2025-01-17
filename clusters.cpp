#include "wave_handler.h"
#include "clusters.hpp"
#include "data_struct_cint.h"
#include "TClonesArray.h"
namespace{
static std::size_t x_strips = 375;
static std::size_t y_strips = 375;
}
namespace reco{
void hit_positon(long long idx
    ,entry_new* entry
    ,std::map<uint16_t,uint32_t> const& f2dmap
    ,std::unordered_map<uint16_t,std::pair<float,float>> const& pmap
    ,TClonesArray* storex
    ,TClonesArray* storey
    ){
  std::vector<wave_t> detx(x_strips), dety(y_strips);
  for (int j=0; j<entry->global_ids.size(); ++j){
    auto x = entry->global_ids.at(j);
    auto det_channel_id = get_dec_id(f2dmap.at(x));
    auto& ref = entry->adcs[j];
    if (det_channel_id.first==0){
      if (det_channel_id.second>x_strips-1){
      }
      else{
        auto& wave_ref = detx.at(det_channel_id.second);
        wave_ref.strip_id = det_channel_id.second;
        wave_ref.mean = pmap.at(x).first;
        wave_ref.sigma = pmap.at(x).second;
        wave_ref.wave = ref.data();
      }
    }
    if (det_channel_id.first==1){
      if (det_channel_id.second>y_strips-1){
      }
      else{
        auto& wave_ref = dety.at(det_channel_id.second);
        wave_ref.strip_id = det_channel_id.second;
        wave_ref.mean = pmap.at(x).first;
        wave_ref.sigma = pmap.at(x).second;
        wave_ref.wave = ref.data();
      }
    }
  } 
  float offset = 7.;
  auto const& judge_threshold = [](wave_t& v, float offset, std::pair<std::size_t,std::size_t> peak_range)->bool{
    if(v.mean>=1.e-3 && v.sigma>=1.e-3 && v.wave){
      uint16_t thr = std::round(offset*v.sigma);
      auto max = std::max_element(v.wave,v.wave+1024);
      //FIXME
      v.peak = *max;
      v.peak_position = std::distance(v.wave,max);
      v._is_calc = true;
      if (v.peak_position<peak_range.first || v.peak_position>=peak_range.second)
        return false;
      return *max-v.mean>=thr;
    }
    return false;
  };

  auto const& get_char = [](wave_t const& v){
    wave_char ret;
    if (v._is_calc){
      ret.peak = v.peak-std::floor(v.mean);
      ret.peak_position = v.peak_position;
    }
    return ret;
  };
  clusters<wave_char> clusters_x, clusters_y;
  auto pr = std::make_pair(400,500);
  clusters_x.calc(detx.begin(),detx.end() ,judge_threshold ,get_char ,offset,pr);
  auto const& get_max = [](wave_char const& v)->uint16_t{ return v.peak; };
  std::size_t index=0;
  if (storex){
    for (auto iter = clusters_x.m_data.begin(); iter!=clusters_x.m_data.end(); ++iter){
      new ((*storex)[index]) cluster();
      auto* store = (cluster*)storex->At(index);
      store->range.first = iter->m_range.first;
      store->range.second= iter->m_range.second;
      store->amp.resize(iter->m_data.size());
      store->amp_time.resize(iter->m_data.size());
      for (int i=0; i<iter->m_data.size(); ++i){
        store->amp[i] = iter->m_data[i].peak;
        store->amp_time[i] = iter->m_data[i].peak_position;
      }
      util::cluster_comp(*store);
      index++;
    }
  }
  clusters_y.calc(dety.begin(),dety.end() ,judge_threshold ,get_char ,offset,pr);
  index=0;
  if (storey){
    //info_out(clusters_y.m_data.size());
    for (auto iter = clusters_y.m_data.begin(); iter!=clusters_y.m_data.end(); ++iter){
      new ((*storey)[index]) cluster();
      auto* store = (cluster*)storey->At(index);
      store->range.first = iter->m_range.first;
      store->range.second= iter->m_range.second;
      store->amp.resize(iter->m_data.size());
      store->amp_time.resize(iter->m_data.size());
      for (int i=0; i<iter->m_data.size(); ++i){
        store->amp[i] = iter->m_data[i].peak;
        store->amp_time[i] = iter->m_data[i].peak_position;
      }
      util::cluster_comp(*store);
      index++;
    }
  }
}
}
