#include "roo_onewire/rom_code.h"

#include "gtest/gtest.h"

void PrintTo(const roo_onewire::RomCode& rom_code, std::ostream* os) {
  *os << std::hex << rom_code.raw();
}

namespace roo_onewire {

TEST(RomCode, DefaultConstructor) {
  RomCode code;
  EXPECT_STREQ(code.toString().c_str(), "0000000000000000");
  EXPECT_FALSE(code.isValidUnicast());
  EXPECT_EQ(0, code.raw());
  EXPECT_TRUE(code.isUnknown());
}

TEST(RomCode, Broadcast) {
  RomCode code(0xFFFFFFFFFFFFFFFFULL);
  EXPECT_STREQ(code.toString().c_str(), "FFFFFFFFFFFFFFFF");
  EXPECT_EQ(code, kBroadcastCode);
  EXPECT_FALSE(code.isValidUnicast());
  EXPECT_TRUE(code.isBroadcast());
  EXPECT_FALSE(code.isUnknown());
}

TEST(RomCode, Simple) {
  RomCode code(0x4C0218308496FF28LL);
  EXPECT_STREQ(code.toString().c_str(), "28FF96843018024C");
  EXPECT_TRUE(code.isValidUnicast());
  EXPECT_FALSE(code.isBroadcast());
  EXPECT_FALSE(code.isUnknown());
  EXPECT_EQ(code.getFamily(), 0x28);
}

TEST(RomCodeTest, ToStringAndFromStringRoundTrip) {
  // Example address: 0x0123456789ABCDEF
  RomCode code(0x0123456789ABCDEFULL);
  String str = code.toString();
  EXPECT_STREQ(str.c_str(), "EFCDAB8967452301");
  RomCode parsed = RomCode::FromString(str.c_str());
  EXPECT_STREQ(parsed.toString().c_str(), str.c_str());
  EXPECT_EQ(parsed.raw(), code.raw());
}

TEST(RomCodeTest, FromStringHandlesLowercase) {
  RomCode code = RomCode::FromString("0123456789abcdef");
  EXPECT_STREQ(code.toString().c_str(), "0123456789ABCDEF");
}

TEST(RomCodeTest, FromStringInvalidInput) {
  RomCode code = RomCode::FromString("0123456789ABCDEX");
  EXPECT_STREQ(code.toString().c_str(), "0000000000000000");
}

TEST(RomCodeTest, ToCharArray) {
  RomCode code(0xA1B2C3D4E5F60718ULL);
  char arr[16];
  code.toCharArray(arr);
  EXPECT_EQ(std::string(arr, 16), "1807F6E5D4C3B2A1");
}

}  // namespace roo_onewire
