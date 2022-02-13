#!/bin/bash

# refresh.sh
# Daniel Robinson 2022
#
# Automates the process of getting a refresh key from the Microsoft API.
# Uses info from config.txt and loads the key into refreshkey.txt with help
# from the user.
#
# This only needs to be run once every 90 days (as of 13/02/22).

# symbolic constants
readonly CONFIG_FILE='./config.txt'
readonly REFRESH_FILE='./refreshkey.txt'

# global variables (not needed in bash but for readability)
client_id=""
redirect_uri=""
code=""

# gets the client_id and redirect_uri from the config file and stores them
getConfig(){
	lncount=`wc -l < $CONFIG_FILE`
	for i in `seq 1 $((++lncount))`; do
		ln=`sed "${i}q;d" $CONFIG_FILE`
		key=`echo $ln | awk '{print $1}'`
		if [ $key = 'client_id' ]; then
			client_id=`echo $ln | awk -F '[""]' '{print $2}'`
		fi
		if [ $key = 'redirect_uri' ]; then
			redirect_uri=`echo $ln | awk -F '[""]' '{print $2}'`
		fi
	done

	if [ -z $client_id ] || [ -z $redirect_uri ]; then
		echo "Please complete config.txt"
		exit 1
	fi
	return 0
}

# prints the URL to be pasted in the web browser and then filters the redirected URL
# note: cURL cannot be used here because Microsoft requires authentication for this step
getCode() {
	echo -e	"Please enter the following into your web browser:\n"
	echo -n	"https://login.microsoftonline.com/organizations/oauth2/v2.0/authorize"
	echo -n "?client_id=$client_id"
	echo -n "&response_type=code"
	echo -n "&redirect_uri=$redirect_uri"
	echo -n "&response_mode=query"
	echo -n "&scope=offline_access%20calendars.read%20calendars.read.shared%20user.read%20calendars.readwrite%20calendars.readwrite.shared"
	echo    "&state=12121"
	echo -e	"\nThe URL should be redirected (after logging into Microsoft). Paste the new URL below:"

	read rawCode
	echo ""

	if [[ "$rawCode" != *"code="* ]]; then
		echo "Please enter the entire URL correctly."
		exit 1
	fi
	code=`echo $rawCode | grep -o -P '(?<=code=).*(?=&state)'`

	return 0
}

# uses the code from getCode() to cURL the refresh key
getRefresh() {
	rawJSON=`curl -s\
	-H "Hostname: https://login.microsoftonline.com" \
	-H "Content-Type: application/x-www-form-urlencoded" \
	-d "client_id=$client_id&scope=offline_access%20calendars.read%20calendars.read.shared%20user.read%20calendars.readwrite%20calendars.readwrite.shared&code=$code&redirect_uri=$redirect_uri&grant_type=authorization_code" \
	-X POST "https://login.microsoftonline.com/organizations/oauth2/v2.0/token"`

	if [[ "$rawJSON" == *"error"* ]]; then
		echo -e "\033[0;91mThere was an error:\033[00m"
		echo $rawJSON
		exit 1
	fi

	arrayJSON=(${rawJSON//,/ })
	for i in `seq 0 ${#arrayJSON[@]}`;do
		if [[ "${arrayJSON[$i]}" == *"refresh_token"* ]]; then
			refreshToken=`echo ${arrayJSON[$i]} | awk -F '[""]' '{print $4}'`
		fi
	done 

	echo "Here is the refresh key returned:"
	echo -e "$refreshToken\n"
	read -p "Submit this to $REFRESH_FILE? (Y/n) " submit 
	
	if [ "$submit" = "y" ] || [ "$submit" = "Y" ]; then
		echo "Key submitted."
		echo $refreshToken > $REFRESH_FILE
	else
		echo "Key not submitted."
	fi

	return 0
}

main() {
	getConfig
	getCode
	getRefresh
	return 0
}

main
exit 0
