

#ifndef LORDECKENCODER_DECK_DESIGN_H
#define LORDECKENCODER_DECK_DESIGN_H

class CardToken {
  public:
   CardToken(std::string code, size_t count) : m_code(std::move(code)), m_count(count){};

   [[nodiscard]] auto code() const { return m_code; }
   [[nodiscard]] auto count() const { return m_count; }

   bool operator==(const CardToken &other) const
   {
      return m_code == other.code() && m_count == other.count();
   }
   bool operator!=(const CardToken &other) const { return not (*this == other); }

  private:
   std::string m_code;
   size_t m_count;
};

class DeckDesign {
  public:
   using value_type = CardToken;

   template < typename... Args >
   DeckDesign(Args &&...args) : m_cards(std::forward< Args >(args)...)
   {
   }
   DeckDesign(DeckDesign &other) = default;
   DeckDesign(const DeckDesign &other) = default;
   DeckDesign &operator=(const DeckDesign &other) = default;
   DeckDesign &operator=(DeckDesign &&other) = default;
   DeckDesign(DeckDesign &&other) = default;
   ~DeckDesign() = default;

   void add(const std::string &card_code, size_t amount)
   {
      m_cards.emplace_back(CardToken{card_code, amount});
   }

   auto &cards() { return m_cards; }
   [[nodiscard]] const auto &cards() const { return m_cards; }

   [[nodiscard]] auto size() const { return m_cards.size(); }
   [[nodiscard]] auto begin() const { return m_cards.begin(); }
   [[nodiscard]] auto begin() { return m_cards.begin(); }
   [[nodiscard]] auto end() const { return m_cards.end(); }
   [[nodiscard]] auto end() { return m_cards.end(); }
   [[nodiscard]] inline auto &operator[](size_t n) { return m_cards[n]; }
   [[nodiscard]] inline auto &operator[](size_t n) const { return m_cards[n]; }
   [[nodiscard]] inline const auto &at(size_t idx) const { return m_cards.at(idx); }
   [[nodiscard]] inline auto &at(size_t idx) { return m_cards.at(idx); }
   template < typename IterType >
   [[nodiscard]] inline auto erase(IterType position)
   {
      return m_cards.erase(position);
   }

   template <
      typename Container,
      typename = std::enable_if_t< std::is_same_v< typename Container::value_type, CardToken > > >
   [[nodiscard]] bool contains(const Container &card_cont) const
   {
      return std::all_of(card_cont.begin(), card_cont.end(), [&](const auto &card) {
         return std::find(begin(), end(), card) != end();
      });
   }

   inline bool operator==(const DeckDesign &other) const
   {
      return (size() == other.size()) && contains(other);
   }
   inline bool operator!=(const DeckDesign &other) const { return not (*this == other); }

  private:
   std::vector< CardToken > m_cards;
};

#endif  // LORDECKENCODER_DECK_DESIGN_H
