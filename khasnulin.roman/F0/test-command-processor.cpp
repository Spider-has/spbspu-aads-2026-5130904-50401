#include "CommandProcessor.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <string>
#include <type_traits>

using namespace khasnulin;

namespace
{
  const std::string invalidCommand = "<INVALID COMMAND>\n";
  const std::string difficulties = "beginner\nintermediate\nadvanced\n";

  std::string runCommands(const std::string &input)
  {
    std::istringstream in(input);
    std::ostringstream out;
    CommandProcessor processor;
    processor.run(in, out);
    return out.str();
  }
}

static_assert(!std::is_copy_constructible< CommandProcessor >::value, "processor must not be copyable");
static_assert(!std::is_copy_assignable< CommandProcessor >::value, "processor must not be copy assignable");

BOOST_AUTO_TEST_SUITE(command_processor_input_tests)

BOOST_AUTO_TEST_CASE(empty_input_gives_no_output)
{
  BOOST_CHECK(runCommands("") == "");
}

BOOST_AUTO_TEST_CASE(empty_lines_give_no_output)
{
  BOOST_CHECK(runCommands("\n\n   \n\r\n") == "");
}

BOOST_AUTO_TEST_CASE(empty_lines_between_commands_are_skipped)
{
  BOOST_CHECK(runCommands("show-difficulties\n\n   \nshow-difficulties\n") == difficulties + difficulties);
}

BOOST_AUTO_TEST_CASE(last_line_without_newline_is_processed)
{
  BOOST_CHECK(runCommands("show-difficulties") == difficulties);
}

BOOST_AUTO_TEST_CASE(windows_line_endings_are_accepted)
{
  BOOST_CHECK(runCommands("show-difficulties\r\nshow-difficulties\r\n") == difficulties + difficulties);
}

