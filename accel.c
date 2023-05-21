/**************************************************************

 a c c e l . h

 Accelerometer module source file

***************************************************************/

#include "Base.h"  // Basic definitions
#include "accel.h" // Header file for this module
#include "lcd.h" // Header file for this module

int32_t whois=0x00;
// Some functions need to be implemented as indicated on P3

/********** PUBLIC FUNCTIONS TO IMPLEMENT ************************
 Those functions will be used outside of lcd.c so there will
 be prototypes for them in lcd.h.
******************************************************************/

// Init the accelerometer SPI bus
void initAccel(void)
{
  // This code is requested in  P3.3 i P3.5
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Enables SPI1 clock
  SPI1->CR1 = (SPI1->CR1 & ~(SPI_CR1_DFF) & (~SPI_CR1_BR))
		  | SPI_CR1_MSTR
		  | SPI_CR1_CPHA
		  | SPI_CR1_CPOL
		  | ((0B011) * SPI_CR1_BR_0)
		  | SPI_CR1_SSM
		  | SPI_CR1_SSI
		  | SPI_CR1_SPE;
  (LIS_CS_PORT->OTYPER) &= ~LIS_CS_BIT; // Sets PE3 to push-pull output
  (LIS_CS_PORT->MODER) = ((LIS_CS_PORT->MODER) & (~(1 << (2 * LIS_CS_PAD + 1)))) | (1 << (2 * LIS_CS_PAD));
  // Sets PE3 to output mode (01)
  (LIS_CS_PORT->BSRR.H.set) |= LIS_CS_BIT; // Enable bit for hihl-level use

  /*
  // Enable accelerometer
  LIS_CS_PORT->BSRR.H.clear = LIS_CS_BIT;
  SPI1->DR = 0x20;      // bits 7 and 6 to off, we want to write to position 20.
  DELAY_US(2);          // Small delay before writing to the buffer
  while (!((SPI1->SR) & BIT0))
    ;
  SPI1->DR |= (1 << 6); // Enable PD in ctrl reg
  DELAY_US(2);          // Another small delay
  LIS_CS_PORT->BSRR.H.set = LIS_CS_BIT;*/

  // We may use writeAccel instead:
  whois = readAccel(0x0F, true);
  writeAccel(0x20, 0B01000111);
}

// WriteAccel: reg - register; mode: 1 - enable bit, 0 - disable bit; val: mask
int32_t writeAccel(int32_t reg, int32_t data)
{
  /*
  Writes a single bit to specified accelerometer register
  INPUTS:
  - Reg: register num. Ex - 0x20
  - BitNum: Integer of bit we want to modify. Ex - 6
  - Val: 1 or 0. 1 will enable bit and 0 disable it.
  Note: Function may return error value if register is reserved.
  */
  // Activates Chip Select
	if(checkRegisters(reg, 0)) return 1001;

   LIS_CS_PORT->BSRR.H.clear = LIS_CS_BIT;

   // Creates the command
   //  6 low bits: Register to read
   //      bit 6 : 0 Disabe autoincrement
   //      bit 7 : 1 Enable autoincrement
   //reg |= BIT(7);

   // Send the command
   SPI1->DR = reg;

   // Small delay before reading the buffer
   DELAY_US(2);

   // Wait to the receiver buffer to fill
   while (!((SPI1->SR) & BIT0))
     ;

   // Empty the receiver buffer
   // We don't need the returned information

   // Write dummy data to obtain a response
   SPI1->DR = data;

   // Small delay before reading the buffer
   DELAY_US(2);

   // Wait to the receiver buffer to fill
   while (!((SPI1->SR) & BIT0))
     ;

   // Deactivates Chip Select
   LIS_CS_PORT->BSRR.H.set = LIS_CS_BIT;
}

