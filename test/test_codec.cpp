
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <filesystem>

#include "deck_codec/base32.h"
#include "deck_codec/codec.h"
#include "deck_codec/string_utils.h"
#include "gtest/gtest.h"

std::map< std::string, DeckDesign > read_case_file(const std::filesystem::path& filepath)
{
   std::map< std::string, DeckDesign > out_data;
   auto p = std::filesystem::current_path();

   auto path = std::filesystem::path(filepath);
   std::ifstream infile(path);
   std::string delimiter = ":";
   std::string line;
   DeckDesign dcomp;
   std::string dcode;
   while(std::getline(infile, line)) {
      string_utils::trim(line);
      if(line.empty()) {
         out_data.emplace(dcode, dcomp);
         continue;
      }
      if(auto delim_pos = line.find(delimiter); delim_pos != std::string::npos) {
         int count = std::stoi(line.substr(0, delim_pos));
         std::string card_code = line.substr(delim_pos + 1, line.size());
         dcomp.add(card_code, count);
      } else {
         dcode = line;
         dcomp = DeckDesign{};
      }
   }
   return out_data;
}

TEST(load_cases, loading)
{
   auto decks = read_case_file("../test/test_cases.txt");
   // Encode each test deck and ensure it's equal to the correct string.
   // Then decode and ensure the deck is unchanged.
   for(auto& [dcode, dcomp] : decks) {
      std::string encoded = DeckCodec::encode(dcomp);
      EXPECT_EQ(dcode, encoded);

      DeckDesign decoded = DeckCodec::decode(dcode);
      EXPECT_EQ(dcomp, decoded);

      DeckDesign decoded_from_encoded = DeckCodec::decode(encoded);
      EXPECT_EQ(dcomp, decoded_from_encoded);
   }
}

TEST(custom_deck, large_deck)
{
   DeckDesign deck = std::vector< CardToken >{
      {"01SI011", 3},
      {"01SI013", 3},
      {"01SI001", 3},
      {"01SI002", 2},
      {"01SI010", 2},
      {"01SI020", 2},
      {"01DE017", 2},
      {"01DE020", 2},
      {"01FR031", 2},
      {"01FR041", 1},
      {"01DE001", 1},
      {"01FR047", 1},
      {"01NX001", 1},
      {"01NX002", 1},
      {"01NX004", 1}};

   std::string encoded = DeckCodec::encode(deck);
   DeckDesign decoded = DeckCodec::decode(encoded);

   EXPECT_EQ(decoded, deck);
}

TEST(small_deck, single_card_deck)
{
   DeckDesign deck(1, CardToken{"01DE002", 1});
   std::string encoded = DeckCodec::encode(deck);
   DeckDesign decoded = DeckCodec::decode(encoded);
   EXPECT_EQ(deck, decoded);
}

TEST(singles, single_card_40_times)
{
   DeckDesign deck(1, CardToken{"01DE002", 40});
   std::string encoded = DeckCodec::encode(deck);
   DeckDesign decoded = DeckCodec::decode(encoded);
   EXPECT_EQ(deck, decoded);
}

TEST(multiples, more_than_3ofs_only)
{
   DeckDesign deck = std::vector< CardToken >{
      {"01DE002", 4}, {"01DE003", 4}, {"01DE004", 4}, {"01DE005", 4}, {"01DE006", 4},
      {"01DE007", 5}, {"01DE008", 6}, {"01DE009", 7}, {"01DE010", 8}, {"01DE011", 9},
      {"01DE012", 4}, {"01DE013", 4}, {"01DE014", 4}, {"01DE015", 4}, {"01DE016", 4},
      {"01DE017", 4}, {"01DE018", 4}, {"01DE019", 4}, {"01DE020", 4}, {"01DE021", 4}};

   std::string encoded = DeckCodec::encode(deck);
   DeckDesign decoded = DeckCodec::decode(encoded);

   EXPECT_EQ(decoded, deck);
}

