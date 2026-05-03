/*
sys_xbox.c - misc xbox utils
Copyright (C) 2026 maximqad

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "build.h"

#if XASH_XBOX

#include <stdarg.h>
#include <string.h>
#include <hal/xbox.h>
#include <xboxkrnl/xboxkrnl.h>
// 16550 UART registers (COM1 base = 0x3F8)
#define UART_BASE       0x3F8
#define UART_THR        (UART_BASE + 0) // Transmit Holding Register
#define UART_IER        (UART_BASE + 1) // Interrupt Enable Register
#define UART_FCR        (UART_BASE + 2) // FIFO Control Register
#define UART_LCR        (UART_BASE + 3) // Line Control Register
#define UART_MCR        (UART_BASE + 4) // Modem Control Register
#define UART_LSR        (UART_BASE + 5) // Line Status Register
#define UART_DLL        (UART_BASE + 0) // Divisor Latch Low  (when DLAB=1)
#define UART_DLH        (UART_BASE + 1) // Divisor Latch High (when DLAB=1)

#define UART_LSR_THRE   0x20 // Transmitter Holding Register Empty

static inline void outb( unsigned short port, unsigned char val )
{
	__asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline unsigned char inb( unsigned short port )
{
	unsigned char val;
	__asm__ volatile ( "inb %1, %0" : "=a"(val) : "Nd"(port) );
	return val;
}

static int xbox_serial_initialized = 0;

/*
====================
Xbox_SerialInit
====================
*/
void Xbox_SerialInit( void )
{
	if( xbox_serial_initialized )
		return;

	outb( UART_IER, 0x00 ); // disable all interrupts
	outb( UART_LCR, 0x80 ); // enable DLAB (set baud rate divisor)
	outb( UART_DLL, 0x01 ); // divisor = 1 (115200 baud)
	outb( UART_DLH, 0x00 );
	outb( UART_LCR, 0x03 ); // 8 bits, no parity, 1 stop bit
	outb( UART_FCR, 0xC7 ); // enable FIFO, clear, 14-byte threshold
	outb( UART_MCR, 0x0B ); // IRQs enabled, RTS/DSR set

	xbox_serial_initialized = 1;
}

/*
====================
Xbox_SerialPutc
====================
*/
static void Xbox_SerialPutc( char c )
{
	int timeout = 100000;

	// wait for transmit buffer empty
	while( !( inb( UART_LSR ) & UART_LSR_THRE ) && --timeout > 0 )
		;

	outb( UART_THR, (unsigned char)c );
}

/*
====================
Xbox_SerialPrint
====================
*/
void Xbox_SerialPrint( const char *msg )
{
	if( !xbox_serial_initialized )
		Xbox_SerialInit();

	while( *msg )
	{
		if( *msg == '\n' )
			Xbox_SerialPutc( '\r' ); // CRLF for terminal compatibility
		Xbox_SerialPutc( *msg++ );
	}
}

void Platform_ShellExecute( const char *path, const char *parms )
{
	
}

/*
====================
Xbox_GetArgv

Check if we were relaunched with launch data (game switch via Sys_NewInstance).
If so, build synthetic argv with -game <gamedir> -changegame.
Returns new argc; *pargv is set to the new argv array.
If no launch data, returns original argc/argv unchanged.
====================
*/
int Xbox_GetArgv( int argc, char **argv, char ***pargv )
{
	static char *xbox_argv[8];
	static char gamedir_buf[256];
	unsigned long launchDataType;
	const unsigned char *launchData;

	if( XGetLaunchInfo( &launchDataType, &launchData ) == 0
		&& launchDataType == LDT_TITLE
		&& launchData[0] != '\0' )
	{
		strncpy( gamedir_buf, (const char *)launchData, sizeof( gamedir_buf ) - 1 );
		gamedir_buf[sizeof( gamedir_buf ) - 1] = '\0';

		xbox_argv[0] = "xash3d";
		xbox_argv[1] = "-game";
		xbox_argv[2] = gamedir_buf;
		xbox_argv[3] = "-changegame";
		xbox_argv[4] = NULL;

		*pargv = xbox_argv;
		return 4;
	}

	*pargv = argv;
	return argc;
}

#endif // XASH_XBOX
