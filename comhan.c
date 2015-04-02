/*
 *   file:  comhan.c
 *
 *   This file is the command handler for the
 *   MPX operating system.
 */

#include <stdio.h>
#include <string.h>
#include "mpx.h"

#define BUF_SIZE 80      /* Size of the command line buffer. */
#define VERSION 0
#define DATE 1
#define DIRECTORY 2
#define STOP 3
#define HELP 4
#define PROMPT 5
#define ALIAS 6



int  length;             /* Length of the command line.      */

char prompt[20] = "mpx>";
const char version[] = "MPX OS - Version 1.0\n";
char date[] = "01/09/1991";

char *cmds[] = {"version", "date", "directory","stop",
				"help", "prompt", "alias", NULL};
char *aliases[] = {"", "", "", "", "", "", "", NULL};

/*
 *   comhan()    This is the command handler for the MPX OS.
 *               It repeatedly prints a prompt, makes a system
 *               request to read from the console, and then
 *               carries out the command.
 *
 *   Parameters: None.
 *   Return value: None.
 */

void comhan() {
	char *args[5];
	int argc;
	int cmd_num;
	char buffer[BUF_SIZE];

	do {
	  printf("%s ",prompt);              /* Print a prompt.         */
	  length = BUF_SIZE;                 /* Reset length of buffer. */
	  sys_req(CON,READ,buffer,&length);  /* Request CON input       */

	  argc = set_args(buffer, args);

	  switch (cmd_num = get_cmd(args[0])) {
		case VERSION:   cmd_version();    break;
		case DATE:      cmd_date(args);   break;
		case DIRECTORY: cmd_directory();  break;
		case STOP:      cmd_stop();       break;
		case HELP:      cmd_help(args);   break;
		case PROMPT:    cmd_prompt(args); break;
		case ALIAS:     cmd_alias(args);  break;
		default:
		  printf("Can't recognize.\n");
		  break;
	  }
	} while (cmd_num != STOP);
}

int get_cmd(char cmd[]){
  /* return the number associated with a command (use STOP, HELP etc) */

  int i =0 ; //loop control

  if (cmd == NULL) {
	return -1;
  }

  strlwr(cmd);

  while (cmds[i] != NULL) {
	if (strcmp(cmds[i], cmd)==0 || strcmp(aliases[i], cmd)==0){
	  return i;
	}
	i++;
  }

  //default - means it wasn't a valid command
  return -1;
}

int set_args(char buffer[], char *args[]) {
  /* use string tok to set the contents of args from buffer
	 and return the number of args (will go into argc) */
  char separators[4] = " =/"; //Characters that separate tokens
  int i = 0; //loop control

  args[i] = strtok(buffer, separators); //Get first token
  while(args[i] != NULL){
	args[++i] = strtok(NULL, separators); //Get next tokens
  }

  return i;
}
/**
 * Print the version number.
 */
void cmd_version() {
  printf("%s", version);
}

/**
 * Print or change the date
 */
void cmd_date(char *args[]){

  int m, d, y;

  if (strcmp("", args[1])==0) {
	printf("%s \n", date);
  } else {

	m = atoi(args[1]);
	d = atoi(args[2]);
	y = atoi(args[3]);

	if (m > 0 && m < 13 && d > 0 && d < 32) {
	  sprintf(date, "%d/%d/%d", m, d, y);
	  printf("Date set to %s \n", date);
	} else {
	  printf("Invalid date. \n");
	}
  }
}
void cmd_directory(){
  int no_proc = directory(direct, DIR_SIZE);
  int i;

  if (no_proc == 1) {
	printf("You have 1 program \n");
  } else if (no_proc == 0) {
	printf("You have no programs \n");
  } else {
	printf("You have %d programs \n", no_proc);
  }

  if (no_proc > 0) {
	printf("Size      Name \n");
	printf("===================================== \n");
  }

  for (i = 0; i < no_proc; i++) {
	printf("%8d  %s \n", direct[i].dirsiz, direct[i].dirnam);
  }
}

/**
 * Print a goodbye message
 */
void cmd_stop(){
  printf("**COMHAN execution complete **\n");
}

/**
 * Print information about the COMHAN commands.
 * Will print all commands or just information specific to the
 * argument if given.
 */
void cmd_help(char *args[]){
 char   ver[] = "version              Display version number\n";
 char   hlp[] = "help                 Provide information about commands\n";
 char   dir[] = "directory            List .mpx files\n";
 char   dat[] = "date [mm/dd/yyyy]    Display or set the system date\n";
 char   stp[] = "stop                 Terminate execution of COMHAN\n";
 char prmpt[] = "prompt string        Change the prompt for commands\n";
 char   als[] = "alias command=string Create an alias for a command\n";

 switch(get_cmd(args[1])){
   case VERSION:   printf(ver);   break;
   case DATE:      printf(dat);   break;
   case DIRECTORY: printf(dir);   break;
   case STOP:      printf(stp);   break;
   case HELP:      printf(hlp);   break;
   case PROMPT:    printf(prmpt); break;
   case ALIAS:     printf(als);   break;
   default:
	 printf("**\tCommand Summary\t**\n");
	 printf("Name                     Use\n");
	 printf("======================================\n");
	 printf(ver);
	 printf(dat);
	 printf(dir);
	 printf(stp);
	 printf(hlp);
	 printf(prmpt);
	 printf(als);
 }
}

/**
 * Change the prompt.
 */
void cmd_prompt(char *args[]){
  strcpy(prompt, args[1]);
}

void cmd_alias(char *args[]){
  //get the number of the command to alias
  int num = get_cmd(args[1]);
  strcpy(aliases[num], args[2]);
  num ++;
}
