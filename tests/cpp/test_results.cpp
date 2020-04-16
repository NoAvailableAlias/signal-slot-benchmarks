#include "tests/hpp/test_results.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

static const char* g_term_reset_color("\033[39;49m");

static void output_padded
(std::ostream& output, const std::string& label, int left, int right,
 char padding = ' ')
{
  for (int i(0); i <= left; ++i)
    output << padding;

  output << label;

  for (int i(0); i <= right; ++i)
    output << padding;
}

static void output_centered
(std::ostream& output, const std::string& label, int width, char padding = ' ')
{
  const int label_size(label.size());
  assert(width >= label.size());

  const int left((width - label_size) / 2);
  const int right(width - label_size - left);

  output_padded(output, label, left, right, padding);
}

static void output_left
(std::ostream& output, const std::string& label, int width, char padding = ' ')
{
  const int label_size(label.size());
  assert(width >= label.size());

  const int left(0);
  const int right(width - label_size);

  output_padded(output, label, left, right, padding);
}

static const char* outcome_term_color(test_outcome outcome)
{
  switch (outcome)
    {
    case test_outcome::other_failure:
      // red
      return "\033[31m";
    case test_outcome::success:
      // green
      return "\033[32m";
    case test_outcome::failure:
      // yellow
      return "\033[33m";
    case test_outcome::not_available:
      // blue
      return "\033[34m";
    }

  assert(false);
  return "\033[01;35m";
}

static std::string outcome_label(test_outcome outcome)
{
  switch(outcome)
    {
    case test_outcome::success: return "**yes**";
    case test_outcome::failure: return "no";
    case test_outcome::not_available: return "n/a";
    case test_outcome::other_failure: return "X";
    }

  assert(false);
  return "";
}

static void output_test_results
(std::ostream& output, const test_results& results)
{
  const int test_count(results.descriptions.size());

  for (int i(0); i != test_count; ++i)
    output << (i+1) << ". " << results.descriptions[i] << '\n';

  output << "\n";

  const std::string first_header("Library");
  
  std::vector<int> widths(test_count + 1, 0);

  widths[0] = first_header.size();
  
  for (int i(0); i != test_count; ++i)
    widths[i+1] = std::to_string(i+1).size();
  
  for (const auto& tag_and_outcomes : results.outcomes)
    {
      widths[0] =
        std::max<int>(widths[0], tag_and_outcomes.first.size());

      for (int i(0); i != test_count; ++i)
        widths[i+1] =
          std::max<int>
          (widths[i+1], outcome_label(tag_and_outcomes.second[i]).size());
    }

  // headers
  output_left(output, first_header, widths[0]);

  for (int i(0); i != test_count; ++i)
    {
      output << '|';
      output_centered(output, std::to_string(i+1), widths[i+1]);
    }

  output << '\n';

  // header separator (dashes)
  output_left(output, "", widths[0], '-');

  for (int i(0); i != test_count; ++i)
    {
      output << '|';
      output_left(output, "", widths[i+1], '-');
    }

  output << '\n';
  

  // rows
  for (const auto& tag_and_outcomes : results.outcomes)
    {
      output_left(output, tag_and_outcomes.first, widths[0]);

      for (int i(0); i != test_count; ++i)
        {
          output << '|'
                    << outcome_term_color(tag_and_outcomes.second[i]);
          output_centered
            (output, outcome_label(tag_and_outcomes.second[i]),
             widths[i+1]);
          output << g_term_reset_color;
        }

      output << '\n';
    }
}

void output_legend(std::ostream& output)
{
  output << "* " << outcome_term_color(test_outcome::success)
         << outcome_label(test_outcome::success)
         << g_term_reset_color << ": the test passed successfully,\n";

  output << "* " << outcome_term_color(test_outcome::failure)
         << outcome_label(test_outcome::failure)
         << g_term_reset_color
         << ": the test did not produce the expected result,\n";

  output << "* " << outcome_term_color(test_outcome::not_available)
         << outcome_label(test_outcome::not_available)
         << g_term_reset_color
         << ": the library does not provide the API required for this test,\n";
  
  output << "* " << outcome_term_color(test_outcome::other_failure)
         << outcome_label(test_outcome::other_failure)
         << g_term_reset_color << ": the test crashed in a way or another.\n";
}

static std::vector<const std::string*> all_tags
(const results_per_category& results)
{
  std::vector<const std::string*> result;
  
  for (const auto& tag_and_outcomes :
         results.category_to_results.begin()->second.outcomes)
    result.emplace_back(&tag_and_outcomes.first);

  return result;
}

static void output_fully_successful_libraries
(std::ostream& output, const results_per_category& results)
{
  std::vector<const std::string*> tags(all_tags(results));

  for (const auto& category_results : results.category_to_results)
    for (const auto& tag_and_outcomes : category_results.second.outcomes)
      {
        const auto predicate
          ([&](const std::string* v) -> bool
           {
             return *v == tag_and_outcomes.first;
           });
        
        for (test_outcome outcome : tag_and_outcomes.second)
          if (outcome != test_outcome::success)
            {
              tags.erase
                (std::remove_if
                 (tags.begin(), tags.end(), predicate), tags.end());
              break;
            }
      }

  if (tags.empty())
    output << "None of the tested libraries passes all the tests.\n";
  else
    {
      output << "The following libraries pass all the tests: ";
      auto begin(tags.begin());
      const auto end(tags.end());

      output << **begin;

      for (++begin; begin != end; ++begin)
        output << ", " << **begin;

      output << ".\n";
    }
}

static void output_libraries_that_never_fail
(std::ostream& output, const results_per_category& results)
{
  std::vector<const std::string*> tags(all_tags(results));

  for (const auto& category_results : results.category_to_results)
    for (const auto& tag_and_outcomes : category_results.second.outcomes)
      {
        const auto predicate
          ([&](const std::string* v) -> bool
           {
             return *v == tag_and_outcomes.first;
           });
        
        for (test_outcome outcome : tag_and_outcomes.second)
          if ((outcome != test_outcome::success)
              && (outcome != test_outcome::not_available))
            {
              tags.erase
                (std::remove_if
                 (tags.begin(), tags.end(), predicate), tags.end());
              break;
            }
      }

  output << "On the tests that could be run (i.e. tests for which the required"
    " API is available in the tested library), ";
  
  if (tags.empty())
    output << "none of the tested libraries passes all the tests.\n";
  else
    {
      output << "the following libraries pass all the tests: ";
      auto begin(tags.begin());
      const auto end(tags.end());

      output << **begin;

      for (++begin; begin != end; ++begin)
        output << ", " << **begin;

      output << ".\n";
    }
}

void output_results(std::ostream& output, const results_per_category& results)
{
  if (results.category_to_results.empty())
    return;

  output << "# Feature Tests\n\n"
    "This page displays the results of several tests on the features of the"
    " signal libraries. See below for a legend of the output:\n\n";
  
  output_legend(output);
  output << "\n"
    "The tests check the behaviour of the library when the signal is"
    " activated in various ways, how the connections are handled and what"
    " happens when signals are swapped.\n"
    "\n";

  output_fully_successful_libraries(output, results);
  output << "\n";
  output_libraries_that_never_fail(output, results);

  output << "\n# Detailed Test results\n\n";

  for (const auto& category_results : results.category_to_results)
    {
      output << "## " <<  category_results.first << "\n\n";
      output_test_results(output, category_results.second);
      output << "\n";
    }
}
