/*
DO NOT CHANGE THE CONTENTS OF THIS FILE IN CASE A NEW VERSION IS DISTRIBUTED.
PUT YOUR OWN TEST CASES IN student_tests.c
*/

#include "unit_tests.h"
#include "hw5.h"
 
static char test_log_outfile[100];

int run_using_system(char *test_name, char *args) {
    char *executable = "./bin/hw5";
    assert(access(executable, F_OK) == 0);

    char cmd[500];
    sprintf(test_log_outfile, "%s/%s.log", TEST_OUTPUT_DIR, test_name);
#if defined(__linux__)
    sprintf(cmd, "ulimit -f 300; ulimit -t 5; valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes --error-exitcode=37 ./bin/hw5 %s >> %s 2>&1",
	    args, test_log_outfile);
#else
    cr_log_warn("Skipping valgrind tests. Run tests on Linux or GitHub for full output.\n");
#endif
    return system(cmd);
}

int run_using_system_no_valgrind(char *test_name, char *args) {
    char executable[100];
    sprintf(executable, "./bin/hw5");
    assert(access(executable, F_OK) == 0);

    char cmd[500];
    sprintf(test_log_outfile, "%s/%s.log", TEST_OUTPUT_DIR, test_name);
    sprintf(cmd, "ulimit -f 300; ulimit -t 5; ./bin/hw5 %s >> %s 2>&1", args, test_log_outfile);
    return system(cmd);
}

void expect_outfile_matches(char *test_name) {
    char cmd[500];
    sprintf(cmd, "diff %s/%s.out.txt %s/%s.out.txt >> %s", TEST_EXPECTED_DIR, test_name, TEST_OUTPUT_DIR, test_name,
        test_log_outfile);
    int err = system(cmd);
    cr_expect_eq(err, 0, "The output was not what was expected (diff exited with status %d).\n", WEXITSTATUS(err));
}

void expect_normal_exit(int status) {
    cr_expect_eq(status, 0, "The program did not exit normally (status = %d).\n", status);
}

void expect_error_exit(int actual_status, int expected_status) {
    cr_expect_eq(WEXITSTATUS(actual_status), expected_status,
		 "The program exited with status %d instead of %d.\n", WEXITSTATUS(actual_status), expected_status);
}

void expect_no_valgrind_errors(int status) {
    cr_expect_neq(WEXITSTATUS(status), 37, "Valgrind reported errors -- see %s", test_log_outfile);
}

void prep_files(char *orig_file, char *input_file) {
    char cmd[200];
    sprintf(cmd, "cp %s/%s %s/%s.in.txt", TEST_ORIG_DIR, orig_file, TEST_INPUT_DIR, input_file);
    system(cmd);
}

TestSuite(base_output, .timeout=TEST_TIMEOUT, .disabled=false); // output file
TestSuite(base_valgrind, .timeout=TEST_TIMEOUT, .disabled=false); // valgrind result

TestSuite(base_invalid_args, .timeout=TEST_TIMEOUT); // return code to OS

static char args[ARGS_TEXT_LEN];

