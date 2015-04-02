/*
 *   file:  comhan.c
 *
 *   This file is the command handler for the
 *   MPX operating system.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpx.h"

#define BUF_SIZE 80      /* Size of the command line buffer. */
#define VERSION 0
#define DATE 1
#define DIRECTORY 2
#define STOP 3
#define HELP 4
#define PROMPT 5
#define ALIAS 6
#define SHOW 7
#define ALLOCATE 8
#define CMD_FREE 9


int length;             /* Length of the command line.      */

char prompt[20] = "mpx>";
const char version[] = "MPX OS - Version 1.0\n";
char date[] = "01/09/1991";

char *cmds[] = {"version", "date", "directory","stop","help",
				"prompt", "alias", "show", "allocate", "free", NULL};
char *aliases[] = {"        ", "        ", "        ", "        ",
				   "        ", "        ", "        ", "        ",
				   "        ", "        ", NULL};

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
	static char *args[7]; //one more than expected number of arguments
	int argc;
	int cmd_num;
	static char buffer[BUF_SIZE];
	int do_stop = 0;

	do {
	  printf("%s ",prompt);              /* Print a prompt.         */
	  length = BUF_SIZE;                 /* Reset length of buffer. */
	  sys_req(CON,READ,buffer,&length);  /* Request CON input       */

	  argc = set_args(buffer, args);

	  switch (cmd_num = get_cmd(args[0])) {
		case VERSION:   cmd_version();        break;
		case DATE:      cmd_date(args);       break;
		case DIRECTORY: cmd_directory();      break;
		case STOP:      do_stop = cmd_stop(); break;
		case HELP:      cmd_help(args);       break;
		case PROMPT:    cmd_prompt(args);     break;
		case ALIAS:     cmd_alias(args);      break;
		case SHOW:      cmd_show(args);       break;
		case ALLOCATE:  cmd_allocate(args);   break;
		case CMD_FREE:  cmd_free(args);       break;
		default:
		  printf("Can't recognize.\n");
		  break;
	  }
	} while (!do_stop);
}

