#pragma once

class exited_with_code_stored
{
public:
  explicit exited_with_code_stored(int* exit_code);

  bool operator()(int exit_status);

  exited_with_code_stored& operator=(const exited_with_code_stored&) = delete;
  
private:
  const int m_expected_exit_code;
  int* m_actual_exit_code;
};
