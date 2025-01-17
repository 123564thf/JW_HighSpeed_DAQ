#include <map>
#include <set>
#include <future>
#include "TTree.h"
#include "TFile.h"

#include "util.hpp"
#include "data_struct_cint.h"
#include "unpack.h"

waveform_by_entry::waveform_by_entry(std::string const& n):parse_base_t(n){
}

/* TODO  how to write valid ??*/ 
bool waveform_by_entry::valid(head_t const& head){
  //if (head.start_tag!=cs_start_tag) return false;
  //auto ps = head.package_size&0x1FFF;
  //if ((head.package_size&0x1FFF) != 20) return false;
  //if (head.package.crc32... /* TODO*/)
  return true;
}

bool waveform_by_entry::valid(body_t const& body){
  //if (body.start_tag!=cs_start_tag) return false;
  //if(body.adc_package_size&0x1FFF!=2060 
  //    && body.adc_package_size&0x1FFF!=14) return false;
  bool ret = 
    body.reserved2==0;
  return ret;
}

bool waveform_by_entry::valid(tail_t const& tail){
  //if (tail.start_tag!=cs_start_tag) return false;
  //if((tail.package_size&0x1FFF)!=12) return false;
  return true;
}

void waveform_by_entry::store(unit_t const& unit){
  if (!m_store_ref || !m_tree_ref) return;
  m_store_ref->event_id = get_event_id(unit.heads[0]);
  uint64_t ts = get_timestamp(unit.heads[0]);
  ts &= 0xFFFFFFFFFFFF;
  m_store_ref->time_stamp = ts*s_ts_unit/1.e+6; /* unit: ms */
  m_store_ref->fec_ids.resize(unit.heads.size());
  std::transform(std::begin(unit.heads),std::end(unit.heads),std::begin(m_store_ref->fec_ids)
      ,[&](head_t const& v){return this->get_fec_id(v);});
  m_store_ref->global_ids.resize(unit.bodys.size());

  m_store_ref->adcs.resize(unit.bodys.size());
  std::size_t index=0;
  for (auto&& x : unit.bodys){
    m_store_ref->global_ids[index] = ((uint16_t)get_fec_id(x)<<8) + (uint16_t)get_channel_id(x);
    m_store_ref->adcs[index].resize(1024);
    std::size_t j=0;
    for (auto&& y : x.adc) m_store_ref->adcs[index][j++] = (y&0x0FFF);
    index++;
  }
  m_tree_ref->Fill();
}
void waveform_by_entry::store(){
  if (!m_store_ref || !m_tree_ref) return;
  m_store_ref->fec_ids.resize(m_unit.heads.size());
  std::transform(std::begin(m_unit.heads),std::end(m_unit.heads),std::begin(m_store_ref->fec_ids)
      ,[&](head_t const& v){return this->get_fec_id(v);});

  m_store_ref->global_ids.resize(m_unit.bodys.size());
  m_store_ref->adcs.resize(m_unit.bodys.size());
  std::size_t index=0;
  for (auto&& x : m_unit.bodys){
    m_store_ref->global_ids[index] = ((uint16_t)get_fec_id(x)<<8) + (uint16_t)get_channel_id(x);
    m_store_ref->adcs[index].resize(1024);
    std::size_t j=0;
    for (auto&& y : x.adc) m_store_ref->adcs[index][j++] = (y&0x0FFF);
    index++;
  }
  m_tree_ref->Fill();
}


