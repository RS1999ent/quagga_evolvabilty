// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: quagga_config.proto

#ifndef PROTOBUF_quagga_5fconfig_2eproto__INCLUDED
#define PROTOBUF_quagga_5fconfig_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_quagga_5fconfig_2eproto();
void protobuf_AssignDesc_quagga_5fconfig_2eproto();
void protobuf_ShutdownFile_quagga_5fconfig_2eproto();

class Configuration;
class WiserProtocolConfig;
class Topology;
class NodeLink;
class Link;
class NodeProperty;

enum ProtocolType {
  PT_UNKNOWN = 0,
  PT_WISER = 1
};
bool ProtocolType_IsValid(int value);
const ProtocolType ProtocolType_MIN = PT_UNKNOWN;
const ProtocolType ProtocolType_MAX = PT_WISER;
const int ProtocolType_ARRAYSIZE = ProtocolType_MAX + 1;

const ::google::protobuf::EnumDescriptor* ProtocolType_descriptor();
inline const ::std::string& ProtocolType_Name(ProtocolType value) {
  return ::google::protobuf::internal::NameOfEnum(
    ProtocolType_descriptor(), value);
}
inline bool ProtocolType_Parse(
    const ::std::string& name, ProtocolType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ProtocolType>(
    ProtocolType_descriptor(), name, value);
}
// ===================================================================

class Configuration : public ::google::protobuf::Message {
 public:
  Configuration();
  virtual ~Configuration();

  Configuration(const Configuration& from);

  inline Configuration& operator=(const Configuration& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Configuration& default_instance();

  void Swap(Configuration* other);

  // implements Message ----------------------------------------------

  Configuration* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Configuration& from);
  void MergeFrom(const Configuration& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional .ProtocolType protocol_type = 1;
  inline bool has_protocol_type() const;
  inline void clear_protocol_type();
  static const int kProtocolTypeFieldNumber = 1;
  inline ::ProtocolType protocol_type() const;
  inline void set_protocol_type(::ProtocolType value);

  // optional .WiserProtocolConfig wiser_protocol_config = 2;
  inline bool has_wiser_protocol_config() const;
  inline void clear_wiser_protocol_config();
  static const int kWiserProtocolConfigFieldNumber = 2;
  inline const ::WiserProtocolConfig& wiser_protocol_config() const;
  inline ::WiserProtocolConfig* mutable_wiser_protocol_config();
  inline ::WiserProtocolConfig* release_wiser_protocol_config();
  inline void set_allocated_wiser_protocol_config(::WiserProtocolConfig* wiser_protocol_config);

  // @@protoc_insertion_point(class_scope:Configuration)
 private:
  inline void set_has_protocol_type();
  inline void clear_has_protocol_type();
  inline void set_has_wiser_protocol_config();
  inline void clear_has_wiser_protocol_config();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::WiserProtocolConfig* wiser_protocol_config_;
  int protocol_type_;
  friend void  protobuf_AddDesc_quagga_5fconfig_2eproto();
  friend void protobuf_AssignDesc_quagga_5fconfig_2eproto();
  friend void protobuf_ShutdownFile_quagga_5fconfig_2eproto();

  void InitAsDefaultInstance();
  static Configuration* default_instance_;
};
// -------------------------------------------------------------------

class WiserProtocolConfig : public ::google::protobuf::Message {
 public:
  WiserProtocolConfig();
  virtual ~WiserProtocolConfig();

  WiserProtocolConfig(const WiserProtocolConfig& from);

  inline WiserProtocolConfig& operator=(const WiserProtocolConfig& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const WiserProtocolConfig& default_instance();

  void Swap(WiserProtocolConfig* other);

  // implements Message ----------------------------------------------

  WiserProtocolConfig* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const WiserProtocolConfig& from);
  void MergeFrom(const WiserProtocolConfig& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional .Topology topology = 1;
  inline bool has_topology() const;
  inline void clear_topology();
  static const int kTopologyFieldNumber = 1;
  inline const ::Topology& topology() const;
  inline ::Topology* mutable_topology();
  inline ::Topology* release_topology();
  inline void set_allocated_topology(::Topology* topology);

  // @@protoc_insertion_point(class_scope:WiserProtocolConfig)
 private:
  inline void set_has_topology();
  inline void clear_has_topology();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::Topology* topology_;
  friend void  protobuf_AddDesc_quagga_5fconfig_2eproto();
  friend void protobuf_AssignDesc_quagga_5fconfig_2eproto();
  friend void protobuf_ShutdownFile_quagga_5fconfig_2eproto();

  void InitAsDefaultInstance();
  static WiserProtocolConfig* default_instance_;
};
// -------------------------------------------------------------------

class Topology : public ::google::protobuf::Message {
 public:
  Topology();
  virtual ~Topology();

