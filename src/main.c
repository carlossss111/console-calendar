#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sendhttps.h"

#define NUM_OF_HEADERS 3

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

	/*frees*/
	free(myResponse);
	free(myHeaders[1]);
	free(authkey);

	return 0;
}
