// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>

#include "support/cli_parser.h"

#include <cmocka.h>

static const pluto_CliOption options[] = {
    {
        .name = "help",
        .description = "Show this help message and exit.",
        .argument_kind = PLUTO_CLI_ARGUMENT_NONE,
        .short_name = 'h',
    },
    {
        .name = "version",
        .description = "Show version information and exit.",
        .argument_kind = PLUTO_CLI_ARGUMENT_NONE,
        .short_name = 'v',
    },
    {
        .name = "output",
        .description = "Specify the output file.",
        .argument_kind = PLUTO_CLI_ARGUMENT_REQUIRED,
        .short_name = 'o',
    },
    {
        .name = "optimize",
        .description = "Enable optimization.",
        .argument_kind = PLUTO_CLI_ARGUMENT_OPTIONAL,
        .short_name = 'O',
    },
    {
        .name = "jflag",
        .description = "An interesting flag.",
        .argument_kind = PLUTO_CLI_ARGUMENT_NONE,
        .short_name = 'j',
    },
    {
        .name = "kflag",
        .description = "An interesting flag.",
        .argument_kind = PLUTO_CLI_ARGUMENT_NONE,
        .short_name = 'k',
    },
    {
        .name = "lflag",
        .description = "An interesting flag.",
        .argument_kind = PLUTO_CLI_ARGUMENT_NONE,
        .short_name = 'l',
    }};

static const int32_t options_length =
    (int32_t)(sizeof(options) / sizeof(options[0]));

static void test_pluto_cli_parser_init(void **state) {
  (void)state; // Unused
  pluto_CliParser parser;
  pluto_cli_parser_init(&parser, options, options_length);
  assert_int_equal(parser.option_index, 1);
  assert_int_equal(parser.suboption_index, 0);
  assert_int_equal(parser.option_count, options_length);
  assert_ptr_equal(parser.options, options);
}

static void
test_pluto_cli_parser_next_long_argument_none(void **state) {
  (void)state; // Unused
  pluto_CliParser parser;
  pluto_cli_parser_init(&parser, options, options_length);

  int32_t argc = 5;
  char const *argv[] = {"program", "--help", "--version", "--jflag", NULL};

  pluto_CliResult result;

  // Parse --help
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'h');
  assert_ptr_equal(result.argument, NULL);

  // Parse --version
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'v');
  assert_ptr_equal(result.argument, NULL);

  // Parse --jflag
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'j');
  assert_ptr_equal(result.argument, NULL);

  // No more options
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, PLUTO_CLI_OPTION_END);
  assert_ptr_equal(result.argument, NULL);
}

static void
test_pluto_cli_parser_next_long_argument_required(void **state) {
  (void)state; // Unused
  pluto_CliParser parser;
  pluto_cli_parser_init(&parser, options, options_length);

  int32_t argc = 5;
  char const *argv[] = {"program", "--output=output.txt", "--output",
                        "output.txt", NULL};

  pluto_CliResult result;

  // Parse --output=output.txt
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'o');
  assert_string_equal(result.argument, "output.txt");

  // Parse --output output.txt
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'o');
  assert_string_equal(result.argument, "output.txt");

  // No more options
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, PLUTO_CLI_OPTION_END);
  assert_ptr_equal(result.argument, NULL);
}

static void test_pluto_cli_parser_next_long_argument_required_missing(
    void **state) {
  (void)state; // Unused
  pluto_CliParser parser;
  pluto_cli_parser_init(&parser, options, options_length);

  int32_t argc = 3;
  char const *argv[] = {"program", "--output", NULL};

  pluto_CliResult result;

  // Parse --output (missing required argument)
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, PLUTO_CLI_OPTION_MISSING_ARGUMENT);
  assert_ptr_equal(result.argument, "output");

  // No more options
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, PLUTO_CLI_OPTION_END);
  assert_ptr_equal(result.argument, NULL);
}

