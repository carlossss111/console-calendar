#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include <curl/curl.h>

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
char* httpsGET()
{
	CURL *curl;
	CURLcode res;

	struct MemoryStruct response;
	response.memory = malloc(1);
	response.size = 0;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	/*specify URL*/
	curl_easy_setopt(curl, CURLOPT_URL, "https://www.example.com/");

	#ifdef DEBUG
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    #endif

	/*user agent header*/
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

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
	curl_global_cleanup();

	return response.memory;
}

/*
* Tests the function
*/
int main(int argc, char** argv){
	char* myResponse;
	myResponse = httpsGET();
	printf("%s\n",myResponse);
	free(myResponse);
	return 0;
}