
#ifndef LORDECKENCODER_UTILS_H
#define LORDECKENCODER_UTILS_H

template < typename T >
struct reversion_wrapper {
   T &iterable;
   auto begin() { return std::rbegin(iterable); }
   auto end() { return std::rend(iterable); }
};
template < typename T >
reversion_wrapper< T > reverse(T &&iterable)
{
   return {iterable};
}

template < typename Container >
[[nodiscard]] bool contains(const Container &first, const Container &second)
{
   return std::all_of(first.begin(), first.end(), [&](const auto &card) {
      return std::find(second.begin(), second.end(), card) != second.end();
   });
}

template < typename Container >
[[nodiscard]] bool container_eq(const Container &first, const Container &second)
{
   return (first.size() == second.size())
          && std::all_of(first.begin(), first.end(), [&](const auto &elem) {
                return std::find(second.begin(), second.end(), elem) != second.end();
             });
}

#endif  // LORDECKENCODER_UTILS_H
