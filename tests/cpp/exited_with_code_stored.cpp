#include "tests/hpp/exited_with_code_stored.hpp"

#include <gtest/gtest.h>

exited_with_code_stored::exited_with_code_stored
(int* exit_code)
  : m_expected_exit_code(*exit_code),
    m_actual_exit_code(exit_code)
{

}

bool exited_with_code_stored::operator()(int exit_status)
{
# if GTEST_OS_WINDOWS || GTEST_OS_FUCHSIA

  *m_actual_exit_code = exit_code;
  return exit_status == m_expected_exit_code_;

#else
  if (!WIFEXITED(exit_status))
    return false;

  const int exit_code(WEXITSTATUS(exit_status));
  *m_actual_exit_code = exit_code;

  return exit_code == m_expected_exit_code;
#endif
}
