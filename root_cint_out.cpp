// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME root_cint_out
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "ROOT/RConfig.hxx"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "data_struct_cint.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_entry_new(void *p = nullptr);
   static void *newArray_entry_new(Long_t size, void *p);
   static void delete_entry_new(void *p);
   static void deleteArray_entry_new(void *p);
   static void destruct_entry_new(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::entry_new*)
   {
      ::entry_new *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::entry_new >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("entry_new", ::entry_new::Class_Version(), "data_struct_cint.h", 6,
                  typeid(::entry_new), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::entry_new::Dictionary, isa_proxy, 4,
                  sizeof(::entry_new) );
      instance.SetNew(&new_entry_new);
      instance.SetNewArray(&newArray_entry_new);
      instance.SetDelete(&delete_entry_new);
      instance.SetDeleteArray(&deleteArray_entry_new);
      instance.SetDestructor(&destruct_entry_new);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::entry_new*)
   {
      return GenerateInitInstanceLocal(static_cast<::entry_new*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::entry_new*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_cluster(void *p = nullptr);
   static void *newArray_cluster(Long_t size, void *p);
   static void delete_cluster(void *p);
   static void deleteArray_cluster(void *p);
   static void destruct_cluster(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::cluster*)
   {
      ::cluster *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::cluster >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("cluster", ::cluster::Class_Version(), "data_struct_cint.h", 18,
                  typeid(::cluster), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::cluster::Dictionary, isa_proxy, 4,
                  sizeof(::cluster) );
      instance.SetNew(&new_cluster);
      instance.SetNewArray(&newArray_cluster);
      instance.SetDelete(&delete_cluster);
      instance.SetDeleteArray(&deleteArray_cluster);
      instance.SetDestructor(&destruct_cluster);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::cluster*)
   {
      return GenerateInitInstanceLocal(static_cast<::cluster*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::cluster*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr entry_new::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *entry_new::Class_Name()
{
   return "entry_new";
}

//______________________________________________________________________________
const char *entry_new::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::entry_new*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int entry_new::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::entry_new*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *entry_new::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::entry_new*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *entry_new::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::entry_new*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr cluster::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *cluster::Class_Name()
{
   return "cluster";
}

//______________________________________________________________________________
const char *cluster::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::cluster*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int cluster::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::cluster*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *cluster::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::cluster*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *cluster::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::cluster*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void entry_new::Streamer(TBuffer &R__b)
{
   // Stream an object of class entry_new.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(entry_new::Class(),this);
   } else {
      R__b.WriteClassBuffer(entry_new::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_entry_new(void *p) {
      return  p ? new(p) ::entry_new : new ::entry_new;
   }
   static void *newArray_entry_new(Long_t nElements, void *p) {
      return p ? new(p) ::entry_new[nElements] : new ::entry_new[nElements];
   }
   // Wrapper around operator delete
   static void delete_entry_new(void *p) {
      delete (static_cast<::entry_new*>(p));
   }
   static void deleteArray_entry_new(void *p) {
      delete [] (static_cast<::entry_new*>(p));
   }
   static void destruct_entry_new(void *p) {
      typedef ::entry_new current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::entry_new

//______________________________________________________________________________
void cluster::Streamer(TBuffer &R__b)
{
   // Stream an object of class cluster.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(cluster::Class(),this);
   } else {
      R__b.WriteClassBuffer(cluster::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_cluster(void *p) {
      return  p ? new(p) ::cluster : new ::cluster;
   }
   static void *newArray_cluster(Long_t nElements, void *p) {
      return p ? new(p) ::cluster[nElements] : new ::cluster[nElements];
   }
   // Wrapper around operator delete
   static void delete_cluster(void *p) {
      delete (static_cast<::cluster*>(p));
   }
   static void deleteArray_cluster(void *p) {
      delete [] (static_cast<::cluster*>(p));
   }
   static void destruct_cluster(void *p) {
      typedef ::cluster current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::cluster

namespace ROOT {
   static TClass *vectorlEvectorlEunsignedsPshortgRsPgR_Dictionary();
   static void vectorlEvectorlEunsignedsPshortgRsPgR_TClassManip(TClass*);
   static void *new_vectorlEvectorlEunsignedsPshortgRsPgR(void *p = nullptr);
   static void *newArray_vectorlEvectorlEunsignedsPshortgRsPgR(Long_t size, void *p);
   static void delete_vectorlEvectorlEunsignedsPshortgRsPgR(void *p);
   static void deleteArray_vectorlEvectorlEunsignedsPshortgRsPgR(void *p);
   static void destruct_vectorlEvectorlEunsignedsPshortgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<vector<unsigned short> >*)
   {
      vector<vector<unsigned short> > *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<vector<unsigned short> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<vector<unsigned short> >", -2, "vector", 429,
                  typeid(vector<vector<unsigned short> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEvectorlEunsignedsPshortgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<vector<unsigned short> >) );
      instance.SetNew(&new_vectorlEvectorlEunsignedsPshortgRsPgR);
      instance.SetNewArray(&newArray_vectorlEvectorlEunsignedsPshortgRsPgR);
      instance.SetDelete(&delete_vectorlEvectorlEunsignedsPshortgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEvectorlEunsignedsPshortgRsPgR);
      instance.SetDestructor(&destruct_vectorlEvectorlEunsignedsPshortgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<vector<unsigned short> > >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<vector<unsigned short> >","std::vector<class std::vector<unsigned short,class std::allocator<unsigned short> >,class std::allocator<class std::vector<unsigned short,class std::allocator<unsigned short> > > >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<vector<unsigned short> >*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEvectorlEunsignedsPshortgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<vector<unsigned short> >*>(nullptr))->GetClass();
      vectorlEvectorlEunsignedsPshortgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEvectorlEunsignedsPshortgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEvectorlEunsignedsPshortgRsPgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<vector<unsigned short> > : new vector<vector<unsigned short> >;
   }
   static void *newArray_vectorlEvectorlEunsignedsPshortgRsPgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<vector<unsigned short> >[nElements] : new vector<vector<unsigned short> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEvectorlEunsignedsPshortgRsPgR(void *p) {
      delete (static_cast<vector<vector<unsigned short> >*>(p));
   }
   static void deleteArray_vectorlEvectorlEunsignedsPshortgRsPgR(void *p) {
      delete [] (static_cast<vector<vector<unsigned short> >*>(p));
   }
   static void destruct_vectorlEvectorlEunsignedsPshortgRsPgR(void *p) {
      typedef vector<vector<unsigned short> > current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<vector<unsigned short> >

namespace ROOT {
   static TClass *vectorlEunsignedsPshortgR_Dictionary();
   static void vectorlEunsignedsPshortgR_TClassManip(TClass*);
   static void *new_vectorlEunsignedsPshortgR(void *p = nullptr);
   static void *newArray_vectorlEunsignedsPshortgR(Long_t size, void *p);
   static void delete_vectorlEunsignedsPshortgR(void *p);
   static void deleteArray_vectorlEunsignedsPshortgR(void *p);
   static void destruct_vectorlEunsignedsPshortgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<unsigned short>*)
   {
      vector<unsigned short> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<unsigned short>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<unsigned short>", -2, "vector", 429,
                  typeid(vector<unsigned short>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEunsignedsPshortgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<unsigned short>) );
      instance.SetNew(&new_vectorlEunsignedsPshortgR);
      instance.SetNewArray(&newArray_vectorlEunsignedsPshortgR);
      instance.SetDelete(&delete_vectorlEunsignedsPshortgR);
      instance.SetDeleteArray(&deleteArray_vectorlEunsignedsPshortgR);
      instance.SetDestructor(&destruct_vectorlEunsignedsPshortgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<unsigned short> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<unsigned short>","std::vector<unsigned short,class std::allocator<unsigned short> >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<unsigned short>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEunsignedsPshortgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<unsigned short>*>(nullptr))->GetClass();
      vectorlEunsignedsPshortgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEunsignedsPshortgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEunsignedsPshortgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<unsigned short> : new vector<unsigned short>;
   }
   static void *newArray_vectorlEunsignedsPshortgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<unsigned short>[nElements] : new vector<unsigned short>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEunsignedsPshortgR(void *p) {
      delete (static_cast<vector<unsigned short>*>(p));
   }
   static void deleteArray_vectorlEunsignedsPshortgR(void *p) {
      delete [] (static_cast<vector<unsigned short>*>(p));
   }
   static void destruct_vectorlEunsignedsPshortgR(void *p) {
      typedef vector<unsigned short> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<unsigned short>

namespace ROOT {
   static TClass *vectorlEunsignedsPchargR_Dictionary();
   static void vectorlEunsignedsPchargR_TClassManip(TClass*);
   static void *new_vectorlEunsignedsPchargR(void *p = nullptr);
   static void *newArray_vectorlEunsignedsPchargR(Long_t size, void *p);
   static void delete_vectorlEunsignedsPchargR(void *p);
   static void deleteArray_vectorlEunsignedsPchargR(void *p);
   static void destruct_vectorlEunsignedsPchargR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<unsigned char>*)
   {
      vector<unsigned char> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<unsigned char>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<unsigned char>", -2, "vector", 429,
                  typeid(vector<unsigned char>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEunsignedsPchargR_Dictionary, isa_proxy, 0,
                  sizeof(vector<unsigned char>) );
      instance.SetNew(&new_vectorlEunsignedsPchargR);
      instance.SetNewArray(&newArray_vectorlEunsignedsPchargR);
      instance.SetDelete(&delete_vectorlEunsignedsPchargR);
      instance.SetDeleteArray(&deleteArray_vectorlEunsignedsPchargR);
      instance.SetDestructor(&destruct_vectorlEunsignedsPchargR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<unsigned char> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<unsigned char>","std::vector<unsigned char,class std::allocator<unsigned char> >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<unsigned char>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEunsignedsPchargR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<unsigned char>*>(nullptr))->GetClass();
      vectorlEunsignedsPchargR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEunsignedsPchargR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEunsignedsPchargR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<unsigned char> : new vector<unsigned char>;
   }
   static void *newArray_vectorlEunsignedsPchargR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<unsigned char>[nElements] : new vector<unsigned char>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEunsignedsPchargR(void *p) {
      delete (static_cast<vector<unsigned char>*>(p));
   }
   static void deleteArray_vectorlEunsignedsPchargR(void *p) {
      delete [] (static_cast<vector<unsigned char>*>(p));
   }
   static void destruct_vectorlEunsignedsPchargR(void *p) {
      typedef vector<unsigned char> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<unsigned char>

namespace ROOT {
   static TClass *vectorlEintgR_Dictionary();
   static void vectorlEintgR_TClassManip(TClass*);
   static void *new_vectorlEintgR(void *p = nullptr);
   static void *newArray_vectorlEintgR(Long_t size, void *p);
   static void delete_vectorlEintgR(void *p);
   static void deleteArray_vectorlEintgR(void *p);
   static void destruct_vectorlEintgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<int>*)
   {
      vector<int> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<int>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<int>", -2, "vector", 429,
                  typeid(vector<int>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEintgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<int>) );
      instance.SetNew(&new_vectorlEintgR);
      instance.SetNewArray(&newArray_vectorlEintgR);
      instance.SetDelete(&delete_vectorlEintgR);
      instance.SetDeleteArray(&deleteArray_vectorlEintgR);
      instance.SetDestructor(&destruct_vectorlEintgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<int> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<int>","std::vector<int,class std::allocator<int> >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<int>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEintgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<int>*>(nullptr))->GetClass();
      vectorlEintgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEintgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEintgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<int> : new vector<int>;
   }
   static void *newArray_vectorlEintgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<int>[nElements] : new vector<int>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEintgR(void *p) {
      delete (static_cast<vector<int>*>(p));
   }
   static void deleteArray_vectorlEintgR(void *p) {
      delete [] (static_cast<vector<int>*>(p));
   }
   static void destruct_vectorlEintgR(void *p) {
      typedef vector<int> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<int>

namespace {
  void TriggerDictionaryInitialization_root_cint_out_Impl() {
    static const char* headers[] = {
"data_struct_cint.h",
nullptr
    };
    static const char* includePaths[] = {
"./",
"C:/Users/admin/Downloads/root/include/",
"D:/qt_app/root_qt/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "root_cint_out dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
)DICTFWDDCLS"
R"DICTFWDDCLS(#pragma clang diagnostic ignored "-Wignored-attributes"
)DICTFWDDCLS"
R"DICTFWDDCLS(#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
)DICTFWDDCLS"
R"DICTFWDDCLS(extern int __Cling_AutoLoading_Map;
)DICTFWDDCLS"
R"DICTFWDDCLS(struct __attribute__((annotate("$clingAutoload$data_struct_cint.h")))  entry_new;
)DICTFWDDCLS"
R"DICTFWDDCLS(struct __attribute__((annotate("$clingAutoload$data_struct_cint.h")))  cluster;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "root_cint_out dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "data_struct_cint.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"cluster", payloadCode, "@",
"entry_new", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("root_cint_out",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_root_cint_out_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_root_cint_out_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_root_cint_out() {
  TriggerDictionaryInitialization_root_cint_out_Impl();
}
