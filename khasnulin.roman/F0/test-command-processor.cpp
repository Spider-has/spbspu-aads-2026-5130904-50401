#include "CommandProcessor.hpp"
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
