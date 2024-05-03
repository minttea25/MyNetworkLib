// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_TEST_TEST_H_
#define FLATBUFFERS_GENERATED_TEST_TEST_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 24 &&
              FLATBUFFERS_VERSION_MINOR == 3 &&
              FLATBUFFERS_VERSION_REVISION == 7,
             "Non-compatible flatbuffers version included");

namespace Test {

struct Vec3;

struct Creature;
struct CreatureBuilder;

enum Color : int8_t {
  Color_Red = 1,
  Color_Green = 2,
  Color_Blue = 3,
  Color_MIN = Color_Red,
  Color_MAX = Color_Blue
};

inline const Color (&EnumValuesColor())[3] {
  static const Color values[] = {
    Color_Red,
    Color_Green,
    Color_Blue
  };
  return values;
}

inline const char * const *EnumNamesColor() {
  static const char * const names[4] = {
    "Red",
    "Green",
    "Blue",
    nullptr
  };
  return names;
}

inline const char *EnumNameColor(Color e) {
  if (::flatbuffers::IsOutRange(e, Color_Red, Color_Blue)) return "";
  const size_t index = static_cast<size_t>(e) - static_cast<size_t>(Color_Red);
  return EnumNamesColor()[index];
}

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(4) Vec3 FLATBUFFERS_FINAL_CLASS {
 private:
  float x_;
  float y_;
  float z_;

 public:
  Vec3()
      : x_(0),
        y_(0),
        z_(0) {
  }
  Vec3(float _x, float _y, float _z)
      : x_(::flatbuffers::EndianScalar(_x)),
        y_(::flatbuffers::EndianScalar(_y)),
        z_(::flatbuffers::EndianScalar(_z)) {
  }
  float x() const {
    return ::flatbuffers::EndianScalar(x_);
  }
  float y() const {
    return ::flatbuffers::EndianScalar(y_);
  }
  float z() const {
    return ::flatbuffers::EndianScalar(z_);
  }
};
FLATBUFFERS_STRUCT_END(Vec3, 12);

struct Creature FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef CreatureBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_FRIENDLY = 4,
    VT_POS = 6,
    VT_HP = 8,
    VT_NAME = 10,
    VT_DROPS = 12,
    VT_COLOR = 14
  };
  bool friendly() const {
    return GetField<uint8_t>(VT_FRIENDLY, 0) != 0;
  }
  const Test::Vec3 *pos() const {
    return GetStruct<const Test::Vec3 *>(VT_POS);
  }
  int16_t hp() const {
    return GetField<int16_t>(VT_HP, 1000);
  }
  const ::flatbuffers::String *name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_NAME);
  }
  const ::flatbuffers::Vector<uint8_t> *drops() const {
    return GetPointer<const ::flatbuffers::Vector<uint8_t> *>(VT_DROPS);
  }
  Test::Color color() const {
    return static_cast<Test::Color>(GetField<int8_t>(VT_COLOR, 3));
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint8_t>(verifier, VT_FRIENDLY, 1) &&
           VerifyField<Test::Vec3>(verifier, VT_POS, 4) &&
           VerifyField<int16_t>(verifier, VT_HP, 2) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           VerifyOffset(verifier, VT_DROPS) &&
           verifier.VerifyVector(drops()) &&
           VerifyField<int8_t>(verifier, VT_COLOR, 1) &&
           verifier.EndTable();
  }
};

struct CreatureBuilder {
  typedef Creature Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_friendly(bool friendly) {
    fbb_.AddElement<uint8_t>(Creature::VT_FRIENDLY, static_cast<uint8_t>(friendly), 0);
  }
  void add_pos(const Test::Vec3 *pos) {
    fbb_.AddStruct(Creature::VT_POS, pos);
  }
  void add_hp(int16_t hp) {
    fbb_.AddElement<int16_t>(Creature::VT_HP, hp, 1000);
  }
  void add_name(::flatbuffers::Offset<::flatbuffers::String> name) {
    fbb_.AddOffset(Creature::VT_NAME, name);
  }
  void add_drops(::flatbuffers::Offset<::flatbuffers::Vector<uint8_t>> drops) {
    fbb_.AddOffset(Creature::VT_DROPS, drops);
  }
  void add_color(Test::Color color) {
    fbb_.AddElement<int8_t>(Creature::VT_COLOR, static_cast<int8_t>(color), 3);
  }
  explicit CreatureBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Creature> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Creature>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<Creature> CreateCreature(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    bool friendly = false,
    const Test::Vec3 *pos = nullptr,
    int16_t hp = 1000,
    ::flatbuffers::Offset<::flatbuffers::String> name = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<uint8_t>> drops = 0,
    Test::Color color = Test::Color_Blue) {
  CreatureBuilder builder_(_fbb);
  builder_.add_drops(drops);
  builder_.add_name(name);
  builder_.add_pos(pos);
  builder_.add_hp(hp);
  builder_.add_color(color);
  builder_.add_friendly(friendly);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<Creature> CreateCreatureDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    bool friendly = false,
    const Test::Vec3 *pos = nullptr,
    int16_t hp = 1000,
    const char *name = nullptr,
    const std::vector<uint8_t> *drops = nullptr,
    Test::Color color = Test::Color_Blue) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  auto drops__ = drops ? _fbb.CreateVector<uint8_t>(*drops) : 0;
  return Test::CreateCreature(
      _fbb,
      friendly,
      pos,
      hp,
      name__,
      drops__,
      color);
}

inline const Test::Creature *GetCreature(const void *buf) {
  return ::flatbuffers::GetRoot<Test::Creature>(buf);
}

inline const Test::Creature *GetSizePrefixedCreature(const void *buf) {
  return ::flatbuffers::GetSizePrefixedRoot<Test::Creature>(buf);
}

inline bool VerifyCreatureBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<Test::Creature>(nullptr);
}

inline bool VerifySizePrefixedCreatureBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<Test::Creature>(nullptr);
}

inline void FinishCreatureBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<Test::Creature> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedCreatureBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<Test::Creature> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace Test

#endif  // FLATBUFFERS_GENERATED_TEST_TEST_H_
