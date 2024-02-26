// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: message.proto

#include "message.pb.h"

#include <algorithm>
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/extension_set.h"
#include "google/protobuf/wire_format_lite.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/reflection_ops.h"
#include "google/protobuf/wire_format.h"
#include "google/protobuf/generated_message_tctable_impl.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"
PROTOBUF_PRAGMA_INIT_SEG
namespace _pb = ::google::protobuf;
namespace _pbi = ::google::protobuf::internal;
namespace _fl = ::google::protobuf::internal::field_layout;

inline constexpr cmdMessage::Impl_::Impl_(
    ::_pbi::ConstantInitialized) noexcept
      : _cached_size_{0},
        cmd_(
            &::google::protobuf::internal::fixed_address_empty_string,
            ::_pbi::ConstantInitialized()),
        output_(
            &::google::protobuf::internal::fixed_address_empty_string,
            ::_pbi::ConstantInitialized()) {}

template <typename>
PROTOBUF_CONSTEXPR cmdMessage::cmdMessage(::_pbi::ConstantInitialized)
    : _impl_(::_pbi::ConstantInitialized()) {}
struct cmdMessageDefaultTypeInternal {
  PROTOBUF_CONSTEXPR cmdMessageDefaultTypeInternal() : _instance(::_pbi::ConstantInitialized{}) {}
  ~cmdMessageDefaultTypeInternal() {}
  union {
    cmdMessage _instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT
    PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 cmdMessageDefaultTypeInternal _cmdMessage_default_instance_;

inline constexpr Hello::Impl_::Impl_(
    ::_pbi::ConstantInitialized) noexcept
      : _cached_size_{0},
        id_(
            &::google::protobuf::internal::fixed_address_empty_string,
            ::_pbi::ConstantInitialized()),
        name_(
            &::google::protobuf::internal::fixed_address_empty_string,
            ::_pbi::ConstantInitialized()) {}

template <typename>
PROTOBUF_CONSTEXPR Hello::Hello(::_pbi::ConstantInitialized)
    : _impl_(::_pbi::ConstantInitialized()) {}
struct HelloDefaultTypeInternal {
  PROTOBUF_CONSTEXPR HelloDefaultTypeInternal() : _instance(::_pbi::ConstantInitialized{}) {}
  ~HelloDefaultTypeInternal() {}
  union {
    Hello _instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT
    PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 HelloDefaultTypeInternal _Hello_default_instance_;
static ::_pb::Metadata file_level_metadata_message_2eproto[2];
static constexpr const ::_pb::EnumDescriptor**
    file_level_enum_descriptors_message_2eproto = nullptr;
static constexpr const ::_pb::ServiceDescriptor**
    file_level_service_descriptors_message_2eproto = nullptr;
const ::uint32_t TableStruct_message_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(
    protodesc_cold) = {
    PROTOBUF_FIELD_OFFSET(::Hello, _impl_._has_bits_),
    PROTOBUF_FIELD_OFFSET(::Hello, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    ~0u,  // no _inlined_string_donated_
    ~0u,  // no _split_
    ~0u,  // no sizeof(Split)
    PROTOBUF_FIELD_OFFSET(::Hello, _impl_.id_),
    PROTOBUF_FIELD_OFFSET(::Hello, _impl_.name_),
    ~0u,
    0,
    PROTOBUF_FIELD_OFFSET(::cmdMessage, _impl_._has_bits_),
    PROTOBUF_FIELD_OFFSET(::cmdMessage, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    ~0u,  // no _inlined_string_donated_
    ~0u,  // no _split_
    ~0u,  // no sizeof(Split)
    PROTOBUF_FIELD_OFFSET(::cmdMessage, _impl_.cmd_),
    PROTOBUF_FIELD_OFFSET(::cmdMessage, _impl_.output_),
    ~0u,
    0,
};

static const ::_pbi::MigrationSchema
    schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
        {0, 10, -1, sizeof(::Hello)},
        {12, 22, -1, sizeof(::cmdMessage)},
};

static const ::_pb::Message* const file_default_instances[] = {
    &::_Hello_default_instance_._instance,
    &::_cmdMessage_default_instance_._instance,
};
const char descriptor_table_protodef_message_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
    "\n\rmessage.proto\"/\n\005Hello\022\n\n\002id\030\001 \001(\t\022\021\n\004"
    "name\030\002 \001(\tH\000\210\001\001B\007\n\005_name\"9\n\ncmdMessage\022\013"
    "\n\003cmd\030\001 \001(\t\022\023\n\006output\030\002 \001(\tH\000\210\001\001B\t\n\007_out"
    "putb\006proto3"
};
static ::absl::once_flag descriptor_table_message_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_message_2eproto = {
    false,
    false,
    131,
    descriptor_table_protodef_message_2eproto,
    "message.proto",
    &descriptor_table_message_2eproto_once,
    nullptr,
    0,
    2,
    schemas,
    file_default_instances,
    TableStruct_message_2eproto::offsets,
    file_level_metadata_message_2eproto,
    file_level_enum_descriptors_message_2eproto,
    file_level_service_descriptors_message_2eproto,
};

// This function exists to be marked as weak.
// It can significantly speed up compilation by breaking up LLVM's SCC
// in the .pb.cc translation units. Large translation units see a
// reduction of more than 35% of walltime for optimized builds. Without
// the weak attribute all the messages in the file, including all the
// vtables and everything they use become part of the same SCC through
// a cycle like:
// GetMetadata -> descriptor table -> default instances ->
//   vtables -> GetMetadata
// By adding a weak function here we break the connection from the
// individual vtables back into the descriptor table.
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_message_2eproto_getter() {
  return &descriptor_table_message_2eproto;
}
// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2
static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_message_2eproto(&descriptor_table_message_2eproto);
// ===================================================================

class Hello::_Internal {
 public:
  using HasBits = decltype(std::declval<Hello>()._impl_._has_bits_);
  static constexpr ::int32_t kHasBitsOffset =
    8 * PROTOBUF_FIELD_OFFSET(Hello, _impl_._has_bits_);
  static void set_has_name(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

Hello::Hello(::google::protobuf::Arena* arena)
    : ::google::protobuf::Message(arena) {
  SharedCtor(arena);
  // @@protoc_insertion_point(arena_constructor:Hello)
}
inline PROTOBUF_NDEBUG_INLINE Hello::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility, ::google::protobuf::Arena* arena,
    const Impl_& from)
      : _has_bits_{from._has_bits_},
        _cached_size_{0},
        id_(arena, from.id_),
        name_(arena, from.name_) {}

Hello::Hello(
    ::google::protobuf::Arena* arena,
    const Hello& from)
    : ::google::protobuf::Message(arena) {
  Hello* const _this = this;
  (void)_this;
  _internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(
      from._internal_metadata_);
  new (&_impl_) Impl_(internal_visibility(), arena, from._impl_);

  // @@protoc_insertion_point(copy_constructor:Hello)
}
inline PROTOBUF_NDEBUG_INLINE Hello::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility,
    ::google::protobuf::Arena* arena)
      : _cached_size_{0},
        id_(arena),
        name_(arena) {}

inline void Hello::SharedCtor(::_pb::Arena* arena) {
  new (&_impl_) Impl_(internal_visibility(), arena);
}
Hello::~Hello() {
  // @@protoc_insertion_point(destructor:Hello)
  _internal_metadata_.Delete<::google::protobuf::UnknownFieldSet>();
  SharedDtor();
}
inline void Hello::SharedDtor() {
  ABSL_DCHECK(GetArena() == nullptr);
  _impl_.id_.Destroy();
  _impl_.name_.Destroy();
  _impl_.~Impl_();
}

PROTOBUF_NOINLINE void Hello::Clear() {
// @@protoc_insertion_point(message_clear_start:Hello)
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.id_.ClearToEmpty();
  cached_has_bits = _impl_._has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    _impl_.name_.ClearNonDefaultToEmpty();
  }
  _impl_._has_bits_.Clear();
  _internal_metadata_.Clear<::google::protobuf::UnknownFieldSet>();
}

const char* Hello::_InternalParse(
    const char* ptr, ::_pbi::ParseContext* ctx) {
  ptr = ::_pbi::TcParser::ParseLoop(this, ptr, ctx, &_table_.header);
  return ptr;
}


PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1
const ::_pbi::TcParseTable<1, 2, 0, 20, 2> Hello::_table_ = {
  {
    PROTOBUF_FIELD_OFFSET(Hello, _impl_._has_bits_),
    0, // no _extensions_
    2, 8,  // max_field_number, fast_idx_mask
    offsetof(decltype(_table_), field_lookup_table),
    4294967292,  // skipmap
    offsetof(decltype(_table_), field_entries),
    2,  // num_field_entries
    0,  // num_aux_entries
    offsetof(decltype(_table_), field_names),  // no aux_entries
    &_Hello_default_instance_._instance,
    ::_pbi::TcParser::GenericFallback,  // fallback
  }, {{
    // optional string name = 2;
    {::_pbi::TcParser::FastUS1,
     {18, 0, 0, PROTOBUF_FIELD_OFFSET(Hello, _impl_.name_)}},
    // string id = 1;
    {::_pbi::TcParser::FastUS1,
     {10, 63, 0, PROTOBUF_FIELD_OFFSET(Hello, _impl_.id_)}},
  }}, {{
    65535, 65535
  }}, {{
    // string id = 1;
    {PROTOBUF_FIELD_OFFSET(Hello, _impl_.id_), -1, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kUtf8String | ::_fl::kRepAString)},
    // optional string name = 2;
    {PROTOBUF_FIELD_OFFSET(Hello, _impl_.name_), _Internal::kHasBitsOffset + 0, 0,
    (0 | ::_fl::kFcOptional | ::_fl::kUtf8String | ::_fl::kRepAString)},
  }},
  // no aux_entries
  {{
    "\5\2\4\0\0\0\0\0"
    "Hello"
    "id"
    "name"
  }},
};

::uint8_t* Hello::_InternalSerialize(
    ::uint8_t* target,
    ::google::protobuf::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Hello)
  ::uint32_t cached_has_bits = 0;
  (void)cached_has_bits;