static void
test_pluto_cli_parser_next_long_argument_optional(void **state) {
  (void)state; // Unused
  pluto_CliParser parser;
  pluto_cli_parser_init(&parser, options, options_length);

  int32_t argc = 4;
  char const *argv[] = {"program", "--optimize=2", "--optimize", NULL};

  pluto_CliResult result;

  // Parse --optimize=2
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'O');
  assert_string_equal(result.argument, "2");

  // Parse --optimize (optional argument missing)
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'O');
  assert_ptr_equal(result.argument, NULL);

  // No more options
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, PLUTO_CLI_OPTION_END);
  assert_ptr_equal(result.argument, NULL);
}

static void
test_pluto_cli_parser_next_long_argument_unrecognized(void **state) {
  (void)state; // Unused
  pluto_CliParser parser;
  pluto_cli_parser_init(&parser, options, options_length);

  int32_t argc = 3;
  char const *argv[] = {"program", "--unknown", NULL};

  pluto_CliResult result;

  // Parse --unknown
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, PLUTO_CLI_OPTION_UNRECOGNIZED);
  assert_string_equal(result.argument, "unknown");

  // No more options
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, PLUTO_CLI_OPTION_END);
  assert_ptr_equal(result.argument, NULL);
}

static void
test_pluto_cli_parser_next_short_argument_none(void **state) {
  (void)state; // Unused
  pluto_CliParser parser;
  pluto_cli_parser_init(&parser, options, options_length);

  int32_t argc = 4;
  char const *argv[] = {"program", "-h", "-v", NULL};

  pluto_CliResult result;

  // Parse -h
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'h');
  assert_ptr_equal(result.argument, NULL);

  // Parse -v
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'v');
  assert_ptr_equal(result.argument, NULL);

  // No more options
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, PLUTO_CLI_OPTION_END);
  assert_ptr_equal(result.argument, NULL);
}

static void
test_pluto_cli_parser_next_short_argument_none_combined(void **state) {
  (void)state; // Unused
  pluto_CliParser parser;
  pluto_cli_parser_init(&parser, options, options_length);

  int32_t argc = 4;
  char const *argv[] = {"program", "-hjkl", "-kljh", NULL};

  pluto_CliResult result;

  // Parse -h
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'h');
  assert_ptr_equal(result.argument, NULL);

  // Parse -j
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'j');
  assert_ptr_equal(result.argument, NULL);

  // Parse -k
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'k');
  assert_ptr_equal(result.argument, NULL);

  // Parse -l
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'l');
  assert_ptr_equal(result.argument, NULL);

  // Parse -k
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'k');
  assert_ptr_equal(result.argument, NULL);

  // Parse -l
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'l');
  assert_ptr_equal(result.argument, NULL);

  // Parse -j
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'j');
  assert_ptr_equal(result.argument, NULL);

  // Parse -h
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'h');
  assert_ptr_equal(result.argument, NULL);

  // No more options
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, PLUTO_CLI_OPTION_END);
  assert_ptr_equal(result.argument, NULL);
}

static void
test_pluto_cli_parser_next_short_argument_required(void **state) {
  (void)state; // Unused
  pluto_CliParser parser;
  pluto_cli_parser_init(&parser, options, options_length);

  int32_t argc = 4;
  char const *argv[] = {"program", "-ooutput.txt", "-o", "output.txt", NULL};

  pluto_CliResult result;

  // Parse -ooutput.txt
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'o');
  assert_string_equal(result.argument, "output.txt");

  // Parse -o output.txt
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'o');
  assert_string_equal(result.argument, "output.txt");

  // No more options
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, PLUTO_CLI_OPTION_END);
  assert_ptr_equal(result.argument, NULL);
}

static void test_pluto_cli_parser_next_short_argument_required_missing(
    void **state) {
  (void)state; // Unused
  pluto_CliParser parser;
  pluto_cli_parser_init(&parser, options, options_length);

  int32_t argc = 3;
  char const *argv[] = {"program", "-o", NULL};

  pluto_CliResult result;

  // Parse -o (missing required argument)
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, PLUTO_CLI_OPTION_MISSING_ARGUMENT);
  assert_ptr_equal(result.argument, "output");

  // No more options
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, PLUTO_CLI_OPTION_END);
  assert_ptr_equal(result.argument, NULL);
}

