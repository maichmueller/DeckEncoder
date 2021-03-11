
#ifndef LORDECKENCODER_CODEC_H
#define LORDECKENCODER_CODEC_H

#include <algorithm>
#include <cstdint>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "deck_design.h"
#include "region.h"
#include "varint.h"

class DeckCodec {
  public:
   DeckCodec() = delete;

   /**
    * Encode the deck design as a byte stream to share with other players.
    * The cards are first split into groups of cards with the same count.
    * Then they are further split in set-faction groups, sorted, and subsequently encoded. The full
    * byte stream is then encoded as base32.
    * @param deck_design DeckDesign,
    *      the deck to encode
    * @return std::string,
    *      the encoded deck code
    */
   static std::string encode(const DeckDesign &deck_design);
   /**
    * Decode the deck code into a deck design object.
    * @param deck_code std::string,
    *      the deck code to decode
    * @return DeckDesign,
    *      the deck extracted from the code
    */
   static DeckDesign decode(const std::string &deck_code);
   /**
    * Check the given deck design for correctness. The following errors are
    * checked:
    *      - incorrect code length
    *      - bad set number
    *      - uknown region initials
    *      - bad card number
    *      - card count less than 1
    * @param deck_comp DeckDesign,
    *      the crafted deck to check
    * @return bool,
    *      boolean indicating correctness or a found error in a card
    */
   static bool verify(const DeckDesign &deck_comp);
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
   static std::vector< std::vector< CardToken > > &_sort_groups(
      std::vector< std::vector< CardToken > > &group_of);
   /**
    * Split the cards (which are assumed to have already been pre-selected in terms of count, i.e.
    * all cards in this vector have equal count) in set-faction groups.
    * @param cards std::vector,
    *      the card vector to group
    * @return std::vector<std::vector>>,
    *      the grouped cards vector containing the group vectors
    */
   static std::vector< std::vector< CardToken > > _group_cards(std::vector< CardToken > &cards);
   /**
    * Encodes the group of 4+ count card tokens, as these are handled more
    * simplistically.
    * @param bytes std::string,
    *      the current byte stream to write to
    * @param Nofs std::vector,
    *      the card token vector holding all 4+ count card tokens.
    */
   static void _encode_Nof(std::string &bytes, const std::vector< CardToken > &Nofs);
   /**
    * Encodes the groups of set-faction combinations in the stream
    * @param bytes std::string,
    *      the byte stream to write to
    * @param groups std::vector<std::vector>,
    *      the group vector to encode
    */
   static void _encode_groups(
      std::string &bytes, const std::vector< std::vector< CardToken > > &groups);

   /**
    * Sorts the group of card tokens by the alphanumeric precedence of card codes.
    * @param group_Nof std::vector,
    *      the group vector
    */
   static void _sort_by_code(std::vector< CardToken > &group_Nof);

   static inline void append_varint(std::string &stream, const Varint &vint)
   {
      stream.insert(stream.end(), vint.begin(), vint.end());
   }
};

#endif  // LORDECKENCODER_CODEC_H