bool waveform_by_entry::parse1(char*& iter, char* const& end){
  std::map<uint32_t,unit_t> in_memory;
  using namespace util;
  auto const& parse_head = [&](head_t& head)->bool{
    read_int(head.start_tag,iter);
    read_int(head.package_size,iter);
    if ((head.package_size&0x1FFF)!=20) return false;
    read_int(head.fec_id,iter);
    read_int<uint64_t,6>(head.time_stamp,iter);
    read_int(head.event_id,iter);
    read_int(head.hit_channel_no,iter);
    read_int(head.reserved,iter);
    read_int(head.crc32,iter);
    return true;
  };
  auto const& parse_body = [&](body_t& bd)->bool{
    read_int(bd.start_tag,iter);
    read_int(bd.adc_package_size,iter);
    if((bd.adc_package_size&0x1FFF)!=2060 
        && (bd.adc_package_size&0x1FFF)!=14)
      return false;
    read_int(bd.reserved0,iter);
    read_int(bd.channel_id,iter);
    read_int(bd.reserved1,iter);
    for (auto&& x : bd.adc) read_int(x,iter);
    read_int(bd.reserved2,iter);
    read_int(bd.crc32,iter);
    return true;
  };
  auto const& parse_tail = [&](tail_t& tail)->bool{
    read_int(tail.start_tag,iter);
    read_int(tail.package_size,iter);
    if ((tail.package_size&0x1FFF)!=12) return false;
    read_int(tail.reserved,iter);
    read_int(tail.event_size,iter);
    read_int(tail.crc32,iter);
    return true;
  };
  char* begin = iter;
  char* back = iter;

  size_t index = 0;
  int tail_count=0;

  uint32_t current_event_idx=0;
  bool is_first_head_get =false;

  size_t bytes = std::distance(iter,end);
  ui::Xslider slider(0,std::distance(iter,end));
  slider.set_label("unpack");
  auto ft0 = std::async(&ui::Xslider::start,std::ref(slider));

  while(std::distance(iter,end)>0){
    slider.progress(bytes-std::distance(iter,end));
    if(iter[0]==cs_start_tag){
      head_t head; back = iter;
      if (parse_head(head) && valid(head)){
        auto fec_id = get_fec_id(head);
        auto evt_id = get_event_id(head);
        if (in_memory.find(evt_id)==in_memory.end()){
          current_event_idx = evt_id;
          is_first_head_get = true;
          in_memory[evt_id];
        }
        in_memory.at(evt_id).heads.push_back(head);
        continue;
      }else{
        iter=back;
      }

      body_t body; back = iter;
      if (parse_body(body) && valid(body)){
        auto fec_id = get_fec_id(body);
        if (is_first_head_get)
          in_memory.at(current_event_idx).bodys.push_back(body);
        //std::cout<<"b"<<body.reserved2<<std::endl;
        continue;
      }else{
        iter = back;
      }
      
      tail_t tail ; back = iter;
      if (parse_tail(tail) && valid(tail)){
        auto fec_id = get_fec_id(tail);
        if(is_first_head_get)
          in_memory.at(current_event_idx).tails.emplace_back(tail);
        continue;
      }else{
        iter = back;
      }

      iter++;
    }else
      iter++;
  }
  slider.stop();

  size_t evt_get = 0;
  for (auto iter = in_memory.begin(); iter != in_memory.end(); ++iter){
    std::set<uint8_t> head_fecs;
    for (auto x : iter->second.heads) head_fecs.insert(get_fec_id(x));
    std::set<uint8_t> tail_fecs;
    for (auto x : iter->second.tails) tail_fecs.insert(get_fec_id(x));
    if (
        //iter->second.heads.size()==m_fec_count
        //&& iter->second.tails.size()==m_fec_count
        //&& head_fecs==tail_fecs
        iter->second.heads.size()==iter->second.tails.size()
        ){
      store(iter->second);
      evt_get ++;
    }
  }
  std::cout<<"event parsed: "<<in_memory.size()<<" event store: "<<evt_get<<std::endl;
  return true;
}