int get_cmd(char cmd[]){
  /* return the number associated with a command (use STOP, HELP etc) */

  int i = 0; //loop control

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
  char separators[5] = " =/,"; //Characters that separate tokens
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
int cmd_stop(){
  char buffer[2];
  int length = 2;

  printf("Are you sure you want to exit? [y/n]: ");
  sys_req(CON,READ,buffer,&length);

  if (strcmp(buffer, "y") == 0 || strcmp(buffer, "Y") == 0) {
	printf("**COMHAN execution complete **\n");
	return 1;
  } else {
	return 0;
  }
}

/**
 * Print information about the COMHAN commands.
 * Will print all commands or just information specific to the
 * argument if given.
 */
void cmd_help(char *args[]){
 char   ver[] = "version              Display version number \n";
 char   hlp[] = "help                 Provide information about commands \n";
 char   dir[] = "directory            List .mpx files \n";
 char   dat[] = "date [mm/dd/yyyy]    Display or set the system date \n";
 char   stp[] = "stop                 Terminate execution of COMHAN \n";
 char prmpt[] = "prompt string        Change the prompt for commands \n";
 char   als[] = "alias command=string Create an alias for a command \n";
 char   shw[] = "show                 Prints PCB information \n";
 char alloc[] = "allocate             Builds PCB with specified options \n";
 char  free[] = "free name            Frees the PCB called name \n";

 switch(get_cmd(args[1])){
   case VERSION:   printf(ver);   break;
   case DATE:      printf(dat);   break;
   case DIRECTORY: printf(dir);   break;
   case STOP:      printf(stp);   break;
   case HELP:      printf(hlp);   break;
   case PROMPT:    printf(prmpt); break;
   case ALIAS:     printf(als);   break;
   case SHOW:      printf(shw);   break;
   case ALLOCATE:  printf(alloc); break;
   case CMD_FREE:  printf(free);  break;
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
	 printf(shw);
	 printf(alloc);
	 printf(free);
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

void print_pcb(pcb * p) {

	char str[4];

	if (p->type == FREE) {
		str[0] = 'f';
	} else if (p->type == SYS_PROCESS) {
		str[0] = 's';
	} else if (p->type == APP_PROCESS) {
		str[0] = 'a';
	} else {
		str[0] = '-';
	}

	if (p->state == READY) {
		str[1] = 'r';
	} else if (p->state == RUNNING) {
		str[1] = 'o';
	} else if (p->state == BLOCKED) {
		str[1] = 'b';
	} else {
		str[1] = '-';
	}

	if (p->suspend == SUSPENDED) {
		str[2] = 'y';
	} else if (p->suspend == NOT_SUSPENDED) {
		str[2] = 'n';
	} else {
		str[2] = '-';
	}

	str[3] = '\0';

	printf("0x%04x %8s %s 0x%04x 0x%04x 0x%04x %3d 0x%04x \n",
		  p, p->name, str, p->chain, p->prev,
		  p->next, p->priority, p->loadaddr);
}

void cmd_show(char *args[]) {
  pcb * current = pcb_list;

  if (!args[1]) {
	printf("Show: free, all, system, application, \n");
	printf("      suspended, ready, init. \n");
	return;
  }

  printf("PCB_Ad Name     TSP chain  prev   next   pri l_addr \n");
  printf("------ -------- --- ------ ------ ------ --- ------ \n");

  if (strcmp(args[1], "init") == 0 || strcmp(args[1], "ready") == 0) {
	current = (strcmp(args[1], "init") == 0)? io_init_queue : ready_queue;

	if (current == NULL) {
	  return;
	}

	do {
	  print_pcb(current);
	  current = current->next;
	} while (current != NULL);
	return;
  }

  do {
	if (strcmp(args[1], "free") == 0 && current->type == FREE) {
		print_pcb(current);
	} else if (strcmp(args[1], "all") == 0) {
		print_pcb(current);
	} else if (strcmp(args[1], "system") == 0 &&
	  current->type == SYS_PROCESS) {
		print_pcb(current);
	} else if (strcmp(args[1], "application") == 0 &&
	  current->type == APP_PROCESS) {
		print_pcb(current);
	} else if (strcmp(args[1], "suspended") == 0 &&
	  current->suspend == SUSPENDED) {
		print_pcb(current);
	} else if (strcmp(args[1], "ready") == 0 &&
	  current->state == READY) {
		print_pcb(current);
	}

	current = current->chain;
  } while (current != NULL);
}

void cmd_allocate(char *args[]) {
  pcb * new;
  int result;
  int type, state, suspend, priority;
  pcb * exists;

  if (!args[1] || !args[2] || !args[3] || !args[4] || !args[5]) {
	printf("***Error*** \n");
	printf("Please use the correct number of arguments. \n");
	return;
  }

  if (strcmp(args[2], "f") != 0 &&
	  strcmp(args[2], "a") != 0 &&
	  strcmp(args[2], "s") != 0) {
		printf("***Error*** \n");
		printf("PCB type must be f(ree), a(pplication) or s(ystem). \n");
		printf("You said: %s \n", args[2]);
		return;
  }

  if (strcmp(args[3], "r") != 0 &&
	  strcmp(args[3], "o") != 0 &&
	  strcmp(args[3], "b") != 0) {
		printf("***Error*** \n");
		printf("PCB state must be r(eady), o(running), or b(locked). \n");
		printf("You said: %s \n", args[3]);
		return;
  }

  if (strcmp(args[4], "y") != 0 &&
	  strcmp(args[4], "n") != 0) {
		printf("***Error*** \n");
		printf("PCB suspended flag must be y(es) or n(o). \n");
		printf("You said: %s \n", args[4]);
		return;
  }

  priority = atoi(args[5]);

  exists = search_pcb(pcb_list, args[1]);

  if(exists != NULL){
	printf("***Error*** \n");
	printf("A pcb with that name already esists. \n");
	return;
  }

  new = get_pcb(pcb_list);

  type = FREE;
  if (strcmp(args[2], "a") == 0) {
	type = APP_PROCESS;
  } else if (strcmp(args[2], "s") == 0) {
	type = SYS_PROCESS;
  }

  state = READY;
  if (strcmp(args[3], "o") == 0) {
	state = RUNNING;
  } else if (strcmp(args[3], "b") == 0) {
	state = BLOCKED;
  }

  suspend = NOT_SUSPENDED;
  if (strcmp(args[4], "y") == 0) {
	suspend = SUSPENDED;
  }

  if (type == APP_PROCESS && (priority < -126 || priority > 126)) {
	printf("***Error*** \n");
	printf("The priority for an application process must be \n");
	printf("between -126 and 126, inclusive. \n");
	return;
  }

  if (type == SYS_PROCESS && (priority < -128 || priority > 127)) {
	printf("***Error*** \n");
	printf("The priority for a system process must be \n");
	printf("between -128 and 127, inclusive. \n");
	return;
  }

  result = build_pcb(new, args[1], type, state, suspend, priority,
					 NULL, NULL, NULL, NULL);

  switch (result) {
	case 1:  break;

	case -1: printf("***Error*** \n");
			 printf("There was no space for another pcb. \n");
			 break;

	case -2: printf("***Error*** \n");
			 printf("Invalid type \n");
			 break;

	case -3: printf("***Error*** \n");
			 printf("Invalid state \n");
			 break;

	case -4: printf("***Error*** \n");
			 printf("Invalid suspend \n");
			 break;
  }

  //put the pcb in a queue
  if (state == READY) {
	insert_pcb(&ready_queue, new, 0); //insert in priority order
  } else if (state == BLOCKED) {
	insert_pcb(&io_init_queue, new, 1); //insert at end of queue
  }

  return;
}

void cmd_free(char *args[]) {
  pcb * to_free = search_pcb(pcb_list, args[1]);
  int result;

  if (to_free->state == READY) {
	remove_pcb(&ready_queue, to_free);
  } else if(to_free->state == BLOCKED) {
	remove_pcb(&io_init_queue, to_free);
  }

  result = free_pcb(pcb_list, to_free);
  switch (result) {
	case  1: printf("PCB freed\n");         break;
	case -1: printf("Couldn't find PCB\n"); break;
	case -2: printf("PCB already free\n");  break;
  }
}