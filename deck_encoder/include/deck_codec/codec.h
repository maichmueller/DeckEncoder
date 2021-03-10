
#ifndef LORDECKENCODER_CODEC_H
#define LORDECKENCODER_CODEC_H

#include <algorithm>
#include <cstdint>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "varint.h"
#include "deck_design.h"

enum struct Region {
   BILGEWATER = 0,
   DEMACIA,
   FRELJORD,
   IONIA,
   NOXUS,
   PILTOVER_ZAUN,
   SHADOW_ISLES,
   TARGON,
   SHURIMA
};

class DeckCodec {
   using byte = uint8_t;

  public:
   DeckCodec() = delete;

   static std::string encode(const DeckDesign &deck_design);
   static DeckDesign decode(const std::string &deck_code);

   static bool verify(const DeckDesign &deck_comp);

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

   /// Sort in-place: The sorting convention of this encoding scheme is
   /// First by the number of set/faction combinations in each top-level list
   /// Second by the alphanumeric order of the card codes within those lists.
   static std::vector< std::vector< CardToken > > &_sort_groups(
      std::vector< std::vector< CardToken > > &group_of);
   static std::vector< std::vector< CardToken > > _group_cards(std::vector< CardToken > &cards);
   static void _encode_Nof(std::string &bytes, const std::vector< CardToken > &ofNs);
   static void _encode_group(
      std::string &bytes, const std::vector< std::vector< CardToken > > &groups);

   static inline void append_varint(std::string &stream, const Varint &vint)
   {
      stream.insert(stream.end(), vint.begin(), vint.end());
   }
   static void _sort_by_code(std::vector< CardToken > &group_Nof);
};

#endif  // LORDECKENCODER_CODEC_H
