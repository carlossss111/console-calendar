#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sendhttps.h"
#include "jsmn.h"

#define NUM_OF_HEADERS 3

/*return whether a given json token is equal to a string*/
static int jsoneq(const char *entireJson, jsmntok_t token, const char *comparisonString) {
  if (token.type == JSMN_STRING && (int)strlen(comparisonString) == token.end - token.start &&
      strncmp(entireJson + token.start, comparisonString, token.end - token.start) == 0) {
    return 1;
  }
  return 0;
}

/*return number of calendar events from a given JSON*/
int numOfEvents(char *jsonString, jsmntok_t *tokenArr, int totalTokens){
	int i = 0;

	/*find string matching "value" and stop*/
	while(i < totalTokens && !jsoneq(jsonString, tokenArr[i], "value"))
		i++;

	/*the next token (after "value") should be an array of calendar events*/
	if(i != totalTokens && tokenArr[++i].type == JSMN_ARRAY)
		return tokenArr[i].size;
	return -1;
}

/*find the search value*/
/*returns -1 if not found*/
int findJsonValue(char *searchString, char *jsonString, jsmntok_t *tokenArr, int totalTokens){
	int i = 0;

	/*find key matching "value" and stop*/
	while(i < totalTokens && !jsoneq(jsonString, tokenArr[i], searchString))
		i++;

	/*check a value is actually found*/
	if(i == totalTokens)
		return -1;

	/*return corresponding value*/
	return i;
}

/*
* Tests the functions
*/
int main(int argc, char** argv){
	/*for networking*/
	char *jsonResponse, *myHeaders[NUM_OF_HEADERS], *authkey;
	char *myURL = "https://graph.microsoft.com/v1.0/me/calendarview/?startdatetime=2021-04-13T00:59:59.000Z&enddatetime=2021-04-13T23:59:59.999Z";

	/*for json parsing*/
	int totalTokens = -1, searchIndex;
	jsmn_parser parser;
	jsmntok_t *tokenArr;

	/*get key from file*/
	authkey = readKey("authkey.txt");

	/*specify headers*/
	myHeaders[0] = "Host: graph.microsoft.com";
	myHeaders[1] = (char *) malloc(strlen("Authorization: ") + strlen(authkey) + 1 * sizeof(char));
	sprintf(myHeaders[1],"Authorization: %s",authkey);
	myHeaders[2] = "Prefer: outlook.timezone=\"Europe/London\"";

	/*send HTTP GET request and print response*/
	jsonResponse = httpsGET(myURL, NUM_OF_HEADERS, myHeaders);
	printf("%s\n",jsonResponse);

	/*check auth key*/
	if(strstr(jsonResponse, "InvalidAuthenticationToken") != NULL) {
		fprintf(stderr, "err ln%d: Authkey provided is not valid.\n", __LINE__);
		exit(EXIT_FAILURE);
	}

	/*initialise parser and get number of tokens*/
	jsmn_init(&parser);
	if((totalTokens = jsmn_parse(&parser, jsonResponse, strlen(jsonResponse), NULL, 0)) < 0){
		fprintf(stderr, "err ln%d: Error code (%d) when parsing JSON, first pass.\n", __LINE__, totalTokens);
		exit(EXIT_FAILURE);
	}

	/*reinitialise parser and allocate memory for the tokens*/
	jsmn_init(&parser);
	tokenArr = malloc(totalTokens * sizeof(jsmntok_t));

	/*parse JSON into tokenArr variable*/
	if((jsmn_parse(&parser, jsonResponse, strlen(jsonResponse), tokenArr, totalTokens)) < 0){
		fprintf(stderr, "err ln%d: Error code (%d) when parsing JSON, second pass.\n", __LINE__, totalTokens);
		exit(EXIT_FAILURE);
	}

	/*print number of events*/
	printf("Number of events on given day: %d\n", numOfEvents(jsonResponse, tokenArr, totalTokens));

	/*list name of events*/
	searchIndex = findJsonValue("subject", jsonResponse, tokenArr, totalTokens) + 1;
	printf("First value:  %.*s\n", tokenArr[searchIndex].end - tokenArr[searchIndex].start,\
		 jsonResponse + tokenArr[searchIndex].start);

	/*frees*/
	free(jsonResponse);
	free(myHeaders[1]);
	free(authkey);
	free(tokenArr);

	return 0;
}
