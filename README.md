# Console Calendar
A terminal application that displays the Microsoft Calendar events for a given day. This currently only works on Linux.

## Usage
When provided with a refresh key and permissions to use Microsoft Graph _(see installation)_, the program will display for any given day: the day of the week; the date; event times; event titles; event descriptions and the total number of events.

The program currently accepts the following arguments:

* (none) - prints today's events.
```
$consolecal
```

* "\[tod\]ay"/"\[tom\]orrow"/"\[yes\]turday" - only requires the first three letters to be entered.
```
$consolecal tomorrow
```

* "DD/MM" - prints events from a date of the same year.
```
$consolecal 05/10
```

Here is an example output:
```
░░░░░░░░░░░████████╗██╗░░░██╗███████╗░██████╗██████╗░░█████╗░██╗░░░██╗░░░░░░░░░░
░░░░░░░░░░░╚══██╔══╝██║░░░██║██╔════╝██╔════╝██╔══██╗██╔══██╗╚██╗░██╔╝░░░░░░░░░░
░░░░░░░░░░░░░░██║░░░██║░░░██║█████╗░░╚█████╗░██║░░██║███████║░╚████╔╝░░░░░░░░░░░
░░░░░░░░░░░░░░██║░░░██║░░░██║██╔══╝░░░╚═══██╗██║░░██║██╔══██║░░╚██╔╝░░░░░░░░░░░░
░░░░░░░░░░░░░░██║░░░╚██████╔╝███████╗██████╔╝██████╔╝██║░░██║░░░██║░░░░░░░░░░░░░
░░░░░░░░░░░░░░╚═╝░░░░╚═════╝░╚══════╝╚═════╝░╚═════╝░╚═╝░░╚═╝░░░╚═╝░░░░░░░░░░░░░
                               ~ ~ 05/10/2021 ~ ~

10:00 - 11:00
=============
Software Engineering Group Project - Online Lecture Engagement
Taught By: De Maere G D
Module code:
Last Updated: 06/09/2021 07:44

12:00 - 13:00
=============
Operating Systems & Concurrency - Lecture
Taught By: Turner A Dr, De Maere G Dr
Module code: COMP/2007/01
Last Updated: 06/09/2021 07:44

14:00 - 16:00
=============
Developing Maintainable Software - Lecture
Taught By: Laramee R Dr, Siebers P Dr
Module code: COMP/2013/01
Last Updated: 06/09/2021 07:44

17:00 - 18:00
=============
Algorithms Correctness and Efficiency - Lecture Engagement
Taught By: Knight, I
Module code:
Last Updated: 06/09/2021 07:44

Found 4 total events.
```

## Installation and Dependencies

This does take a hot minute to install, if you know me then let me know if you need help with any of the stages. Any suggestions on how to streamline this are also appreciated but I'm not that worried because this'll probably only be used by me.

### Registering the Application with Microsoft
_Note: If you are at my university I can probably give you my id etc and you can skip this step._

1. Clone this directory to anywhere you would like for now.
1. This application needs to be registered with Microsoft because it is using it's services. Follow this [quickstart guide](https://docs.microsoft.com/en-us/azure/active-directory/develop/quickstart-register-app#register-an-application) up to but not including "add credentials". The configuration of the app doesn't matter, you can pick things like a default redirect-uri.
1. Find the permissions tab and add delegated permissions to Microsoft Graph under "calendar".
1. Take note of the "client id" and the "redirect uri" on the main page.
1. Go back to the directory where you downloaded this. Open config.txt in a text editor and enter the client id and redirect uri you have registered.

### Getting a Refresh Key
_Note: Unfortunately the refresh key only lasts 90 days so this step will need to be taken again after that time._

Run the `refresh.sh` script and follow the instructions. This will request a refresh key for you and will pipe it into refreshkey.txt.

(If there were any issues consult [the Microsoft guide](https://docs.microsoft.com/en-us/graph/auth-v2-user)).

### Dependencies
This program requires lib-cURL-dev and JSMN. Lib-cURL-dev needs downloading and installing manually, JSMN is a only single 12KB file that is downloaded and installed automatically in the next step.
* [lib-cURL dev](https://curl.se/) - Handles the HTTPS requests to the Microsoft Graph API. Download and install globally through a package manager like synaptic.
* [JSMN parser](https://github.com/zserge/jsmn) - Handles the parsing of the Microsoft Graph response. Does **not** require manual installation or download, this is done in the Makefile.

### Final Installation
1. Verify that the config.txt and the refreshkey.txt have been written to (they can also be changed later).
1. Run the Makefile **from the directory it is located in**.

```
$make compile_and_link
```
The program will compile and the necessary files will be linked in `/usr/local/bin` so they can be called globally. The local files can be deleted if you want now.

The program should now be installed!

### Changing ascii.txt (optional)
If you would like different ascii art for days of the week, rewrite the values in ascii.txt between the numbers. Do not move the numbers as they are used to find the correct part of the file. Unicode probably won't work.

In main.c change the `#define HEADING_WIDTH 80` at the top to the width of one line of the new ascii art (I'm looking into having this changed automatically but it's a bit weird since some ascii art counts 1 column as 3 characters etc etc). 

If the heading width was changed, recompile with `make compile`. 

If you would not like any day of the week heading, just delete ascii.txt and change `#define HEADING_WIDTH 80` to 0 and recompile.

## Source Files
The `sendhttps` file uses libcurl to make GET and POST requests to a given URL and with given headers and an optional payload. This file can easily be reused in other projects and has almost nothing completely specific to this project.

The `Makefile` is implemented to download jsmn.h, compile and run. It has memory tests commands aswell. Read the file to see the different options.

The `main` file uses the [JSMN parser](https://github.com/zserge/jsmn) and prints out calendar events from all available calendarviews. It has functions for parsing objects, navigating and printing json values aswell as handling the date.

## To do
* Take day-of-the-week words like 'tuesday' as valid inputs.
* Sort events by time when coming from different calendars (will require some refactoring).

_Readme correct as of 13/02/22._

### My comments be like:

![My Commenting Style](./src/mycomments.gif)