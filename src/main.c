#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sendhttps.h"
#include "jsmn.h"

#define NUM_OF_HEADERS 3

/*structure for grouping JSON properties*/
typedef struct JsonResponse{
		char *raw;
		int tokenTotal;
		jsmntok_t *tokenPtr; /*nested struct*/
		jsmn_parser parser;  /*nested struct*/
}JsonResponse;

/*wrapper for jsmn_parse cause why not*/
JsonResponse jsmnParseWrapper(JsonResponse json){
	jsmn_init(&json.parser);

	/*load number of available tokens into json.tokenTotal*/
	if(json.tokenTotal < 0){
		if((json.tokenTotal = jsmn_parse(&json.parser, json.raw, strlen(json.raw), NULL, 0)) < 0){
			fprintf(stderr, "err ln%d: Error code (%d) when parsing JSON UNKNOWN size.\n", __LINE__, json.tokenTotal);
			exit(EXIT_FAILURE);
		}
	}
	/*if json.tokenTotal is known, parse it properly and load the tokens into json.tokenPtr*/
	else{
		if((jsmn_parse(&json.parser, json.raw, strlen(json.raw), json.tokenPtr, json.tokenTotal)) < 0){
			fprintf(stderr, "err ln%d: Error code (%d) when parsing JSON of KNOWN size.\n", __LINE__, json.tokenTotal);
			exit(EXIT_FAILURE);
		}
	}

	return json;
}

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
	char *myResponse, *myHeaders[NUM_OF_HEADERS], *authkey;
	char *myURL = "https://graph.microsoft.com/v1.0/me/calendarview/?startdatetime=2021-04-13T00:59:59.000Z&enddatetime=2021-04-13T23:59:59.999Z";

	/*for json parsing*/
	JsonResponse json;
	json.raw = NULL;
	json.tokenTotal = -1;
	json.tokenPtr = NULL;

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

	/*check auth key*/
	if(strstr(myResponse, "InvalidAuthenticationToken") != NULL) {
		fprintf(stderr, "err ln%d: Authkey provided is not valid.\n", __LINE__);
		exit(EXIT_FAILURE);
	}

	/*get number of tokens for json.tokenTotal*/
	json.raw = myResponse;
	json = jsmnParseWrapper(json);

	/*allocate memory for the json.tokenPtr and populate it*/
	json.tokenPtr = malloc(json.tokenTotal * sizeof(jsmntok_t));
	json = jsmnParseWrapper(json); /*the json.tokenPtr is populated here*/

	/*print number of events*/
	printf("Number of events on given day: %d\n", numOfEvents(json.raw, json.tokenPtr, json.tokenTotal));

	/*frees*/
	free(myResponse);/*shared by json.raw*/
	free(myHeaders[1]);
	free(authkey);
	free(json.tokenPtr);

	return 0;
}
