// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_PACKET_NETCORE_PACKET_H_
#define FLATBUFFERS_GENERATED_PACKET_NETCORE_PACKET_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 24 &&
              FLATBUFFERS_VERSION_MINOR == 3 &&
              FLATBUFFERS_VERSION_REVISION == 7,
             "Non-compatible flatbuffers version included");

namespace NetCore {
namespace Packet {

struct PacketInfo;

struct TestPacket;
struct TestPacketBuilder;

struct Packet;
struct PacketBuilder;

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(2) PacketInfo FLATBUFFERS_FINAL_CLASS {
 private:
  uint16_t id_;
  uint16_t size_;

 public:
  PacketInfo()
      : id_(0),
        size_(0) {
  }
  PacketInfo(uint16_t _id, uint16_t _size)
      : id_(::flatbuffers::EndianScalar(_id)),
        size_(::flatbuffers::EndianScalar(_size)) {
  }
  uint16_t id() const {
    return ::flatbuffers::EndianScalar(id_);
  }
  uint16_t size() const {
    return ::flatbuffers::EndianScalar(size_);
  }
};
FLATBUFFERS_STRUCT_END(PacketInfo, 4);

struct TestPacket FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef TestPacketBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_INFO = 4,
    VT_MSG = 6
  };
  const NetCore::Packet::PacketInfo *info() const {
    return GetStruct<const NetCore::Packet::PacketInfo *>(VT_INFO);
  }
  const ::flatbuffers::String *msg() const {
    return GetPointer<const ::flatbuffers::String *>(VT_MSG);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<NetCore::Packet::PacketInfo>(verifier, VT_INFO, 2) &&
           VerifyOffset(verifier, VT_MSG) &&
           verifier.VerifyString(msg()) &&
           verifier.EndTable();
  }
};

struct TestPacketBuilder {
  typedef TestPacket Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_info(const NetCore::Packet::PacketInfo *info) {
    fbb_.AddStruct(TestPacket::VT_INFO, info);
  }
  void add_msg(::flatbuffers::Offset<::flatbuffers::String> msg) {
    fbb_.AddOffset(TestPacket::VT_MSG, msg);
  }
  explicit TestPacketBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<TestPacket> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<TestPacket>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<TestPacket> CreateTestPacket(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const NetCore::Packet::PacketInfo *info = nullptr,
    ::flatbuffers::Offset<::flatbuffers::String> msg = 0) {
  TestPacketBuilder builder_(_fbb);
  builder_.add_msg(msg);
  builder_.add_info(info);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<TestPacket> CreateTestPacketDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const NetCore::Packet::PacketInfo *info = nullptr,
    const char *msg = nullptr) {
  auto msg__ = msg ? _fbb.CreateString(msg) : 0;
  return NetCore::Packet::CreateTestPacket(
      _fbb,
      info,
      msg__);
}

struct Packet FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef PacketBuilder Builder;
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           verifier.EndTable();
  }
};

struct PacketBuilder {
  typedef Packet Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  explicit PacketBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Packet> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Packet>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<Packet> CreatePacket(
    ::flatbuffers::FlatBufferBuilder &_fbb) {
  PacketBuilder builder_(_fbb);
  return builder_.Finish();
}

inline const NetCore::Packet::Packet *GetPacket(const void *buf) {
  return ::flatbuffers::GetRoot<NetCore::Packet::Packet>(buf);
}

inline const NetCore::Packet::Packet *GetSizePrefixedPacket(const void *buf) {
  return ::flatbuffers::GetSizePrefixedRoot<NetCore::Packet::Packet>(buf);
}

inline bool VerifyPacketBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<NetCore::Packet::Packet>(nullptr);
}

inline bool VerifySizePrefixedPacketBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<NetCore::Packet::Packet>(nullptr);
}

inline void FinishPacketBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<NetCore::Packet::Packet> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedPacketBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<NetCore::Packet::Packet> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace Packet
}  // namespace NetCore

#endif  // FLATBUFFERS_GENERATED_PACKET_NETCORE_PACKET_H_
