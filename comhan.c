/*
 *   file:  comhan.c
 *
 *   This file is the command handler for the
 *   MPX operating system.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dos.h>
#include "mpx.h"

#define BUF_SIZE 80      /* Size of the command line buffer. */

#define VERSION        0
#define DATE           1
#define DIRECTORY      2
#define STOP           3
#define HELP           4
#define PROMPT         5
#define ALIAS          6
#define SHOW           7
#define ALLOCATE       8
#define CMD_FREE       9
#define CMD_LOAD      10
#define CMD_RESUME    11
#define CMD_RUN       12
#define CMD_SUSPEND   13
#define CMD_TERMINATE 14
#define CMD_SETPRI    15
#define CMD_DISPATCH  16
#define CLOCK     17

#define NUM_CMDS 18

int length;             /* Length of the command line.      */
unsigned sp_save;       /* A stack save for mod 4 dispatch. */

char prompt[20] = "mpx>";
const char version[] = "MPX OS - Version 1.0\n";
char date[] = "01/09/1991";

char *cmds[] = { "version", "date", "directory", "stop",
				 "help", "prompt", "alias", "show",
				 "allocate", "free", "load", "resume",
				 "run", "suspend", "terminate", "setpriority",
				 "dispatch", "clock", NULL};
char *aliases[] = {"        ", "        ", "        ", "        ",
				   "        ", "        ", "        ", "        ",
				   "        ", "        ", "        ", "        ",
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
	static char buffer[BUF_SIZE];
	int do_stop = 0;

	do {
	  printf("%s ",prompt);              /* Print a prompt.         */
	  //length = 4; //mpx>
	  //sys_req(COM, WRITE, prompt, &length);
	  length = BUF_SIZE;                 /* Reset length of buffer. */
	  sys_req(CON,READ,buffer,&length);  /* Request CON input       */

	  set_args(buffer, args);

	  switch (get_cmd(args[0])) {
		case VERSION:       cmd_version();        break;
		case DATE:          cmd_date(args);       break;
		case DIRECTORY:     cmd_directory();      break;
		case STOP:          do_stop = cmd_stop(); break;
		case HELP:          cmd_help(args);       break;
		case PROMPT:        cmd_prompt(args);     break;
		case ALIAS:         cmd_alias(args);      break;
		case SHOW:          cmd_show(args);       break;
//		case ALLOCATE:      cmd_allocate(args);   break;
//		case CMD_FREE:      cmd_free(args);       break;
		case CMD_LOAD:      cmd_load(args);       break;
		case CMD_RESUME:    cmd_resume(args);     break;
		case CMD_RUN:       cmd_run(args);        break;
		case CMD_SUSPEND:   cmd_suspend(args);    break;
		case CMD_TERMINATE: cmd_terminate(args);  break;
		case CMD_SETPRI:    cmd_setpri(args);     break;
//		case CMD_DISPATCH:  cmd_dispatch();       break;
		case CLOCK:         cmd_clock(args);      break;
		default:
		  printf("Can't recognize. %s\n", args[0]);
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
  char separators[5] = " /,:"; //Characters that separate tokens
  // TODO: put = back in separators
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
	printf("========= =========================== \n");
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
  pcb *p = pcb_list;

  printf("Are you sure you want to exit? [y/n]: ");
  sys_req(CON,READ,buffer,&length);

  if (strcmp(buffer, "y") == 0 || strcmp(buffer, "Y") == 0) {
	do {
	  if(p->type != FREE) {
		freemem(p->loadaddr);
		free_pcb(pcb_list, p);
	  }
	  p = p->chain;
	} while(p != NULL);
	sys_exit();
	printf("** COMHAN execution complete **\n");
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
void cmd_help(char *args[]) {
  int i;
  char *help[NUM_CMDS];

  help[VERSION]       = "version              Display version number";
  help[HELP]          = "help                 Provide information about commands";
  help[DIRECTORY]     = "directory            List .mpx files";
  help[DATE]          = "date [mm/dd/yyyy]    Display or set the system date";
  help[STOP]          = "stop                 Terminate execution of COMHAN";
  help[PROMPT]        = "prompt string        Change the prompt for commands";
  help[ALIAS]         = "alias command=string Create an alias for a command";
  help[SHOW]          = "show                 Prints PCB information";
  help[ALLOCATE]      = "allocate             Builds PCB with specified options";
  help[CMD_FREE]      = "free name            Frees the PCB called name";
  help[CMD_LOAD]      = "load name[=ppp]      Creates a process called name with priority ppp";
  help[CMD_RESUME]    = "resume name          Resumes the process called name";
  help[CMD_RUN]       = "run name[=ppp]       Runs a process called name with priority ppp";
  help[CMD_SUSPEND]   = "suspend name         Suspends the process called name";
  help[CMD_TERMINATE] = "terminate name       Terminates the process called name";
  help[CMD_SETPRI]    = "setpriority name=ppp Sets the priority of process name";
  help[CMD_DISPATCH]  = "dispatch             Runs each process once";
  help[CLOCK]         = "clock [stop|start]   Perform clock operations";

  // print header
  printf(               "  Name                 Use \n");
  printf(               "  ==================== ================================================= \n");

  if (args[1] != NULL) {
	// print help for specific command
	printf("  %s \n", help[ get_cmd(args[1]) ] );
  } else {
	// list all help
	for (i = 0; i < NUM_CMDS; i++) {
	  printf("  %s \n", help[i]);
	}
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
	disable();
	current = (strcmp(args[1], "init") == 0)? io_init_queue : ready_queue_locked;
	enable();

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

/**
 * Load
 */
void cmd_load(char *args[]) {
  int i, ret, priority = 0;
  unsigned segp;
  pcb *p;
  int paragraphs;
  char* name = args[1];
  // figure out how many directory entries
  int num = directory(direct, DIR_SIZE);

  if (num < 1) {
	printf("Error: no programs available to load. \n");
	return;
  }

  // search in the directory for the specified program
  i = 0;
  while (stricmp(direct[i].dirnam, name) != 0) {
	i++;
	if (i == num) {
	  // no program found
	  printf("Error: No program found with that name. \n");
	  return;
	}
  }

  // figure out how much memory we need
  paragraphs = (unsigned) ceil(direct[i].dirsiz / 16);

  // try to allocate the needed memory
  if (allocmem(paragraphs, &segp) != -1) {
	// insufficient memory
	printf("Error: Insufficient memory. \n");
	return;
  }

  // read the file into memory
  if (load((unsigned *)segp, direct[i].dirnam) != 0) {
	// load error
	printf("Error: Could not load program into memory. \n");
	return;
  }

  // if a valid priority was specified, replace the default
  if (args[2] != NULL) {
	if (atoi(args[2]) >= -126 && atoi(args[2]) <= 126) {
	  priority = atoi(args[2]);
	} else {
	  printf("Warning: invalid priority specified, using default. \n");
	}
  }

  // now we can put the proc into a PCB
  p = get_pcb(pcb_list);

  if (p == NULL) {
	printf("Error: No free PCBs. \n");
	return;
  }

  if (build_pcb(p, args[1], APP_PROCESS, READY, SUSPENDED,
	priority, segp, 0 /* ip */, 0 /* ds */, 0x200) != 1) {
	printf("Error: Unable to build PCB. \n");
	return;
  }
  disable();
  insert_pcb(&ready_queue_locked, p, 0);
  enable();
  return;
}

/**
 * Resume
 */
void cmd_resume(char *args[]) {

  pcb *p = pcb_list;

  if (args[1] != NULL) {
	if (strcmp(args[1], "*") == 0) {
	  // resume all processes
	  // set suspended to NOT_SUSPENDED for all application PCBs
	  do {
		if (p->type == APP_PROCESS) {
		  p->suspend = NOT_SUSPENDED;
		}
		p = p->chain;
	  } while (p != NULL);
	} else {
	  // search for the correct PCB and set suspended to NOT_SUSPENDED
	  p = search_pcb(pcb_list, args[1]);
	  if (p != NULL) {
		p->suspend = NOT_SUSPENDED;
	  } else {
		printf("No process named %s. \n", args[1]);
	  }
	}
  } else {
	printf("No process specified. Please run resume proc \n");
	printf("for process proc. To resume all processes, \n");
	printf("type resume * \n");
  }
}

/**
 * Run
 */
void cmd_run(char *args[]) {
  pcb *p;
  // load the process ...
  cmd_load(args);

  // ... then set it to be not suspended
  p = search_pcb(pcb_list, args[1]);
  if (p != NULL){
	p->suspend = NOT_SUSPENDED;
  } else {
	printf("Error: process did not load correctly. \n");
  }
}

/**
 * Suspend
 */
void cmd_suspend(char *args[]) {
  pcb *p;

  // Get the pcb...
  p = search_pcb(pcb_list, args[1]);

  if (p != NULL) {
	// ... and suspend it
	p->suspend = SUSPENDED;
  } else {
	printf("No process with the specified name. \n");
  }
}

/**
 * Terminate
 */
void cmd_terminate(char *args[]) {
   pcb *p;

   // the "terminate *" case
   if (strcmp(args[1], "*") == 0) {
	 p = pcb_list;
	 do {
	   if (p->type == APP_PROCESS) {
		 // remove from queue as needed
		 disable();
		 if (p->state == READY) {
		   remove_pcb(&ready_queue_locked, p);
		 } else if (p->state == BLOCKED) {
		   remove_pcb(&io_init_queue, p);
		 }
		 enable();

		 // free it
		 freemem(p->loadaddr);
		 free_pcb(pcb_list, p);
	   }
	   p = p -> chain;
	 } while (p != NULL);
   } else {
	 // the "terminate proc" case
	 p = search_pcb(pcb_list, args[1]);

	 if (p != NULL && p->type == APP_PROCESS) {

	   disable();
	   // remove from queue as needed
	   if (p->state == READY) {
		 remove_pcb(&ready_queue_locked, p);
	   } else if (p->state == BLOCKED) {
		 remove_pcb(&io_init_queue, p);
	   }
	   enable();

	   // free it
	   freemem(p->loadaddr);
	   free_pcb(pcb_list, p);
	 } else {
	   printf("No process with the specified name. \n");
	 }
   }
   return;
}

/**
 * Set Priority
 */
void cmd_setpri(char *args[]) {
  pcb *p;
  int new_priority;

  // Get the pcb
  p = search_pcb(pcb_list, args[1]);
  new_priority = atoi(args[2]);

  if (p != NULL) {
	// Check the priority is valid
	if (p->type == APP_PROCESS && new_priority >= -128 && new_priority <= 127) {
	  disable();
	  remove_pcb(&ready_queue_locked, p);      // Take pcb out of ready queue
	  p->priority = new_priority;       // Change priority
	  insert_pcb(&ready_queue_locked, p, 0);   // Re-insert pcb in ready queue
	  enable();
	} else {
	  printf("Error: invalid priority. \n");
	}
  } else {
	printf("Error: invalid process name. \n");
  }
}

/*
 * stop, start, set, or show the clock based on args[1]
 */
void cmd_clock(char *args[]){
  int hr, m, s;
  int ret;
  // decide what clock operation to perform
  if (strcmp(args[1], "stop") == 0) {
	stop_clock();
	return;
  } else if (strcmp(args[1], "start") == 0) {
	start_clock();
	return;
  } else if (strcmp(args[1], "") == 0) {
	read_clock(&hr, &m, &s);
	printf("The current time is %d:%d:%d\n", hr, m, s);
	return;
  } else {
	// the remaining case is to set the clock
	// parse the input - should be "hh:mm:ss"
	ret = set_clock(atoi(args[1]), atoi(args[2]), atoi(args[3]));
	// call sys_sppt's set_clock(hr, mn, s)
	// if set_clock returns -1 print error message
	if (ret == -1) {
	  printf("Error setting clock\n");
	}
  }
}
