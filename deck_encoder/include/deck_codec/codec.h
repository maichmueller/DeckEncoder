
#ifndef LORDECKENCODER_CODEC_H
#define LORDECKENCODER_CODEC_H

#include <algorithm>
#include <cstdint>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "base32.h"
#include "card_token.h"
#include "region.h"
#include "utils.h"
#include "varint.h"

class DeckCodec {
  public:
   DeckCodec() = delete;

   /**
    * Encode the deck design as a byte stream to share with other players.
    * The cards are first split into groups of cards with the same count.
    * Then they are further split in set-faction groups, sorted, and subsequently encoded. The full
    * byte stream is then encoded as base32.
    * @param deck std::vector<CardCountType>,
    *      the deck to encode
    * @return std::string,
    *      the encoded deck code
    */
   template < typename DeckContainer >
   static std::string encode(const DeckContainer &deck);
   /**
    * Decode the deck code into a deck design object.
    * @param deck_code std::string,
    *      the deck code to decode
    * @return std::vector<CardCountType>,
    *      the deck extracted from the code
    */
   template < typename CodeCountType >
   static std::vector< CodeCountType > decode(const std::string &deck_code);
   /**
    * Check the given deck design for correctness. The following errors are
    * checked:
    *      - incorrect code length
    *      - bad set number
    *      - uknown region initials
    *      - bad card number
    *      - card count less than 1
    * @param deck_comp std::vector<CardCountType>,
    *      the crafted deck to check
    * @return bool,
    *      boolean indicating correctness or a found error in a card
    */
   template < typename DeckContainer >
   static bool verify(const DeckContainer &deck_comp);
   /**
    * Parses the card code and returns the tuple of contained information. Each
    * code is of the form XXYYZZZ, with
    *      XX  = the set number (left-padded with '0' if required)
    *      YY  = the region chars
    *      ZZZ = the card number (left-padded with '0' of required)
    * @param code std::string,
    *      the card code to parse
    * @return, std::tuple,
    *      the tuple of int, Region, int holding the set number, the region, and
    *      the card number repsectively
    */
   static std::tuple< int, Region, int > parse_card_code(const std::string &code);

  private:
   static const size_t CARD_CODE_LENGTH = 7;
   static const size_t FORMAT = 1;
   static const size_t VERSION = 3;
   static const size_t MAX_KNOWN_VERSION = 3;

   static const std::map< Region, size_t > &region_to_id();
   static const std::map< size_t, Region > &id_to_region();
   static const std::map< Region, std::string > &region_to_str();
   static const std::map< std::string, Region > &str_to_region();

   static Region _to_region(const std::string &str) { return str_to_region().at(str); }
   static std::string _to_str(Region reg) { return region_to_str().at(reg); }
   static Region _to_region(size_t id) { return id_to_region().at(id); }
   static size_t _to_int(Region reg) { return region_to_id().at(reg); }

   static bool _parseable_as_int(const std::string &s);
   /**
    * Sorts in-place the groups of set-faction combination. Each group is first
    * sorted by the number of card tokens contained, and, if required, by the
    * alphanumeric precedence of the code of their first entry as tiebreaker. In a
    * second step in-group sorting is done via alphanumerical card code comparison.
    * sorting step
    * @param group_of std::vector<std::vector>>,
    *      the vector holding the set-faction groups
    * @return std::vector&,
    *      the given reference to the input variable to allow chaining of commands.
    */
   template < typename CodeCountType >
   static std::vector< std::vector< CodeCountType > > &_sort_groups(
      std::vector< std::vector< CodeCountType > > &group_of);
   /**
    * Split the cards (which are assumed to have already been pre-selected in terms of count, i.e.
    * all cards in this vector have equal count) in set-faction groups.
    * @param cards std::vector,
    *      the card vector to group
    * @return std::vector<std::vector>>,
    *      the grouped cards vector containing the group vectors
    */
   template < typename CodeCountType >
   static std::vector< std::vector< CodeCountType > > _group_cards(
      std::vector< CodeCountType > &cards);
   /**
    * Encodes the group of 4+ count card tokens, as these are handled more
    * simplistically.
    * @param bytes std::string,
    *      the current byte stream to write to
    * @param Nofs std::vector,
    *      the card token vector holding all 4+ count card tokens.
    */
   template < typename CodeCountType >
   static void _encode_Nof(std::string &bytes, const std::vector< CodeCountType > &Nofs);
   /**
    * Encodes the groups of set-faction combinations in the stream
    * @param bytes std::string,
    *      the byte stream to write to
    * @param groups std::vector<std::vector>,
    *      the group vector to encode
    */
   template < typename CodeCountType >
   static void _encode_groups(
      std::string &bytes, const std::vector< std::vector< CodeCountType > > &groups);

