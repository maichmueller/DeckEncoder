

#include <iostream>
#include <queue>
#include <set>
#include <stack>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include "../test/read_cases.h"
#include "include/base32.h"

int main() {
   for(auto& [code, dc] : read_case_file("../tests/test_cases.txt")) {
      std::cout << code << "\n";
      for(auto cardcount : dc) {
         std::cout << "code " << cardcount.code() << " <-> count: " << cardcount.count() << "\n";
      }

   }
}