BOOST_AUTO_TEST_CASE(extra_spaces_around_command_are_accepted)
{
  BOOST_CHECK(runCommands("   show-difficulties   \n") == difficulties);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(command_processor_dispatch_tests)

BOOST_AUTO_TEST_CASE(show_difficulties_prints_fixed_order)
{
  BOOST_CHECK(runCommands("show-difficulties\n") == difficulties);
}

BOOST_AUTO_TEST_CASE(show_difficulties_with_argument_is_invalid)
{
  BOOST_CHECK(runCommands("show-difficulties extra\n") == invalidCommand);
}

BOOST_AUTO_TEST_CASE(unknown_command_is_invalid)
{
  BOOST_CHECK(runCommands("unknown-command\n") == invalidCommand);
}

BOOST_AUTO_TEST_CASE(command_name_is_case_sensitive)
{
  BOOST_CHECK(runCommands("Show-Difficulties\n") == invalidCommand);
}

BOOST_AUTO_TEST_CASE(quoted_command_name_is_invalid)
{
  BOOST_CHECK(runCommands("\"show-difficulties\"\n") == invalidCommand);
}

BOOST_AUTO_TEST_CASE(each_invalid_line_gives_one_message)
{
  BOOST_CHECK(runCommands("first\nsecond\nthird\n") == invalidCommand + invalidCommand + invalidCommand);
}

BOOST_AUTO_TEST_CASE(tokenizer_error_is_invalid_and_work_continues)
{
  BOOST_CHECK(runCommands("show-difficulties \"broken\nshow-difficulties\n") ==
              invalidCommand + difficulties);
}

BOOST_AUTO_TEST_CASE(work_continues_after_invalid_command)
{
  BOOST_CHECK(runCommands("unknown-command\nshow-difficulties\n") == invalidCommand + difficulties);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(command_processor_exit_tests)

BOOST_AUTO_TEST_CASE(exit_gives_no_output)
{
  BOOST_CHECK(runCommands("exit\n") == "");
}

BOOST_AUTO_TEST_CASE(exit_stops_processing)
{
  BOOST_CHECK(runCommands("exit\nshow-difficulties\n") == "");
}

BOOST_AUTO_TEST_CASE(commands_before_exit_are_processed)
{
  BOOST_CHECK(runCommands("show-difficulties\nexit\nshow-difficulties\n") == difficulties);
}

BOOST_AUTO_TEST_CASE(exit_with_extra_spaces_stops_processing)
{
  BOOST_CHECK(runCommands("   exit   \nshow-difficulties\n") == "");
}

BOOST_AUTO_TEST_CASE(exit_with_argument_is_invalid_and_work_continues)
{
  BOOST_CHECK(runCommands("exit now\nshow-difficulties\n") == invalidCommand + difficulties);
}

BOOST_AUTO_TEST_CASE(quoted_exit_is_invalid_and_work_continues)
{
  BOOST_CHECK(runCommands("\"exit\"\nshow-difficulties\n") == invalidCommand + difficulties);
}

BOOST_AUTO_TEST_CASE(input_end_without_exit_stops_processing)
{
  BOOST_CHECK(runCommands("show-difficulties\nshow-difficulties") == difficulties + difficulties);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(command_processor_construction_tests)

BOOST_AUTO_TEST_CASE(constructs_with_explicit_model_parameters)
{
  std::istringstream in("show-difficulties\n");
  std::ostringstream out;
  CommandProcessor processor{Learner(), Recommender()};
  processor.run(in, out);
  BOOST_CHECK(out.str() == difficulties);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(add_user_and_show_users_tests)

BOOST_AUTO_TEST_CASE(add_user_prints_nothing)
{
  BOOST_CHECK(runCommands("add-user roman\n") == "");
}

BOOST_AUTO_TEST_CASE(show_users_on_empty_storage_prints_nothing)
{
  BOOST_CHECK(runCommands("show-users\n") == "");
}

BOOST_AUTO_TEST_CASE(show_users_prints_alphabetically)
{
  std::string input = "add-user roman\n"
                      "add-user alex\n"
                      "add-user ivan\n"
                      "show-users\n";
  BOOST_CHECK(runCommands(input) == "alex\nivan\nroman\n");
}

BOOST_AUTO_TEST_CASE(user_names_are_case_sensitive)
{
  std::string input = "add-user roman\n"
                      "add-user Roman\n"
                      "show-users\n";
  BOOST_CHECK(runCommands(input) == "Roman\nroman\n");
}

BOOST_AUTO_TEST_CASE(show_users_respects_limit)
{
  std::string input = "add-user roman\n"
                      "add-user alex\n"
                      "add-user ivan\n"
                      "show-users 2\n";
  BOOST_CHECK(runCommands(input) == "alex\nivan\n");
}

BOOST_AUTO_TEST_CASE(show_users_limit_greater_than_size_prints_all)
{
  std::string input = "add-user roman\n"
                      "add-user alex\n"
                      "show-users 10\n";
  BOOST_CHECK(runCommands(input) == "alex\nroman\n");
}

BOOST_AUTO_TEST_CASE(show_users_invalid_limit_is_invalid)
{
  std::string input = "add-user roman\n"
                      "show-users 0\n"
                      "show-users -1\n"
                      "show-users all\n"
                      "show-users 2 3\n";
  BOOST_CHECK(runCommands(input) == invalidCommand + invalidCommand + invalidCommand + invalidCommand);
}

BOOST_AUTO_TEST_CASE(duplicate_user_is_invalid)
{
  std::string input = "add-user roman\n"
                      "add-user roman\n"
                      "show-users\n";
  BOOST_CHECK(runCommands(input) == invalidCommand + "roman\n");
}

BOOST_AUTO_TEST_CASE(add_user_without_name_is_invalid)
{
  BOOST_CHECK(runCommands("add-user\n") == invalidCommand);
}

BOOST_AUTO_TEST_CASE(add_user_with_quoted_name_is_invalid)
{
  std::string input = "add-user \"roman\"\n"
                      "show-users\n";
  BOOST_CHECK(runCommands(input) == invalidCommand);
}

BOOST_AUTO_TEST_CASE(add_user_with_invalid_tag_creates_nothing)
{
  std::string input = "add-user roman cpp 123\n"
                      "show-users\n";
  BOOST_CHECK(runCommands(input) == invalidCommand);
}

BOOST_AUTO_TEST_CASE(add_user_with_quoted_tag_creates_nothing)
{
  std::string input = "add-user roman cpp \"backend\"\n"
                      "show-users\n";
  BOOST_CHECK(runCommands(input) == invalidCommand);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(show_user_info_tests)

BOOST_AUTO_TEST_CASE(user_without_tags_and_ratings)
{
  std::string input = "add-user roman\n"
                      "show-user-info roman\n";
  BOOST_CHECK(runCommands(input) == "User: roman\nPreferences:\nRatings count: 0\n");
}

BOOST_AUTO_TEST_CASE(preferences_are_sorted_and_unique)
{
  std::string input = "add-user roman cpp backend async CPP\n"
                      "show-user-info roman\n";
  BOOST_CHECK(runCommands(input) == "User: roman\nPreferences: async backend cpp\nRatings count: 0\n");
}

BOOST_AUTO_TEST_CASE(ratings_count_counts_distinct_articles)
{
  std::string input = "add-user roman\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "add-article \"Python Basics\" python beginner\n"
                      "rate-article roman \"Advanced C++\" 10\n"
                      "rate-article roman \"Python Basics\" 7\n"
                      "rate-article roman \"Advanced C++\" 9\n"
                      "show-user-info roman\n";
  BOOST_CHECK(runCommands(input) == "User: roman\nPreferences:\nRatings count: 2\n");
}

BOOST_AUTO_TEST_CASE(unknown_user_is_invalid)
{
  BOOST_CHECK(runCommands("show-user-info roman\n") == invalidCommand);
}

BOOST_AUTO_TEST_CASE(wrong_argument_count_is_invalid)
{
  std::string input = "add-user roman\n"
                      "show-user-info\n"
                      "show-user-info roman extra\n";
  BOOST_CHECK(runCommands(input) == invalidCommand + invalidCommand);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(add_article_and_show_article_info_tests)

BOOST_AUTO_TEST_CASE(add_article_prints_nothing)
{
  BOOST_CHECK(runCommands("add-article \"Advanced C++\" cpp advanced\n") == "");
}

BOOST_AUTO_TEST_CASE(article_without_tags_and_ratings)
{
  std::string input = "add-article \"Advanced C++\" cpp advanced\n"
                      "show-article-info \"Advanced C++\"\n";
  std::string expected = "Article: \"Advanced C++\"\n"
                         "Category: cpp | Difficulty: advanced\n"
                         "Tags:\n"
                         "Avg Rating: 0.0/10 (0 ratings)\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(tags_are_sorted_and_unique)
{
  std::string input = "add-article \"Advanced C++\" cpp advanced templates cpp move-semantics TEMPLATES\n"
                      "show-article-info \"Advanced C++\"\n";
  std::string expected = "Article: \"Advanced C++\"\n"
                         "Category: cpp | Difficulty: advanced\n"
                         "Tags: cpp move-semantics templates\n"
                         "Avg Rating: 0.0/10 (0 ratings)\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(category_and_difficulty_are_normalized)
{
  std::string input = "add-article \"Advanced C++\" CPP Advanced\n"
                      "show-article-info \"Advanced C++\"\n";
  std::string expected = "Article: \"Advanced C++\"\n"
                         "Category: cpp | Difficulty: advanced\n"
                         "Tags:\n"
                         "Avg Rating: 0.0/10 (0 ratings)\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(article_name_is_normalized)
{
  std::string input = "add-article \"  Advanced   C++ \" cpp advanced\n"
                      "show-article-info \"Advanced C++\"\n";
  std::string expected = "Article: \"Advanced C++\"\n"
                         "Category: cpp | Difficulty: advanced\n"
                         "Tags:\n"
                         "Avg Rating: 0.0/10 (0 ratings)\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(article_name_keeps_special_chars)
{
  std::string input = "add-article \"Who's next: $5, a&b?\" finance beginner\n"
                      "show-article-info \"Who's next: $5, a&b?\"\n";
  std::string expected = "Article: \"Who's next: $5, a&b?\"\n"
                         "Category: finance | Difficulty: beginner\n"
                         "Tags:\n"
                         "Avg Rating: 0.0/10 (0 ratings)\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(article_names_are_case_sensitive)
{
  std::string input = "add-article \"Advanced C++\" cpp advanced\n"
                      "add-article \"advanced c++\" cpp beginner\n"
                      "list-articles\n";
  BOOST_CHECK(runCommands(input) == "\"Advanced C++\" [cpp | advanced]\n\"advanced c++\" [cpp | beginner]\n");
}

BOOST_AUTO_TEST_CASE(duplicate_article_is_invalid)
{
  std::string input = "add-article \"Advanced C++\" cpp advanced\n"
                      "add-article \"Advanced C++\" python beginner\n"
                      "list-articles\n";
  BOOST_CHECK(runCommands(input) == invalidCommand + "\"Advanced C++\" [cpp | advanced]\n");
}

BOOST_AUTO_TEST_CASE(invalid_arguments_create_nothing)
{
  std::string input = "add-article Advanced cpp advanced\n"
                      "add-article \"Advanced C++\" cpp expert\n"
                      "add-article \"Advanced C++\" advanced advanced\n"
                      "add-article \"Advanced C++\" cpp\n"
                      "add-article \"Advanced C++\" \"cpp\" advanced\n"
                      "add-article \"Advanced C++\" cpp advanced templates 123\n"
                      "add-article \"\" cpp advanced\n"
                      "list-articles\n";
  std::string expected;
  for (int i = 0; i < 7; ++i)
  {
    expected += invalidCommand;
  }
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(unknown_article_is_invalid)
{
  BOOST_CHECK(runCommands("show-article-info \"Advanced C++\"\n") == invalidCommand);
}

BOOST_AUTO_TEST_CASE(unquoted_article_name_is_invalid)
{
  std::string input = "add-article \"Python\" python beginner\n"
                      "show-article-info Python\n";
  BOOST_CHECK(runCommands(input) == invalidCommand);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(average_rating_tests)

BOOST_AUTO_TEST_CASE(one_rating_uses_plural_word)
{
  std::string input = "add-user roman\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "rate-article roman \"Advanced C++\" 7\n"
                      "show-article-info \"Advanced C++\"\n";
  std::string expected = "Article: \"Advanced C++\"\n"
                         "Category: cpp | Difficulty: advanced\n"
                         "Tags:\n"
                         "Avg Rating: 7.0/10 (1 ratings)\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(average_of_two_ratings)
{
  std::string input = "add-user roman\n"
                      "add-user alex\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "rate-article roman \"Advanced C++\" 10\n"
                      "rate-article alex \"Advanced C++\" 9\n"
                      "show-article-info \"Advanced C++\"\n";
  std::string expected = "Article: \"Advanced C++\"\n"
                         "Category: cpp | Difficulty: advanced\n"
                         "Tags:\n"
                         "Avg Rating: 9.5/10 (2 ratings)\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(average_is_rounded_to_one_digit)
{
  std::string input = "add-user roman\n"
                      "add-user alex\n"
                      "add-user ivan\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "rate-article roman \"Advanced C++\" 10\n"
                      "rate-article alex \"Advanced C++\" 10\n"
                      "rate-article ivan \"Advanced C++\" 9\n"
                      "show-article-info \"Advanced C++\"\n";
  std::string expected = "Article: \"Advanced C++\"\n"
                         "Category: cpp | Difficulty: advanced\n"
                         "Tags:\n"
                         "Avg Rating: 9.7/10 (3 ratings)\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(average_is_rounded_down_when_needed)
{
  std::string input = "add-user roman\n"
                      "add-user alex\n"
                      "add-user ivan\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "rate-article roman \"Advanced C++\" 1\n"
                      "rate-article alex \"Advanced C++\" 1\n"
                      "rate-article ivan \"Advanced C++\" 2\n"
                      "show-article-info \"Advanced C++\"\n";
  std::string expected = "Article: \"Advanced C++\"\n"
                         "Category: cpp | Difficulty: advanced\n"
                         "Tags:\n"
                         "Avg Rating: 1.3/10 (3 ratings)\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(maximal_average)
{
  std::string input = "add-user roman\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "rate-article roman \"Advanced C++\" 10\n"
                      "show-article-info \"Advanced C++\"\n";
  std::string expected = "Article: \"Advanced C++\"\n"
                         "Category: cpp | Difficulty: advanced\n"
                         "Tags:\n"
                         "Avg Rating: 10.0/10 (1 ratings)\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(rerating_replaces_previous_value)
{
  std::string input = "add-user roman\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "rate-article roman \"Advanced C++\" 3\n"
                      "rate-article roman \"Advanced C++\" 8\n"
                      "show-article-info \"Advanced C++\"\n";
  std::string expected = "Article: \"Advanced C++\"\n"
                         "Category: cpp | Difficulty: advanced\n"
                         "Tags:\n"
                         "Avg Rating: 8.0/10 (1 ratings)\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(repeated_info_has_same_format)
{
  std::string input = "add-user roman\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "rate-article roman \"Advanced C++\" 7\n"
                      "show-article-info \"Advanced C++\"\n"
                      "show-user-ratings roman\n"
                      "show-article-info \"Advanced C++\"\n";
  std::string info = "Article: \"Advanced C++\"\n"
                     "Category: cpp | Difficulty: advanced\n"
                     "Tags:\n"
                     "Avg Rating: 7.0/10 (1 ratings)\n";
  BOOST_CHECK(runCommands(input) == info + "\"Advanced C++\" - 7/10\n" + info);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(list_articles_tests)

BOOST_AUTO_TEST_CASE(empty_storage_prints_nothing)
{
  BOOST_CHECK(runCommands("list-articles\n") == "");
}

BOOST_AUTO_TEST_CASE(prints_alphabetically_with_category_and_difficulty)
{
  std::string input = "add-article \"Neural Networks Primer\" ai intermediate\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "add-article \"Introduction to Python\" backend beginner\n"
                      "list-articles\n";
  std::string expected = "\"Advanced C++\" [cpp | advanced]\n"
                         "\"Introduction to Python\" [backend | beginner]\n"
                         "\"Neural Networks Primer\" [ai | intermediate]\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(respects_limit)
{
  std::string input = "add-article \"Neural Networks Primer\" ai intermediate\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "add-article \"Introduction to Python\" backend beginner\n"
                      "list-articles 2\n";
  std::string expected = "\"Advanced C++\" [cpp | advanced]\n"
                         "\"Introduction to Python\" [backend | beginner]\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(limit_greater_than_size_prints_all)
{
  std::string input = "add-article \"Advanced C++\" cpp advanced\n"
                      "list-articles 100\n";
  BOOST_CHECK(runCommands(input) == "\"Advanced C++\" [cpp | advanced]\n");
}

BOOST_AUTO_TEST_CASE(invalid_limit_is_invalid)
{
  std::string input = "add-article \"Advanced C++\" cpp advanced\n"
                      "list-articles 0\n"
                      "list-articles ten\n"
                      "list-articles \"5\"\n"
                      "list-articles 1 2\n";
  BOOST_CHECK(runCommands(input) == invalidCommand + invalidCommand + invalidCommand + invalidCommand);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(rate_article_and_show_user_ratings_tests)

BOOST_AUTO_TEST_CASE(rate_article_prints_nothing)
{
  std::string input = "add-user roman\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "rate-article roman \"Advanced C++\" 10\n";
  BOOST_CHECK(runCommands(input) == "");
}

BOOST_AUTO_TEST_CASE(user_without_ratings_prints_nothing)
{
  std::string input = "add-user roman\n"
                      "show-user-ratings roman\n";
  BOOST_CHECK(runCommands(input) == "");
}

BOOST_AUTO_TEST_CASE(ratings_are_sorted_by_article_name)
{
  std::string input = "add-user roman\n"
                      "add-article \"Neural Networks Primer\" ai intermediate\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "rate-article roman \"Neural Networks Primer\" 7\n"
                      "rate-article roman \"Advanced C++\" 10\n"
                      "show-user-ratings roman\n";
  BOOST_CHECK(runCommands(input) == "\"Advanced C++\" - 10/10\n\"Neural Networks Primer\" - 7/10\n");
}

BOOST_AUTO_TEST_CASE(rerating_updates_value)
{
  std::string input = "add-user roman\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "rate-article roman \"Advanced C++\" 3\n"
                      "rate-article roman \"Advanced C++\" 10\n"
                      "show-user-ratings roman\n";
  BOOST_CHECK(runCommands(input) == "\"Advanced C++\" - 10/10\n");
}

BOOST_AUTO_TEST_CASE(ratings_of_other_users_are_not_shown)
{
  std::string input = "add-user roman\n"
                      "add-user alex\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "add-article \"Python Basics\" python beginner\n"
                      "rate-article alex \"Advanced C++\" 5\n"
                      "rate-article roman \"Python Basics\" 8\n"
                      "show-user-ratings roman\n";
  BOOST_CHECK(runCommands(input) == "\"Python Basics\" - 8/10\n");
}

BOOST_AUTO_TEST_CASE(invalid_rate_article_changes_nothing)
{
  std::string input = "add-user roman\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "rate-article ivan \"Advanced C++\" 5\n"
                      "rate-article roman \"Python Basics\" 5\n"
                      "rate-article roman \"Advanced C++\" 0\n"
                      "rate-article roman \"Advanced C++\" 11\n"
                      "rate-article roman \"Advanced C++\" \"5\"\n"
                      "rate-article roman Advanced 5\n"
                      "rate-article roman \"Advanced C++\"\n"
                      "rate-article roman \"Advanced C++\" 5 6\n"
                      "show-user-ratings roman\n"
                      "show-user-info roman\n";
  std::string expected;
  for (int i = 0; i < 8; ++i)
  {
    expected += invalidCommand;
  }
  expected += "User: roman\nPreferences:\nRatings count: 0\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(unknown_user_ratings_is_invalid)
{
  BOOST_CHECK(runCommands("show-user-ratings roman\n") == invalidCommand);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(seed_style_scenario_tests)

BOOST_AUTO_TEST_CASE(seed_commands_then_inspection)
{
  std::string input = "add-user olga-ivanova politics markets\n"
                      "add-user jack-smith sports\n"
                      "add-article \"Stocks fall, bonds rise\" finance intermediate markets newsus\n"
                      "add-article \"Cup final tonight\" sports beginner football\n"
                      "rate-article olga-ivanova \"Stocks fall, bonds rise\" 9\n"
                      "rate-article jack-smith \"Stocks fall, bonds rise\" 2\n"
                      "rate-article jack-smith \"Cup final tonight\" 10\n"
                      "show-users\n"
                      "list-articles\n"
                      "show-user-info jack-smith\n"
                      "show-user-ratings jack-smith\n"
                      "show-article-info \"Stocks fall, bonds rise\"\n";
  std::string expected = "jack-smith\n"
                         "olga-ivanova\n"
                         "\"Cup final tonight\" [sports | beginner]\n"
                         "\"Stocks fall, bonds rise\" [finance | intermediate]\n"
                         "User: jack-smith\n"
                         "Preferences: sports\n"
                         "Ratings count: 2\n"
                         "\"Cup final tonight\" - 10/10\n"
                         "\"Stocks fall, bonds rise\" - 2/10\n"
                         "Article: \"Stocks fall, bonds rise\"\n"
                         "Category: finance | Difficulty: intermediate\n"
                         "Tags: markets newsus\n"
                         "Avg Rating: 5.5/10 (2 ratings)\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_SUITE_END()

using namespace khasnulin;

namespace
{
  std::string repeatInvalid(int count)
  {
    std::string result;
    for (int i = 0; i < count; ++i)
    {
      result += invalidCommand;
    }
    return result;
  }
}

BOOST_AUTO_TEST_SUITE(add_user_preferences_tests)

BOOST_AUTO_TEST_CASE(adds_sorted_preferences_and_prints_nothing)
{
  std::string input = "add-user roman\n"
                      "add-user-preferences roman cpp backend async\n"
                      "show-user-info roman\n";
  BOOST_CHECK_EQUAL(runCommands(input), "User: roman\nPreferences: async backend cpp\nRatings count: 0\n");
}

BOOST_AUTO_TEST_CASE(extends_existing_preferences)
{
  std::string input = "add-user roman cpp\n"
                      "add-user-preferences roman async\n"
                      "show-user-info roman\n";
  BOOST_CHECK(runCommands(input) == "User: roman\nPreferences: async cpp\nRatings count: 0\n");
}

BOOST_AUTO_TEST_CASE(existing_preference_is_not_an_error)
{
  std::string input = "add-user roman cpp\n"
                      "add-user-preferences roman cpp CPP\n"
                      "show-user-info roman\n";
  BOOST_CHECK(runCommands(input) == "User: roman\nPreferences: cpp\nRatings count: 0\n");
}

BOOST_AUTO_TEST_CASE(without_tags_is_invalid)
{
  std::string input = "add-user roman\n"
                      "add-user-preferences roman\n";
  BOOST_CHECK_EQUAL(runCommands(input), invalidCommand);
}

BOOST_AUTO_TEST_CASE(unknown_user_is_invalid)
{
  BOOST_CHECK(runCommands("add-user-preferences roman cpp\n") == invalidCommand);
}

BOOST_AUTO_TEST_CASE(unknown_user_creates_no_tags)
{
  std::string input = "add-user-preferences roman cpp\n"
                      "show-tags\n";
  BOOST_CHECK(runCommands(input) == invalidCommand);
}

BOOST_AUTO_TEST_CASE(invalid_tag_adds_nothing)
{
  std::string input = "add-user roman cpp\n"
                      "add-user-preferences roman go 123\n"
                      "add-user-preferences roman go \"rust\"\n"
                      "show-user-info roman\n";
  BOOST_CHECK(runCommands(input) == repeatInvalid(2) + "User: roman\nPreferences: cpp\nRatings count: 0\n");
}

BOOST_AUTO_TEST_CASE(quoted_user_is_invalid)
{
  std::string input = "add-user roman\n"
                      "add-user-preferences \"roman\" cpp\n";
  BOOST_CHECK(runCommands(input) == invalidCommand);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(add_article_tags_tests)

BOOST_AUTO_TEST_CASE(adds_sorted_tags_and_prints_nothing)
{
  std::string input = "add-article \"Advanced C++\" cpp advanced\n"
                      "add-article-tags \"Advanced C++\" cpp templates move-semantics\n"
                      "show-article-info \"Advanced C++\"\n";
  std::string expected = "Article: \"Advanced C++\"\n"
                         "Category: cpp | Difficulty: advanced\n"
                         "Tags: cpp move-semantics templates\n"
                         "Avg Rating: 0.0/10 (0 ratings)\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(extends_existing_tags_without_duplicates)
{
  std::string input = "add-article \"Advanced C++\" cpp advanced cpp\n"
                      "add-article-tags \"Advanced C++\" templates CPP\n"
                      "show-article-info \"Advanced C++\"\n";
  std::string expected = "Article: \"Advanced C++\"\n"
                         "Category: cpp | Difficulty: advanced\n"
                         "Tags: cpp templates\n"
                         "Avg Rating: 0.0/10 (0 ratings)\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(without_tags_is_invalid)
{
  std::string input = "add-article \"Advanced C++\" cpp advanced\n"
                      "add-article-tags \"Advanced C++\"\n";
  BOOST_CHECK(runCommands(input) == invalidCommand);
}

BOOST_AUTO_TEST_CASE(unknown_article_is_invalid)
{
  std::string input = "add-article-tags \"Advanced C++\" cpp\n"
                      "show-tags\n";
  BOOST_CHECK(runCommands(input) == invalidCommand);
}

BOOST_AUTO_TEST_CASE(unquoted_article_is_invalid)
{
  std::string input = "add-article \"Python\" python beginner\n"
                      "add-article-tags Python basics\n";
  BOOST_CHECK(runCommands(input) == invalidCommand);
}

BOOST_AUTO_TEST_CASE(invalid_tag_adds_nothing)
{
  std::string input = "add-article \"Advanced C++\" cpp advanced cpp\n"
                      "add-article-tags \"Advanced C++\" templates 123\n"
                      "show-article-info \"Advanced C++\"\n";
  std::string expected = invalidCommand + "Article: \"Advanced C++\"\n"
                                          "Category: cpp | Difficulty: advanced\n"
                                          "Tags: cpp\n"
                                          "Avg Rating: 0.0/10 (0 ratings)\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(show_tags_tests)

BOOST_AUTO_TEST_CASE(empty_storage_prints_nothing)
{
  BOOST_CHECK(runCommands("show-tags\n") == "");
}

BOOST_AUTO_TEST_CASE(prints_union_of_preferences_and_article_tags_sorted)
{
  std::string input = "add-user roman rust cpp\n"
                      "add-article \"Advanced C++\" cpp advanced templates cpp\n"
                      "add-article \"Intro to Go\" go beginner backend\n"
                      "show-tags\n";
  BOOST_CHECK(runCommands(input) == "backend\ncpp\nrust\ntemplates\n");
}

BOOST_AUTO_TEST_CASE(extra_argument_is_invalid)
{
  BOOST_CHECK(runCommands("show-tags all\n") == invalidCommand);
}

BOOST_AUTO_TEST_CASE(tag_disappears_with_its_last_user)
{
  std::string input = "add-user roman rust\n"
                      "delete-user roman\n"
                      "show-tags\n";
  BOOST_CHECK(runCommands(input) == "");
}

BOOST_AUTO_TEST_CASE(tag_disappears_with_its_last_article)
{
  std::string input = "add-article \"Advanced C++\" cpp advanced templates\n"
                      "delete-article \"Advanced C++\"\n"
                      "show-tags\n";
  BOOST_CHECK(runCommands(input) == "");
}

BOOST_AUTO_TEST_CASE(shared_tag_stays_while_used)
{
  std::string input = "add-user roman cpp rust\n"
                      "add-article \"Advanced C++\" cpp advanced cpp\n"
                      "delete-user roman\n"
                      "show-tags\n";
  BOOST_CHECK(runCommands(input) == "cpp\n");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(show_articles_categories_tests)

BOOST_AUTO_TEST_CASE(empty_storage_prints_nothing)
{
  BOOST_CHECK_EQUAL(runCommands("show-articles-categories\n"), "");
}

BOOST_AUTO_TEST_CASE(prints_unique_categories_sorted)
{
  std::string input = "add-article \"Advanced C++\" cpp advanced\n"
                      "add-article \"Intro to Python\" python beginner\n"
                      "add-article \"C++ Basics\" cpp beginner\n"
                      "add-article \"Neural Networks Primer\" ai intermediate\n"
                      "show-articles-categories\n";
  BOOST_CHECK(runCommands(input) == "ai\ncpp\npython\n");
}

BOOST_AUTO_TEST_CASE(extra_argument_is_invalid)
{
  BOOST_CHECK(runCommands("show-articles-categories all\n") == invalidCommand);
}

BOOST_AUTO_TEST_CASE(category_disappears_with_its_last_article)
{
  std::string input = "add-article \"Advanced C++\" cpp advanced\n"
                      "add-article \"Intro to Python\" python beginner\n"
                      "delete-article \"Advanced C++\"\n"
                      "show-articles-categories\n";
  BOOST_CHECK(runCommands(input) == "python\n");
}

BOOST_AUTO_TEST_CASE(shared_category_stays_while_used)
{
  std::string input = "add-article \"Advanced C++\" cpp advanced\n"
                      "add-article \"C++ Basics\" cpp beginner\n"
                      "delete-article \"Advanced C++\"\n"
                      "show-articles-categories\n";
  BOOST_CHECK(runCommands(input) == "cpp\n");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(delete_user_tests)

BOOST_AUTO_TEST_CASE(removes_user_and_prints_nothing)
{
  std::string input = "add-user roman\n"
                      "add-user alex\n"
                      "delete-user roman\n"
                      "show-users\n";
  BOOST_CHECK(runCommands(input) == "alex\n");
}

BOOST_AUTO_TEST_CASE(unknown_user_is_invalid)
{
  BOOST_CHECK(runCommands("delete-user unknown_user\n") == invalidCommand);
}

BOOST_AUTO_TEST_CASE(wrong_arguments_are_invalid)
{
  std::string input = "add-user roman\n"
                      "delete-user\n"
                      "delete-user roman extra\n"
                      "delete-user \"roman\"\n"
                      "show-users\n";
  BOOST_CHECK(runCommands(input) == repeatInvalid(3) + "roman\n");
}

BOOST_AUTO_TEST_CASE(deleting_twice_is_invalid)
{
  std::string input = "add-user roman\n"
                      "delete-user roman\n"
                      "delete-user roman\n";
  BOOST_CHECK(runCommands(input) == invalidCommand);
}

BOOST_AUTO_TEST_CASE(removes_user_ratings_from_articles)
{
  std::string input = "add-user roman\n"
                      "add-user alex\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "rate-article roman \"Advanced C++\" 10\n"
                      "rate-article alex \"Advanced C++\" 6\n"
                      "delete-user alex\n"
                      "show-article-info \"Advanced C++\"\n";
  std::string expected = "Article: \"Advanced C++\"\n"
                         "Category: cpp | Difficulty: advanced\n"
                         "Tags:\n"
                         "Avg Rating: 10.0/10 (1 ratings)\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(deleted_user_is_unknown_for_other_commands)
{
  std::string input = "add-user roman\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "delete-user roman\n"
                      "show-user-info roman\n"
                      "show-user-ratings roman\n"
                      "rate-article roman \"Advanced C++\" 5\n"
                      "recommend roman\n";
  BOOST_CHECK(runCommands(input) == repeatInvalid(4));
}

BOOST_AUTO_TEST_CASE(user_can_be_added_again_from_scratch)
{
  std::string input = "add-user roman cpp\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "rate-article roman \"Advanced C++\" 10\n"
                      "delete-user roman\n"
                      "add-user roman\n"
                      "show-user-info roman\n"
                      "show-article-info \"Advanced C++\"\n";
  std::string expected = "User: roman\n"
                         "Preferences:\n"
                         "Ratings count: 0\n"
                         "Article: \"Advanced C++\"\n"
                         "Category: cpp | Difficulty: advanced\n"
                         "Tags:\n"
                         "Avg Rating: 0.0/10 (0 ratings)\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(delete_article_tests)

BOOST_AUTO_TEST_CASE(removes_article_and_prints_nothing)
{
  std::string input = "add-article \"Advanced C++\" cpp advanced\n"
                      "add-article \"Intro to Python\" python beginner\n"
                      "delete-article \"Advanced C++\"\n"
                      "list-articles\n";
  BOOST_CHECK(runCommands(input) == "\"Intro to Python\" [python | beginner]\n");
}

BOOST_AUTO_TEST_CASE(unknown_article_is_invalid)
{
  BOOST_CHECK(runCommands("delete-article \"Non Existent\"\n") == invalidCommand);
}

BOOST_AUTO_TEST_CASE(wrong_arguments_are_invalid)
{
  std::string input = "add-article \"Python\" python beginner\n"
                      "delete-article\n"
                      "delete-article Python\n"
                      "delete-article \"Python\" extra\n"
                      "list-articles\n";
  BOOST_CHECK(runCommands(input) == repeatInvalid(3) + "\"Python\" [python | beginner]\n");
}

BOOST_AUTO_TEST_CASE(deleting_twice_is_invalid)
{
  std::string input = "add-article \"Advanced C++\" cpp advanced\n"
                      "delete-article \"Advanced C++\"\n"
                      "delete-article \"Advanced C++\"\n";
  BOOST_CHECK(runCommands(input) == invalidCommand);
}

BOOST_AUTO_TEST_CASE(removes_article_from_user_ratings)
{
  std::string input = "add-user roman\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "add-article \"Intro to Python\" python beginner\n"
                      "rate-article roman \"Advanced C++\" 10\n"
                      "rate-article roman \"Intro to Python\" 7\n"
                      "delete-article \"Advanced C++\"\n"
                      "show-user-ratings roman\n"
                      "show-user-info roman\n";
  std::string expected = "\"Intro to Python\" - 7/10\n"
                         "User: roman\n"
                         "Preferences:\n"
                         "Ratings count: 1\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_CASE(deleted_article_is_unknown_for_other_commands)
{
  std::string input = "add-user roman\n"
                      "add-article \"Advanced C++\" cpp advanced\n"
                      "delete-article \"Advanced C++\"\n"
                      "show-article-info \"Advanced C++\"\n"
                      "rate-article roman \"Advanced C++\" 5\n"
                      "add-article-tags \"Advanced C++\" cpp\n"
                      "similar \"Advanced C++\"\n";
  BOOST_CHECK(runCommands(input) == repeatInvalid(4));
}

BOOST_AUTO_TEST_CASE(article_can_be_added_again_from_scratch)
{
  std::string input = "add-user roman\n"
                      "add-article \"Advanced C++\" cpp advanced templates\n"
                      "rate-article roman \"Advanced C++\" 10\n"
                      "delete-article \"Advanced C++\"\n"
                      "add-article \"Advanced C++\" python beginner\n"
                      "show-article-info \"Advanced C++\"\n"
                      "show-user-info roman\n";
  std::string expected = "Article: \"Advanced C++\"\n"
                         "Category: python | Difficulty: beginner\n"
                         "Tags:\n"
                         "Avg Rating: 0.0/10 (0 ratings)\n"
                         "User: roman\n"
                         "Preferences:\n"
                         "Ratings count: 0\n";
  BOOST_CHECK(runCommands(input) == expected);
}

BOOST_AUTO_TEST_SUITE_END()
