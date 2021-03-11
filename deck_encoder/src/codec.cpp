
#include "deck_codec/codec.h"

#include "deck_codec/base32.h"
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
