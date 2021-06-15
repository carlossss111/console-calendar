#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sendhttps.h"

/*
* Tests the functions
*/
int main(int argc, char** argv){
	char *myResponse, *myHeaders[2], *authkey;
	char *myURL = "https://graph.microsoft.com/v1.0/me/";

	/*get key from file*/
	authkey = readKey("authkey.txt");

	/*specify headers*/
	myHeaders[0] = "Host: graph.microsoft.com";
	myHeaders[1] = (char *) malloc(strlen("Authorization: ") + strlen(authkey) + 1 * sizeof(char));
	sprintf(myHeaders[1],"Authorization: %s",authkey);

	/*send HTTP GET request and print response*/
	myResponse = httpsGET(myURL, 2, myHeaders);
	printf("%s\n",myResponse);

	/*frees*/
	free(myResponse);
	free(myHeaders[1]);
	free(authkey);

	return 0;
}
