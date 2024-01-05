#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "criterion/criterion.h"

#define TEST_ORIG_DIR "tests.in.orig"
#define TEST_EXPECTED_DIR "tests.out.exp"
#define TEST_INPUT_DIR "tests.in"
#define TEST_OUTPUT_DIR "tests.out"
#define ARGS_TEXT_LEN 200
#define TEST_TIMEOUT 5

int run_using_system(char *test_name, char *args);
int run_using_system_no_valgrind(char *test_name, char *args);
void expect_normal_exit(int status);
void expect_error_exit(int actual_status, int expected_status);
void expect_no_valgrind_errors(int status);
void expect_outfile_matches(char *test_name);
void report_return_value(int return_act, int return_exp);
void prep_files(char *orig_file, char *input_file);