int32_t checkRegisters(int32_t reg, int32_t mode){
	/* Returns whether input register is available to read/write or not
	 * whois: accelerometer ID
	 * reg: register to check
	 * mode: 0 for write and 1 for read
	 * OUTPUTS:
	 * 0: OK
	 * 1: error
	 */
	if(whois==0x3F){
			//LIS3DSH
			 if (reg >= 0x0C && reg<=0x0F && mode==1)
					 return 0;
				 else if (reg >= 0x10 && reg<=0x17)
				 	return 0;
	       else if (reg >= 0x18 && reg <= 0x1A && mode==1)
				 	return 0;
				 else if (reg >= 0x1B && reg<=0x25)
					 return 0;
				 else if (((reg >= 0x27 && reg<=0x2D) || reg==0x2F) && mode==1)
				 	 return 0;
				 else if (reg == 0x2E)
				 	 return 0;
				 else if ((reg >= 0x40 && reg<=0x5B) && mode==0)
				 	 return 0;
				 else if ((reg >= 0x5C && reg<=0x5F) && mode==0)
				 	 return 0;
				 else if ((reg >= 0x60 && reg<=0x7B) && mode==0)
				 	 return 0;
				 else if ((reg >= 0x7C && reg<=0x7F) && mode==1)
				 	 return 0;
				 else
					 return 1;
		}
		else if(whois==0x3B){
			//LIS302DL
			if (reg ==  0x0F && mode==1)
				return 0;
			else if (reg >=  0x20 && reg<=0x22)
				return 0;
			else if (reg >=  0x20 && reg<=0x22)
				return 0;
			else if (reg == 0x23 && mode==1)
				return 0;
			else if (reg >=  0x27 && reg<=0x2D && mode==1)
				return 0;
			else if (reg >=  0x30 && reg<=0x38 && ~(0x31 || 0x35))
				return 0;
			else if((reg==0x31 || reg==0x35 || reg==0x39) && mode==1)
				return 0;
			else
				return 1;

		}
	else{
		if (reg ==  0x0F && mode==1) return 0;
		else return 1;
	}

}

void accelToLCD(void)
{
  int8_t dispChr = 0x2A;            // Character in LCD 0x2A
  int32_t beginAccelX, beginAccelY, // Stores value of accelerometer upon initialization
      sensibilityLcd = 5,           // Sensitivity of LCD: 7 digits to move 1 case
      accelValX, accelValY,
      delayBetweenMeasures = 2;           // Delay between takes in uS
  int32_t numArray = 5;                     // Number of values for mean. WARNING - represents 2 pow 5 = 32
  LCD_ClearDisplay();
  LCD_GotoXY(8, 0);
  LCD_SendChar(dispChr);
  LCD_GotoXY(8, 1);
  LCD_SendChar(dispChr);
  // Reads values of start
  beginAccelX = readAccel(0x29, true); // Reads X accel
  beginAccelY = readAccel(0x2B, true); // Reads Y Accel

  if (0)
  {
    while(1)
    {
      // Repeats forever
      accelValX = (readAccel(0x29, true) - beginAccelX); // Reads X accel
      accelValY = (readAccel(0x2B, true) - beginAccelY); // Reads Y Accel

      // Calculate LCD position: accelValX/sensitivityLcd
      LCD_ClearDisplay();
      LCD_GotoXY(8+(accelValX / sensibilityLcd), 0);
      LCD_SendChar(dispChr);
      LCD_GotoXY(8+(accelValY / sensibilityLcd), 1);
      LCD_SendChar(dispChr);
      SLEEP_MS(100); // Get values every 200 seconds to avoid unviewable data

      // Will be extended to a mean value another day
    }
  }
  else
  {
    int32_t i = 0;
    int32_t meanX;
    int32_t meanY;
    while (1)
    {
      for (i = 0; i < (1<<numArray); i++)
      {
        meanX += (readAccel(0x29, true) - beginAccelX); // Reads X accel
        meanY += (readAccel(0x2B, true) - beginAccelY); // Reads Y Accel
        DELAY_US(delayBetweenMeasures);
      }
      meanX >>= numArray; // Divide meanX by 2 pow numArray (32 in example)
      meanY >>= numArray; // Divide meanY by 2 pow numArray (32 in example)

      if(meanX > 35) meanX = 35;
      if(meanY > 35) meanY = 35;

      LCD_ClearDisplay();
      LCD_GotoXY(8+(meanX / sensibilityLcd), 0);
	  LCD_SendChar(dispChr);
	  LCD_GotoXY(8+(meanY / sensibilityLcd), 1);
	  LCD_SendChar(dispChr);
      SLEEP_MS(50); // Get values every 200 seconds to avoid unviewable data
    }
  }
}

