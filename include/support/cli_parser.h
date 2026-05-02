// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PLUTO_SUPPORT_CLI_PARSER_H
#define PLUTO_SUPPORT_CLI_PARSER_H

#include <stdint.h>

// .. c:enum:: pluto_CliArgumentKind
//    Enumeration for the kind of argument an option takes.
//    c:data `PLUTO_CLI_ARGUMENT_NONE`: The option does not take an argument.
//    c:data `PLUTO_CLI_ARGUMENT_REQUIRED`: The option requires an argument.
//    c:data `PLUTO_CLI_ARGUMENT_OPTIONAL`: The option takes an optional argument.
typedef enum pluto_CliArgumentKind {
  PLUTO_CLI_ARGUMENT_NONE,
  PLUTO_CLI_ARGUMENT_REQUIRED,
  PLUTO_CLI_ARGUMENT_OPTIONAL,
} pluto_CliArgumentKind;

// .. c:struct:: pluto_CliOption
//    Structure representing a command-line option.
//    c:member name: The long name of the option
//    c:member description: A description of the option for help messages.
//    c:member argument_kind: The kind of argument the option takes
//    c:member short_name: The short name of the option
typedef struct pluto_CliOption {
  char const *name;
  char const *description;
  pluto_CliArgumentKind argument_kind;
  char short_name;
} pluto_CliOption;

// .. c:struct:: pluto_CliParser
//    Structure representing the state of the command-line option parser.
//    c:member option_index: The current index in the argv array being parsed.
//    c:member suboption_index: The current suboption_index
//    c:member option_count: The total number of options defined.
//    c:member options: A pointer to an array of options.
typedef struct pluto_CliParser {
  int32_t option_index;
  int32_t suboption_index;
  int32_t option_count;
  pluto_CliOption const *options;
} pluto_CliParser;

// .. c:macro:: PLUTO_CLI_OPTION_END
//    A constant indicating that there are no more options to parse.
//    Value: 0
#define PLUTO_CLI_OPTION_END 0
// .. c:macro:: PLUTO_CLI_OPTION_MISSING_ARGUMENT
//    A constant indicating that a required argument for an option is missing.
//    Value: -1
#define PLUTO_CLI_OPTION_MISSING_ARGUMENT -1
// .. c:macro:: PLUTO_CLI_OPTION_UNRECOGNIZED_ARGUMENT_KIND
//    A constant indicating that an option has an unrecognized argument kind.
//    Value: -2
#define PLUTO_CLI_OPTION_UNRECOGNIZED_ARGUMENT_KIND -2
// .. c:macro:: PLUT_CLI_OPTION_UNRECOGNIZED
//    A constant indicating that an unrecognized option was encountered.
//    Value: '?'
#define PLUTO_CLI_OPTION_UNRECOGNIZED '?'
// .. c:macro:: PLUT_CLI_OPTION_POSITIONAL_ARGUMENT
//    A constant indicating that a positional argument was encountered.
//    Value: '!'
#define PLUTO_CLI_OPTION_POSITIONAL_ARGUMENT '!'

// .. c:struct:: pluto_CliResult
//    Structure representing the result of parsing a command-line option.
//    :member option: The short name of the option parsed, or the error code.
//    :member argument: The argument associated with the option
typedef struct pluto_CliResult {
  int32_t option;
  char const *argument;
} pluto_CliResult;

// .. c:function:: void pluto_cli_parser_init(CliOptionParser *parser,
// CliOption const *options, int32_t option_count)
//    Initializes a CliOptionParser structure with the provided options.
//    :param parser: A pointer to the CliOptionParser structure to initialize.
//    :param options: A pointer to an array of available options.
//    :param option_count: The number of options.
void pluto_cli_parser_init(pluto_CliParser *parser,
                           pluto_CliOption const *options,
                           int32_t option_count);

// .. c:function:: pluto_CliResult
// pluto_cli_parser_next(CliOptionParser *parser, int32_t argc, char const *argv[])
//    Parses the next command-line option from the argv array.
//    :param parser: A pointer to the initialized CliOptionParser structure.
//    :param argc: The number of command-line arguments
//    :param argv: The array of command-line arguments
//    :returns: A CliOptionResult structure containing the result
pluto_CliResult pluto_cli_parser_next(pluto_CliParser *parser,
                                      int32_t argc,
                                      char const *argv[]);

#endif // !PLUTO_SUPPORT_CLI_PARSER_H

