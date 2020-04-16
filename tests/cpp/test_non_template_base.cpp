#include "tests/hpp/test_non_template_base.hpp"

#include "tests/hpp/test_results.hpp"

const constexpr int test_non_template_base::m_exit_code_guard;

const std::optional<test_outcome>& test_non_template_base::outcome() const
{
  return m_current_outcome;
}
  
void test_non_template_base::store_test_result_success()
{
  store_test_result(test_outcome::success);
}

void test_non_template_base::store_test_result_failure()
{
  store_test_result(test_outcome::failure);
}

void test_non_template_base::store_test_result_not_available()
{
  store_test_result(test_outcome::not_available);
}

void test_non_template_base::store_test_result_other_failure()
{
  store_test_result(test_outcome::other_failure);
}

void test_non_template_base::store_library_tag(std::string tag)
{
  m_library_tag = std::move(tag);
}
  
void test_non_template_base::test_description
(std::string category, std::string description)
{
  m_current_category = std::move(category);
  m_current_description = std::move(description);
}

void test_non_template_base::clear_test_result()
{
  m_current_outcome.reset();
}

void test_non_template_base::commit_test_result
(results_per_category& results) const
{
  test_results& test_results(results.category_to_results[m_current_category]);

  {
    const auto end(test_results.descriptions.end());
    const auto it
      (std::find
       (test_results.descriptions.begin(), end, m_current_description));

    if (it == end)
      test_results.descriptions.push_back(m_current_description);
  }

  test_results.outcomes[m_library_tag].emplace_back(*m_current_outcome);
}
  
int test_non_template_base::run_forked_test(const std::function<void()>& f)
{
  f();

  if (this->HasFailure())
    this->store_test_result_failure();

  if (m_current_outcome)
    return m_exit_code_guard + (int)*m_current_outcome;

  return 0;
}

void test_non_template_base::store_result_from_exit_code(int exit_code)
{
  if (!this->HasFailure())
    {
      store_test_result_success();
      return;
    }
    
  int as_outcome_value(exit_code - m_exit_code_guard);

  switch(as_outcome_value)
    {
    case (int)test_outcome::not_available:
      store_test_result_not_available();
      break;
    case (int)test_outcome::failure:
      store_test_result_failure();
      break;
    default:
      store_test_result_other_failure();
    }
}
  
void test_non_template_base::store_test_result(test_outcome outcome)
{
  if (!m_current_outcome)
    m_current_outcome = outcome;
}
