/****************************************************

 l c d . c

 P2 LCD Functions

*****************************************************/

#include "Base.h" // Basic definitions
#include "lcd.h"  // LCD definitions

// Some of the function need to be completed
// as is requested on the manual

/***************** PRIVATE FUNCTIONS TO BE IMPLEMENTED *************
 In C, all functions are public by default.
 Those functions, as they are not needed outside of lcd.c
 won't have prototypes on lcd.h.
 They will also be declared static so that the compiler can
 optimize their usage knowing they are not used outside this file.
****************************************************************/

// Initializes DB7..DB4, RS, E i BL in Push-Pull output mode
// and sets the ouptuts to "0"
static void lcdGPIOInit(void)
{
   // This code is requested in P2.4
   // Remember that LCD is connected to port E from PE9 to PE15
   GPIO_ModePushPull(LCD_PORT, LCD_BL_PAD);
   GPIO_ModePushPull(LCD_PORT, LCD_E_PAD);
   GPIO_ModePushPull(LCD_PORT, LCD_RS_PAD);
   GPIO_ModePushPull(LCD_PORT, LCD_DB4_PAD);
   GPIO_ModePushPull(LCD_PORT, LCD_DB5_PAD);
   GPIO_ModePushPull(LCD_PORT, LCD_DB6_PAD);
   GPIO_ModePushPull(LCD_PORT, LCD_DB7_PAD);
}

void GPIO_ModePushPull(GPIO_TypeDef *port, int32_t linia)
{
   //(port->LCD_PORT->MODER)|=1<<(linia*2+1);

   (port->OTYPER) &= ~(1 << linia); //Sets select line to 0 (push-pull output)
   (port->MODER) = ((port->MODER) & (~(1 << (2 * linia + 1)))) | (1 << (2 * linia));
   //2 lines per port. Sets the two bits to 01

   (port->OSPEEDR) &= ~(0x3<<(linia*2)); // Reduces LCD latency by setting speed to 00 -> 2MHz
   (port->PUPDR) &= ~(0x3<<(linia*2));   //Sets no pull-up no pull-down to LCD pins
   (port->ODR) &= ~(1<<linia); //Sets PIN to 0 (off). Will be changed later with BSSR
}

// Send 4 bits to the LCD and generates an enable pulse
//     nibbleCmd : Bits 3..0 Nibble to send to DB7...DB4
//     RS        : TRUE (RS="1")   FALSE (RS="0")
static void lcdNibble(int32_t nibbleCmd, int32_t RS)
{
   // This code is requested in P2.6
   uint32_t i;
   if (RS)
      (LCD_PORT->BSRR.H.set) = BIT(LCD_RS_PAD); //Enables PIN_RS -> Set content to IR
   else
      (LCD_PORT->BSRR.H.clear) = BIT(LCD_RS_PAD); //Disables PIN_RS -> Set content to IR

   for (i = 0; i <= 3; i++)
   {
      // nibbleCMD is 4 bytes
      if ((nibbleCmd & (1 << i)) == 0)
      {
         // If nibbleCmd byte at position i is 1
         (LCD_PORT->BSRR.H.clear) = BIT(i + LCD_DB4_PAD); // Disable pin

         // Considering DB4 to DB7 are connected in ascending order
      }
      else
      {
         (LCD_PORT->BSRR.H.set) = BIT(i + LCD_DB4_PAD); // Enable pin
      }
   }
   /*Sets nibbleCmd valor to gpio outputs 15..12 as well as
   RS value to port 11
   */
   DELAY_US(10);                // Waits before pulse
   (LCD_PORT->BSRR.H.set) = BIT(LCD_E_PAD);   // Enables E (makes a pulse)
   DELAY_US(10);                // Waits while E is ON
   (LCD_PORT->BSRR.H.clear) = BIT(LCD_E_PAD); // Disables E
   DELAY_US(10);                // Waits another waitTimeNibble uS
}

/********** PUBLIC FUNCTIONS TO IMPLEMENT ************************
 Those functions will be used outside of lcd.c so there will
 be prototypes for them in lcd.h.
******************************************************************/

// Backlight configuration
//     on evaluates to TRUE   Turn on backlight
//     on evaluates to FALSE  Turn off backlight
void LCD_Backlight(int32_t on)
{
   // This code is requested in P2.5
   if (on)
   {
      (LCD_PORT->BSRR.H.set) = BIT(LCD_BL_PAD); // Enables LCD backlight if on is true
   }
   else
   {
      (LCD_PORT->BSRR.H.clear) = BIT(LCD_BL_PAD); // Disables LCD backlight if on is false
   }
}

// Clear the LCD and set the cursor position at 0,0
void LCD_ClearDisplay(void)
{
   // This code is requested in P2.7
   // Code in 2 parts: clear display and set cursor

   // Part 1 - clear display instruction to IR
   lcdNibble(0x0, 0); // RS is 0 as we write in instruction register
   lcdNibble(0x1, 0); // RS is 0 as we write in instruction register
   DELAY_US(2000);    // Sleep 1.5 + 0.1 = 1.6 micro seconds to ensure correct behaviour
}

