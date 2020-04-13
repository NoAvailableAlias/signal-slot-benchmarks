#pragma once

#include "tests/hpp/exited_with_code_stored.hpp"

#include <gtest/gtest.h>

#define FENCED_TYPED_TEST(test_suite, test_name, category, description, body) \
  TYPED_TEST(test_suite, test_name)                                     \
  {                                                                     \
    this->test_description(category, description);                      \
                                                                        \
    auto test_runner                                                    \
      ([this]() -> void                                                 \
       {                                                                \
         body                                                           \
       });                                                              \
                                                                        \
    int exit_code(0);                                                   \
    EXPECT_EXIT                                                         \
      (exit(this->run_forked_test(test_runner)),                        \
       exited_with_code_stored(&exit_code),                             \
       "");                                                             \
                                                                        \
    this->store_result_from_exit_code(exit_code);                       \
  }