   /**
    * Sorts the group of card tokens by the alphanumeric precedence of card codes.
    * @param group_Nof std::vector,
    *      the group vector
    */
   template < typename CodeCountType >
   static void _sort_by_code(std::vector< CodeCountType > &group_Nof);

   static inline void append_varint(std::string &stream, const Varint &vint)
   {
      stream.insert(stream.end(), vint.begin(), vint.end());
   }
};

template < typename DeckContainer >
bool DeckCodec::verify(const DeckContainer &deck_comp)
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

template < typename DeckContainer >
std::string DeckCodec::encode(const DeckContainer &deck)
{
   if(not verify(deck)) {
      throw std::invalid_argument("The provided deck contains invalid card codes.");
   }

   using CardT = typename DeckContainer::value_type;

   std::string result{(FORMAT << 4) | VERSION};  // i.e. 00010011 = 19

   std::array< std::vector< CardT >, 3 > groups_of_Xs;
   std::vector< CardT > group_of_N;
   for(const auto &deck_card : deck) {
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

template < typename CodeCountType >
std::vector< CodeCountType > DeckCodec::decode(const std::string &deck_code)
{
   std::vector< CodeCountType > result;
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

            CodeCountType new_entry{set_string.append(region_string).append(card_string), i};
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

      CodeCountType newEntry{
         four_plus_set_string.append(four_plus_faction_string).append(four_plus_number_string),
         four_plus_count};
      result.emplace_back(newEntry);
   }
   return result;
}

template < typename CodeCountType >
std::vector< std::vector< CodeCountType > > &DeckCodec::_sort_groups(
   std::vector< std::vector< CodeCountType > > &group_of)
{
   std::stable_sort(
      group_of.begin(),
      group_of.end(),
      [](const std::vector< CodeCountType > &g1, const std::vector< CodeCountType > &g2) {
         auto s1 = g1.size();
         auto s2 = g2.size();
         return (s1 < s2) || (s1 == s2) && (g1[0].code() < g2[0].code());
      });

   for(auto &g : group_of) {
      _sort_by_code(g);
   }
   return group_of;
}

template < typename CodeCountType >
std::vector< std::vector< CodeCountType > > DeckCodec::_group_cards(
   std::vector< CodeCountType > &cards)
{
   std::vector< std::vector< CodeCountType > > result;
   while(not cards.empty()) {
      std::vector< CodeCountType > current_set;

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

template < typename CardCountType >
void DeckCodec::_encode_groups(
   std::string &bytes, const std::vector< std::vector< CardCountType > > &groups)
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

template < typename CardCountType >
void DeckCodec::_encode_Nof(std::string &bytes, const std::vector< CardCountType > &Nofs)
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

template < typename CardCountType >
void DeckCodec::_sort_by_code(std::vector< CardCountType > &group_Nof)
{
   std::stable_sort(group_Nof.begin(), group_Nof.end(), [](const auto &dc1, const auto &dc2) {
      return dc1.code() < dc2.code();
   });
}

#endif  // LORDECKENCODER_CODEC_H