// Configures the display
//     If Disp is TRUE turn on the display, if not, turns off
//     If Cursor is TRUE show the cursor, if not, hides it
//     If Blink is TRUE turn on blinking, if not, deactivate blinking
void LCD_Config(int32_t Disp, int32_t Cursor, int32_t Blink)
{
   // This code is requested in P2.7
   lcdNibble(0x0, 0);                                   // RS is 0 as we write in instruction register
   lcdNibble(0x8 | ((Disp!=0) << 2 | (Cursor!=0) << 1 | (Blink!=0)), 0); // RS is 0 as we write in instruction register
   DELAY_US(40);                                        // Sleep 40 micro seconds to ensure correct behaviour
}

// Set the cursor at the given position
//    col: Columnn (0..LCD_COLUMNS-1)
//    row: Row     (0..LCD_ROWS-1)
void LCD_GotoXY(int32_t col, int32_t row)
{
   // This code is requested in P2.7
   uint32_t pos; //Start of DDRAM address
   switch (row%(LCD_ROWS)){
   case 0:
      pos = 0;  //Line 1 begins at position 0
      break;
   case 1:
      pos = 64; //Line 2 begins at position 64
      break;
   case 2:
      pos = 19; //Line 3 begins at position 64
      break;
   case 3:
      pos = 83; //Line 4 begins at position 64
      break;
   default:
      pos = 0; //Won t happen, but just to be sure
      break;
   }
   pos += col%(LCD_COLUMNS); // Generates number for address pointer
   // We have to set an instruction to IR in order to set DDRAM adress
   lcdNibble(0x8 | pos >> 4, 0); // Write 1xxx to IR (1/2) as RS=0
   lcdNibble(pos, 0);     // Write xxxx to IR (2/2) as RS=0
   DELAY_US(40);                 // Sleep 40 micro seconds to ensure correct behaviour
}

// Send a character to the LCD at the current position
//     car: Charater to send
void LCD_SendChar(char car)
{
   // This code is requested in P2.7
   lcdNibble(car >> 4, 1);   // Set data to data register needs RS to 1
   lcdNibble(car, 1); // Set data to data register needs RS to 1
   DELAY_US(40); //Wait 40uS just to be sure...
}

// Send a string to the LCD at the current position
//     string: String to send
void LCD_SendString(char *string)
{
   // This code is requested in P2.7
   uint8_t i;
   for (i = 0; string[i] != '\0'; i++)
   {
      //While string is not ended
      LCD_SendChar(string[i]);//Sends single character to LCD
   }
}

void LCD_CustomChar(int32_t pos, uint8_t *data)
{
   uint8_t i,
   posBegin = (pos*8)%NUM_BITS_CGRAM; //Each custom char takes 7 positions. We begin at pos 8 x number
   //NUM_BITS_CGRAM
   //Set CGRAM address pointer in order to write content to custom position
   lcdNibble(0x4 | posBegin >> 4, 0); // RS is 0 as we write in instruction register
   lcdNibble(posBegin, 0);     // RS is 0 as we write in instruction register
   DELAY_US(40); //Wait 40uS just to be sure...

   // Now that CGRAM address counter is set to pos, we can write data enabling RS
   //We need to write each line to the address.
   //Each character has 7 lines of 5 rows. We send each line individually.
   //Each line fills a position.
   for (i = 0; i<=7; i++)
   {
      //While string is not ended
      LCD_SendChar(data[i]);//Sends single character to LCD
      //Will write in input data to data register of CGRAM.
   }
   //All data is now on CGRAM
   //Let s switch back to DDRAM to avoid problems
   LCD_GotoXY(0, 0);
   //Note that if a cursor was enabled, it will go to position 00.
   //If sendString or char is called without previous gotoXY, it will possibly overwrite existing data.
}

/********** PUBLIC FUNCTIONS ALREADY IMPLEMENTED ***************
 Those functions are provided to reduce the codding effort
 for the LCD module
****************************************************************/

// Init the LCD in 4 bits mode
// This function depends on the implementation of the
// private functions lcdSendNibble and lcdGPIOInit
void LCD_Init(void)
{
   // Initializes the LCD GPIOs
   lcdGPIOInit();

   // Wait 50ms for vdd to stabilize
   SLEEP_MS(50);

   // Init command 1
   lcdNibble(0x03, 0);
   SLEEP_MS(10); // Wait > 4.1ms

   // Repeat command
   lcdNibble(0x03, 0);

   // Wait > 100us
   DELAY_US(500);

   // Repeat command
   lcdNibble(0x03, 0);

   // Init command 2
   lcdNibble(0x02, 0);

   // Mode 4 bits 2 lines 5x8 chars
   lcdNibble(0x08, 0);
   lcdNibble(0x02, 0);
   DELAY_US(50);

   // Display on blink on and cursor on
   lcdNibble(0x00, 0);
   lcdNibble(0x0E, 0);
   DELAY_US(50);

   // Clear Display
   lcdNibble(0x00, 0);
   lcdNibble(0x01, 0);
   SLEEP_MS(5);

   // Cursor set direction
   lcdNibble(0x00, 0);
   lcdNibble(0x06, 0);
   DELAY_US(50);

   // Set ram address
   lcdNibble(0x08, 0);
   lcdNibble(0x00, 0);
   DELAY_US(50);

   // Send OK
   lcdNibble(0x04, 1);
   lcdNibble(0x0F, 1);
   DELAY_US(50);
   lcdNibble(0x04, 1);
   lcdNibble(0x0B, 1);
   DELAY_US(50);
}
