#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sendhttps.h"
#include "jsmn.h"

#define NUM_OF_HEADERS 3

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
	}
	/*if json.tokenTotal is known, parse it properly and load the tokens into json.tokenPtr*/
	else{
		if((jsmn_parse(&json->parser, json->raw, strlen(json->raw), json->tokenPtr, json->tokenTotal)) < 0){
			fprintf(stderr, "err ln%d: Error code (%d) when parsing JSON of KNOWN size.\n", __LINE__, json->tokenTotal);
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}

/*return whether a given json token is equal to a string*/
int jsoneq(const char *entireJson, jsmntok_t token, const char *comparisonString) {
  if (token.type == JSMN_STRING && (int)strlen(comparisonString) == token.end - token.start && \
      strncmp(entireJson + token.start, comparisonString, token.end - token.start) == 0) {
    return 1;
  }
  return 0;
}

/*return index of searched key/value within an object*/
int getKeyValue(JsonWrapper json, char *searchKey, int objToken){
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

/*given a key and an object token, print the corresponding value if in the object token*/
void printKeyValue(JsonWrapper json, char *searchKey, int objToken){
	int i = getKeyValue(json,searchKey,objToken);
	printf("%.*s",json.tokenPtr[i].end - json.tokenPtr[i].start,json.raw + json.tokenPtr[i].start);
	return;
}

/*return number of calendar events from a given JSON*/
/*returns -1 if not found*/
int eventCount(JsonWrapper json){
	int i = 0;

	/*find string matching "value" and stop*/
	i = getKeyValue(json,"value",0);

	/*the next token (after "value") should be an array of calendar events*/
	if(i != json.tokenTotal && json.tokenPtr[i].type == JSMN_ARRAY)
		return json.tokenPtr[i].size;
	return -1;
}

/*navigates to the token after the selected one ends, returns that index*/
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
* Tests the functions
*/
int main(int argc, char** argv){
	/*for networking*/
	char *myResponse, *myHeaders[NUM_OF_HEADERS], *authkey;
	char *myURL = "https://graph.microsoft.com/v1.0/me/calendarview/?startdatetime=2021-04-13T00:59:59.000Z&enddatetime=2021-04-13T23:59:59.999Z";
	
	int i;
	int tokenIndex;

	/*for json parsing*/
	JsonWrapper *json;
	json = malloc(sizeof(JsonWrapper));
	json->raw = NULL;
	json->tokenTotal = -1;
	json->tokenPtr = NULL;

	/*get key from file*/
	authkey = readKey("authkey.txt");

	/*specify headers*/
	myHeaders[0] = "Host: graph.microsoft.com";
	myHeaders[1] = (char *) malloc(strlen("Authorization: ") + strlen(authkey) + 1 * sizeof(char));
	sprintf(myHeaders[1],"Authorization: %s",authkey);
	myHeaders[2] = "Prefer: outlook.timezone=\"Europe/London\"";

	/*send HTTP GET request and print response*/
	myResponse = httpsGET(myURL, NUM_OF_HEADERS, myHeaders);
	#ifdef DEBUG
	printf("RAW_RESPONSE: %s\n",myResponse);
	#endif

	/*check auth key*/
	if(strstr(myResponse, "InvalidAuthenticationToken") != NULL) {
		fprintf(stderr, "err ln%d: Authkey provided is not valid.\n", __LINE__);
		exit(EXIT_FAILURE);
	}

	/*get number of tokens for json.tokenTotal*/
	json->raw = myResponse;
	jsmnParseWrapper(json);

	/*allocate memory for the json.tokenPtr and populate it*/
	json->tokenPtr = malloc(json->tokenTotal * sizeof(jsmntok_t));
	jsmnParseWrapper(json); /*the json.tokenPtr is populated here*/

	/*print number of events*/
	printf("Number of events on given day: %d\n", eventCount(*json));

	/*navigate j to list of calendar events*/
	if((tokenIndex = getKeyValue(*json,"value",0)) < 0){
		fprintf(stderr, "err ln%d: Cannot find list of events.\n", __LINE__);
		exit(EXIT_FAILURE);
	}
	tokenIndex++;

	/*loop through each event*/
	for(i = 0; i < eventCount(*json);i++){

		/*print calendar event*/
		printf("Subject: ");
		printKeyValue(*json, "subject", tokenIndex);
		putchar('\n');
		printf("Start: ");
		printKeyValue(*json, "dateTime", tokenIndex);
		putchar('\n');
		printf("End: ");
		printKeyValue(*json, "dateTime", getKeyValue(*json, "end", tokenIndex));
		putchar('\n');
		printf("Body: ");
		printKeyValue(*json, "bodyPreview", tokenIndex);
		putchar('\n');

		/*go to next calendar event*/
		tokenIndex = navigateToNext(*json,tokenIndex);
	}

	/*frees*/
	free(myHeaders[1]);
	free(authkey);
	free(json->raw);
	free(json->tokenPtr);
	free(json);

	return 0;
}