/* Check output file. */
Test(base_output, simple_search01, .description="Perform a simple replacement over entire file. Terms have the same length.") {
    char *test_name = "simple_search01";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s the -r WOLFIE %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, simple_search02, .description="Perform a simple replacement in a single line. Start line and end line are the same.") {
    char *test_name = "simple_search02";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s machine -r device -l 23,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, simple_search03, .description="Perform a simple replacement within the range. Search text appears outside the range as well.") {
    char *test_name = "simple_search03";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s early -r \"WELL TIMED\" -l 11,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, simple_search04, .description="Perform a simple replacement within the given range. Replace text is alphanumerical.") {
    char *test_name = "simple_search04";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-r 1st -l 1,20 -s first %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, simple_search05, .description="Perform a simple replacement within the specified range. Search term is followed by a punctuation mark.") {
    char *test_name = "simple_search05";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-l 1,22 -s intelligence -r wisdom %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, simple_search06, .description="Perform a simple replacement over entire file. Multiple occurences of the search text (at the beginning, in the middle, at the end).") {
    char *test_name = "simple_search06";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s Turing -r \"TURING THE GREAT\" %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, simple_search07, .description="Perform a simple replacement over entire file. Search terms are followed by different characters.") {
    char *test_name = "simple_search07";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s Manchester -r MANCHESTER %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, simple_search08, .description="Perform a simple replacement over entire file. Search text appears at the beginning of lines.") {
    char *test_name = "simple_search08";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-r During -s In %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, simple_search09, .description="Perform a simple replacement over entire file. Input text contains a single word.") {
    char *test_name = "simple_search09";
    prep_files("oneword.txt", test_name);    
    sprintf(args, "-s Homework -r \"This homework is effortless.\" %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, simple_search10, .description="Perform a simple replacement over entire file. Search term doesn't appear in the input file.") {
    char *test_name = "simple_search10";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s CSE -r \"Computer Science\" -l 1,45 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search01, .description="Perform a wildcard prefix replacement over some lines. Terms have different lengths.") {
    char *test_name = "wildcard_search01";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s re* -w -r \"New York\" %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search02, .description="Perform a wildcard prefix replacement over some lines. Terms have different lengths.") {
    char *test_name = "wildcard_search02";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s pro* -r TURING -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search03, .description="Perform a wildcard prefix replacement in a single line. Start line and end line are the same.") {
    char *test_name = "wildcard_search03";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s mac* -r device -w -l 23,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search04, .description="Perform a wildcard prefix replacement within the range. Search text appears outside the range as well.") {
    char *test_name = "wildcard_search04";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s ea* -w -r \"WELL TIMED\" -l 11,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search05, .description="Perform a wildcard prefix replacement within the specified range. Search term is followed by a punctuation mark.") {
    char *test_name = "wildcard_search05";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-l 1,22 -s intel* -w -r wisdom %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}


Test(base_output, wildcard_search06, .description="Perform a a wildcard prefix replacement over entire file. Search terms are followed by different characters.") {
    char *test_name = "wildcard_search06";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s Manche* -r MANCHESTER -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search07, .description="Perform a a wildcard prefix replacement over entire file. Multiple occurences of the search text (at the beginning, in the middle, at the end).") {
    char *test_name = "wildcard_search07";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-w -s Tur* -r \"TURING THE GREAT\" %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search08, .description="Perform a wildcard prefix replacement over entire file. Input text contains a single word.") {
    char *test_name = "wildcard_search08";
    prep_files("oneword.txt", test_name);    
    sprintf(args, "-s Hom* -r \"This homework is effortless.\" -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search09, .description="Perform a wildcard prefix replacement over entire file. Search term doesn't appear in the input file.") {
    char *test_name = "wildcard_search09";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s CSE* -r \"Computer Science\" -w -l 1,45 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search10, .description="Perform a wildcard prefix replacement over entire file. No space after punctuation marks.") {
    char *test_name = "wildcard_search10";
    prep_files("punctuation.txt", test_name);    
    sprintf(args, "-w -s Bu* -r \" However,\" %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search11, .description="Perform a wildcard prefix replacement in some lines. Search term is not a prefix.") {
    char *test_name = "wildcard_search11";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s pal* -w -r \"all\" -l 30,44 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search12, .description="Perform a wildcard prefix replacement over entire file. Search text is a word itself.") {
    char *test_name = "wildcard_search12";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s with* -r WITH -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search13, .description="Perform a wildcard suffix replacement over entire file. Terms have the same length.") {
    char *test_name = "wildcard_search13";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s *re -w -r \"New York\" %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search14, .description="Perform a wildcard suffix replacement over entire file. Terms have different lengths.") {
    char *test_name = "wildcard_search14";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-w -s *ring -r TURING %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search15, .description="Perform a wildcard suffix replacement in a single line. Start line and end line are the same.") {
    char *test_name = "wildcard_search15";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s *chine -w -r device -l 23,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search16, .description="Perform a wildcard suffix replacement within the range. Search text appears outside the range as well.") {
    char *test_name = "wildcard_search16";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s *rly -w -r \"WELL TIMED\" -l 11,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search17, .description="Perform a wildcard suffix replacement within the specified range. Search term is followed by a punctuation mark.") {
    char *test_name = "wildcard_search17";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-l 1,22 -s *gence -r wisdom -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search18, .description="Perform a a wildcard suffix replacement over entire file. Search terms are followed by different characters.") {
    char *test_name = "wildcard_search18";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s *ster -r MANCHESTER -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search19, .description="Perform a wildcard suffix replacement over entire file. Input text contains a single word.") {
    char *test_name = "wildcard_search19";
    prep_files("oneword.txt", test_name);    
    sprintf(args, "-w -s *ork -r \"This homework is effortless.\" %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search20, .description="Perform a wildcard suffix replacement over entire file. Search term doesn't appear in the input file.") {
    char *test_name = "wildcard_search20";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s *CSE -r \"Computer Science\" -w -l 1,45 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search21, .description="Perform a wildcard suffix replacement over entire file. No space after punctuation marks.") {
    char *test_name = "wildcard_search21";
    prep_files("punctuation.txt", test_name);    
    sprintf(args, "-s *So -r \" Therefore,\" -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search22, .description="Perform a wildcard suffix replacement over input file. Search term is not always a suffix.") {
    char *test_name = "wildcard_search22";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s *her -w -r HER %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search23, .description="Perform a wildcard suffix replacement over entire file. Search text is a word itself.") {
    char *test_name = "wildcard_search23";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s *with -w -r WITH %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

Test(base_output, wildcard_search24, .description="Perform a wildcard suffix replacement over some lines. Search term appears at EOF") {
    char *test_name = "wildcard_search24";
    prep_files("punctuation.txt", test_name);    
    sprintf(args, "-s *ode -w -r \"CODE.\" %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    run_using_system_no_valgrind(test_name, args);
    expect_outfile_matches(test_name);
}

/* Check memory usage. */
Test(base_valgrind, simple_search01) {
    char *test_name = "simple_search01";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s the -r WOLFIE %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, simple_search02) {
    char *test_name = "simple_search02";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s machine -r device -l 23,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));}

Test(base_valgrind, simple_search03) {
    char *test_name = "simple_search03";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s early -r \"WELL TIMED\" -l 11,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));}

Test(base_valgrind, simple_search04) {
    char *test_name = "simple_search04";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-r 1st -l 1,20 -s first %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));}

Test(base_valgrind, simple_search05) {
    char *test_name = "simple_search05";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-l 1,22 -s intelligence -r wisdom %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, simple_search06) {
    char *test_name = "simple_search06";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s Turing -r \"TURING THE GREAT\" %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, simple_search07) {
    char *test_name = "simple_search07";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s Manchester -r MANCHESTER %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, simple_search08) {
    char *test_name = "simple_search08";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-r During -s In %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, simple_search09) {
    char *test_name = "simple_search09";
    prep_files("oneword.txt", test_name);    
    sprintf(args, "-s Homework -r \"This homework is effortless.\" %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, simple_search10) {
    char *test_name = "simple_search10";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s CSE -r \"Computer Science\" -l 1,45 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search01) {
    char *test_name = "wildcard_search01";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s re* -w -r \"New York\" %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search02) {
    char *test_name = "wildcard_search02";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s pro* -r TURING -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search03) {
    char *test_name = "wildcard_search03";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s mac* -r device -w -l 23,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search04) {
    char *test_name = "wildcard_search04";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s ea* -w -r \"WELL TIMED\" -l 11,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search05) {
    char *test_name = "wildcard_search05";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-l 1,22 -s intel* -w -r wisdom %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}


Test(base_valgrind, wildcard_search06) {
    char *test_name = "wildcard_search06";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s Manche* -r MANCHESTER -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search07) {
    char *test_name = "wildcard_search07";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-w -s Tur* -r \"TURING THE GREAT\" %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search08) {
    char *test_name = "wildcard_search08";
    prep_files("oneword.txt", test_name);    
    sprintf(args, "-s Hom* -r \"This homework is effortless.\" -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search09) {
    char *test_name = "wildcard_search09";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s CSE* -r \"Computer Science\" -w -l 1,45 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search10) {
    char *test_name = "wildcard_search10";
    prep_files("punctuation.txt", test_name);    
    sprintf(args, "-w -s Bu* -r \" However,\" %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search11) {
    char *test_name = "wildcard_search11";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s pal* -w -r \"all\" -l 30,44 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search12) {
    char *test_name = "wildcard_search12";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s with* -r WITH -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search13) {
    char *test_name = "wildcard_search13";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s *re -w -r \"New York\" %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search14) {
    char *test_name = "wildcard_search14";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-w -s *ring -r TURING %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search15) {
    char *test_name = "wildcard_search15";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s *chine -w -r device -l 23,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search16) {
    char *test_name = "wildcard_search16";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s *rly -w -r \"WELL TIMED\" -l 11,23 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search17) {
    char *test_name = "wildcard_search17";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-l 1,22 -s *gence -r wisdom -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search18) {
    char *test_name = "wildcard_search18";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s *ster -r MANCHESTER -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search19) {
    char *test_name = "wildcard_search19";
    prep_files("oneword.txt", test_name);    
    sprintf(args, "-w -s *ork -r \"This homework is effortless.\" %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search20) {
    char *test_name = "wildcard_search20";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s *CSE -r \"Computer Science\" -w -l 1,45 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search21) {
    char *test_name = "wildcard_search21";
    prep_files("punctuation.txt", test_name);    
    sprintf(args, "-s *So -r \" Therefore,\" -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search22) {
    char *test_name = "wildcard_search22";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s *her -w -r HER %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search23) {
    char *test_name = "wildcard_search23";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s *with -w -r WITH %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

Test(base_valgrind, wildcard_search24) {
    char *test_name = "wildcard_search24";
    prep_files("punctuation.txt", test_name);    
    sprintf(args, "-s *ode -w -r \"CODE.\" %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    expect_no_valgrind_errors(run_using_system(test_name, args));
}

/* Tests for invalid arguments. */
Test(base_invalid_args, args_missing01, .description="Argument missing.") {
    char *test_name = "args_missing01";
    sprintf(args, "-s -r end %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, MISSING_ARGUMENT);
}

Test(base_invalid_args, duplicate_argument01, .description="Duplicate argument.") {
    char *test_name = "duplicate_argument01";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-r the -r WOLFIE -s -l 13,4 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, DUPLICATE_ARGUMENT);
}

Test(base_invalid_args, input_file_missing, .description="Input file is missing.") {
    char *test_name = "input_file_missing";
    sprintf(args, "-s the -r end /%s.in.txt %s/%s.out.txt", test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, INPUT_FILE_MISSING);
}

Test(base_invalid_args, output_file_unwritable01, .description="Output file is unwritable.") {
    char *test_name = "output_file_unwritable01"; 
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s the -r WOLFIE %s/%s.in.txt /%s.out.txt", TEST_INPUT_DIR, test_name, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, OUTPUT_FILE_UNWRITABLE);
}

Test(base_invalid_args, output_file_unwritable02, .description="Output file is unwritable.") {
    char *test_name = "output_file_unwritable02";
    prep_files("unix.txt", test_name);  
    char cmd[200];
    sprintf(cmd, "touch %s/%s.out.txt; chmod 400 %s/%s.out.txt", TEST_OUTPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    system(cmd);
    sprintf(args, "-s the -r end %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, OUTPUT_FILE_UNWRITABLE);
}

Test(base_invalid_args, s_argument_missing01, .description="-s option is missing.") {
    char *test_name = "s_argument_missing01";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-l 1,10 -w -r WOLFIE %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, S_ARGUMENT_MISSING);
}

Test(base_invalid_args, s_argument_missing02, .description="-s option is missing.") {
    char *test_name = "s_argument_missing02";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-l 1,10 -w random garbage hello %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, S_ARGUMENT_MISSING);
}

Test(base_invalid_args, s_argument_missing03, .description="-s argument is missing") {
    char *test_name = "s_argument_missing03";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-r the -g WOLFIE -s -l 13,4 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, S_ARGUMENT_MISSING);
}

Test(base_invalid_args, r_argument_missing01, .description="-r option is missing.") {
    char *test_name = "r_argument_missing01";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-l 1,10 -s WOLFIE %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, R_ARGUMENT_MISSING);
}

Test(base_invalid_args, r_argument_missing02, .description="-r option is missing.") {
    char *test_name = "r_argument_missing02";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s hello -l 1,10 -w random garbage hello %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, R_ARGUMENT_MISSING);
}

Test(base_invalid_args, r_argument_missing03, .description="-r argument is missing") {
    char *test_name = "r_argument_missing03";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s the -r -g WOLFIE -l 13,4 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, R_ARGUMENT_MISSING);
}

Test(base_invalid_args, l_argument_invalid01, .description="l argument is invalid. Negative line number.") {
    char *test_name = "l_argument_invalid01";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s bar -r WOLFIE -l -1,-5 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, L_ARGUMENT_INVALID);
}

Test(base_invalid_args, l_argument_invalid02, .description="l argument is invalid. End line less than start line.") {
    char *test_name = "l_argument_invalid02";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s *foo -r WOLFIE -l 20,2 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, L_ARGUMENT_INVALID);
}

Test(base_invalid_args, l_argument_invalid03, .description="l argument is invalid. End line missing.") {
    char *test_name = "l_argument_invalid03";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s sbu -r WOLFIE -l 20, %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, L_ARGUMENT_INVALID);
}

Test(base_invalid_args, l_argument_invalid04, .description="l argument is invalid. Comma missing.") {
    char *test_name = "l_argument_invalid04";
    prep_files("turing.txt", test_name);    
    sprintf(args, "-s the -r NET -l 20 2 %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, L_ARGUMENT_INVALID);
}

Test(base_invalid_args, wildcard_invalid01, .description="Search text is not properly formatted.") {
    char *test_name = "wildcard_invalid01";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s *hello* -r HELLO -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, WILDCARD_INVALID);
}

Test(base_invalid_args, wildcard_invalid02, .description="Search text is not properly formatted. No asterisks.") {
    char *test_name = "wildcard_invalid02";
    prep_files("unix.txt", test_name);    
    sprintf(args, "-s machine -r BOT -w %s/%s.in.txt %s/%s.out.txt", TEST_INPUT_DIR, test_name, TEST_OUTPUT_DIR, test_name);
    int status = run_using_system_no_valgrind(test_name, args);
    expect_error_exit(status, WILDCARD_INVALID);
}