  Topology(const Topology& from);

  inline Topology& operator=(const Topology& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Topology& default_instance();

  void Swap(Topology* other);

  // implements Message ----------------------------------------------

  Topology* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Topology& from);
  void MergeFrom(const Topology& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .NodeLink node_links = 1;
  inline int node_links_size() const;
  inline void clear_node_links();
  static const int kNodeLinksFieldNumber = 1;
  inline const ::NodeLink& node_links(int index) const;
  inline ::NodeLink* mutable_node_links(int index);
  inline ::NodeLink* add_node_links();
  inline const ::google::protobuf::RepeatedPtrField< ::NodeLink >&
      node_links() const;
  inline ::google::protobuf::RepeatedPtrField< ::NodeLink >*
      mutable_node_links();

  // @@protoc_insertion_point(class_scope:Topology)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::RepeatedPtrField< ::NodeLink > node_links_;
  friend void  protobuf_AddDesc_quagga_5fconfig_2eproto();
  friend void protobuf_AssignDesc_quagga_5fconfig_2eproto();
  friend void protobuf_ShutdownFile_quagga_5fconfig_2eproto();

  void InitAsDefaultInstance();
  static Topology* default_instance_;
};
// -------------------------------------------------------------------

class NodeLink : public ::google::protobuf::Message {
 public:
  NodeLink();
  virtual ~NodeLink();

  NodeLink(const NodeLink& from);

  inline NodeLink& operator=(const NodeLink& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const NodeLink& default_instance();

  void Swap(NodeLink* other);

  // implements Message ----------------------------------------------

  NodeLink* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const NodeLink& from);
  void MergeFrom(const NodeLink& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional .NodeProperty primary_node = 1;
  inline bool has_primary_node() const;
  inline void clear_primary_node();
  static const int kPrimaryNodeFieldNumber = 1;
  inline const ::NodeProperty& primary_node() const;
  inline ::NodeProperty* mutable_primary_node();
  inline ::NodeProperty* release_primary_node();
  inline void set_allocated_primary_node(::NodeProperty* primary_node);

  // repeated .Link links = 2;
  inline int links_size() const;
  inline void clear_links();
  static const int kLinksFieldNumber = 2;
  inline const ::Link& links(int index) const;
  inline ::Link* mutable_links(int index);
  inline ::Link* add_links();
  inline const ::google::protobuf::RepeatedPtrField< ::Link >&
      links() const;
  inline ::google::protobuf::RepeatedPtrField< ::Link >*
      mutable_links();

  // @@protoc_insertion_point(class_scope:NodeLink)
 private:
  inline void set_has_primary_node();
  inline void clear_has_primary_node();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::NodeProperty* primary_node_;
  ::google::protobuf::RepeatedPtrField< ::Link > links_;
  friend void  protobuf_AddDesc_quagga_5fconfig_2eproto();
  friend void protobuf_AssignDesc_quagga_5fconfig_2eproto();
  friend void protobuf_ShutdownFile_quagga_5fconfig_2eproto();

  void InitAsDefaultInstance();
  static NodeLink* default_instance_;
};
// -------------------------------------------------------------------

class Link : public ::google::protobuf::Message {
 public:
  Link();
  virtual ~Link();

  Link(const Link& from);

