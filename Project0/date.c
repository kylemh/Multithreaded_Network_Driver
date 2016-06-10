/*
Author: Kyle Holmberg
Duck ID: kmh
SIN: 951 312 729
Project 0 for CIS 415 - Operating Systems
date.c

This is my own work.
 */

#include "date.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <stdbool.h>
#include <errno.h>

struct date {
	int day;
	int month;
	int year;
};

// date_create creates a Date structure from `datestr` which is expected to be of the form "dd/mm/yyyy"
// returns pointer to Date structure if successful, NULL if not (syntax error)
Date *date_create(char *datestr) {
    if (datestr == NULL || strlen(datestr) != 10) {
		return NULL; //syntax error covered
	}

	//variables used to assign struct vals
    int xDay = 0;
    int xMonth = 0;
    int xYear = 0;

    char *strtokPtr;
    strtokPtr = strtok(datestr,"/"); /* token for dd value */
    xDay = atoi(strtokPtr);
    strtokPtr = strtok(NULL, "/");  /* token for mm value */
    xMonth = atoi(strtokPtr);
    strtokPtr = strtok(NULL, "/");  /* token for yy value */
    xYear= atoi(strtokPtr);

    if (xYear < 1950 || xYear > 2500) {
    	//User Error: Year value is out of bounds!
    	return NULL;
    }

    //Check to see if year is a leap year.
    //Use boolean to find maximum amount of days allowed for February
    bool isLeapYear;
    if (xYear % 4 == 0 && xYear % 100 != 0) {
    	isLeapYear = true;
    } else if (xYear % 400 == 0) {
    	isLeapYear = true;
    } else {
    	isLeapYear = false;
    }

    int maxAllowedDays;
  	switch(xMonth){
  		//If 'knuckle' month (has 31 days)
  		case 4:
  		case 6:
  		case 9:
  		case 11:
  			maxAllowedDays = 30;
  			break;

  		//If february
  		case 2:
  			if (isLeapYear == true) {
  				maxAllowedDays = 29;
  			} else {
  				maxAllowedDays = 28;
  			}
  			break;

  		//If 'non-knuckle' month (has 30 days)
  		case 1:
  		case 3:
  		case 5:
  		case 7:
  		case 8:
  		case 10:
  		case 12:
  			maxAllowedDays = 31;
  			break;

  		//User Error: Month value is out of bounds!
  		default:
  			printf("\nERROR: Date was not created because it does not contain a valid month (01, 02, ..., 12)\n");
  			printf("Date not entered: %d/%d/%d\n", xDay, xMonth, xYear);
  			return NULL;
  	}

    if (xDay < 1 || xDay > maxAllowedDays) {
    	//User Error: Day value is out of bounds!
    	printf("\nERROR: Date was not created because it has more days than that date allows!\n");
  		printf("Date not entered: %d/%d/%d\n", xDay, xMonth, xYear);
  		if (isLeapYear == true) {
  			printf("Keep in mind, %d is a leap year!\n", xYear);
  		} else {
  			printf("Keep in mind, %d is NOT a leap year!\n", xYear);
  		}
    	return NULL;
    }

    /* Create Instance of Date Struct */
    Date* dateInstance = (Date*)malloc(sizeof(Date));
    dateInstance->day = xDay;
    dateInstance->month = xMonth;
    dateInstance->year = xYear;
    return dateInstance;
}

// date_duplicate creates a duplicate of `d' returns pointer to new Date structure if successful, NULL if not (mem alloc failure)
Date *date_duplicate(Date *d) {
	if (d == NULL) {
		return NULL;
	}

	Date* dupe = (Date*)malloc(sizeof(Date));
	if (sizeof(dupe) != sizeof(Date)) {
		return NULL;
	}

	*dupe = *d;
	return dupe;
}

// date_compare compares two dates, returning <0, 0, >0 if date1<date2, date1==date2, date1>date2, respectively
int date_compare(Date *date1, Date *date2) {
	// Compare Year
	if (date1->year < date2->year) {
		return -1;
	} else if (date1->year > date2->year) {
		return 1;
	} else { // years are equal
		// Compare Month
		if (date1->month < date2->month) {
			return -2;
		} else if (date1->month > date2->month) {
			return 2;
		} else { // months are equal
			// Compare Day
			if (date1->day < date2->day) {
				return -3;
			} else if (date1->day > date2->day) {
				return 3;
			} else { // begindate == enddate
				return 0;
			}
		}
	}
}

// date_destroy returns any storage associated with `d' to the system
void date_destroy(Date *d) {
	free(d);
}