bool waveform_by_entry::parse(char*& iter, char* const& end){
  using namespace util;
  auto const& parse_head = [&](head_t& head)->bool{
    read_int(head.start_tag,iter);
    read_int(head.package_size,iter);
    if ((head.package_size&0x1FFF)!=20) return false;
    read_int(head.fec_id,iter);
    read_int<uint64_t,6>(head.time_stamp,iter);
    read_int(head.event_id,iter);
    read_int(head.hit_channel_no,iter);
    read_int(head.reserved,iter);
    read_int(head.crc32,iter);
    return true;
  };
  auto const& parse_body = [&](body_t& bd)->bool{
    read_int(bd.start_tag,iter);
    read_int(bd.adc_package_size,iter);
    if((bd.adc_package_size&0x1FFF)!=2060 
        && (bd.adc_package_size&0x1FFF)!=14)
      return false;
    read_int(bd.reserved0,iter);
    read_int(bd.channel_id,iter);
    read_int(bd.reserved1,iter);
    for (auto&& x : bd.adc) read_int(x,iter);
    read_int(bd.reserved2,iter);
    read_int(bd.crc32,iter);
    return true;
  };
  auto const& parse_tail = [&](tail_t& tail)->bool{
    read_int(tail.start_tag,iter);
    read_int(tail.package_size,iter);
    if ((tail.package_size&0x1FFF)!=12) return false;
    read_int(tail.reserved,iter);
    read_int(tail.event_size,iter);
    read_int(tail.crc32,iter);
    return true;
  };

  enum class e_state : uint8_t{
    k_unknow
    ,k_have_head
    ,k_in_body
    ,k_have_tail
  };

  int head_count = 0;
  char* begin = iter;

  e_state stream_state = e_state::k_unknow;
  char* back = iter;
  while(std::distance(iter,end)>0){
    if(iter[0]==cs_start_tag){
      back = iter;
      if (stream_state==e_state::k_unknow 
           || stream_state==e_state::k_have_head
           || stream_state==e_state::k_in_body
           || stream_state==e_state::k_have_tail){
        m_unit.heads.resize(m_unit.heads.size()+1);
        auto& h_ref = m_unit.heads.back();
        if (parse_head(h_ref) && valid(h_ref)){
          //info_out("head get");
          //info_out(std::distance(begin,back));
          //info_out((int)get_fec_id(h_ref));
          head_count++;
          //info_out(head_count);
          stream_state=e_state::k_have_head;
          continue;
        }else{
          iter = back;
          m_unit.heads.erase(std::prev(m_unit.heads.end()));
        }
      }
        
      back = iter;
      if (stream_state==e_state::k_have_head
          || stream_state==e_state::k_in_body
          || stream_state==e_state::k_have_tail){
        char* back = iter;
        m_unit.bodys.resize(m_unit.bodys.size()+1);
        auto& b_ref = m_unit.bodys.back();
        if (parse_body(b_ref) && valid(b_ref)){
          //info_out("body get");
          //info_out(std::distance(begin,back));
          stream_state=e_state::k_in_body;
          continue;
        }
        else{
          iter = back;
          m_unit.bodys.erase(std::prev(m_unit.bodys.end()));
        }
      }

      back = iter;
      if ((stream_state==e_state::k_have_head
          || stream_state==e_state::k_in_body
          || stream_state==e_state::k_have_tail)){
        m_unit.tails.resize(m_unit.tails.size()+1);
        auto& t_ref = m_unit.tails.back();
        if (parse_tail(t_ref) && valid(t_ref)){
          //info_out("tail get");
          //info_out(std::distance(begin,back));
          //info_out((int)get_fec_id(t_ref));
          stream_state=e_state::k_have_tail;
          head_count--;
          //info_out(head_count);
          if (head_count==0) { 
            //display(std::cout);
            store(); clear();
            stream_state=e_state::k_unknow;
          }
          continue;
        }else{
          iter = back;
          m_unit.tails.erase(std::prev(m_unit.tails.end()));
        }
      }

      iter++;
    }else {
      iter++;
    }
  }
  return true;
  
}

std::ostream& waveform_by_entry::display(std::ostream& os) const{
  os
    <<m_unit.heads.size()<<"\n";
    os<<"\t";
    for (auto&& x : m_unit.heads) os<<(int)get_fec_id(x)<<" ";
  os
    <<"\n"
    <<m_unit.bodys.size()<<"\n";
    for (auto&& x : m_unit.bodys) os<<(int)get_fec_id(x)<<" ";
  os
    <<"\n"
    <<m_unit.tails.size()<<"\n";
  return os; }

  
