mpx
===

This was a project for Dr. Benjamin's Operating Systems class at Muhlenberg College that I took in the Fall 2013 semester. Over the course of the semester, we implemented more and more features.

For this project, we used computers running MS-DOS and wrote the mpx operating system in C such that the DOS interrupt vector table was overwritten with our own functions. So the computer would boot into DOS and then we would run the mpx program to assume control of the system. Exiting mpx would return normal control to DOS by resetting the saved interrupt vectors.

A brief overview of features implemented:

* Running programs (with optional priority)
* Reading/setting the system date and time
* Listing directory
* Show running processes
* Get help
* Change the command prompt
* Aliasing commands

To accomplish this, we implemented system requests including requesting input and sending output through COM ports to line printers and dumb terminals. Our process dispatcher uses and round-robin algorithm with configurable process priorities.

Since all of our development was done using the Borland C++ compiler on MS-DOS, we weren't using git at the time, but I've made posthumous commits and tags based on the dates that I made backups of our code. I'm mostly pushing this to GitHub for my own reference/future enjoyment, but it may be interesting to others as well. For more details, a good starting point would be to check out the Technical Manual in [docs/tech/TechMan.pdf](/docs/tech/TechMan.pdf).
