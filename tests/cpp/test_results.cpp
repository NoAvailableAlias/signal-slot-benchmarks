#include "tests/hpp/test_results.hpp"

#include <cassert>
#include <iostream>

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
          output << "\033[39;49m";
        }

      output << '\n';
    }
}

void output_results(std::ostream& output, const results_per_category& results)
{
  if (results.category_to_results.empty())
    return;

  for (const auto& category_results : results.category_to_results)
    {
      output << "# " <<  category_results.first << "\n\n";
      output_test_results(output, category_results.second);
      output << "\n";
    }
}
