/*
 * main.c
 *
 *  Created on: 2016 Jul 26 12:35:55
 *  Author: AdrianK
 */




#include <DAVE.h>
#include <limits.h>
#include <stdio.h>

/**

 * @brief main() - Application entry point
 *
 * <b>Details of function</b><br>
 * This routine is the application entry point. It is invoked by the device startup code. It is responsible for
 * invoking the APP initialization dispatcher routine - DAVE_Init() and hosting the place-holder for user application
 * code.
 */
#define STARTBYTE 10 // start byte is always 10
#define FCS_BYTES 2 // amount of bytes for check sequence
#define READLENGTH 11 // amount of data coming from phone

//function declaration
int8_t check_fcs_equality(uint8_t *bytes, uint8_t length);
uint16_t get_checksequence(uint8_t *bytes, uint8_t length);
void convert_bytes_to_data(int16_t *data, uint8_t *bytes, uint8_t length);
void transmit_data(int16_t *data, uint8_t length);
void print(uint8_t *bytes, uint8_t length, uint8_t is_sorted);
extern void initialise_monitor_handles(void);


int main(void)
{
  DAVE_STATUS_t status;

  status = DAVE_Init();           /* Initialization of DAVE APPs  */
  initialise_monitor_handles();
  if(status == DAVE_STATUS_FAILURE)
  {
    /* Placeholder for error handler code. The while loop below can be replaced with an user error handler. */
    XMC_DEBUG("DAVE APPs initialization failed\n");
  }
  uint8_t read_data[READLENGTH];
  int16_t control_data[(READLENGTH - 1 - FCS_BYTES) / 2];
  uint64_t init_status = (UART_STATUS_t)UART_Init(&UART_0);
  if(init_status == UART_STATUS_SUCCESS)
  {
  	while(1)
    {
  		// receive raw data from connection
  		UART_Receive(&UART_0, read_data, READLENGTH);
  		print(read_data, sizeof(read_data), 2);
  		// set the startbyte on the first index of array
  	  	while(read_data[0] != STARTBYTE)
  	  	{
  	  		uint8_t firstByte = read_data[0];
  	  		for(int i = 0; i < READLENGTH - 1; i++)
  	  		{
  	  			read_data[i] = read_data[i + 1];
  	  		}
  	  		read_data[READLENGTH -  1] = firstByte;
  	  	}

  		if(check_fcs_equality(read_data, READLENGTH))
  		{
  			convert_bytes_to_data(control_data, read_data, READLENGTH);
  		}
     }
  }
  return 0;
}



void print(uint8_t *bytes, uint8_t length, uint8_t is_sorted)
{
	int i;
	for(i = 0; i < length; i++)
	{
		printf("%d ", bytes[i]);
	}
	if(is_sorted != 2)
	{
		printf("- %s\n", (is_sorted == 0 ? "sorted" : "unsorted"));
	}
	else
	{
		printf("\n");
	}
}

// converts data into raw byte array and transmits them to connected device
// *data: array on which the control data is saved
// length: length of data
void transmit_data(int16_t *data, uint8_t length)
{
    uint8_t send_data_length = (length * 2) + FCS_BYTES + 1, i, position = 1;
    uint8_t send_data[send_data_length];
    send_data[0] = 255;
    send_data[send_data_length - 1] = 255;
    send_data[send_data_length - 2] = 255;
    for(i = 0; i < length; i++)
    {
        send_data[position++] = (uint8_t) data[i];
        send_data[position++] = (uint8_t)(data[i] >> 8);
    }

    uint16_t one_complement_sum = get_checksequence(send_data, sizeof(send_data));
    one_complement_sum += 1;
    send_data[0] = 10;
    send_data[send_data_length - 1] = (uint8_t) one_complement_sum;
    send_data[send_data_length - 2] = (uint8_t)(one_complement_sum >> 8);

    UART_Transmit(&UART_0, send_data, sizeof(send_data));
}

// convert raw data bytes from connection to control data
// *data: array on which the control data is saved
// *bytes: raw data bytes
// length: length of bytes
void convert_bytes_to_data(int16_t *data, uint8_t *bytes, uint8_t length)
{
    int i;

    for(i = 1; i < length - 2; i += 2)
    {
        uint16_t help = (uint16_t)(bytes[i + 1] | (bytes[i] << 8));
        data[i / 2] = help < SHRT_MAX ? help : (USHRT_MAX - help + 1) * -1;
    }
}

// check if the bytes were sent correctly
// *bytes: raw data bytes
// length: length of bytes
//return: 0 if bytes were sent correctly otherwise 1
int8_t check_fcs_equality(uint8_t *bytes, uint8_t length)
{
    uint16_t one_complement = get_checksequence(bytes, length);
    one_complement += 1;
    int8_t first_byte = (int8_t) one_complement;
    int8_t second_byte = one_complement >> 8;
    return (first_byte == bytes[length - 2] && second_byte == bytes[length - 1]) ? 0 : 1;
}

// calculates the sum of the one complements of numbers in byte[]
// *bytes: raw data bytes
// length: length of bytes
uint16_t get_checksequence(uint8_t *bytes, uint8_t length)
{
    int i;
    uint16_t result = 255 * 8;

    for(i = 1; i < length - FCS_BYTES; i++)
    {
        result -= bytes[i];
    }

    return result;
}

