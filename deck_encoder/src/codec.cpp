
#include "deck_codec/codec.h"

#include <iostream>

#include "deck_codec/base32.h"
#include "deck_codec/utils.h"
#include "deck_codec/varint.h"

const std::map< std::string, Region > &DeckCodec::str_to_region()
{
   static const std::map< std::string, Region > lookup = {
      {"DE", Region::DEMACIA},
      {"FR", Region::FRELJORD},
      {"IO", Region::IONIA},
      {"NX", Region::NOXUS},
      {"PZ", Region::PILTOVER_ZAUN},
      {"SI", Region::SHADOW_ISLES},
      {"BW", Region::BILGEWATER},
      {"SH", Region::SHURIMA},
      {"MT", Region::TARGON}};
   return lookup;
}

const std::map< Region, std::string > &DeckCodec::region_to_str()
{
   static const std::map< Region, std::string > lookup = {
      {Region::DEMACIA, "DE"},
      {Region::FRELJORD, "FR"},
      {Region::IONIA, "IO"},
      {Region::NOXUS, "NX"},
      {Region::PILTOVER_ZAUN, "PZ"},
      {Region::SHADOW_ISLES, "SI"},
      {Region::BILGEWATER, "BW"},
      {Region::SHURIMA, "SH"},
      {Region::TARGON, "MT"}};
   return lookup;
}
const std::map< size_t, Region > &DeckCodec::id_to_region()
{
   static const std::map< size_t, Region > lookup = {
      {0, Region::DEMACIA},
      {1, Region::FRELJORD},
      {2, Region::IONIA},
      {3, Region::NOXUS},
      {4, Region::PILTOVER_ZAUN},
      {5, Region::SHADOW_ISLES},
      {6, Region::BILGEWATER},
      {7, Region::SHURIMA},
      {9, Region::TARGON},
   };
   return lookup;
}
const std::map< Region, size_t > &DeckCodec::region_to_id()
{
   static const std::map< Region, size_t > lookup = {
      {Region::DEMACIA, 0},
      {Region::FRELJORD, 1},
      {Region::IONIA, 2},
      {Region::NOXUS, 3},
      {Region::PILTOVER_ZAUN, 4},
      {Region::SHADOW_ISLES, 5},
      {Region::BILGEWATER, 6},
      {Region::SHURIMA, 7},
      {Region::TARGON, 9}};
   return lookup;
}

DeckDesign DeckCodec::decode(const std::string &deck_code)
{
   std::vector< CardToken > result;
   std::string bytes;

   try {
      bytes = base32::decode(deck_code);
   } catch(std::invalid_argument &e) {
      throw std::invalid_argument(
         std::string("base32 decoding failed with the message: ") + e.what());
   }
   if(bytes.empty()) {
      throw std::invalid_argument("base32 decoding led to empty byte string.");
   }
   // a copy of the decoded bytes
   std::string byte_list = bytes;

   // grab format and version
   size_t format = bytes[0] >> 4U;
   size_t version = bytes[0] & 0xF;
   byte_list.erase(byte_list.begin());

   if(version > MAX_KNOWN_VERSION) {
      throw std::invalid_argument(
         "The provided code requires a higher version "
         "of this library; please update.");
   }

   for(size_t i = 3; i > 0; i--) {
      int num_group_ofs = Varint::pop_varint(byte_list);

      for(int j = 0; j < num_group_ofs; j++) {
         int num_ofs_in_this_group = Varint::pop_varint(byte_list);
         int set = Varint::pop_varint(byte_list);
         Region region = _to_region(Varint::pop_varint(byte_list));

         for(int k = 0; k < num_ofs_in_this_group; k++) {
            int card = Varint::pop_varint(byte_list);

            std::string set_string = string_utils::pad_left(std::to_string(set), 2, '0');
            std::string region_string = _to_str(region);
            std::string card_string = string_utils::pad_left(std::to_string(card), 3, '0');

            CardToken new_entry{set_string.append(region_string).append(card_string), i};
            result.emplace_back(new_entry);
         }
      }
   }

   // the remainder of the deck code is comprised of entries for cards with
   // counts >= 4 this will only happen in Limited and special game modes. the
   // encoding is simply [count] [cardcode]
   while(not byte_list.empty()) {
      size_t four_plus_count = Varint::pop_varint(byte_list);
      int four_plus_set = Varint::pop_varint(byte_list);
      Region four_plus_faction = _to_region(Varint::pop_varint(byte_list));
      int four_plus_number = Varint::pop_varint(byte_list);

      std::string four_plus_set_string = string_utils::pad_left(
         std::to_string(four_plus_set), 2, '0');
      std::string four_plus_faction_string = _to_str(four_plus_faction);
      std::string four_plus_number_string = string_utils::pad_left(
         std::to_string(four_plus_number), 3, '0');

      CardToken newEntry{
         four_plus_set_string.append(four_plus_faction_string).append(four_plus_number_string),
         four_plus_count};
      result.emplace_back(newEntry);
   }
   return DeckDesign{result};
}

