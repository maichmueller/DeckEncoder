
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <filesystem>

#include "deck_codec/base32.h"
#include "deck_codec/codec.h"
#include "deck_codec/string_utils.h"
#include "gtest/gtest.h"

std::map< std::string, std::vector<CardToken> > read_case_file(const std::filesystem::path& filepath)
{
   std::map< std::string, std::vector<CardToken> > out_data;
   auto p = std::filesystem::current_path();

   auto path = std::filesystem::path(filepath);
   std::ifstream infile(path);
   std::string delimiter = ":";
   std::string line;
   std::vector<CardToken> dcomp;
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
         dcomp.emplace_back(card_code, count);
      } else {
         dcode = line;
         dcomp = std::vector<CardToken>{};
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

      std::vector<CardToken> decoded = DeckCodec::decode<CardToken>(dcode);
      EXPECT_TRUE(container_eq(dcomp, decoded));

      std::vector<CardToken> decoded_from_encoded = DeckCodec::decode<CardToken>(encoded);
      EXPECT_TRUE(container_eq(dcomp, decoded_from_encoded));
   }
}

TEST(custom_deck, large_deck)
{
   std::vector<CardToken> deck{
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
   std::vector<CardToken> decoded = DeckCodec::decode<CardToken>(encoded);
   EXPECT_TRUE(container_eq(decoded, deck));
}

TEST(small_deck, single_card_deck)
{
   std::vector<CardToken> deck(1, CardToken{"01DE002", 1});
   std::string encoded = DeckCodec::encode(deck);
   std::vector<CardToken> decoded = DeckCodec::decode<CardToken>(encoded);
   EXPECT_TRUE(container_eq(decoded, deck));
}

TEST(singles, single_card_40_times)
{
   std::vector<CardToken> deck(1, CardToken{"01DE002", 40});
   std::string encoded = DeckCodec::encode(deck);
   std::vector<CardToken> decoded = DeckCodec::decode<CardToken>(encoded);
   EXPECT_TRUE(container_eq(decoded, deck));
}

TEST(multiples, more_than_3ofs_only)
{
   std::vector<CardToken> deck = std::vector< CardToken >{
      {"01DE002", 4}, {"01DE003", 4}, {"01DE004", 4}, {"01DE005", 4}, {"01DE006", 4},
      {"01DE007", 5}, {"01DE008", 6}, {"01DE009", 7}, {"01DE010", 8}, {"01DE011", 9},
      {"01DE012", 4}, {"01DE013", 4}, {"01DE014", 4}, {"01DE015", 4}, {"01DE016", 4},
      {"01DE017", 4}, {"01DE018", 4}, {"01DE019", 4}, {"01DE020", 4}, {"01DE021", 4}};

   std::string encoded = DeckCodec::encode(deck);
   std::vector<CardToken> decoded = DeckCodec::decode<CardToken>(encoded);

   EXPECT_TRUE(container_eq(decoded, deck));
}

TEST(order, order_should_not_matter)
{
   std::vector<CardToken> deck1 = std::vector< CardToken >{{"01DE002", 1}, {"01DE003", 2}, {"02DE003", 3}};
   std::vector<CardToken> deck2 = std::vector< CardToken >{{"01DE003", 2}, {"02DE003", 3}, {"01DE002", 1}};

   std::string encoded1 = DeckCodec::encode(deck1);
   std::string encoded2 = DeckCodec::encode(deck2);
   EXPECT_EQ(encoded1, encoded2);

   std::vector<CardToken> deck3 = std::vector< CardToken >{{"01DE002", 4}, {"01DE003", 2}, {"02DE003", 3}};
   std::vector<CardToken> deck4 = std::vector< CardToken >{{"01DE003", 2}, {"02DE003", 3}, {"01DE002", 4}};

   std::string encoded3 = DeckCodec::encode(deck1);
   std::string encoded4 = DeckCodec::encode(deck2);
   EXPECT_EQ(encoded3, encoded4);
}

TEST(order, order_should_not_matter2)
{
   // importantly this order test includes more than 1 card with counts >3, which are sorted by card
   // code and appending to the <=3 encodings.
   std::vector<CardToken> deck1 = std::vector< CardToken >{
      {"01DE002", 4}, {"01DE003", 2}, {"02DE003", 3}, {"01DE004", 5}};

   std::vector<CardToken> deck2 = std::vector< CardToken >{
      {"01DE004", 5}, {"01DE003", 2}, {"02DE003", 3}, {"01DE002", 4}};

   std::string encoded1 = DeckCodec::encode(deck1);
   std::string encoded2 = DeckCodec::encode(deck2);
   EXPECT_EQ(encoded1, encoded2);
}

TEST(specific_regions, bilgewater)
{
   std::vector<CardToken> deck = std::vector< CardToken >{
      {"01DE002", 4}, {"02BW003", 2}, {"02BW010", 3}, {"01DE004", 5}};

   std::string encoded = DeckCodec::encode(deck);
   std::vector<CardToken> decoded = DeckCodec::decode<CardToken>(encoded);
   EXPECT_TRUE(container_eq(decoded, deck));
}

TEST(specific_regions, shurima)
{
   std::vector<CardToken> deck = std::vector< CardToken >{
      {"01DE002", 4}, {"02BW003", 2}, {"02BW010", 3}, {"04SH047", 5}};

   std::string encoded = DeckCodec::encode(deck);
   std::vector<CardToken> decoded = DeckCodec::decode<CardToken>(encoded);
   EXPECT_TRUE(container_eq(decoded, deck));
}

TEST(specific_regions, targon)
{
   std::vector<CardToken> deck = std::vector< CardToken >{
      {"01DE002", 4}, {"03MT003", 2}, {"03MT010", 3}, {"02BW004", 5}};

   std::string encoded = DeckCodec::encode(deck);
   std::vector<CardToken> decoded = DeckCodec::decode<CardToken>(encoded);
   EXPECT_TRUE(container_eq(decoded, deck));
}

TEST(invalids, bad_version)
{
   // make sure that a deck with an invalid version fails
   std::vector<CardToken> deck = std::vector< CardToken >{
      {"01DE002", 4}, {"01DE003", 2}, {"02DE003", 3}, {"01DE004", 5}};

   std::string encoded = DeckCodec::encode(deck);
   std::string b32_decoded = base32::decode(encoded);

   // replace the format_version info with a wrong one
   b32_decoded[0] = 88;
   std::string bad_encoded = base32::encode(b32_decoded);

   EXPECT_THROW(DeckCodec::decode<CardToken>(bad_encoded), std::invalid_argument);
}

TEST(invalids, bad_card_codes)
{
   std::vector<CardToken> deck = std::vector< CardToken >{{"01DE02", 1}};
   EXPECT_THROW(DeckCodec::encode(deck), std::invalid_argument);

   deck = std::vector< CardToken >{{"01XX002", 1}};
   EXPECT_THROW(DeckCodec::encode(deck), std::invalid_argument);
}

TEST(invalids, bad_card_counts)
{
   std::vector<CardToken> deck = std::vector< CardToken >{{"01DE002", 0}};
   EXPECT_THROW(DeckCodec::encode(deck), std::invalid_argument);
}

TEST(invalids, garbage_decoding)
{
   std::string bad_encoding_not_base32 = "I'm no card code!";
   EXPECT_THROW(DeckCodec::decode<CardToken>(bad_encoding_not_base32), std::invalid_argument);
   std::string bad_encoding32 = "ABCDEFG";
   EXPECT_THROW(DeckCodec::decode<CardToken>(bad_encoding32), std::invalid_argument);
   std::string bad_encoding_empty = "";
   EXPECT_THROW(DeckCodec::decode<CardToken>(bad_encoding_empty), std::invalid_argument);
}