  // string id = 1;
  if (!this->_internal_id().empty()) {
    const std::string& _s = this->_internal_id();
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
        _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "Hello.id");
    target = stream->WriteStringMaybeAliased(1, _s, target);
  }

  cached_has_bits = _impl_._has_bits_[0];
  // optional string name = 2;
  if (cached_has_bits & 0x00000001u) {
    const std::string& _s = this->_internal_name();
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
        _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "Hello.name");
    target = stream->WriteStringMaybeAliased(2, _s, target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target =
        ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
            _internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Hello)
  return target;
}

::size_t Hello::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Hello)
  ::size_t total_size = 0;

  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string id = 1;
  if (!this->_internal_id().empty()) {
    total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                    this->_internal_id());
  }

  // optional string name = 2;
  cached_has_bits = _impl_._has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                    this->_internal_name());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::google::protobuf::Message::ClassData Hello::_class_data_ = {
    Hello::MergeImpl,
    nullptr,  // OnDemandRegisterArenaDtor
};
const ::google::protobuf::Message::ClassData* Hello::GetClassData() const {
  return &_class_data_;
}

void Hello::MergeImpl(::google::protobuf::Message& to_msg, const ::google::protobuf::Message& from_msg) {
  auto* const _this = static_cast<Hello*>(&to_msg);
  auto& from = static_cast<const Hello&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:Hello)
  ABSL_DCHECK_NE(&from, _this);
  ::uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (!from._internal_id().empty()) {
    _this->_internal_set_id(from._internal_id());
  }
  if ((from._impl_._has_bits_[0] & 0x00000001u) != 0) {
    _this->_internal_set_name(from._internal_name());
  }
  _this->_internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(from._internal_metadata_);
}

