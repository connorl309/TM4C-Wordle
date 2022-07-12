// Key.h
// This software configures the piano keys and LaunchPad I/O
// Lab 6 requires a minimum of 4 keys, but you could have more
// Runs on TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 10/15/21 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#ifndef KEY_H
#define KEY_H
#include <stdint.h>
// Header files contain the prototypes for public functions
// this file explains what the module does

// **************Key_Init*********************
// Initialize piano key inputs, called once to initialize the digital ports
// Input: none 
// Output: none
void Key_Init(void);

// **************Key_In*********************
// Input from piano key inputs 
// Input: none 
// Output: 0 to 7 depending on keys
//   0x01 is just Key0, 0x02 is just Key1, 0x04 is just Key2, 0x08 is just Key3
//   bit n is set if key n is pressed
uint32_t Key_In(void);

// negative logic built-in SW 1 connected to PF4
// negative logic built-in SW 2 connected to PF0
// built-in red LED connected to PF3
// built-in blue LED connected to PF2
// built-in green LED connected to PF1
// Function parameter              LED(s)   PortF
#define DARK      0x00   // dark     ---    0
#define RED       0x01   // red      R--    0x02
#define BLUE      0x02   // blue     --B    0x04
#define GREEN     0x04   // green    -G-    0x08
#define YELLOW    0x05   // yellow   RG-    0x0A
#define SKYBLUE   0x06   // sky blue -GB    0x0C
#define WHITE     0x07   // white    RGB    0x0E
#define PINK      0x03   // pink     R-B    0x06

#define SW1 2
#define SW2 1
#define PF4             (*((volatile uint32_t *)0x40025040))
#define PF3             (*((volatile uint32_t *)0x40025020))
#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
#define PF0             (*((volatile uint32_t *)0x40025004))
  
//------------LaunchPad_Init------------
// Initialize Switch input and LED output
// Input: none
// Output: none
void LaunchPad_Init(void);

//------------LaunchPad_Input------------
// Input from Switches, 
// Convert hardware negative logic to software positive logic 
// Input: none
// Output: 0x00 none
//         0x01 SW2 pressed (from PF4)
//         0x02 SW1 pressed (from PF1)
//         0x03 both SW1 and SW2 pressed
uint8_t LaunchPad_Input(void);

//------------LaunchPad__Output------------
// Output to LaunchPad LEDs 
// Positive logic hardware and positive logic software
// Input: 0 off, bit0=red,bit1=blue,bit2=green
// Output: none
void LaunchPad_Output(uint8_t data);

#endif