TEST(order, order_should_not_matter)
{
   DeckDesign deck1 = std::vector< CardToken >{{"01DE002", 1}, {"01DE003", 2}, {"02DE003", 3}};
   DeckDesign deck2 = std::vector< CardToken >{{"01DE003", 2}, {"02DE003", 3}, {"01DE002", 1}};

   std::string encoded1 = DeckCodec::encode(deck1);
   std::string encoded2 = DeckCodec::encode(deck2);
   EXPECT_EQ(encoded1, encoded2);

   DeckDesign deck3 = std::vector< CardToken >{{"01DE002", 4}, {"01DE003", 2}, {"02DE003", 3}};
   DeckDesign deck4 = std::vector< CardToken >{{"01DE003", 2}, {"02DE003", 3}, {"01DE002", 4}};

   std::string encoded3 = DeckCodec::encode(deck1);
   std::string encoded4 = DeckCodec::encode(deck2);
   EXPECT_EQ(encoded3, encoded4);
}

TEST(order, order_should_not_matter2)
{
   // importantly this order test includes more than 1 card with counts >3, which are sorted by card
   // code and appending to the <=3 encodings.
   DeckDesign deck1 = std::vector< CardToken >{
      {"01DE002", 4}, {"01DE003", 2}, {"02DE003", 3}, {"01DE004", 5}};

   DeckDesign deck2 = std::vector< CardToken >{
      {"01DE004", 5}, {"01DE003", 2}, {"02DE003", 3}, {"01DE002", 4}};

   std::string encoded1 = DeckCodec::encode(deck1);
   std::string encoded2 = DeckCodec::encode(deck2);
   EXPECT_EQ(encoded1, encoded2);
}

TEST(specific_regions, bilgewater)
{
   DeckDesign deck = std::vector< CardToken >{
      {"01DE002", 4}, {"02BW003", 2}, {"02BW010", 3}, {"01DE004", 5}};

   std::string encoded = DeckCodec::encode(deck);
   DeckDesign decoded = DeckCodec::decode(encoded);
   EXPECT_EQ(deck, decoded);
}

TEST(specific_regions, shurima)
{
   DeckDesign deck = std::vector< CardToken >{
      {"01DE002", 4}, {"02BW003", 2}, {"02BW010", 3}, {"04SH047", 5}};

   std::string encoded = DeckCodec::encode(deck);
   DeckDesign decoded = DeckCodec::decode(encoded);
   EXPECT_EQ(deck, decoded);
}

TEST(specific_regions, targon)
{
   DeckDesign deck = std::vector< CardToken >{
      {"01DE002", 4}, {"03MT003", 2}, {"03MT010", 3}, {"02BW004", 5}};

   std::string encoded = DeckCodec::encode(deck);
   DeckDesign decoded = DeckCodec::decode(encoded);
   EXPECT_EQ(deck, decoded);
}

TEST(invalids, bad_version)
{
   // make sure that a deck with an invalid version fails
   DeckDesign deck = std::vector< CardToken >{
      {"01DE002", 4}, {"01DE003", 2}, {"02DE003", 3}, {"01DE004", 5}};

   std::string encoded = DeckCodec::encode(deck);
   std::string b32_decoded = base32::decode(encoded);

   // replace the format_version info with a wrong one
   b32_decoded[0] = 88;
   std::string bad_encoded = base32::encode(b32_decoded);

   EXPECT_THROW(DeckCodec::decode(bad_encoded), std::invalid_argument);
}

TEST(invalids, bad_card_codes)
{
   DeckDesign deck = std::vector< CardToken >{{"01DE02", 1}};
   EXPECT_THROW(DeckCodec::encode(deck), std::invalid_argument);

   deck = std::vector< CardToken >{{"01XX002", 1}};
   EXPECT_THROW(DeckCodec::encode(deck), std::invalid_argument);
}

TEST(invalids, bad_card_counts)
{
   DeckDesign deck = std::vector< CardToken >{{"01DE002", 0}};
   EXPECT_THROW(DeckCodec::encode(deck), std::invalid_argument);
}

TEST(invalids, garbage_decoding)
{
   std::string bad_encoding_not_base32 = "I'm no card code!";
   EXPECT_THROW(DeckCodec::decode(bad_encoding_not_base32), std::invalid_argument);
   std::string bad_encoding32 = "ABCDEFG";
   EXPECT_THROW(DeckCodec::decode(bad_encoding32), std::invalid_argument);
   std::string bad_encoding_empty = "";
   EXPECT_THROW(DeckCodec::decode(bad_encoding_empty), std::invalid_argument);
}