  inline Link& operator=(const Link& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Link& default_instance();

  void Swap(Link* other);

  // implements Message ----------------------------------------------

  Link* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Link& from);
  void MergeFrom(const Link& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional .NodeProperty adjacent_node = 1;
  inline bool has_adjacent_node() const;
  inline void clear_adjacent_node();
  static const int kAdjacentNodeFieldNumber = 1;
  inline const ::NodeProperty& adjacent_node() const;
  inline ::NodeProperty* mutable_adjacent_node();
  inline ::NodeProperty* release_adjacent_node();
  inline void set_allocated_adjacent_node(::NodeProperty* adjacent_node);

  // optional uint64 link_cost = 2;
  inline bool has_link_cost() const;
  inline void clear_link_cost();
  static const int kLinkCostFieldNumber = 2;
  inline ::google::protobuf::uint64 link_cost() const;
  inline void set_link_cost(::google::protobuf::uint64 value);

  // @@protoc_insertion_point(class_scope:Link)
 private:
  inline void set_has_adjacent_node();
  inline void clear_has_adjacent_node();
  inline void set_has_link_cost();
  inline void clear_has_link_cost();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::NodeProperty* adjacent_node_;
  ::google::protobuf::uint64 link_cost_;
  friend void  protobuf_AddDesc_quagga_5fconfig_2eproto();
  friend void protobuf_AssignDesc_quagga_5fconfig_2eproto();
  friend void protobuf_ShutdownFile_quagga_5fconfig_2eproto();

  void InitAsDefaultInstance();
  static Link* default_instance_;
};
// -------------------------------------------------------------------

class NodeProperty : public ::google::protobuf::Message {
 public:
  NodeProperty();
  virtual ~NodeProperty();

  NodeProperty(const NodeProperty& from);

