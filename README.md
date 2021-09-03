# Console Calendar
:warning: __very unfinished and W.I.P!__

A terminal application that displays the Microsoft Calendar events for a given day.

## Current Progress
The `sendhttps` header file is complete. This uses [libcurl4-gnutls-dev (ver: 7.68.0-1ubuntu2.6)](https://curl.se/) and returns the response from MS-Graph.

The `Makefile` is implemented to download `jsmn.h`, compile and run.

The `main` file uses the [JSMN parser](https://github.com/zserge/jsmn) and prints out calendar events from the main calendarview. It looks like this currently:
```
Number of events on given day: 2

   All Day
=============
Mid-term Holiday

21:00 - 22:00
=============
Cocktail mixo
desc: meet with the group and bring stuff to make espresso martinis
```

## To Do
1. Display ALL calendar events, i.e from different calendarviews.
1. Allow user to supply arguments to specify which day to show.
1. Get the authkey automatically. (register with OAuth?)
1. Make the display look better, maybe add colour.

_Readme correct as of 03/09/21._