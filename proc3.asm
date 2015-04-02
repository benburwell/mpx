;*************************************************************************
;
;	MPX-PC  Test Process
;	Name:		PROC3
;	Description:	Test process
;	Module:		4
;
;************************************************************************


          CON	   EQU     01H
          PRT      EQU     02H
	  WRITE    EQU     01H

	  CR	   EQU	   0DH
	  LF	   EQU	   0AH

	  STACK    SEGMENT STACK
		   DB	   32 DUP ('STACK   ')   ; 256 BYTES
	  STACK    ENDS

	  DATA	   SEGMENT
	  MSG1	   DB	   'Proc3 dispatched. ',CR,LF,'$'
	  BUF      DB      70 dup(?)
          LEN      DW      20
	  DATA	   ENDS

	  CODE	   SEGMENT
		   ASSUME  CS:CODE,DS:DATA,ES:DATA,SS:STACK

	  MAIN	   PROC    FAR
	 START:    MOV	   AX,DATA   ; SET UP DATA SEGMENT
		   MOV	   DS,AX
		   MOV	   ES,AX     ; SET UP EXTRA SEGMENT

	; Set up stack for dummy sys_call

		   MOV     DX,OFFSET LEN
		   PUSH    DX	            ; LOAD LENGTH PARM
	           MOV	   DX,OFFSET MSG1
		   PUSH    DX               ; LOAD MESSAGE ADDRESS
		   MOV     DX,WRITE
		   PUSH    DX               ; LOAD OPERATION TYPE
		   MOV     DX,PRT
		   PUSH    DX               ; LOAD OPERATION NUMBER
		   MOV     DX,0AAH
		   PUSH    DX		; PUSH EXTRA BYTE TO MAINTAIN
		   MOV     DX,0ABH	; COMPATIBILITY WITH C ROUTINES
                   PUSH    DX

	LOOP:	   MOV	   DX,OFFSET MSG1
		   MOV	   AH,09H    ; PRINT STRING
		   INT	   21H

		   MOV	   CX,0       ; RUN ONLY ONCE
		   INT	   60H	      ; INT TO DISP
		   JMP     LOOP

	  MAIN	   ENDP
	  CODE	   ENDS
		   END	   START


