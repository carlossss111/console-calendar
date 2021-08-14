#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
int numEvents(char* jsonString){
	int totalTokens = -1;
	int i = 0;
	int result = -1;
	jsmn_parser parser;
	jsmntok_t *tokenArr;
	
	/*initialise parser and get number of tokens*/
	jsmn_init(&parser);
	if((totalTokens = jsmn_parse(&parser, jsonString, strlen(jsonString), NULL, 0)) < 0){
		fprintf(stderr, "err ln%d: Error code (%d) when parsing JSON.\n", __LINE__, totalTokens);
		exit(EXIT_FAILURE);
	}

	/*reinitialise parser and allocate memory for the tokens*/
	jsmn_init(&parser);
	tokenArr = malloc(totalTokens * sizeof(jsmntok_t));

	/*parse JSON into tokenArr variable*/
	if((jsmn_parse(&parser, jsonString, strlen(jsonString), tokenArr, totalTokens)) < 0){
		fprintf(stderr, "err ln%d: Error code (%d) when parsing JSON.\n", __LINE__, totalTokens);
		exit(EXIT_FAILURE);
	}

	/*find string matching "value" and stop*/
	while(i < totalTokens && !jsoneq(jsonString, tokenArr[i], "value")){
		i++;
	}

	/*the next token (after "value") should be an array of calendar events*/
	if(i != totalTokens && tokenArr[++i].type == JSMN_ARRAY)
		result = tokenArr[i].size;

	/*free token list and return result*/
	free(tokenArr);
	return result;
}

/*
* Tests the functions
*/
int main(int argc, char** argv){
	char *myResponse, *myHeaders[NUM_OF_HEADERS], *authkey;
	char *myURL = "https://graph.microsoft.com/v1.0/me/calendarview/?startdatetime=2021-04-13T00:59:59.000Z&enddatetime=2021-04-13T23:59:59.999Z";

	/*get key from file*/
	authkey = readKey("authkey.txt");

	/*specify headers*/
	myHeaders[0] = "Host: graph.microsoft.com";
	myHeaders[1] = (char *) malloc(strlen("Authorization: ") + strlen(authkey) + 1 * sizeof(char));
	sprintf(myHeaders[1],"Authorization: %s",authkey);
	myHeaders[2] = "Prefer: outlook.timezone=\"Europe/London\"";

	/*send HTTP GET request and print response*/
	myResponse = httpsGET(myURL, NUM_OF_HEADERS, myHeaders);
	printf("%s\n",myResponse);

	/*validate JSON*/
	/*TODO*/

	/*check auth key*/
	if(strstr(myResponse, "InvalidAuthenticationToken") != NULL) {
		fprintf(stderr, "err ln%d: Authkey provided is not valid.\n", __LINE__);
		free(myResponse);
		free(myHeaders[1]);
		free(authkey);
		return 1;
	}

	/*print the number of events on the given day*/
	printf("\nNumber of Events: %d\n", numEvents(myResponse));

	/*frees*/
	free(myResponse);
	free(myHeaders[1]);
	free(authkey);

	return 0;
}
