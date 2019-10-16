#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>
#include "commandShell.h"

int main(int targ, char const *targv[]) {
  /* code */
  char *tmpCmd;
  int argc;
  char **argv;
  char tmpChar;
  char lineBuff[257];
  size_t lineIdx = 0;
  fprintf( stdout, "Charles Dorner's Command Shell\n\n" );


  do {
    //input a command line
    fprintf ( stdout, "$ " );
    lineIdx = 0;
    do {
      tmpChar = fgetc(stdin);
      lineBuff[lineIdx] = tmpChar;
      lineIdx++;
    } while (tmpChar != '\n' && lineIdx < 256);
    lineBuff[lineIdx] = 0;

    //parse that line into white space delimited fields.
    argv = parse(lineBuff, &argc);

    //call the function to execute the command
    if (argc < 1 ) {
      continue;
    }

    for (int cmdIdx = 0; cmdIdx < 5; cmdIdx++) {
      tmpCmd = commands[cmdIdx].name;
      if (compareStrings( argv[0], tmpCmd, stringSize(tmpCmd)) == 0) {
        commands[cmdIdx].functionp(argc, argv);
      }
    }

    //free storage
    if (argv != (char**)NULLPTR){
      for ( int idx = 0; idx < argc; idx++) {
        if (argv[idx] != (char*) NULLPTR) {
          free(argv[idx]);
          argv[idx] = (char*) NULLPTR;
        }
      }
      free(argv);
      argv = (char**) NULLPTR;
    }
  } while(lineBuff[0] != '@');

  return 0;
}

char **parse(char *pBuff, int *pArgc) {
  int argc = 0;
  char **argv;
  int buffLen = stringSize(pBuff);
  int tokenLen = 0;
  // count the tokens in the buffer
  for ( int buffIdx = 0; buffIdx < buffLen; buffIdx++){
    if ( pBuff[buffIdx] == SPACE
       || pBuff[buffIdx] == TAB
       || pBuff[buffIdx] == NEWLINE ) {
      if ( tokenLen > 0 ) {
        argc++;
        tokenLen = 0;
      }
    } else {
      tokenLen++;
    }
  }
  if ( tokenLen > 0 ) {
    argc++;
  }

//  fprintf(stdout, "Number of Tokens = %d\n", argc);
  //allocate pArgv
  size_t argvSize = (argc+1) * sizeof(char*);
  argv = malloc(argvSize);
  argv[argc] = (char*)NULLPTR;

  //copy tokens into argv
  char * tmpPtr;
  int argvIdx = 0;
  int buffIdx = 0;
  if (argc > 0 ) {
    for ( buffIdx = 0; buffIdx < buffLen; buffIdx++){
      if ( pBuff[buffIdx] == SPACE || pBuff[buffIdx] == TAB ) {
        if ( tokenLen > 0 ) {
          tmpPtr = malloc(tokenLen+1);
          copyString(pBuff+buffIdx-tokenLen, tmpPtr, tokenLen);
        //  printf("Next token 1[%s]\n", tmpPtr);
          argv[argvIdx] = tmpPtr;
      //    printf("Next token 2[%s]\n", argv[argvIdx]);
          argvIdx++;
          tokenLen = 0;
        }
      } else {
        tokenLen++;
      }
    }

    if ( tokenLen > 0 ) {
      tmpPtr = malloc(tokenLen+1);
      copyString(pBuff+buffIdx-tokenLen, tmpPtr, tokenLen);
    //  printf("Last token 1[%s]\n", tmpPtr);
      argv[argvIdx] = tmpPtr;
    //  printf("Last token 2[%s]\n", argv[argvIdx]);
      argc++;
    }
  }
  *pArgc = argc;
  return argv;
}

//return 0 if strings are equal, - number if first string is less than second,
//+ is first string is > than second

int compareStrings(char* firstString, char* secondString, int maxCount){
  int returnValue = 0;
  for (int idx = 0; idx < maxCount; idx++){
    if ( firstString[idx] == '\0' || secondString[idx] == '\0' ) {
      break;
    }
    if (firstString[idx] == secondString[idx]){
      continue;
    } else if ( firstString[idx] < secondString[idx] ) {
      returnValue = -1;
      break;
    } else {
      returnValue = +1;
      break;
    }
  }
  return returnValue;
}

//copy at most size count characters from source to destination followed by
//trailing null
int copyString(char* pSource, char* pDestination, size_t pCount) {
  int idx = 0;
//  printf("Source = %s, size = %ld\n", pSource, pCount);
//  printf("Copy String:[");
  while (idx < pCount) {
    if (pSource[idx] == '\0' || pSource[idx] == NEWLINE) {
      break;
    }
    pDestination[idx] = pSource[idx];
  // printf("%c", pSource[idx]);
    idx++;
  }
  pDestination[idx] = '\0';
//  printf("]\n");
}

int stringSize(char* theString) {
  int length = 0;
  while (theString[length] != '\0')
    length++;
  return length;
}

int longToString(long pValue, char* pBuff, size_t pLength) {
  char tmpChar;
  int returnValue = 0;
  int buffIdx = pLength;
  pBuff[buffIdx] = '\0';
  buffIdx--;
  long tmpLong = pValue, tmpDigit;
  while(tmpLong > 0) {
    if (buffIdx < 0) {
      fprintf(stderr, "Value Too Large %ld \n", pValue);
      return -1;
    }
    tmpDigit = tmpLong % 10;
    pBuff[buffIdx] = '0' + tmpDigit;
    tmpLong = tmpLong / 10;
    buffIdx--;
  }
  while(buffIdx >=0) {
    pBuff[buffIdx] = '0';
    buffIdx--;
  }
  return returnValue;
}

