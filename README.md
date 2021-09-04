# Console Calendar
:warning: __very unfinished and W.I.P!__ :warning:

A terminal application that displays the Microsoft Calendar events for a given day.

## Current Progress
The `sendhttps` header file is complete. This uses [libcurl4-gnutls-dev (ver: 7.68.0-1ubuntu2.6)](https://curl.se/) and returns the response from MS-Graph.

The `Makefile` is implemented to download `jsmn.h`, compile and run.

The `main` file uses the [JSMN parser](https://github.com/zserge/jsmn) and prints out calendar events from all available calendarviews. It takes 1 argument for "today"/"tomorrow"/"yesturday". If no argument is supplied then it will print today's events. Here is an example output:
```
   All Day
=============
Mid-term Holiday

21:00 - 22:00
=============
Cocktail mixo
desc: meet with the group and bring stuff to make espresso martinis

Showing 2 total events.
```

## To Do
1. Allow user to supply arguments to specify which specific date to show.
1. Get the authkey automatically. (register with OAuth?)
1. Make the display look better, maybe add colour.

_Readme correct as of 04/09/21._