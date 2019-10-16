#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>

const char *NULLPTR = 0;
const char SPACE = ' ';
const char TAB = '\t';
const char NEWLINE = '\n';

int cmd_date(int argc, char *argv[]);
int cmd_echo(int argc, char *argv[]);
int cmd_exit(int argc, char *argv[]);
int cmd_help(int argc, char *argv[]);
int cmd_clockdate(int argc, char *argv[]);
char **parse(char *pBuff, int *pArgc);
int compareStrings (char* firstString, char* secondString, int maxCount);
int stringSize (char* theString);
int copyString(char* pSource, char* pDestination, size_t pCount);
int longToString(long pValue, char* pBuff, size_t pLength);
int epochDaysToYMDString(long pDaysSinceEpoch, char* pTarget);
int clockDateToString(long pSecondsSinceEpoch, long pMilliseconds, char* pTarget);
int daysInYear(int pYear);
int daysInMonth(int pYear, int pMonth);

struct commandEntry {
  char *name;
  int (*functionp)(int argc, char *argv[]);
} commands[] = {{"date", cmd_date},
                {"echo", cmd_echo},
                {"exit", cmd_exit},
                {"help", cmd_help},
                {"clockdate", cmd_clockdate}};

struct monthData {
  char* name;
  int days;
  bool leapYear;
} months[] = {{"January", 31, false},
              {"February", 28, true},
              {"March", 31, false},
              {"April", 30, false},
              {"May", 31, false},
              {"June", 30, false},
              {"July", 31, false},
              {"August", 31, false},
              {"September", 30, false},
              {"October", 31, false},
              {"November", 30, false},
              {"December", 31, false}};
