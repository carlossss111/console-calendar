#ifndef _SEND_HTTPS_H
#define _SEND_HTTPS_H
/*returns refresh key from a file*/
char* readKey(char* path);

/*returns character pointer reply from a GET or POST request*/
char* httpsRequest(char* myURL, int numOfMyHeaders, char* myHeaders[], char* payload);

/*GET request wrapper*/
#define httpsGET(url, numberOfHeaders, headers) httpsRequest(url, numberOfHeaders, headers, "")

/*POST request wrapper*/
#define httpsPOST(url, numberOfHeaders, headers, payload) httpsRequest(url,numberOfHeaders,headers, payload)
#endif