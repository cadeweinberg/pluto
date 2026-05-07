// SPDX-License-Identifier: MIT

/*!
 * \file support/cli_parser.c
 * \brief pluto_CliParser implementation
 */

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "support/cli_parser.h"

void pluto_cli_parser_init(pluto_CliParser *parser, pluto_CliOption const *options,
                            int32_t option_count) {
  assert(parser != NULL);
  assert(options != NULL);
  assert(option_count >= 0);
  parser->option_index = 1; // Start from 1 to skip the program name (argv[0])
  parser->suboption_index =
      0; // Start from 0 for parsing suboptions in short options (e.g., -abc)
  parser->option_count = option_count;
  parser->options = options;
}

static pluto_CliResult _pluto_parse_result(pluto_CliParser *parser, int32_t option,
                                    char const *argument) {
  parser->option_index++; // Move to the next option for the next call
  return (pluto_CliResult){.option = option, .argument = argument};
}

static pluto_CliResult _pluto_parse_subresult(pluto_CliParser *parser, int32_t option,
                                       char const *argument) {
  parser->suboption_index++; // Move to the next suboption for the next call
  return (pluto_CliResult){.option = option, .argument = argument};
}

static size_t option_text_length(char const *option_text) {
  size_t length = 0;
  while (option_text[length] != '\0' && option_text[length] != '=' &&
         option_text[length] != ':' && option_text[length] != ' ') {
    length++;
  }
  return length;
}

static pluto_CliResult _pluto_parse_long_option_argument(pluto_CliParser *parser,
                                                  pluto_CliOption const *option,
                                                  char const *option_text,
                                                  int32_t argc,
                                                  char const *argv[]) {
  switch (option->argument_kind) {
  case PLUTO_CLI_ARGUMENT_NONE:
    return _pluto_parse_result(parser, option->short_name, NULL);

  case PLUTO_CLI_ARGUMENT_REQUIRED: {
    // Check if the option is in the form --option=value
    char const *argument = NULL;
    size_t current_arg_len = strlen(option_text);
    size_t option_name_len = strlen(option->name);
    if (current_arg_len > option_name_len &&
        (option_text[option_name_len] == '=' ||
         option_text[option_name_len] == ':')) {
      argument = option_text + option_name_len + 1; // Skip the '=' or ':'
      return _pluto_parse_result(parser, option->short_name, argument);
    }

    // Otherwise, the argument should be the next argv element
    if (parser->option_index + 1 < argc &&
        argv[parser->option_index + 1] != NULL &&
        argv[parser->option_index + 1][0] != '-') {
      argument = argv[parser->option_index + 1]; // Next argv element
      parser->option_index++;                    // Consume the argument
      return _pluto_parse_result(parser, option->short_name, argument);
    }

    parser->option_index++; // Move to the next option for the next call
    return _pluto_parse_result(parser, PLUTO_CLI_OPTION_MISSING_ARGUMENT,
                        option->name); // Missing required argument
  }

  case PLUTO_CLI_ARGUMENT_OPTIONAL: {
    // Check if the option is in the form --option=value
    char const *argument = NULL;
    size_t current_arg_len = strlen(option_text);
    size_t option_name_len = strlen(option->name);
    if (current_arg_len > option_name_len &&
        (option_text[option_name_len] == '=' ||
         option_text[option_name_len] == ':')) {
      argument = option_text + option_name_len + 1; // Skip the '=' or ':'
      return _pluto_parse_result(parser, option->short_name, argument);
    }

    if (parser->option_index + 1 < argc &&
        argv[parser->option_index + 1] != NULL &&
        argv[parser->option_index + 1][0] != '-') {
      char const *next_arg = argv[parser->option_index + 1];
      // Optional argument provided as the next argv element (e.g.,
      // --opt value)
      parser->option_index++; // Consume the argument
      return _pluto_parse_result(parser, option->short_name, next_arg);
    }
    // Optional argument not provided, return the option with NULL argument
    return _pluto_parse_result(parser, option->short_name, NULL);
  }

  default:
    // Invalid argument kind
    return _pluto_parse_result(parser, PLUTO_CLI_OPTION_UNRECOGNIZED_ARGUMENT_KIND,
                        option->name); // Unrecognized option
  }
}

static pluto_CliResult _pluto_parse_long_option(pluto_CliParser *parser,
                                         char const *option_text, int32_t argc,
                                         char const *argv[]) {
  size_t option_length = option_text_length(option_text);
  for (int32_t i = 0; i < parser->option_count; ++i) {
    pluto_CliOption const *option = &parser->options[i];
    if (strncmp(option->name, option_text, option_length) == 0 &&
        option->name[option_length] == '\0') {
      return _pluto_parse_long_option_argument(parser, option, option_text, argc,
                                        argv);
    }
  }

  return _pluto_parse_result(parser, PLUTO_CLI_OPTION_UNRECOGNIZED,
                      option_text); // Unrecognized option
}