void Hello::CopyFrom(const Hello& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Hello)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

PROTOBUF_NOINLINE bool Hello::IsInitialized() const {
  return true;
}

::_pbi::CachedSize* Hello::AccessCachedSize() const {
  return &_impl_._cached_size_;
}
void Hello::InternalSwap(Hello* PROTOBUF_RESTRICT other) {
  using std::swap;
  auto* arena = GetArena();
  ABSL_DCHECK_EQ(arena, other->GetArena());
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_impl_._has_bits_[0], other->_impl_._has_bits_[0]);
  ::_pbi::ArenaStringPtr::InternalSwap(&_impl_.id_, &other->_impl_.id_, arena);
  ::_pbi::ArenaStringPtr::InternalSwap(&_impl_.name_, &other->_impl_.name_, arena);
}

::google::protobuf::Metadata Hello::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_message_2eproto_getter, &descriptor_table_message_2eproto_once,
      file_level_metadata_message_2eproto[0]);
}
// ===================================================================

class cmdMessage::_Internal {
 public:
  using HasBits = decltype(std::declval<cmdMessage>()._impl_._has_bits_);
  static constexpr ::int32_t kHasBitsOffset =
    8 * PROTOBUF_FIELD_OFFSET(cmdMessage, _impl_._has_bits_);
  static void set_has_output(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

cmdMessage::cmdMessage(::google::protobuf::Arena* arena)
    : ::google::protobuf::Message(arena) {
  SharedCtor(arena);
  // @@protoc_insertion_point(arena_constructor:cmdMessage)
}
inline PROTOBUF_NDEBUG_INLINE cmdMessage::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility, ::google::protobuf::Arena* arena,
    const Impl_& from)
      : _has_bits_{from._has_bits_},
        _cached_size_{0},
        cmd_(arena, from.cmd_),
        output_(arena, from.output_) {}

cmdMessage::cmdMessage(
    ::google::protobuf::Arena* arena,
    const cmdMessage& from)
    : ::google::protobuf::Message(arena) {
  cmdMessage* const _this = this;
  (void)_this;
  _internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(
      from._internal_metadata_);
  new (&_impl_) Impl_(internal_visibility(), arena, from._impl_);

  // @@protoc_insertion_point(copy_constructor:cmdMessage)
}
inline PROTOBUF_NDEBUG_INLINE cmdMessage::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility,
    ::google::protobuf::Arena* arena)
      : _cached_size_{0},
        cmd_(arena),
        output_(arena) {}

