#pragma once

#include <optional>
#include <string>

#include "tests/hpp/test_outcome.hpp"

#include <gtest/gtest.h>

struct results_per_category;

class test_non_template_base:
  public testing::Test
{
public:
  const std::optional<test_outcome>& outcome() const;
  
  void store_test_result_success();
  void store_test_result_failure();
  void store_test_result_not_available();
  void store_test_result_other_failure();
  
  void store_library_tag(std::string tag);
  
  void test_description(std::string category, std::string description);

  void clear_test_result();
  void commit_test_result(results_per_category& results) const;
  
  int run_forked_test(const std::function<void()>& f);

  void store_result_from_exit_code(int exit_code);
  
private:  
  void store_test_result(test_outcome outcome);

private:
  std::string m_library_tag;
  std::string m_current_category;
  std::string m_current_description;
  std::optional<test_outcome> m_current_outcome;

  static const constexpr int m_exit_code_guard = 200;
};