/********** PUBLIC FUNCTIONS ALREADY IMPLEMENTED ***************
 Those functions are provided to reduce the codding effort
 for the accelerometer module
****************************************************************/

// Read an accelerometer register
//      reg   : Number of the register to read
//      sign  : If true, will read as int8_t (-128 a 127)
//              If false, will read as uint8_t (0 a 255)
//
// In case of error returns numbers greater than 1000
//             1001 : Try to access a reserved register

int32_t readAccel(int32_t reg, int32_t sign)
{
  int8_t val;      // Read value
  int8_t *psigned; // Pointer to signed value

  // Limit the register number to 6 bits 0..63
  reg &= 0x3F;

  // Although it is not mandatory for read,
  // we verify its is not a reserved register
  /*if (reg < 0x0F)
    return 1001;
  if ((reg < 0x20) && (reg > 0x0F))
    return 1001;
  if ((reg < 0x27) && (reg > 0x23))
    return 1001;
  if ((reg < 0x30) && (reg > 0x2D))
    return 1001;*/

  if(checkRegisters(reg, 1)) return 1001;


  // Activates Chip Select
  LIS_CS_PORT->BSRR.H.clear = LIS_CS_BIT;

  // Creates the command
  //  6 low bits: Register to read
  //      bit 6 : 0 Disabe autoincrement
  //      bit 7 : 1 Enable autoincrement
  reg |= BIT(7);

  // Send the command
  SPI1->DR = reg;

  // Small delay before reading the buffer
  DELAY_US(2);

  // Wait to the receiver buffer to fill
  while (!((SPI1->SR) & BIT0))
    ;

  // Empty the receiver buffer
  // We don't need the returned information
  val = (SPI1->DR);

  // Write dummy data to obtain a response
  SPI1->DR = 0xFF;

  // Small delay before reading the buffer
  DELAY_US(2);

  // Wait to the receiver buffer to fill
  while (!((SPI1->SR) & BIT0))
    ;

  // Recover the response in the reception buffer
  val = (SPI1->DR);

  // Deactivates Chip Select
  LIS_CS_PORT->BSRR.H.set = LIS_CS_BIT;

  // The obtained data will be uint8 (0..255)

  // Return the read value if sign is false
  if (!sign)
    return val;

  // If the data is signed we need to return in two's complement

  // Point to the returned value
  psigned = &val;

  // Return as signed value
  return *psigned;
}

// Converts from integer to string using the indicated radix
// Modified from:
// https://www.fooe.net/trac/llvm-msp430/browser/trunk/mspgcc/msp430-libc/src/stdlib/itoa.c
// Sign is considered for all radix

// Integer to string conversion in the given radix
//      num:   Number to convert
//      str:   Pointer to the string where the result should be located
//               the user should reserve the needed space fo the string.
//      radix: Radix to use. Typically it will be:
//                  2  Binary
//                  10 Decimal
//                  16 Hexadecimal
char *itoa(int32_t num, char *str, int32_t radix)
{
  int32_t sign = 0; // To remember the sign if negative
  int32_t pos = 0;  // String position
  int32_t i;        // Generic use variable

  // Save sign
  if (num < 0)
  {
    sign = 1;
    num = -num;
  }

  // Construct a backward string of the number.
  do
  {
    i = num % radix;
    if (i < 10)
      str[pos] = i + '0';
    else
      str[pos] = i - 10 + 'A';
    pos++;
    num /= radix;
  } while (num > 0);

  // Now add the sign
  if (sign)
    str[pos] = '-';
  else
    pos--;

  // Add the final null
  str[pos + 1] = 0;

  // Pos is now the position of the final digit (before null)

  // Now reverse the string
  i = 0;
  do
  {
    sign = str[i];
    str[i++] = str[pos];
    str[pos--] = sign;
  } while (i < pos);

  return str;
}
