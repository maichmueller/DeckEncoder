#include "deck_codec/base32.h"

#include "gtest/gtest.h"

TEST(base32_unittests, base32_basic)
{
   std::vector< std::string > origs{
      "the quick brown fox jumps over the lazy dog",
      "input",
      "Base32 encode function",
      "quite long sample sentence here for encoding purposes"};
   // the padding "=" symbols, which other encoders would produce need to be removed. This seems to
   // have been a deliberate choice in the RIOT API
   std::vector< std::string > origs_encoded{
      "ORUGKIDROVUWG2ZAMJZG653OEBTG66BANJ2W24DTEBXXMZLSEB2GQZJANRQXU6JAMRXWO",
      "NFXHA5LU",
      "IJQXGZJTGIQGK3TDN5SGKIDGOVXGG5DJN5XA",
      "OF2WS5DFEBWG63THEBZWC3LQNRSSA43FNZ2GK3TDMUQGQZLSMUQGM33SEBSW4Y3PMRUW4ZZAOB2XE4DPONSXG"};

   for(int i = 0; i < origs.size(); i++) {
      std::string encoded = base32::encode(origs[i]);
      EXPECT_EQ(encoded, origs_encoded[i]);

      auto decoded = base32::decode(origs_encoded[i]);
      EXPECT_EQ(decoded, origs[i]);
   }
}