  inline NodeProperty& operator=(const NodeProperty& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const NodeProperty& default_instance();

  void Swap(NodeProperty* other);

  // implements Message ----------------------------------------------

  NodeProperty* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const NodeProperty& from);
  void MergeFrom(const NodeProperty& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional string node_name = 1;
  inline bool has_node_name() const;
  inline void clear_node_name();
  static const int kNodeNameFieldNumber = 1;
  inline const ::std::string& node_name() const;
  inline void set_node_name(const ::std::string& value);
  inline void set_node_name(const char* value);
  inline void set_node_name(const char* value, size_t size);
  inline ::std::string* mutable_node_name();
  inline ::std::string* release_node_name();
  inline void set_allocated_node_name(::std::string* node_name);

  // optional string interface_ip = 2;
  inline bool has_interface_ip() const;
  inline void clear_interface_ip();
  static const int kInterfaceIpFieldNumber = 2;
  inline const ::std::string& interface_ip() const;
  inline void set_interface_ip(const ::std::string& value);
  inline void set_interface_ip(const char* value);
  inline void set_interface_ip(const char* value, size_t size);
  inline ::std::string* mutable_interface_ip();
  inline ::std::string* release_interface_ip();
  inline void set_allocated_interface_ip(::std::string* interface_ip);

  // @@protoc_insertion_point(class_scope:NodeProperty)
 private:
  inline void set_has_node_name();
  inline void clear_has_node_name();
  inline void set_has_interface_ip();
  inline void clear_has_interface_ip();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* node_name_;
  ::std::string* interface_ip_;
  friend void  protobuf_AddDesc_quagga_5fconfig_2eproto();
  friend void protobuf_AssignDesc_quagga_5fconfig_2eproto();
  friend void protobuf_ShutdownFile_quagga_5fconfig_2eproto();

  void InitAsDefaultInstance();
  static NodeProperty* default_instance_;
};
// ===================================================================


// ===================================================================

// Configuration

// optional .ProtocolType protocol_type = 1;
inline bool Configuration::has_protocol_type() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Configuration::set_has_protocol_type() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Configuration::clear_has_protocol_type() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Configuration::clear_protocol_type() {
  protocol_type_ = 0;
  clear_has_protocol_type();
}
inline ::ProtocolType Configuration::protocol_type() const {
  // @@protoc_insertion_point(field_get:Configuration.protocol_type)
  return static_cast< ::ProtocolType >(protocol_type_);
}
inline void Configuration::set_protocol_type(::ProtocolType value) {
  assert(::ProtocolType_IsValid(value));
  set_has_protocol_type();
  protocol_type_ = value;
  // @@protoc_insertion_point(field_set:Configuration.protocol_type)
}

// optional .WiserProtocolConfig wiser_protocol_config = 2;
inline bool Configuration::has_wiser_protocol_config() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Configuration::set_has_wiser_protocol_config() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Configuration::clear_has_wiser_protocol_config() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Configuration::clear_wiser_protocol_config() {
  if (wiser_protocol_config_ != NULL) wiser_protocol_config_->::WiserProtocolConfig::Clear();
  clear_has_wiser_protocol_config();
}
inline const ::WiserProtocolConfig& Configuration::wiser_protocol_config() const {
  // @@protoc_insertion_point(field_get:Configuration.wiser_protocol_config)
  return wiser_protocol_config_ != NULL ? *wiser_protocol_config_ : *default_instance_->wiser_protocol_config_;
}
inline ::WiserProtocolConfig* Configuration::mutable_wiser_protocol_config() {
  set_has_wiser_protocol_config();
  if (wiser_protocol_config_ == NULL) wiser_protocol_config_ = new ::WiserProtocolConfig;
  // @@protoc_insertion_point(field_mutable:Configuration.wiser_protocol_config)
  return wiser_protocol_config_;
}
inline ::WiserProtocolConfig* Configuration::release_wiser_protocol_config() {
  clear_has_wiser_protocol_config();
  ::WiserProtocolConfig* temp = wiser_protocol_config_;
  wiser_protocol_config_ = NULL;
  return temp;
}
inline void Configuration::set_allocated_wiser_protocol_config(::WiserProtocolConfig* wiser_protocol_config) {
  delete wiser_protocol_config_;
  wiser_protocol_config_ = wiser_protocol_config;
  if (wiser_protocol_config) {
    set_has_wiser_protocol_config();
  } else {
    clear_has_wiser_protocol_config();
  }
  // @@protoc_insertion_point(field_set_allocated:Configuration.wiser_protocol_config)
}

// -------------------------------------------------------------------

// WiserProtocolConfig

// optional .Topology topology = 1;
inline bool WiserProtocolConfig::has_topology() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void WiserProtocolConfig::set_has_topology() {
  _has_bits_[0] |= 0x00000001u;
}
inline void WiserProtocolConfig::clear_has_topology() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void WiserProtocolConfig::clear_topology() {
  if (topology_ != NULL) topology_->::Topology::Clear();
  clear_has_topology();
}
inline const ::Topology& WiserProtocolConfig::topology() const {
  // @@protoc_insertion_point(field_get:WiserProtocolConfig.topology)
  return topology_ != NULL ? *topology_ : *default_instance_->topology_;
}
inline ::Topology* WiserProtocolConfig::mutable_topology() {
  set_has_topology();
  if (topology_ == NULL) topology_ = new ::Topology;
  // @@protoc_insertion_point(field_mutable:WiserProtocolConfig.topology)
  return topology_;
}
inline ::Topology* WiserProtocolConfig::release_topology() {
  clear_has_topology();
  ::Topology* temp = topology_;
  topology_ = NULL;
  return temp;
}
inline void WiserProtocolConfig::set_allocated_topology(::Topology* topology) {
  delete topology_;
  topology_ = topology;
  if (topology) {
    set_has_topology();
  } else {
    clear_has_topology();
  }
  // @@protoc_insertion_point(field_set_allocated:WiserProtocolConfig.topology)
}

// -------------------------------------------------------------------

// Topology

// repeated .NodeLink node_links = 1;
inline int Topology::node_links_size() const {
  return node_links_.size();
}
inline void Topology::clear_node_links() {
  node_links_.Clear();
}
inline const ::NodeLink& Topology::node_links(int index) const {
  // @@protoc_insertion_point(field_get:Topology.node_links)
  return node_links_.Get(index);
}
inline ::NodeLink* Topology::mutable_node_links(int index) {
  // @@protoc_insertion_point(field_mutable:Topology.node_links)
  return node_links_.Mutable(index);
}
inline ::NodeLink* Topology::add_node_links() {
  // @@protoc_insertion_point(field_add:Topology.node_links)
  return node_links_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::NodeLink >&
Topology::node_links() const {
  // @@protoc_insertion_point(field_list:Topology.node_links)
  return node_links_;
}
inline ::google::protobuf::RepeatedPtrField< ::NodeLink >*
Topology::mutable_node_links() {
  // @@protoc_insertion_point(field_mutable_list:Topology.node_links)
  return &node_links_;
}

// -------------------------------------------------------------------

// NodeLink

// optional .NodeProperty primary_node = 1;
inline bool NodeLink::has_primary_node() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void NodeLink::set_has_primary_node() {
  _has_bits_[0] |= 0x00000001u;
}
inline void NodeLink::clear_has_primary_node() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void NodeLink::clear_primary_node() {
  if (primary_node_ != NULL) primary_node_->::NodeProperty::Clear();
  clear_has_primary_node();
}
inline const ::NodeProperty& NodeLink::primary_node() const {
  // @@protoc_insertion_point(field_get:NodeLink.primary_node)
  return primary_node_ != NULL ? *primary_node_ : *default_instance_->primary_node_;
}
inline ::NodeProperty* NodeLink::mutable_primary_node() {
  set_has_primary_node();
  if (primary_node_ == NULL) primary_node_ = new ::NodeProperty;
  // @@protoc_insertion_point(field_mutable:NodeLink.primary_node)
  return primary_node_;
}
inline ::NodeProperty* NodeLink::release_primary_node() {
  clear_has_primary_node();
  ::NodeProperty* temp = primary_node_;
  primary_node_ = NULL;
  return temp;
}
inline void NodeLink::set_allocated_primary_node(::NodeProperty* primary_node) {
  delete primary_node_;
  primary_node_ = primary_node;
  if (primary_node) {
    set_has_primary_node();
  } else {
    clear_has_primary_node();
  }
  // @@protoc_insertion_point(field_set_allocated:NodeLink.primary_node)
}

// repeated .Link links = 2;
inline int NodeLink::links_size() const {
  return links_.size();
}
inline void NodeLink::clear_links() {
  links_.Clear();
}
inline const ::Link& NodeLink::links(int index) const {
  // @@protoc_insertion_point(field_get:NodeLink.links)
  return links_.Get(index);
}
inline ::Link* NodeLink::mutable_links(int index) {
  // @@protoc_insertion_point(field_mutable:NodeLink.links)
  return links_.Mutable(index);
}
inline ::Link* NodeLink::add_links() {
  // @@protoc_insertion_point(field_add:NodeLink.links)
  return links_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::Link >&
NodeLink::links() const {
  // @@protoc_insertion_point(field_list:NodeLink.links)
  return links_;
}
inline ::google::protobuf::RepeatedPtrField< ::Link >*
NodeLink::mutable_links() {
  // @@protoc_insertion_point(field_mutable_list:NodeLink.links)
  return &links_;
}

// -------------------------------------------------------------------

// Link

// optional .NodeProperty adjacent_node = 1;
inline bool Link::has_adjacent_node() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Link::set_has_adjacent_node() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Link::clear_has_adjacent_node() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Link::clear_adjacent_node() {
  if (adjacent_node_ != NULL) adjacent_node_->::NodeProperty::Clear();
  clear_has_adjacent_node();
}
inline const ::NodeProperty& Link::adjacent_node() const {
  // @@protoc_insertion_point(field_get:Link.adjacent_node)
  return adjacent_node_ != NULL ? *adjacent_node_ : *default_instance_->adjacent_node_;
}
inline ::NodeProperty* Link::mutable_adjacent_node() {
  set_has_adjacent_node();
  if (adjacent_node_ == NULL) adjacent_node_ = new ::NodeProperty;
  // @@protoc_insertion_point(field_mutable:Link.adjacent_node)
  return adjacent_node_;
}
inline ::NodeProperty* Link::release_adjacent_node() {
  clear_has_adjacent_node();
  ::NodeProperty* temp = adjacent_node_;
  adjacent_node_ = NULL;
  return temp;
}
inline void Link::set_allocated_adjacent_node(::NodeProperty* adjacent_node) {
  delete adjacent_node_;
  adjacent_node_ = adjacent_node;
  if (adjacent_node) {
    set_has_adjacent_node();
  } else {
    clear_has_adjacent_node();
  }
  // @@protoc_insertion_point(field_set_allocated:Link.adjacent_node)
}

// optional uint64 link_cost = 2;
inline bool Link::has_link_cost() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Link::set_has_link_cost() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Link::clear_has_link_cost() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Link::clear_link_cost() {
  link_cost_ = GOOGLE_ULONGLONG(0);
  clear_has_link_cost();
}
inline ::google::protobuf::uint64 Link::link_cost() const {
  // @@protoc_insertion_point(field_get:Link.link_cost)
  return link_cost_;
}
inline void Link::set_link_cost(::google::protobuf::uint64 value) {
  set_has_link_cost();
  link_cost_ = value;
  // @@protoc_insertion_point(field_set:Link.link_cost)
}

// -------------------------------------------------------------------

// NodeProperty

// optional string node_name = 1;
inline bool NodeProperty::has_node_name() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void NodeProperty::set_has_node_name() {
  _has_bits_[0] |= 0x00000001u;
}
inline void NodeProperty::clear_has_node_name() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void NodeProperty::clear_node_name() {
  if (node_name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    node_name_->clear();
  }
  clear_has_node_name();
}
inline const ::std::string& NodeProperty::node_name() const {
  // @@protoc_insertion_point(field_get:NodeProperty.node_name)
  return *node_name_;
}
inline void NodeProperty::set_node_name(const ::std::string& value) {
  set_has_node_name();
  if (node_name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    node_name_ = new ::std::string;
  }
  node_name_->assign(value);
  // @@protoc_insertion_point(field_set:NodeProperty.node_name)
}
inline void NodeProperty::set_node_name(const char* value) {
  set_has_node_name();
  if (node_name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    node_name_ = new ::std::string;
  }
  node_name_->assign(value);
  // @@protoc_insertion_point(field_set_char:NodeProperty.node_name)
}
inline void NodeProperty::set_node_name(const char* value, size_t size) {
  set_has_node_name();
  if (node_name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    node_name_ = new ::std::string;
  }
  node_name_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:NodeProperty.node_name)
}
inline ::std::string* NodeProperty::mutable_node_name() {
  set_has_node_name();
  if (node_name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    node_name_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:NodeProperty.node_name)
  return node_name_;
}
inline ::std::string* NodeProperty::release_node_name() {
  clear_has_node_name();
  if (node_name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = node_name_;
    node_name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void NodeProperty::set_allocated_node_name(::std::string* node_name) {
  if (node_name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete node_name_;
  }
  if (node_name) {
    set_has_node_name();
    node_name_ = node_name;
  } else {
    clear_has_node_name();
    node_name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:NodeProperty.node_name)
}

// optional string interface_ip = 2;
inline bool NodeProperty::has_interface_ip() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void NodeProperty::set_has_interface_ip() {
  _has_bits_[0] |= 0x00000002u;
}
inline void NodeProperty::clear_has_interface_ip() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void NodeProperty::clear_interface_ip() {
  if (interface_ip_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    interface_ip_->clear();
  }
  clear_has_interface_ip();
}
inline const ::std::string& NodeProperty::interface_ip() const {
  // @@protoc_insertion_point(field_get:NodeProperty.interface_ip)
  return *interface_ip_;
}
inline void NodeProperty::set_interface_ip(const ::std::string& value) {
  set_has_interface_ip();
  if (interface_ip_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    interface_ip_ = new ::std::string;
  }
  interface_ip_->assign(value);
  // @@protoc_insertion_point(field_set:NodeProperty.interface_ip)
}
inline void NodeProperty::set_interface_ip(const char* value) {
  set_has_interface_ip();
  if (interface_ip_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    interface_ip_ = new ::std::string;
  }
  interface_ip_->assign(value);
  // @@protoc_insertion_point(field_set_char:NodeProperty.interface_ip)
}
inline void NodeProperty::set_interface_ip(const char* value, size_t size) {
  set_has_interface_ip();
  if (interface_ip_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    interface_ip_ = new ::std::string;
  }
  interface_ip_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:NodeProperty.interface_ip)
}
inline ::std::string* NodeProperty::mutable_interface_ip() {
  set_has_interface_ip();
  if (interface_ip_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    interface_ip_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:NodeProperty.interface_ip)
  return interface_ip_;
}
inline ::std::string* NodeProperty::release_interface_ip() {
  clear_has_interface_ip();
  if (interface_ip_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = interface_ip_;
    interface_ip_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void NodeProperty::set_allocated_interface_ip(::std::string* interface_ip) {
  if (interface_ip_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete interface_ip_;
  }
  if (interface_ip) {
    set_has_interface_ip();
    interface_ip_ = interface_ip;
  } else {
    clear_has_interface_ip();
    interface_ip_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:NodeProperty.interface_ip)
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::ProtocolType> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::ProtocolType>() {
  return ::ProtocolType_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_quagga_5fconfig_2eproto__INCLUDED