inline void cmdMessage::SharedCtor(::_pb::Arena* arena) {
  new (&_impl_) Impl_(internal_visibility(), arena);
}
cmdMessage::~cmdMessage() {
  // @@protoc_insertion_point(destructor:cmdMessage)
  _internal_metadata_.Delete<::google::protobuf::UnknownFieldSet>();
  SharedDtor();
}
inline void cmdMessage::SharedDtor() {
  ABSL_DCHECK(GetArena() == nullptr);
  _impl_.cmd_.Destroy();
  _impl_.output_.Destroy();
  _impl_.~Impl_();
}

PROTOBUF_NOINLINE void cmdMessage::Clear() {
// @@protoc_insertion_point(message_clear_start:cmdMessage)
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.cmd_.ClearToEmpty();
  cached_has_bits = _impl_._has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    _impl_.output_.ClearNonDefaultToEmpty();
  }
  _impl_._has_bits_.Clear();
  _internal_metadata_.Clear<::google::protobuf::UnknownFieldSet>();
}

const char* cmdMessage::_InternalParse(
    const char* ptr, ::_pbi::ParseContext* ctx) {
  ptr = ::_pbi::TcParser::ParseLoop(this, ptr, ctx, &_table_.header);
  return ptr;
}


PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1
const ::_pbi::TcParseTable<1, 2, 0, 28, 2> cmdMessage::_table_ = {
  {
    PROTOBUF_FIELD_OFFSET(cmdMessage, _impl_._has_bits_),
    0, // no _extensions_
    2, 8,  // max_field_number, fast_idx_mask
    offsetof(decltype(_table_), field_lookup_table),
    4294967292,  // skipmap
    offsetof(decltype(_table_), field_entries),
    2,  // num_field_entries
    0,  // num_aux_entries
    offsetof(decltype(_table_), field_names),  // no aux_entries
    &_cmdMessage_default_instance_._instance,
    ::_pbi::TcParser::GenericFallback,  // fallback
  }, {{
    // optional string output = 2;
    {::_pbi::TcParser::FastUS1,
     {18, 0, 0, PROTOBUF_FIELD_OFFSET(cmdMessage, _impl_.output_)}},
    // string cmd = 1;
    {::_pbi::TcParser::FastUS1,
     {10, 63, 0, PROTOBUF_FIELD_OFFSET(cmdMessage, _impl_.cmd_)}},
  }}, {{
    65535, 65535
  }}, {{
    // string cmd = 1;
    {PROTOBUF_FIELD_OFFSET(cmdMessage, _impl_.cmd_), -1, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kUtf8String | ::_fl::kRepAString)},
    // optional string output = 2;
    {PROTOBUF_FIELD_OFFSET(cmdMessage, _impl_.output_), _Internal::kHasBitsOffset + 0, 0,
    (0 | ::_fl::kFcOptional | ::_fl::kUtf8String | ::_fl::kRepAString)},
  }},
  // no aux_entries
  {{
    "\12\3\6\0\0\0\0\0"
    "cmdMessage"
    "cmd"
    "output"
  }},
};

