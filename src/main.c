#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sendhttps.h"
#include "jsmn.h"

#define NUM_OF_HEADERS 3
#define CAL_ID_LENGTH 152

/*return today in YYYY-MM-DD format, needs freeing*/
char *getToday(){
	char *date;
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	date = (char *) malloc(strlen("YYYY-MM-DD") + 1 * sizeof(char));
	sprintf(date, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	
	return date;
}

/*return tomorrow in YYYY-MM-DD format, needs freeing*/
char *getTomorrow(){
	char *date;
	time_t t = time(NULL);
	struct tm tm;

	t += 24*60*60;
	tm = *localtime(&t);
	date = (char *) malloc(strlen("YYYY-MM-DD") + 1 * sizeof(char));
	sprintf(date, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	
	return date;
}

/*return yesturday in YYYY-MM-DD format, needs freeing*/
char *getYesturday(){
	char *date;
	time_t t = time(NULL);
	struct tm tm;

	t -= 24*60*60;
	tm = *localtime(&t);
	date = (char *) malloc(strlen("YYYY-MM-DD") + 1 * sizeof(char));
	sprintf(date, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	
	return date;
}

/*return provided date in YYYY-MM-DD format, needs freeing*/
char *getDate(char *argvv){
	char *date;
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	/*light validation using ASCII tables to make sure input is numerical i.e DD/MM*/
	if(
	  argvv[1] < 48   || argvv[1] > 57 ||\
	  argvv[2] != '/' ||\
	  argvv[3] < 48   || argvv[3] > 49 ||\
	  argvv[4] < 48   || argvv[4] > 57  ){
		printf("Argument for date is garbled. Should formatted like './consolecal DD/MM'\n");
		exit(EXIT_FAILURE);
	}

	date = (char *) malloc(strlen("YYYY-MM-DD") + 1 * sizeof(char));
	sprintf(date, "%04d-%c%c-%c%c", tm.tm_year + 1900, argvv[3], argvv[4], argvv[0], argvv[1]);
	
	return date;
}

/*print the day of the week from ascii.txt.*/
void printAsciiArt(int dayOfWeek){
	char *path = "./src/ascii.txt", c;
	FILE *fp;

	/*validation and file opening*/
	if(dayOfWeek < 0 || dayOfWeek > 6){
		fprintf(stderr,"err ln%d: Day of the week does not exist!\n", __LINE__);
		exit(EXIT_FAILURE);
	}
	if(!(fp = fopen(path,"r"))){
		fprintf(stderr, "err ln %d: Error opening file %s.\n", __LINE__, path);
		exit(EXIT_FAILURE);
	}

	dayOfWeek += 48;/*to ASCII*/

	/*find correct part of the file*/
	do{
		c = fgetc(fp);
	}while(c != dayOfWeek && c != EOF);
	fgetc(fp); c = fgetc(fp);

	/*print day of week*/
	while(c != dayOfWeek+1 && c != EOF){
		putchar(c);
		c = fgetc(fp);
	}
	fclose(fp);
}

/*structure for grouping JSON properties*/
typedef struct JsonWrapper{
		char *raw;
		int tokenTotal;
		jsmntok_t *tokenPtr; /*nested struct*/
		jsmn_parser parser;  /*nested struct*/
}JsonWrapper;

/*wrapper for jsmn_parse cause why not*/
int jsmnParseWrapper(JsonWrapper *json){
	jsmn_init(&json->parser);

	/*load number of available tokens into json.tokenTotal*/
	if(json->tokenTotal < 0){
		if((json->tokenTotal = jsmn_parse(&json->parser, json->raw, strlen(json->raw), NULL, 0)) < 0){
			fprintf(stderr, "err ln%d: Error code (%d) when parsing JSON UNKNOWN size.\n", __LINE__, json->tokenTotal);
			exit(EXIT_FAILURE);
		}
		return 0;
	}

	/*if json.tokenTotal is known, parse it properly and load the tokens into json.tokenPtr*/
	if((jsmn_parse(&json->parser, json->raw, strlen(json->raw), json->tokenPtr, json->tokenTotal)) < 0){
		fprintf(stderr, "err ln%d: Error code (%d) when parsing JSON of KNOWN size.\n", __LINE__, json->tokenTotal);
		exit(EXIT_FAILURE);
	}
	return 0;
}

/*return whether a given json token is equal to a string*/
int jsoneq(const char *entireJson, jsmntok_t token, const char *comparisonString) {
	if ((int)strlen(comparisonString) == token.end - token.start && \
		strncmp(entireJson + token.start, comparisonString, token.end - token.start) == 0) {
    return 1;
	}
	return 0;
}

/*each key has a matching value right after it. returns the value when given a key and a object to search within*/
/*returns -1 if not found*/
int nextIndexOf(JsonWrapper json, char *searchKey, int objToken){
	int i = objToken;
	int objEnd = json.tokenPtr[objToken].end;

	/*check an object type has been provided*/
	if(json.tokenPtr[objToken].type != JSMN_OBJECT)
		return -1;

	/*navigate to key*/
	while(json.tokenPtr[i].start < objEnd && !jsoneq(json.raw, json.tokenPtr[i], searchKey))
		i++;
	if(json.tokenPtr[i].start >= json.tokenPtr[objToken].end)
		return -1;

	/*print value corresponding to key*/
	return ++i;
}

/*returns token pointer matching the key*/
jsmntok_t nextTokenOf(JsonWrapper json, char *searchKey, int objToken){
	return json.tokenPtr[nextIndexOf(json, searchKey, objToken)];
}

/*given a key and an object token, print the corresponding value*/
void printNext(JsonWrapper json, char *searchKey, int objToken){
	jsmntok_t token = nextTokenOf(json, searchKey, objToken);
	printf("%.*s", token.end - token.start, json.raw + token.start);
}

/*given a key and an object token, print part of the value*/
void printNextn(JsonWrapper json, char *searchKey, int objToken, int size, int start){
	printf("%.*s", size, json.raw + nextTokenOf(json, searchKey, objToken).start + start); 
}

/*return number of calendar events from a given JSON*/
/*returns -1 if not found*/
int eventCount(JsonWrapper json){
	int i = 0;

	/*find string matching "value" and stop*/
	i = nextIndexOf(json,"value",0);

	/*the next token (after "value") should be an array of calendar events*/
	if(i != json.tokenTotal && json.tokenPtr[i].type == JSMN_ARRAY)
		return json.tokenPtr[i].size;
	return -1;
}

/*navigates to the object/array after the selected one ends, returns that index*/
/*returns -1 if not found*/
int navigateToNext(JsonWrapper json, int index){
	int end = json.tokenPtr[index].end;
	while(index < json.tokenTotal && json.tokenPtr[index].start < end)
		index++;
	if(index >= json.tokenTotal)
		return -1;
	return index;
}

/*
* Parses a given microsoft calendar,
* displays the events, descriptions and times,
* returns number of events parsed.
*/
int displayCalendar(char *url, char *headers[NUM_OF_HEADERS], char *authkey){
	/*counters*/
	int i;
	int tokenIndex;
	int subTokenIndex;
	int eventTotal = 0;

	/*for json parsing*/
	JsonWrapper *json;
	json = malloc(sizeof(JsonWrapper));
	json->raw = NULL;
	json->tokenTotal = -1;
	json->tokenPtr = NULL;

	/*send HTTP GET request and print response*/
	json->raw = httpsGET(url, NUM_OF_HEADERS, headers);
	#ifdef DEBUG
	printf("RAW_RESPONSE: %s\n", json->raw);
	#endif

	/*get number of tokens for json.tokenTotal*/
	jsmnParseWrapper(json);

	/*allocate memory for the json.tokenPtr and populate it*/
	json->tokenPtr = malloc(json->tokenTotal * sizeof(jsmntok_t));
	jsmnParseWrapper(json); /*the json.tokenPtr is populated here*/

	/*navigate j to list of calendar events*/
	if((tokenIndex = nextIndexOf(*json,"value",0)) < 0){
		fprintf(stderr, "err ln%d: Cannot find list of calendar events.\n", __LINE__);
		exit(EXIT_FAILURE);
	}
	tokenIndex++;

	/*loop through each event*/
	for(i = 0; i < eventCount(*json);i++){
		/*
		* print a calendar event
		*/

		if(jsoneq(json->raw, nextTokenOf(*json, "isAllDay", tokenIndex),"true"))
			printf("   All Day");
		else{
			/*print the HH:MM part of the starting dateTime*/
			subTokenIndex = nextIndexOf(*json, "start", tokenIndex);
			printNextn(*json, "dateTime", subTokenIndex, 5, 11);/*5 characters long, 11th character start*/

			/*print the HH:MMpart of the ending dateTime*/
			subTokenIndex = nextIndexOf(*json, "end", tokenIndex);
			printf(" - ");
			printNextn(*json, "dateTime", subTokenIndex, 5, 11);
		}
		putchar('\n');

		/*print event heading*/
		printf("=============\n");
		printNext(*json, "subject", tokenIndex);
		putchar('\n');

		/*print the event description, if there is any*/
		if(nextTokenOf(*json, "bodyPreview", tokenIndex).start \
		!= nextTokenOf(*json, "bodyPreview", tokenIndex).end){
			printf("desc: ");
			printNext(*json, "bodyPreview", tokenIndex);
			putchar('\n');
		}

		putchar('\n');

		/*go to next calendar event*/
		tokenIndex = navigateToNext(*json,tokenIndex);
	}

	/*end of program*/
	eventTotal = eventCount(*json);

	free(json->raw);
	free(json->tokenPtr);
	free(json);

	return eventTotal;
}

int main(int argc, char** argv){
	/*for networking*/
	char *myResponse, *myHeaders[NUM_OF_HEADERS], *authkey, calendarId[CAL_ID_LENGTH+1];
	char *listCalendarsURL = "https://graph.microsoft.com/v1.0/me/calendars";

	char *domain = "https://graph.microsoft.com/v1.0/me/calendars/";
	char *queryPt1 = "/calendarview/?startdatetime=";
	char *queryPt2 = "T00:00:00.000Z&enddatetime=";
	char *queryPt3 = "T23:59:59.999Z";
	char *dateStr, *query, *eventURL;

	int eventTotal = 0;

	/*for json parsing*/
	int i;
	int tokenIndex;
	JsonWrapper *json;
	json = malloc(sizeof(JsonWrapper));
	json->raw = NULL;
	json->tokenTotal = -1;
	json->tokenPtr = NULL;

	/*get formatted date*/
	if(argc < 2 || strncmp("today",argv[1],3) == 0)
		dateStr = getToday();
	else if(strncmp("tomorrow",argv[1],3) == 0)
		dateStr = getTomorrow();
	else if(strncmp("yesturday",argv[1],3) == 0)
		dateStr = getYesturday();
	else if(argv[1][0] > 47 && argv[1][0] < 52)/*see ASCII table*/
		dateStr = getDate(argv[1]);
	else if(strcmp("help", argv[1]) == 0)
		printf("Please see README.md for usage instructions.\n");
	else{
		printf("Argument not recognised!\n"); 
		return 1;
	}

	/*construct the date with the query string*/
	query = (char *) malloc(strlen(queryPt1) + strlen(dateStr)+ strlen(queryPt2)\
	 + strlen(dateStr) + strlen(queryPt3) + 1 * sizeof(char));
	sprintf(query, "%s%s%s%s%s", queryPt1, dateStr, queryPt2, dateStr, queryPt3);
	
	/*get key from file*/
	authkey = readKey("authkey.txt");

	/*specify headers*/
	myHeaders[0] = "Host: graph.microsoft.com";
	myHeaders[1] = (char *) malloc(strlen("Authorization: ") + strlen(authkey) + 1 * sizeof(char));
	sprintf(myHeaders[1],"Authorization: %s",authkey);
	myHeaders[2] = "Prefer: outlook.timezone=\"Europe/London\"";

	/*get list of calendars*/
	myResponse = httpsGET(listCalendarsURL, NUM_OF_HEADERS, myHeaders);
	#ifdef DEBUG
	printf("RAW_RESPONSE: %s\n",myResponse);
	#endif

	/*check auth key*/
	if(strstr(myResponse, "InvalidAuthenticationToken") != NULL) {
		fprintf(stderr, "err ln%d: Authkey provided is not valid.\n", __LINE__);
		exit(EXIT_FAILURE);
	}

	/*get number of tokens*/
	json->raw = myResponse;
	jsmnParseWrapper(json);

	/*allocate memory for the json.tokenPtr and populate it*/
	json->tokenPtr = malloc(json->tokenTotal * sizeof(jsmntok_t));
	jsmnParseWrapper(json); /*the json.tokenPtr is populated here*/

	/*navigate token index to list of calendars*/
	if((tokenIndex = nextIndexOf(*json,"value",0)) < 0){
		fprintf(stderr, "err ln%d: Cannot find list of calendars.\n", __LINE__);
		exit(EXIT_FAILURE);
	}
	tokenIndex++;

	/*print day of the week*/
	printAsciiArt(0);

	/*loop through each calendar*/
	for(i = 0; i < eventCount(*json);i++){
		/*store the calendar id*/
		sprintf(calendarId, "%.*s", CAL_ID_LENGTH, json->raw + nextTokenOf(*json, "id", tokenIndex).start); 

		/*construct the complete calendarview URL*/
		eventURL = (char *) malloc(strlen(domain) + strlen(calendarId) + strlen(query) + 1 * sizeof(char));
		sprintf(eventURL, "%s%s%s", domain, calendarId, query);

		/*display events from given calendar and add to the total*/
		eventTotal += displayCalendar(eventURL, myHeaders, authkey);

		/*free constructed URL and go to next calendar event*/
		tokenIndex = navigateToNext(*json,tokenIndex);
		free(eventURL);
	}

	printf("Found %d total events.\n", eventTotal);

	/*frees*/
	free(query);
	free(dateStr);
	free(myHeaders[1]);
	free(authkey);
	free(json->raw);
	free(json->tokenPtr);
	free(json);

	return 0;
}