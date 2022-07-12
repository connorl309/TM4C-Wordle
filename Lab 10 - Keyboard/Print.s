; Print.s
; Student names: Connor Leu and Victor Olugbusi
; Last modification date: change this to the last modification date or look very silly
; Runs on TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix
	PRESERVE8
    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
; R0=0,    then output "0"
; R0=3,    then output "3"
; R0=89,   then output "89"
; R0=123,  then output "123"
; R0=9999, then output "9999"
; R0=4294967295, then output "4294967295"
; MUST USE LOCALS

fp rn r11
count equ 0
num equ 4

LCD_OutDec
	push {r4-r9, r11, lr}
	sub sp, #8
	mov fp, sp
	
	cmp r0, #0 ; check if we are printing straight 0
	beq JustZero
	
	str r0, [fp, #num]
	mov r4, #0
	str r4, [fp, #count]
	mov r5, #10
Loop
	ldr r4, [fp, #num]
	cmp r4, #0
	beq Next
	udiv r6, r4, r5
	mul r7, r6, r5
	sub r8, r4, r7 ; r8 is remainder
	push {r8}
	str r6, [fp, #num]
	ldr r6, [fp, #count]
	add r6, #1
	str r6, [fp, #count]
	B Loop
Next 
	pop {r0}
	add r0, #0x30
	BL ST7735_OutChar
	ldr r6, [fp, #count]
	subs r6, #1
	str r6, [fp, #count]
	bne Next
donedone
	add sp, #8
	pop {r4-r9, r11, pc}
JustZero
	mov r0, #0x30
	BL ST7735_OutChar
	add sp, #8
	pop {r4-r9, r11, pc}

;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000"
;       R0=3,    then output "0.003"
;       R0=89,   then output "0.089"
;       R0=123,  then output "0.123"
;       R0=9999, then output "9.999"
;       R0>9999, then output "*.***"
; Invariables: This function must not permanently modify registers R4 to R11
num2 equ 0
thousands equ 4
remainder equ 8
hundreds equ 12
tens equ 16
ones equ 20
LCD_OutFix
	push {r4-r8, lr}
	sub sp, #24
	mov fp, sp
	str r0, [fp, #num2] ; save number on stack
	ldr r4, =9999
	cmp r0, #0
	beq AllZeros
	cmp r0, r4
	bhi TooBig
	
	; otherwise 3 or 4 digit number and we need to do some funny stuff
	mov r5, #1000
	udiv r4, r0, r5 ; get thousands digit
	str r4, [fp, #thousands] ; save on stack
	
	; hundreds = (r0 / 100) % 10
	mov r5, #100
	udiv r4, r0, r5
	mov r5, #10
	udiv r6, r4, r5
	mul r7, r6, r5
	sub r8, r4, r7 ; r8 is hundreds digit
	str r8, [fp, #hundreds]
	
	; tens = (r0 / 10) % 10
	mov r5, #10
	udiv r4, r0, r5
	mov r5, #10
	udiv r6, r4, r5
	mul r7, r6, r5
	sub r8, r4, r7 ; r8 is hundreds digit
	str r8, [fp, #tens]
	
	; ones = r0 % 10
	mov r5, #10
	udiv r6, r0, r5
	mul r7, r6, r5
	sub r8, r0, r7 ; r8 is hundreds digit
	str r8, [fp, #ones]
	
	; print!
	;
	
	ldr r0, [fp, #thousands]
	add r0, #0x30 ; ascii offset
	bl ST7735_OutChar
	
	mov r0, #'.'
	bl ST7735_OutChar
	
	ldr r0, [fp, #hundreds] ; hundreds place
	add r0, #0x30 ; ascii offset
	bl ST7735_OutChar
	
	ldr r0, [fp, #tens] ; tens place
	add r0, #0x30 ; ascii offset
	bl ST7735_OutChar
	
	ldr r0, [fp, #ones]
	add r0, #0x30 ; ascii offset
	bl ST7735_OutChar
	
	b done
;
; prints *.***
TooBig
	mov r0, #'*'
	bl ST7735_OutChar
	mov r0, #'.'
	bl ST7735_OutChar
	mov r0, #'*'
	bl ST7735_OutChar
	mov r0, #'*'
	bl ST7735_OutChar
	mov r0, #'*'
	bl ST7735_OutChar
	b done
;
; Prints 0.000
AllZeros
	mov r0, #'0'
	bl ST7735_OutChar
	mov r0, #'.'
	bl ST7735_OutChar
	mov r0, #'0'
	bl ST7735_OutChar
	mov r0, #'0'
	bl ST7735_OutChar
	mov r0, #'0'
	bl ST7735_OutChar
	b done
;
; restores everything
done
	add sp, #24
	pop {r4-r8, pc}
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *
     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
