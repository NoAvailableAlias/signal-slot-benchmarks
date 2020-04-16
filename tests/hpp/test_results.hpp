#pragma once

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

#include "tests/hpp/test_outcome.hpp"

struct test_results
{
  std::vector<std::string> descriptions;
  std::map<std::string, std::vector<test_outcome>> outcomes;
};

struct results_per_category
{
  std::map<std::string, test_results> category_to_results;
};

void output_results(std::ostream& output, const results_per_category& results);