static pluto_CliResult _pluto_parse_short_option_argument(pluto_CliParser *parser,
                                                   pluto_CliOption const *option,
                                                   char const *option_text,
                                                   int32_t argc,
                                                   char const *argv[]) {
  switch (option->argument_kind) {
  case PLUTO_CLI_ARGUMENT_NONE:
    if (option_text[parser->suboption_index + 1] != '\0') {
      // More short options combined (e.g., -abc)
      return _pluto_parse_subresult(parser, option->short_name, NULL);
    }
    // no more suboptions, move to the next argv element for the next call
    parser->suboption_index = 0; // Reset suboption index for the next option
    return _pluto_parse_result(parser, option->short_name, NULL);

  case PLUTO_CLI_ARGUMENT_REQUIRED: {
    if (option_text[parser->suboption_index + 1] != '\0') {
      // Argument provided in the same argv element (e.g., -O2)
      char const *argument = option_text + parser->suboption_index + 1;
      return _pluto_parse_result(parser, option->short_name, argument);
    }

    // Argument should be the next argv element
    if (parser->option_index + 1 < argc &&
        argv[parser->option_index + 1] != NULL &&
        argv[parser->option_index + 1][0] != '-') {
      char const *argument =
          argv[parser->option_index + 1]; // Next argv element
      parser->option_index++;             // Consume the argument
      return _pluto_parse_result(parser, option->short_name, argument);
    }

    parser->option_index++; // Move to the next option for the next call
    return _pluto_parse_result(parser, PLUTO_CLI_OPTION_MISSING_ARGUMENT,
                        option->name); // Missing required argument
  }

  case PLUTO_CLI_ARGUMENT_OPTIONAL:
    if (option_text[parser->suboption_index + 1] != '\0') {
      // Optional argument provided in the same argv element (e.g., -O2)
      char const *argument = option_text + parser->suboption_index + 1;
      return _pluto_parse_result(parser, option->short_name, argument);
    }

    if (parser->option_index + 1 < argc) {
      char const *next_arg = argv[parser->option_index + 1];
      if (next_arg[0] != '-') {
        // Optional argument provided as the next argv element (e.g.,
        // -O 2)
        parser->option_index++; // Consume the argument
        return _pluto_parse_result(parser, option->short_name, next_arg);
      }
    }
    // Optional argument not provided, return the option with NULL argument
    return _pluto_parse_result(parser, option->short_name, NULL);

  default:
    // Invalid argument kind
    return _pluto_parse_result(parser, PLUTO_CLI_OPTION_UNRECOGNIZED_ARGUMENT_KIND,
                        option->name);
  }
}

static pluto_CliResult _pluto_parse_short_option(pluto_CliParser *parser,
                                          char const *option_text, int32_t argc,
                                          char const *argv[]) {
  char current_char = option_text[parser->suboption_index];
  for (int32_t i = 0; i < parser->option_count; ++i) {
    pluto_CliOption const *option = &parser->options[i];
    if (option->short_name == current_char) {
      return _pluto_parse_short_option_argument(parser, option, option_text, argc,
                                         argv);
    }
  }

  return _pluto_parse_result(parser, PLUTO_CLI_OPTION_UNRECOGNIZED,
                      option_text); // Unrecognized option
}

pluto_CliResult pluto_cli_parser_next(pluto_CliParser *parser,
                                      int32_t argc,
                                      char const *argv[]) {
  assert(parser != NULL);
  assert(argc >= 0);
  assert(argv != NULL);

  if (parser->option_index >= argc || argv[parser->option_index] == NULL) {
    return _pluto_parse_result(parser, PLUTO_CLI_OPTION_END, NULL); // No more options
  }

  char const *current_arg = argv[parser->option_index];
  if (current_arg[0] == '-') {
    // Long option (e.g., --help)
    if (current_arg[1] == '-') {
      char const *option_text = current_arg + 2; // Skip the "--"
      return _pluto_parse_long_option(parser, option_text, argc, argv);
    }
    // Short option (e.g., -h)
    char const *option_text = current_arg + 1; // Skip the "-"
    return _pluto_parse_short_option(parser, option_text, argc, argv);
  }

  // Positional argument
  return _pluto_parse_result(parser, PLUTO_CLI_OPTION_POSITIONAL_ARGUMENT, current_arg);
}
