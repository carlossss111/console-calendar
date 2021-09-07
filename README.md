# Console Calendar
A terminal application that displays the Microsoft Calendar events for a given day.

## Current Progress
The `sendhttps` header file is complete. This uses [libcurl4-gnutls-dev (ver: 7.68.0-1ubuntu2.6)](https://curl.se/) and returns the response from MS-Graph.

The `Makefile` is implemented to download `jsmn.h`, compile and run.

The `main` file uses the [JSMN parser](https://github.com/zserge/jsmn) and prints out calendar events from all available calendarviews. It takes 1 argument for "today"/"tomorrow"/"yesturday". If no argument is supplied then it will print today's events. 

Example output
```
$ ./a.out 05/10
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

## To Do
1. Write installation instructions and finish readme
1. Test test test for bugs

_Readme correct as of 07/09/21._