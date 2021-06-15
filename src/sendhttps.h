#ifndef _SEND_HTTPS_H
#define _SEND_HTTPS_H
/*returns character pointer reply from a GET request*/
char* httpsGET(char* myURL, int numOfMyHeaders, char* myHeaders[]);

/*returns the content of a file*/
char* readKey(char* path);
#endif