long stringToLong(char* pStr) {
  long returnValue = 0;
  int digitValue;
  size_t strSize = stringSize(pStr);
  for (int idx = 0; idx < strSize; idx++) {
    digitValue = pStr[idx] - '0';
    if (digitValue >= 0 && digitValue <= 9) {
      returnValue = 10 * returnValue + digitValue;
    }
    else {
      fprintf(stderr, "Non digit character encountered: %c\n", pStr[idx]);
    }
  }
  return returnValue;
}

int cmd_date(int argc, char *argv[]) {
  struct timeval* sysTimePtr = malloc(sizeof (struct timeval));
  int rVal = gettimeofday(sysTimePtr, (struct timezone*) NULLPTR);
  if (rVal != 0) {
    fprintf(stderr, "Unable to get time of day");
    return -1;
  }
  char dateTimeStr[36];
  long secondsSinceEpoch = sysTimePtr -> tv_sec;
  long milliseconds = sysTimePtr -> tv_usec;
  clockDateToString(secondsSinceEpoch, milliseconds, dateTimeStr);
  fprintf(stdout, "%s\n", dateTimeStr);
  return 0;
}

int cmd_echo(int argc, char *argv[]) {
  fprintf(stdout, "Echo: ");
  for(int ii=1; ii<argc; ++ii)
	{
    if (argv[ii] != NULLPTR) {
		    fprintf(stdout, "%s ",argv[ii]);
    }
	}
  fprintf(stdout, "\n");
  return 0;
}

int cmd_exit(int argc, char *argv[]) {
  fprintf(stdout, "Exiting program...\n");
  exit(0);// Program will exit with return code zero.
}

int cmd_help(int argc, char *argv[]){
  fprintf(stdout, "Commands:\n");
  fprintf(stdout, "date, d : date will output current date with time.\n");
  fprintf(stdout, "echo, e : echo will output each of the arguments in argv.\n");
  fprintf(stdout, "clockdate, c : clockdate will convert the epoch seconds to readable date.\n");
  fprintf(stdout, "exit : exit will exit from the shell.\n");
  return 0;
}

int clockDateToString(long pSecondsSinceEpoch, long pMilliseconds, char* pTarget) {
  int returnValue=0;
  long minutesSinceEpoch = pSecondsSinceEpoch / 60;
  long seconds = pSecondsSinceEpoch % 60;
  long hoursSinceEpoch = minutesSinceEpoch / 60;
  long minutes = hoursSinceEpoch % 60;
  long daysSinceEpoch = hoursSinceEpoch / 24;
  long hours = daysSinceEpoch % 24;
  epochDaysToYMDString(daysSinceEpoch, pTarget);
  int tmpOffset = stringSize(pTarget);
  char* basePtr = pTarget + tmpOffset;
  *basePtr = ' ';
  basePtr++;
  longToString(hours, basePtr, 2);
  basePtr+=2;
  *basePtr = ':';
  basePtr++;
  longToString(minutes, basePtr, 2);
  basePtr+=2;
  *basePtr = ':';
  basePtr++;
  longToString(seconds, basePtr, 2);
  basePtr+=2;
  *basePtr = '.';
  basePtr++;
  longToString(pMilliseconds, basePtr, 6);
  return returnValue;
}

int epochDaysToYMDString(long pDaysSinceEpoch, char* pTarget) {
  int returnValue;
  long daysSoFar = 0, yearDay = 0;
  int year, month, day;
  //determine the year
  year = 1970;
  while ((daysSoFar + daysInYear(year)) < pDaysSinceEpoch) {
    daysSoFar += daysInYear(year);
    year++;
  }

  //determine the month
  yearDay = pDaysSinceEpoch - daysSoFar;
  month = 0;
  daysSoFar = 0;
  while (daysSoFar + daysInMonth(year, month) < yearDay) {
    if (month > 11) {
      break;
    }
    daysSoFar += daysInMonth(year, month);
    month++;
  }

  //determine the day
  day = yearDay - daysSoFar;

  //populate the target
  char* basePtr = pTarget;
  int tmpOffset = stringSize(months[month].name);
  copyString(months[month].name, basePtr, tmpOffset);
  basePtr+= tmpOffset;
  *basePtr = ' ';
  basePtr++;
  longToString(day+1, basePtr, 2);
  basePtr+=2;
  *basePtr = ',';
  basePtr++;
  *basePtr = ' ';
  basePtr++;
  longToString(year, basePtr, 4);
  basePtr+=4;
  *basePtr = '\0';
  return returnValue;
}

int daysInYear(int pYear) {
  if ((pYear % 4) == 0) {
    return 366;
  }
  return 365;
}

int daysInMonth(int pYear, int pMonth) {
  if (pMonth < 0 || pMonth > 11) {
    fprintf(stderr, "Month out of range%d\n", pMonth);
    return -1;
  }
  int tmpDays = months[pMonth].days;
  if (months[pMonth].leapYear && (pYear % 4) == 0) {
    tmpDays++;
  }
  return tmpDays;
}

int cmd_clockdate(int argc, char *argv[]){
  char dateTimeStr[36];
  fprintf(stdout, "clockdate: ");
  if (argc < 3) {
    fprintf(stderr, "Argument Count too Short\n");
    return 1; //this should be defined error code
  }
  long secondsSinceEpoch = stringToLong(argv[1]);
  long milliseconds = 0;
  clockDateToString(secondsSinceEpoch, milliseconds, dateTimeStr);
  fprintf(stdout, "%s\n", dateTimeStr);
  return 0;
}