::uint8_t* cmdMessage::_InternalSerialize(
    ::uint8_t* target,
    ::google::protobuf::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:cmdMessage)
  ::uint32_t cached_has_bits = 0;
  (void)cached_has_bits;

  // string cmd = 1;
  if (!this->_internal_cmd().empty()) {
    const std::string& _s = this->_internal_cmd();
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
        _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "cmdMessage.cmd");
    target = stream->WriteStringMaybeAliased(1, _s, target);
  }

  cached_has_bits = _impl_._has_bits_[0];
  // optional string output = 2;
  if (cached_has_bits & 0x00000001u) {
    const std::string& _s = this->_internal_output();
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
        _s.data(), static_cast<int>(_s.length()), ::google::protobuf::internal::WireFormatLite::SERIALIZE, "cmdMessage.output");
    target = stream->WriteStringMaybeAliased(2, _s, target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target =
        ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
            _internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:cmdMessage)
  return target;
}

::size_t cmdMessage::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:cmdMessage)
  ::size_t total_size = 0;

  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string cmd = 1;
  if (!this->_internal_cmd().empty()) {
    total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                    this->_internal_cmd());
  }

  // optional string output = 2;
  cached_has_bits = _impl_._has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    total_size += 1 + ::google::protobuf::internal::WireFormatLite::StringSize(
                                    this->_internal_output());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::google::protobuf::Message::ClassData cmdMessage::_class_data_ = {
    cmdMessage::MergeImpl,
    nullptr,  // OnDemandRegisterArenaDtor
};
const ::google::protobuf::Message::ClassData* cmdMessage::GetClassData() const {
  return &_class_data_;
}

void cmdMessage::MergeImpl(::google::protobuf::Message& to_msg, const ::google::protobuf::Message& from_msg) {
  auto* const _this = static_cast<cmdMessage*>(&to_msg);
  auto& from = static_cast<const cmdMessage&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:cmdMessage)
  ABSL_DCHECK_NE(&from, _this);
  ::uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (!from._internal_cmd().empty()) {
    _this->_internal_set_cmd(from._internal_cmd());
  }
  if ((from._impl_._has_bits_[0] & 0x00000001u) != 0) {
    _this->_internal_set_output(from._internal_output());
  }
  _this->_internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(from._internal_metadata_);
}

void cmdMessage::CopyFrom(const cmdMessage& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:cmdMessage)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

PROTOBUF_NOINLINE bool cmdMessage::IsInitialized() const {
  return true;
}

::_pbi::CachedSize* cmdMessage::AccessCachedSize() const {
  return &_impl_._cached_size_;
}
void cmdMessage::InternalSwap(cmdMessage* PROTOBUF_RESTRICT other) {
  using std::swap;
  auto* arena = GetArena();
  ABSL_DCHECK_EQ(arena, other->GetArena());
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_impl_._has_bits_[0], other->_impl_._has_bits_[0]);
  ::_pbi::ArenaStringPtr::InternalSwap(&_impl_.cmd_, &other->_impl_.cmd_, arena);
  ::_pbi::ArenaStringPtr::InternalSwap(&_impl_.output_, &other->_impl_.output_, arena);
}

::google::protobuf::Metadata cmdMessage::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_message_2eproto_getter, &descriptor_table_message_2eproto_once,
      file_level_metadata_message_2eproto[1]);
}
// @@protoc_insertion_point(namespace_scope)
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google
// @@protoc_insertion_point(global_scope)
#include "google/protobuf/port_undef.inc"