static void
test_pluto_cli_parser_next_short_argument_unrecognized(void **state) {
  (void)state; // Unused
  pluto_CliParser parser;
  pluto_cli_parser_init(&parser, options, options_length);

  int32_t argc = 3;
  char const *argv[] = {"program", "-u", NULL};

  pluto_CliResult result;

  // Parse -u (unrecognized option)
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, PLUTO_CLI_OPTION_UNRECOGNIZED);
  assert_string_equal(result.argument, "u");

  // No more options
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, PLUTO_CLI_OPTION_END);
  assert_ptr_equal(result.argument, NULL);
}

static void test_pluto_cli_parser_next_positional(void **state) {
  (void)state; // Unused
  pluto_CliParser parser;
  pluto_cli_parser_init(&parser, options, options_length);

  int32_t argc = 3;
  char const *argv[] = {"program", "positional_arg", NULL};

  pluto_CliResult result;

  // Parse positional argument
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, PLUTO_CLI_OPTION_POSITIONAL_ARGUMENT);
  assert_string_equal(result.argument, "positional_arg");

  // No more options
  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, PLUTO_CLI_OPTION_END);
  assert_ptr_equal(result.argument, NULL);
}

static void test_pluto_cli_parser_next_mixed(void **state) {
  (void)state; // Unused
  pluto_CliParser parser;
  pluto_cli_parser_init(&parser, options, options_length);

  char const *argv[] = {
	  "program",
	  "--hflag",
	  "--jflag",
	  "--kflag",
	  "--output=hello.txt",
	  "-ohi.txt",
	  "-kjl",
  };
  int32_t argc = (int32_t)(sizeof(argv) / sizeof(*argv));

  pluto_CliResult result;

  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, PLUTO_CLI_OPTION_UNRECOGNIZED);
  assert_string_equal(result.argument, "hflag");

  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'j');
  assert_ptr_equal(result.argument, NULL);

  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'k');
  assert_ptr_equal(result.argument, NULL);

  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'o');
  assert_string_equal(result.argument, "hello.txt");

  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'o');
  assert_string_equal(result.argument, "hi.txt");

  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'k');
  assert_ptr_equal(result.argument, NULL);

  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'j');
  assert_ptr_equal(result.argument, NULL);

  result = pluto_cli_parser_next(&parser, argc, argv);
  assert_int_equal(result.option, 'l');
  assert_ptr_equal(result.argument, NULL);
}

int32_t main (int32_t argc, char const *argv[]) {
  const struct CMUnitTest cli_parser_tests[] = {
    cmocka_unit_test(test_pluto_cli_parser_init),
    cmocka_unit_test(test_pluto_cli_parser_next_long_argument_none),
    cmocka_unit_test(test_pluto_cli_parser_next_long_argument_required),
    cmocka_unit_test(test_pluto_cli_parser_next_long_argument_required_missing),
    cmocka_unit_test(test_pluto_cli_parser_next_long_argument_optional),
    cmocka_unit_test(test_pluto_cli_parser_next_long_argument_unrecognized),
    cmocka_unit_test(test_pluto_cli_parser_next_short_argument_none),
    cmocka_unit_test(test_pluto_cli_parser_next_short_argument_none_combined),
    cmocka_unit_test(test_pluto_cli_parser_next_short_argument_required),
    cmocka_unit_test(test_pluto_cli_parser_next_short_argument_required_missing),
    cmocka_unit_test(test_pluto_cli_parser_next_short_argument_unrecognized),
    cmocka_unit_test(test_pluto_cli_parser_next_positional),
    cmocka_unit_test(test_pluto_cli_parser_next_mixed),
  };
  
  return cmocka_run_group_tests(cli_parser_tests, NULL, NULL);
}
