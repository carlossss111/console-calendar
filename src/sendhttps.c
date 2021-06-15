#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include <curl/curl.h>

#define MAX_AUTHKEY_LENGTH 4096

/*memory structure to write
* HTTP response into.
*/
struct MemoryStruct {
	char *memory;
	size_t size;
};

/*callback function used to
* write to a variable
*/
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
	char *ptr = realloc(mem->memory, mem->size + realsize + 1);
	if(ptr == NULL) {
    	fprintf(stderr,"err ln%d: out of memory\n",__LINE__);
    	return 0;
  	}
 
	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;
 
	return realsize;
}
 
/* Send HTTP request
* (value returned requires freeing)
*/
char* httpsGET(char* myURL, int numOfMyHeaders, char* myHeaders[])
{
	CURL *curl;
	CURLcode res;
	struct curl_slist *headers = NULL;
	struct MemoryStruct response;
	int i;

	response.memory = malloc(1);
	response.size = 0;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	/*apply headers*/
	for(i = 0; i < numOfMyHeaders; i++){
		headers = curl_slist_append(headers, myHeaders[i]);
	}
	if(numOfMyHeaders)
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	/*specify URL*/
	curl_easy_setopt(curl, CURLOPT_URL, myURL);

	#ifdef DEBUG
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    #endif

	/*write instructions to memory*/
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);

	/*send HTTPS request*/
	res = curl_easy_perform(curl);

	/*error check*/
	if(res != CURLE_OK) {
		fprintf(stderr, "err ln%d: %s\n",__LINE__,curl_easy_strerror(res));
		exit(EXIT_FAILURE);
	}

	/*frees*/
	curl_easy_cleanup(curl);
	if(numOfMyHeaders)
		curl_slist_free_all(headers);
	curl_global_cleanup();

	return response.memory;
}

/*Reads the "authkey.txt" file.
* (returned string needs freeing)
*/
char* readKey(char* path){
	FILE* fp; 
	char* buffer = ""; 

	/*open file*/
	if(!(fp = fopen(path,"r"))){
		fprintf(stderr, "err ln%d: cannot open authkey file.\n", __LINE__);
		exit(EXIT_FAILURE);
	}

	/*read file*/
	buffer = (char *) malloc(MAX_AUTHKEY_LENGTH * sizeof(char));
	buffer = fgets(buffer, MAX_AUTHKEY_LENGTH, fp);

	fclose(fp);
	return buffer;
}