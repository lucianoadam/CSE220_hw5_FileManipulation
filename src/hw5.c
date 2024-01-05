/*
 * Name: Luciano Adam
 * ID: 115157187
 * Class: CSE 220.01
 * Prof.: Abid M. Malik
 * Recitation: R03
 * Assignment: HW4
 * Due Date: 11/3/2023 23:59 EST
 */


#include "../include/hw5.h"

#define MIN_ARGUMENTS 7 // Bare minimum number of arguments needed for call to be valid
#define BASE_10 10      // Base of numbers in -l
#define DELIM ","       // Delimiter used when parsing arguments in -l


void replaceText(char s[MAX_SEARCH_LEN], char r[MAX_SEARCH_LEN], long range[2], char w, FILE *fin, FILE *fout) {
    char line[MAX_LINE] = {'\0'};    // Buffer to store each line from the input file
    char buffer[MAX_LINE] = {'\0'};  // Buffer to store modified lines before writing to output file
    long current_line = 1;           // Variable to keep track of the current line number

    while (fgets(line, MAX_LINE, fin) != NULL) {
        if (current_line >= range[0] && current_line <= range[1]){
            char *ptr = line, *buf_ptr = buffer;        // Pointers to traverse the line and buffer
            size_t slen = strlen(s), rlen = strlen(r);  // Lengths of search and replace strings
            while (*ptr != '\0') {
                if (w == 0) {
                    // Wildcard is not used, perform regular search and replace
                    if (strncmp(ptr, s, slen) == 0) {
                        strcpy(buf_ptr, r);      // Replace the string
                        buf_ptr += rlen;         // Move buffer pointer past replaced string
                        ptr += slen;             // Move line pointer past searched string
                    } else *buf_ptr++ = *ptr++;  // Copy character to buffer
                }
                else {
                    // Wildcard is used, handle special cases
                    while (*ptr != '\0' && !isalnum(*ptr)) *buf_ptr++ = *ptr++;  // Check for non-alphanumeric character
                    char *word_start = ptr;
                    while (*ptr != '\0' && isalnum(*ptr)) ptr++;  // Find the end of the word
                    char *word_end = ptr;
                    size_t wlen = word_end - word_start;          // Length calculations
                    if (wlen >= slen && ((w == 1 && strncmp(word_end - slen, s, slen) == 0) || (w == 2 && strncmp(word_start, s, slen) == 0))) {
                        // The action of replacing is the same regardless of wildcard status
                        strcpy(buf_ptr, r);  // Replace the word
                        buf_ptr += rlen;     // Move buffer pointer past replaced word
                    }
                    else {
                        strcpy(buf_ptr, word_start); // Copy non-matching word to buffer
                        buf_ptr += wlen;             // Move buffer pointer past copied word
                    }
                }
            }
            *buf_ptr = '\0';       // Null-terminate the buffer
            fputs(buffer, fout);   // Write the modified line to output
        } else fputs(line, fout);  // Write the original line to output
        current_line++;            // Move to the next line
    }

    fclose(fin);   // Close input file
    fclose(fout);  // Close output file
}


int main(int argc, char *argv[]) {
    // Making sure it has at least the minimum amout of arguments
    if (argc < MIN_ARGUMENTS) return MISSING_ARGUMENT;

    // Opening and checking files
    FILE *fin = fopen(argv[argc - 2], "r"), *fout = fopen(argv[argc - 1], "w");
    if (fin == NULL) return INPUT_FILE_MISSING;
    if (fout == NULL) return OUTPUT_FILE_UNWRITABLE;

    // Defining variables for checking the existence of certain options
    char sflag = 0, rflag = 0, lflag = 0, wflag = 0;

    // Defining variables that store arguments and lines in the file
    char sopt[MAX_SEARCH_LEN] = {'\0'}, ropt[MAX_SEARCH_LEN] = {'\0'};
    long lrange[2];

    // Loop for parsing options
    int opt;
    while(((opt = getopt(argc, argv, "s:r:l:w")) != -1)){
        switch (opt){
            case 's':
                if (sflag) return DUPLICATE_ARGUMENT;
                sflag = 1;
                if (optarg == NULL || optarg[0] == '-') return S_ARGUMENT_MISSING;
                strncpy(sopt, optarg, MAX_SEARCH_LEN);
                break;
            case 'r':
                if (rflag) return DUPLICATE_ARGUMENT;
                rflag = 1;
                if (optarg == NULL || optarg[0] == '-') return R_ARGUMENT_MISSING;
                strncpy(ropt, optarg, MAX_SEARCH_LEN);
                break;
            case 'l':
                if (lflag) return DUPLICATE_ARGUMENT;
                lflag = 1;
                if (optarg == NULL || optarg[0] == '-') return L_ARGUMENT_INVALID;
                if (strchr(optarg, ',') == NULL) return L_ARGUMENT_INVALID;
                char *firstNum = strtok(optarg, DELIM);
                char *secondNum = strtok(NULL, DELIM);
                if (firstNum == NULL || secondNum == NULL) return L_ARGUMENT_INVALID;
                lrange[0] = strtol(firstNum, NULL, BASE_10);
                lrange[1] = strtol(secondNum, NULL, BASE_10);
                if (lrange[0] > lrange[1] || lrange[0] == 0 || lrange[1] == 0)
                    return L_ARGUMENT_INVALID; // Check if the range is valid
                break;
            case 'w':
                if (wflag) return DUPLICATE_ARGUMENT;
                wflag = 1;
                break;
        }
    }

    // Checking if necessary options are provided
    if (!sflag) return S_ARGUMENT_MISSING;
    if (!rflag) return R_ARGUMENT_MISSING;
    if (!lflag) lrange[0] = 1, lrange[1] = __LONG_MAX__;
    if (wflag){
        const int A = sopt[0] == '*', B = sopt[strlen(sopt) - 1] == '*';
        if (A == B) return WILDCARD_INVALID;
        if (B){
            wflag = 2; // Repurposed wflag as a ternary switch
            sopt[strlen(sopt) - 1] = '\0'; // Removing star
        }
        else memmove(sopt, sopt + 1, strlen(sopt)); // Removing star
    }

    // Call the replaceText function with the provided options and files
    replaceText(sopt, ropt, lrange, wflag, fin, fout);

    return 0;
}