std::string DeckCodec::encode(const DeckDesign &deck_design)
{
   if(not verify(deck_design)) {
      throw std::invalid_argument("The provided deck contains invalid card codes.");
   }

   std::string result{(FORMAT << 4) | VERSION};  // i.e. 00010011 = 19

   std::array< std::vector< CardToken >, 3 > groups_of_Xs;
   std::vector< CardToken > group_of_N;
   for(const auto &deck_card : deck_design) {
      if(auto count = deck_card.count(); count == 3) {
         groups_of_Xs[2].emplace_back(deck_card);
      } else if(count == 2) {
         groups_of_Xs[1].emplace_back(deck_card);
      } else if(count == 1) {
         groups_of_Xs[0].emplace_back(deck_card);
      } else if(count < 1) {
         throw std::invalid_argument(
            "Invalid count of " + std::to_string(count) + " for card " + deck_card.code());
      } else {
         group_of_N.emplace_back(deck_card);
      }
   }

   // Encode
   for(auto &group_Xofs : reverse(groups_of_Xs)) {
      // build the lists of set and faction combinations within the groups of
      // similar card
      auto groups = _group_cards(group_Xofs);
      // to ensure that the same decklist in any order produces the same code,
      // sort them
      _sort_groups(groups);
      _encode_groups(result, groups);
   }
   _sort_by_code(group_of_N);
   _encode_Nof(result, group_of_N);
   return base32::encode(result);
}

std::vector< std::vector< CardToken > > DeckCodec::_group_cards(std::vector< CardToken > &cards)
{
   std::vector< std::vector< CardToken > > result;
   while(not cards.empty()) {
      std::vector< CardToken > current_set;

      // get info from first
      auto &first = cards.front();
      std::string first_code = first.code();
      auto [set, region, number] = parse_card_code(first_code);

      // now add that to our new list, remove from old
      current_set.emplace_back(first);
      cards.erase(cards.begin());

      // the weird lambda capturing is due to structured bindings [set,
      // region,...] not being explicitly capturable by lambdas in C++17. The
      // assignment is a workaround
      auto ptr_to_start_erase = std::remove_if(
         cards.begin(),
         cards.end(),
         [&set = set, &region = region, &current_set = current_set](const auto &card) {
            std::string this_code = card.code();
            int this_set = std::stoi(this_code.substr(0, 2));
            Region this_region = _to_region(this_code.substr(2, 2));

            if(this_set == set && this_region == region) {
               // asside from removing the cards from the list, also
               // store them in the current set vector
               current_set.emplace_back(card);
               return true;
            }
            return false;
         });
      cards.erase(ptr_to_start_erase, cards.end());
      result.emplace_back(current_set);
   }
   return result;
}

void DeckCodec::_encode_groups(
   std::string &bytes, const std::vector< std::vector< CardToken > > &groups)
{
   auto n_groups_vint = Varint::from_int(groups.size());
   append_varint(bytes, n_groups_vint);

   for(const auto &group : groups) {
      // how many cards in current group?
      append_varint(bytes, Varint::from_int(group.size()));

      // determine this group by first declaring the set number and faction
      std::string curr_code = group[0].code();
      auto [set, region, number] = parse_card_code(curr_code);
      for(const auto &vint : {Varint::from_int(set), Varint::from_int(_to_int(region))}) {
         append_varint(bytes, vint);
      }

      // now the cards within this group, as identified by the third section of
      // card code only now,
      for(const auto &deck_card : group) {
         std::string code = deck_card.code();
         auto number_vint = Varint::from_int(std::stoi(code.substr(4, 3)));
         append_varint(bytes, number_vint);
      }
   }
}

bool DeckCodec::verify(const DeckDesign &deck_comp)
{
   return std::all_of(deck_comp.begin(), deck_comp.end(), [](const auto &deck_card) {
      if(deck_card.code().size() != CARD_CODE_LENGTH) {
         return false;
      }
      auto card_code = deck_card.code();

      if(not _parseable_as_int(card_code.substr(0, 2))) {
         return false;
      }
      if(str_to_region().find(card_code.substr(2, 2)) == str_to_region().end()) {
         return false;
      }
      if(not _parseable_as_int(card_code.substr(4, 3))) {
         return false;
      }
      if(deck_card.count() < 1) {
         return false;
      }
      return true;
   });
}
bool DeckCodec::_parseable_as_int(const std::string &s)
{
   return s.find_first_not_of("0123456789") == std::string::npos;
}

std::tuple< int, Region, int > DeckCodec::parse_card_code(const std::string &code)
{
   int set = std::stoi(code.substr(0, 2));
   Region region = _to_region(code.substr(2, 2));
   int number = std::stoi(code.substr(4, 3));
   return {set, region, number};
}

std::vector< std::vector< CardToken > > &DeckCodec::_sort_groups(
   std::vector< std::vector< CardToken > > &group_of)
{
   std::stable_sort(
      group_of.begin(),
      group_of.end(),
      [](const std::vector< CardToken > &g1, const std::vector< CardToken > &g2) {
         auto s1 = g1.size();
         auto s2 = g2.size();
         return (s1 < s2) || (s1 == s2) && (g1[0].code() < g2[0].code());
      });

   for(auto &g : group_of) {
      _sort_by_code(g);
   }
   return group_of;
}

void DeckCodec::_encode_Nof(std::string &bytes, const std::vector< CardToken > &Nofs)
{
   for(const auto &deck_card : Nofs) {
      auto count_as_varint = Varint::from_int(deck_card.count());
      bytes.insert(bytes.end(), count_as_varint.begin(), count_as_varint.end());

      auto [set, region, number] = parse_card_code(deck_card.code());
      int region_int = _to_int(region);

      for(const auto &varint :
          {Varint::from_int(set), Varint::from_int(region_int), Varint::from_int(number)}) {
         append_varint(bytes, varint);
      }
   }
}

void DeckCodec::_sort_by_code(std::vector< CardToken > &group_Nof)
{
   std::stable_sort(
      group_Nof.begin(), group_Nof.end(), [](const CardToken &dc1, const CardToken &dc2) {
         return dc1.code() < dc2.code();
      });
}
