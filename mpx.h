/*
 *     file:    mpx.h
 *
 *     Header file for the MPX Operating System.
 *
 *     This file contains constant, structure and function
 *     prototypes for the MPX Operating System
 */

/* MPX System request numbers. */

#define  EXIT_CODE 0  /* Process requesting termination. code.    */
#define  CON  1       /* The console device - keyboard & monitor. */
#define  PRT  2       /* The printer device - LPT1.               */
#define  COM  3       /* The serial port - COM1.         
#define */

/* MPX System request types. */

#define  READ   0  /* Read from device. */
#define  WRITE  1  /* Write to device.  */
#define  WAIT   2  /* Semaphore P operation for device. */
#define  SIGNAL 3  /* Semaphore V operation for device. */


#define MAXSIZE  20    /* Size of the directory array. */

struct dirstruct {         /* Data type for a directory entry.        */
	char dirnam[9];        /* The name of a .mpx file.         */
	int  dirsiz;           /* The size of the file (in bytes). */
};

typedef struct dirstruct dir;  /* Use dir as the data typer name.     */

/**
 * PCB
 */

#define FREE          0
#define SYS_PROCESS   1
#define APP_PROCESS   2
#define READY         0
#define RUNNING       1
#define BLOCKED       2
#define NOT_SUSPENDED 0
#define SUSPENDED     1
#define STACK_SIZE  400

struct pcbstruct {
	struct pcbstruct * chain;
	struct pcbstruct * next;
	struct pcbstruct * prev;
	char name[9];
	short type;
	short priority;
	short state;
	short suspend;
	unsigned stack_ptr;
	unsigned stack[STACK_SIZE];
	unsigned loadaddr;
	unsigned parm_add; // added during mod because sys_call said so
	int mem_size;
};
typedef struct pcbstruct pcb;

/* Function prototypes. */

/* main.c */
int main(void);

/* comhan.c */
void comhan(void);                    /* The MPX/OS command handler.  */
int get_cmd(char args[]);
void cmd_version(void);
void cmd_date(char *[]);
void cmd_directory(void);
int cmd_stop(void);
void cmd_help(char *[]);
void cmd_prompt(char *[]);
void cmd_alias(char *[]);
void cmd_show(char *[]);
void cmd_allocate(char *[]);
void cmd_free(char *[]);

void cmd_load(char *[]);
void cmd_resume(char *[]);
void cmd_run(char *[]);
void cmd_suspend(char *[]);
void cmd_terminate(char *[]);
void cmd_setpri(char *[]);
void cmd_dispatch();

void sys_req(int,int,char *,int *);   /* MPX system request function. */
int  directory(dir *, int);           /* Support function to load the */
					  /* directory array.             */

/**
 * PCB.C
 */
pcb * search_pcb(pcb *, char[]);
pcb * get_pcb(pcb *);
int free_pcb(pcb *, pcb *);
int build_pcb(pcb *, char[] /*name*/, int /*type*/, int /*state*/,
	int /*suspend*/, int /*priority*/, unsigned /*_cs*/, unsigned /*_ip*/,
	unsigned /*_ds*/, unsigned /*psw*/);
int insert_pcb(pcb**, pcb *, int);
int remove_pcb(pcb**, pcb *);

/**
 * TestN.C
 */
void test1(void);
void test2(void);
void test3(void);
void test4(void);
void test5(void);

/**
 * Sys_sppt.c
 */
void sys_inti(void);
void sys_exit(void);
void interrupt dispatch(void);
void interrupt sys_call(void);

/**
 * Load.c
 */
int load(unsigned *,char []);

/*
 *   Global variable EXTERN directives.
 *
 *       These extern declarations allow the variables to be
 *       accessed from any source code file which includes
 *       this header file.  The memory space for the variables
 *       is declared in a *.c file.
 */
#define DIR_SIZE 20

extern dir direct[];  /* Array of directory entries -     see direct.c */
extern int directory(dir *direct, int dir_size);
extern pcb * pcb_list;
extern pcb * ready_queue;
extern pcb * io_init_queue;
extern pcb * cop; /* The currently operating process. */
extern unsigned sys_stack[];
extern unsigned sp_save; /* So that mod 4 can return to cmd_dispatch */