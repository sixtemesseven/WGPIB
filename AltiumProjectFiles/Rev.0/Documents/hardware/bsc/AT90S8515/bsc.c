/*
Program acts as a system GPIB controller using USB.
This is binary version.
Command syntax:
IB<CR>
	Powers on.
	Returns: <ACK>
IBC<cmd><CR> or IBc<cmd><CR>
	Sends <cmd> byte as bus command.
	C: Releases ATN, c: does not release ATN.
	Returns: <ACK>, 1, 2, 8
IB<DLE><STX><data bytes><DLE><ETX>
	Sends data.
	EOI is set with the last byte if Write Mode is 0-3.
	<DLE> in data bytes should be replaced with <DLE><DLE>.
	<DLE><ACK> can be sent in data sequence (but not before first byte). The controller will answer with ACK.
	Returns: <ACK>, 1, 2, 8
IB?<CR>
	Receives data. Format:
	<DLE><STX><data bytes><DLE><ETX>
	<DLE> in data bytes is replaced with <DLE><DLE>.
	Operation can be interrupted by sending <ESC>. In this case transfer will be normally ended with <DLE><ETX>.
	Returns: <ACK>, 3, 9
IBB<CR>
	Receives 1 byte. (Useful for seriall polling).
IBZ<CR>
	Performs interface clear.
	Returns: <ACK>
IBO<CR>
	Switches the controller to off state.
	Sets timeouts and Write Mode to deafult state.
	Returns: /
	
New commands in V2.3:
IBe<Write Mode><CR>
	Sets Write Mode (decimal value). Default: 0.
	0-3 - EOI will be asserted with the last byte send.
	4-7 - EOI will not be asserted with the last byte send.
	Returns: <ACK> or <NAK> if the number is invalid.
IBt<TimeOut><CR>
	Sets Handshake timeout (decimal value) in increments of 32.768 ms.
	Handshake timeout is disabled when Total Timeout is specified.
	Valid range is 2-255 (32-8323 ms). Use 0 to disable.
	Default: 30 (1 sec)
	Returns: <ACK>
IBT<TimeOutTot><CR>
	Sets total timeout for write and read (decimal value) in increments of 32.768 ms.
	Transfer will not be interrupted during handshake, i.e. no bytes will be lost.
	Total timeout is disabled for read until specified.
	Valid range is 2-65535 (32-2,147 s). Use 0 to disable.
	Default: 0 (disabled)
	Returns: <ACK>
IBm<ren><CR>
	Valid values of ren: 0, 1. Default: 1
	1 asserts REN, 0 leaves REN unasserted.
	
New commands in V2.4:
IBf<TimeOutFirst><CR>
	Sets timeout for transfer of first byte for write and read (decimal value) in increments of 32.768 ms.
	Valid range is 2-65535 (32-2,147 s). Use 0 to disable.
	Default: 30 (1 sec)
	Returns: <ACK>
IBS<CR>
	Returns state of GPIB control lines as a byte (without ACK).
	Bits order:
		7	6	5	4	3		2		1	0
		SRQ	ATN	EOI	DAV	NRFD	NDAC	IFC	REN
	Note that lines are active zero.
IBQx<CR>
	Enable (x=1) or disable (x=0) SRQ interrupt.
	If the interrupt is enable, the controller will send ENQ each time SRQ is asserted.
	ENQ will also be sent, if SRQ is asserted when the interrupt enable command is received.
	ENQ can only be sent while the controller is idle (not during data transfer).
	Returns: <ACK>
	
Note: At least one timeout should always be enabled.


Debug commands; used to directly acces lines on the bus.
All data send and received are in binary format.
Debug mode should be first enabled, otherwise commands are not recognized.

IBDE\AA<CR>
	Enables debug mode.
	Returns: <ACK>
IBDTx<CR>
	Sets talk/listen to listen (x=0) or talk (x=1).
	Returns: <ACK>
IBDCx<CR>
	Puts x on control bus.
	Bits order:
		7	6	5	4	3	2	1	0
		SRQ	ATN	EOI	DAV	NRFD	NDAC	IFC	REN
	Returns: <ACK>
IBDMx<CR>
	Puts inverted x on a data (D7-D0) bus.
	Returns: <ACK>
IBD?x<CR>
	Reads control (x=C), data (M) or both buses (x=?).
	Return: one or two bytes only (without ACK).


Returns:
	Interface will return 1 byte after each command.
	<ACK>		Normal execution
	<NAK>		Command syntax error
	0x01		Not Ready
			Timeout occured before NRFD become false.
	0x02		Not Accepted
			Timeout occured before byte was accepted.
	0x03		Not DAV Released
			Timeout occured before DAV was released.
	0x08		No Listeners
			Timeout occured before NRFD or NDAC become true.
	0x09		No Data
			Timeout occured before reception (DAV edge) of a byte.
	Note: All timeouts applies to byte-byte basis.

Hardware: AT90S8515 8 MHz, FT245, SN75160/161, Rev V1.0 & Rev V1.1
V2.4.1
Bostjan Glazar, LSD, FE, November 2006
Tested 10.2005 with HW 1.0 & 1.1

Code: 1592 W, Const.: 54 W

/********
Updates in V2.4.1 version Nov. 2006:

- T1 delay updated to standard.


/********
Updates in V2.4.0b version Sep. 2005:

- Each timeout can now be disabled.
- IBf, IBQ and IBS commands added.
- Handshake timeout not used any more for write while waiting for listener.
- Reading of one byte (IBB) corrected to return null in case of timeout.


/********
Updates in V2.3.0b (Beta) version Mar. 2005:

- Interruption of data received corrected.
- Corrected bug with releasing REN on timeout.
- Added total timeout.
- Added option for Write Mode (disabling EOI).
- Added ACK during write.
- Compacted code in SendBinData function.
- Debug commands are now standard repertory.
- Operation with variable flags corrected.

               
/********
Updates in V2.2.1b (Beta) version Oct. 2004:

- Powering ON will now assert REN line.
  Because of problems with some devices.
  Code: 1086 W, Const.: 50 W


/********
Updates in V2.0.1a version Dec. 2003:

- Previous version released.


/********
Updates in V0.8.1b (Beta) version Aug. 2003:

- Added receiving of 1 byte (for seriall pulling).
- Added escape option on receive.


/********
Updates in V0.8a (Alpha) version:

- Binary mode introduced.
  Any byte can be sent or received. ASCII mode is no longer provided.
- Commands are send as codes and not as mnemonics.
- All responses are 1 byte long what simplifies driver.


/********
Updates in V0.7.2 Alpha version:

- USB suspend added (will power off)
- IBIDN, IBIFC, IBL, IBUNL, IBGET, IBSDC, IBDCL commands added
- REN line is no longer used
- On Data read, OK response is added. CR preceeds ERROR on no data to prevent
  missinterpreting ERROR response as data. Normally LF or CR from device will
  be sent before OK string.

/********
Updates in V0.7.1 Alpha version:

- Receiving capability added.
- SetTalk and SetListen routins does not controll DC any more.
  (It is always 0.)
- SendCmd wait for NRFD or NDAC to go low after ATN=0 instead of waiting for
  a while. They can also set NoListener.
- Repaired a bug in receiving a command.

- SendDataCR renamed to SendData.
  Another routine with parameter (number of bytes) can be added with
  the same name.
- Command and error branching changed.

/********

FTDI Info:
Product description: GPIB Controller
Consumption: 320 mA

*/

#include <90s8515.h>
#include <stdlib.h>
#include <ctype.h>
#include <delay.h>
#include <sleep.h>
#include <string.h>

// ASCII characters
#define NUL 0
#define SOH 1
#define STX 2
#define ETX 3
#define EOT 4
#define ENQ 5
#define ACK 6
#define BEL 7

#define DLE 16
#define NAK 21
#define ETB 23
#define CAN 24
#define ESC 27

// Port connection
#define PORTUSB PORTB
#define DDRUSB DDRB
#define PINUSB PINB
#define PORTIB PORTA
#define PINIB PINA
#define PORTIBctrl PORTC
#define PINIBctrl PINC
#define DDRIB DDRA
#define DDRIBctrl DDRC

// PORTD lines (used to control hardware)
#define TE PORTD.7		// Talk Enable, O
#define DC PORTD.6		// Direction control, O
#define PWR PORTD.5		// Power, inverted O
#define TXE PIND.4		// Transmitt FIFO empty, inverted I
#define RXF PIND.3		// Receive FIFO empty, inverted I
#define PWREN PIND.2	// Power enable from USB, inverted I
#define WR PORTD.1		// Write to USB, O
#define RD PORTD.0		// Read to USB, inverted O

// GPIB controll lines
#define SRQout PORTC.7	// Service Request
#define SRQin PINC.7
#define ATNout PORTC.6	// Attention
#define ATNin PINC.6
#define EOIout PORTC.5	// End Or Identity
#define EOIin PINC.5
#define DAVout PORTC.4	// Data Valid
#define DAVin PINC.4
#define NRFDout PORTC.3	// Not Ready For Data
#define NRFDin PINC.3
#define NDACout PORTC.2	// Not Data Accepted
#define NDACin PINC.2
#define IFCout PORTC.1 	// Interface Clear
#define IFCin PINC.1
#define RENout PORTC.0	// Remote Enable
#define RENin PINC.0

// Error and sleep bits; variable brk
#define NotAccBrk 0x01  // Not Accepted Break; Listener(s) didn't release NDAC.
#define NotRdyBrk 0x02  // Not Ready Break; Listener(s) were not ready in time. (NRFD pulled low)
#define NoLstn 0x03  	// No Listener; NRFD and NDAC were sensed high
#define NoData 0x04  	// No Data received: Timer expired while waiting for data
#define NotDAVrel 0x05  // DAV line not released: Timer expired while waiting for DAV line to be released during data reception
#define DataFrmtErr 0x06  // DLE not followed by DLE or ETX
#define TimOutBrk 0x40  // Timout Break; Timer expired
#define SleepBrk 0x80  // Sleep Break; USB went into suspend mode

// Variable flags
#define PCByteRdy 0x01  // Byte in PCDat is valid
#define XmtBlkBrk 0x02  // Transmitting data; Used to wait for DLE ETX on error
#define RenState 0x04	// REN state; new in V2.3
#define UseFirst 0x08	// Use TMaxFirst instead of TMax for timeout

#define DDRtalk 0x73   // 01110011
#define DDRlstn 0x4f   // 01001111
#define DDRcomm 0x43   // 01000011
#define DDR_EOI DDRIBctrl.5

// SRQst bits - SRQ status variable - used for SRQ interrupt
#define SRQstate 0x01	// Current state of SRQ line, 1 means active
#define SRQen 0x02		// SRQ interrupt enable flag

#define TMax_def 30			// Number of timer interrupts for timeout between bytes, 30x32.768ms= 1 s
#define TMaxTot_def 0		// Number of timer interrupts for total timeout, Nx32.768ms  // New code V2.3, Total timeout disabled by default
#define TMaxFirst_def 30	// Number of timer interrupts for timeout before the first byte, 30x32.768ms= 1 s	// New V2.4

#define InstrMax 10		// Maximum length of command (incl. CR)

#define Receive 1

const unsigned char StrIDN0[]="USB GPIB Controller\r\n";
const unsigned char StrIDN1[]="B.G., LPVO, FE, Slovenia\r\n";
const unsigned char StrIDN2[]="HW V1.0, August 2003, FW V2.4.1, November 2006\r\n";

const unsigned char StrDataSend[]={DLE, STX};

unsigned char timer=0, brk=0, flags, PCDat;
unsigned int timer_tot=0;
	// timer is incremented on every TOV0 (ca. 4 ms)
	// brk (break) - status variable for interrupts

// Current timeouts; unit: number if timer interrupt	
unsigned char TMax;		// Byte timeout
unsigned int TMaxTot;		// Total timeout
unsigned int TMaxFirst;	// First byte timeout

//Routine increments timer and sets brk if reaches TMax or TMaxFirst, depending on UseFirst flag.
//The flag is reset when timer is detected zero (first byte transfered).
//timer_tot is similarly incremented and compared to TMaxTot
interrupt [TIM0_OVF] void timer0_ovf(void){
//	if(++timer==TMax) brk|=TimOutBrk;
//	if(++timer_tot==TMaxTot) brkT|=TimOutBrk;  		// New code V2.3; Total timeout
	if(!timer) flags&=~UseFirst;
	if((~flags&UseFirst) && TMax) if(++timer==TMax) brk|=TimOutBrk;		// Modified code V2.4
	if((flags&UseFirst) && TMaxFirst) if(++timer==TMaxFirst) brk|=TimOutBrk;		// Modified code V2.4
	if(TMaxTot) if(++timer_tot==TMaxTot) brk|=TimOutBrk;
}

//Routine sets brk when USB goes to suspend state.
interrupt [EXT_INT0] void ext_int0(void){
	brk|=SleepBrk;
//	brkT|=SleepBrk;  // New code V2.3; Total timeout
}

//Routine is called on first received byte after power-down.
interrupt [EXT_INT1] void ext_int1(void){
	TIMSK=0x02;  // Enable timer interrupts
	GIMSK=0x40;  // Disable ext. int.
	TCNT0=0;
	timer=0;
}


//Routine sets DDRs and 75160/161 to talk mode.
void SetTalk(void){
	DDRIBctrl=DDRcomm;
	TE=1;
	DDRIBctrl=DDRtalk;
	DDRIB=0xff;
}

//Routine sets DDRs and 75160/161 to listen mode.
void SetListen(void){
	DDRIB=0;
	DDRIBctrl=DDRcomm;
	TE=0;
	DDRIBctrl=DDRlstn;
}


/*
Routine send command to GPIB
It can be interrupted with variable brk.
If brk is set it is ORed with bit according to state (NRFD or DAV wait).
Talk mode should be set prior to call.
ATN is not relesed at normal operation. If brk is set, routine releases ATN.
*/
unsigned char SendCmd(unsigned char cmd){
	ATNout=0;
	PORTIB=~cmd;
	while(NDACin&&NRFDin) if(brk) {brk|=NoLstn; goto Ret;}
	timer=0;
	while(!NRFDin) if(brk) {brk|=NotRdyBrk; goto Ret;}
	DAVout=0;
	while(!NDACin) if(brk) {brk|=NotAccBrk; goto Ret;}
	DAVout=1;
	return brk;
Ret:
	ATNout=1;
	return brk;
}


/*
This routine send a byte to PC through USB. It returns if brk is set.
*/
unsigned char SendPCChr(char byte){
	DDRUSB=0xff;
	PORTUSB=byte;
	while(TXE) {timer=0; if(brk) goto Brk;}
	timer=0;
	WR=1;
	WR=0;
Brk:
	DDRUSB=0;
	return brk;
}


/*
Routine send string terminated with null to the PC
If brk is set during transfer, the later is interrupted and brk returned.
*/
unsigned char SendPCStr(char flash str[]){
	unsigned char i;
	DDRUSB=0xff;
	for(i=0;str[i];i++){
		PORTUSB=str[i];
		while(TXE) {timer=0; if(brk) goto Brk;}
		timer=0;
		WR=1;
		WR=0;
	}
Brk:
	DDRUSB=0;
	return brk;
}



/*
This routine sends binary data from PC (USB) to GPIB bus.
Data should begin without header.
DLE should be followed by another DLE. Transfer is ended with DLE STX.
eoi different from 0 signals to send EOI with the last byte.
Returns global variable brk which can also be set to
NoLstn, DataFrmtErr, NotRdyBrk, NotAccBrk.
Transfer will also be interrupted by setting brk (timeout).
Routine resets timer variable for each byte after first.
Initially timer is set to 1 and UseFirst flag is set to enable usage of TimeOutFirst.
GPIB and USB direction should be set before calling.
*/
unsigned char SendBinData(int eoi){
	unsigned char PCDat;
	timer_tot=0;		// BrkT deleted in V2.4
	timer=1;
	flags|=UseFirst; TMax++;	// Use timeout for first byte initially
	while(NDACin&&NRFDin) if(brk) {brk|=NoLstn; return brk;}			// Wait for listener

SendBinData1:
	while(RXF) {timer=1; if(brk) return brk;}						// Wait for data from PC
	RD=0;
	timer=1;											// Changed to 1 in V2.4
	PCDat=PINUSB; RD=1;

	if(PCDat==DLE){
		while(RXF) {timer=1; if(brk) return brk;}	// Changed to 1 in V2.4
		RD=0;
		timer=1;										// Changed to 1 in V2.4
		PCDat=PINUSB; RD=1;
		if(PCDat==DLE)
			PORTIB=~PCDat;
		else if(PCDat==ETX){
			flags&=~XmtBlkBrk;
			return brk; 
		}
		else if(PCDat==ACK){
			if(brk=SendPCChr(ACK)) return brk;
			goto SendBinData1;
		}
		else{
			brk|=DataFrmtErr;
			return brk;
		}
	}
	else
		PORTIB=~PCDat;

	while(1){
		while(RXF) {timer=!!(flags&UseFirst); if(brk) return brk;}
		RD=0;
		#asm("nop");
//
		PCDat=PINUSB; RD=1;

		if(PCDat!=DLE){
			while(!NRFDin) if(brk) {brk|=NotRdyBrk; return brk;}
			DAVout=0;
			while(!NDACin) if(brk) {brk|=NotAccBrk; return brk;}
			DAVout=1;
			PORTIB=~PCDat;
		}

		else{
			while(RXF) {timer=!!(flags&UseFirst); if(brk) return brk;}
			RD=0;
			#asm("nop");
//			timer=0;							// moved to end of loop in order to use different timeout after first byte
			PCDat=PINUSB; RD=1;

			// Modified and new code V2.3: compacted, ACK
			if(PCDat==ETX)
				if(eoi) EOIout=0;

			while(!NRFDin) if(brk) {brk|=NotRdyBrk; return brk;}
			DAVout=0;
			while(!NDACin) if(brk) {brk|=NotAccBrk; return brk;}
			DAVout=1;

			if(PCDat==DLE)
				PORTIB=~PCDat;
			else if(PCDat==ETX){
				flags&=~XmtBlkBrk;
				break;
			}
			else if(PCDat==ACK){
				if(brk=SendPCChr(ACK)) return brk;
				goto SendBinData1;
			}
			else{  // ERROR
				brk|=DataFrmtErr;
				flags&=~XmtBlkBrk;
				break;
			}
		}  // else DLE
		timer=0;		// Moved V2.4

	}  // while(1)
	EOIout=1;
	return brk;
} // SendBinData



/*
This routine receives data from GPIB bus.
Data is send is BSC protocol as above, however the routine send DLE STX at the beginning.
As SendBinData this routine also uses two timeouts between bytes.
Routine normally return 0. On error it returns the value of brk variable.
Data transfer can be interrupted by sending ESC from the PC. Reception of this character
is only monitored when the routine has nothing to do.
*/
unsigned char RcvBinData(){
	unsigned char eoi;
	NDACout=0;
	ATNout=1;
	DDRUSB=0xff;  // USB port is output

	PORTUSB=DLE;  // Send DLE, STX
	while(TXE) {timer=0; if(brk) goto Brk1;}  // wait for not full FIFO
	WR=1; WR=0;
	PORTUSB=STX;
	while(TXE) {timer=0; if(brk) goto Brk1;}  // wait for not full FIFO
	WR=1; WR=0;
	timer=1;
	flags|=UseFirst; TMax++;	// Use timeout for first byte initially
	timer_tot=0;  // BrkT deleted in V2.4

	do{
		while(TXE) {
			timer=!!(flags&UseFirst); if(brk) goto Brk;  // wait for not full FIFO
			if(brk) goto Brk;  // wait for not full FIFO
			if(!RXF) if(~flags&PCByteRdy){  // Check for escape code
				DDRUSB=0x00;  // USB port is input   // Corrected V2.3
				RD=0;
				#asm("nop");
				PCDat=PINUSB; RD=1;
				DDRUSB=0xff;   //
				if(PCDat==ESC) goto Brk;
				else flags|=PCByteRdy;
			}
		}
		NRFDout=1;  // Ready for data
		while(DAVin){
			if(brk) {brk|=NoData; goto Brk;}  // Wait for data
			if(!RXF) if(~flags&PCByteRdy){  // Check for escape code
				DDRUSB=0x00;  // USB port is input   // Corrected V2.3
				RD=0;
				#asm("nop");
				PCDat=PINUSB; RD=1;
				DDRUSB=0xff;   //
				if(PCDat==ESC) goto Brk;
				else flags|=PCByteRdy;
			}
		}
		PORTUSB=~PINIB; WR=1; WR=0;  // Accept and send data
		eoi=PINIBctrl&0x20;  // Save EOI
		NRFDout=0;  // Not ready for more data
		NDACout=1;  // Data received

		if(PORTUSB==DLE){  // send another DLE after DLE
			while(TXE) {timer=0; if(brk) goto Brk;}  // wait for not full FIFO
			WR=1; WR=0;
		}
		timer=0;

		while(!DAVin) if(brk) {brk|=NotDAVrel; goto Brk;}  // Wait for DAV rel.
		NDACout=0;  // Data not accepted (no data on bus)
	}while(eoi);  // Finish when EOI is active

Brk:
	if(!(brk&SleepBrk)){  // Send DLE, ETX
		PORTUSB=DLE;
		while(TXE) {timer=0; if(brk) goto Brk1;}  // wait for not full FIFO
		WR=1; WR=0;
		while(TXE) {timer=0; if(brk) goto Brk1;}  // wait for not full FIFO
		PORTUSB=ETX;
		WR=1; WR=0;
	}
Brk1:
	DDRUSB=0;
	return brk;
} // RcvBinData


/*
This routine reads only 1 byte from the GPIB bus and sends it to the PC in binary form.
The routine returns brk, which is normally zero.
*/
unsigned char RcvBinByte(){
	NDACout=0;
	ATNout=1;
	DDRUSB=0xff;  // USB port is output

	while(TXE) {timer=0; if(brk) goto Brk;}  // wait for not full FIFO
	timer=0;
	NRFDout=1;  // Ready for data
	while(DAVin) if(brk) {brk|=NoData; goto Brk;}  // Wait for data
	PORTUSB=~PINIB; WR=1; WR=0;  // Accept and send data
	NRFDout=0;  // Not ready for more data
	NDACout=1;  // Data received

Brk:
	if(brk&~SleepBrk){  // NULL in case of no data
		PORTUSB=0; WR=1; WR=0;
	}
Brk1:
	NRFDout=1;
	DDRUSB=0;
	return brk;
} // RcvBinByte





void main(){

unsigned char PCstr[InstrMax+1]; // This string holds received command, excluding IB header
unsigned char i; //,state;

unsigned char debug;		// Debug flag

unsigned char WMode;		// New code V2.3; GPIB write mode; 0-3 send EOI, 4-7 do not send EOI

// Currently set timeouts. These variables are copied to alike named without _set at start of a command.
unsigned char TMax_set;	// Byte timeout
unsigned int TMaxTot_set;	// Total timeout

unsigned char SRQst;		// Flags for SRQ operation. Definition at the beginning

TMax_set=TMax_def;   		// Set timeouts to default state
TMaxTot_set=TMaxTot_def;
TMaxFirst=TMaxFirst_def;

WMode=0;					// Default write mode 

flags=0;					// REN asserted by default

debug=0;					// We are not in debug mode (yet)

SRQst=0;					// SRQ interrupt is disabled

DDRIB=0;					// GPIB interface is in listen mode
DDRIBctrl=0;
PORTD=0x2d;
DDRD=0xe3;

// Configure external interrupts
MCUCR=0x13;  // INT0 (RXF) on low level, INT1 (PWREN)

sleep_enable();
brk=0;
#asm("sei");   //enable interrupts


if(RXF){
	GIMSK=0x80;  // Enable wake-up on RXF
	powerdown();
}

// Timer0 initialisation
TCCR0=0x05;  // CK/1024/256= 32.768 ms interrupt
TIMSK=0x02;  // enable TOV0
GIMSK=0x40;  // Enable PWREN int.


while(1){
Start:
TMax=0;					// Disable timeouts, V2.4
TMaxTot=0;

//timer=0; timer_tot=0;

for(i=0;i<3;){    				// wait for "IB"
	if(!(flags&PCByteRdy)){	// Skip waiting for a byte if it was read before (during previous read command).
		while(RXF){
			if(brk) goto Brk;  // wait for byte from USB
			if(SRQin) SRQst&=~SRQstate;		// Check SRQ state and send ENQ if newly set to the PC
			else{
				if(SRQst==0x02) if(SendPCChr(ENQ)) goto Brk;
				SRQst|=SRQstate;
			}
		}
		RD=0;
		#asm("nop");
		PCDat=PINUSB; RD=1;  	// read byte
    }    
    flags&=~PCByteRdy;			// Clear the "byte waiting" flag
	switch(i){
		case 0:
			if(PCDat=='I') i++;  // check for letter I
			else if(PCDat=='\r'||PCDat=='\n'||PCDat==ETX||PCDat==ESC) i=0;
			else i=2;
			break;
		case 1:
			if(PCDat=='B') i=3;  // check for letter B
			else if(PCDat=='\r'||PCDat=='\n'||PCDat==ETX) i=0;
			else i=2;
			break;
		default:
			if(PCDat=='\r'||PCDat=='\n'||PCDat==ETX) i=0;  // wait for CR (or LF) after errored beginning
	}
}


for(i=0;i<InstrMax;i++){   		// get cmd w/o param.
	while(RXF) if(brk) goto Brk;  // wait for byte from USB
	RD=0;
	#asm("nop");
	PCDat=PINUSB; RD=1;  			// read byte

	
	if(PCDat=='\n') PCDat='\r';  //replace LF /w CR
	PCstr[i]=PCDat;
//	if((PCDat=='\r'||PCDat==STX)&& (i!=1 || PCstr[0]!='c' && PCstr[0]!='C')){  //  The condition after && allows any character after first C to be send thus enabling any bus command.
	if((PCDat=='\r'||PCDat==STX)&& (i!=1 || PCstr[0]!='c' && PCstr[0]!='C') || (i==3 && PCstr[0]=='D')){ // Aditional ORed condition allows any character in debug mode. String is 3 characters long incl. CR.
		PCstr[i+1]=0;
		break;
	}
}


if(i>=InstrMax){   		// Check that command is not too long
	if(SendPCChr(NAK)) goto Brk;
}


else if(PCstr[0]=='O'){	// Power OFF
	DDRUSB=0;
	DDRIB=0;
	DDRIBctrl=0;
	PORTIB=0;    			// disables pull-ups on GPIB
	PORTIBctrl=0;
	DC=0; TE=0;
	PWR=1;  				// power off

	GIMSK=0x80;  			// Enable wake-up on RXF
	TIMSK=0;
	powerdown();
	brk=0;
	
	TMax_set=TMax_def;   	// New code V2.3
	TMaxTot_set=TMaxTot_def;
	WMode=0;				// Default write mode 
//	ntotr=1;
	flags&=~RenState;

	goto Start;
}

// New code V2.3 set timeouts
else if(PCstr[0]=='t'){			// byte timeout
	TMax_set=atoi(PCstr+1);
	if(SendPCChr(ACK)) goto Brk;
	goto Start;
}
else if(PCstr[0]=='T'){			// Total timeout
	TMaxTot_set=atoi(PCstr+1);
	if(SendPCChr(ACK)) goto Brk;
//	ntotr=0;
	goto Start;
}
// Special timeout for first byte V2.4
else if(PCstr[0]=='f'){
	TMaxFirst=atoi(PCstr+1);
	if(SendPCChr(ACK)) goto Brk;
	goto Start;
}


else if(PCstr[0]=='e'){		// Set write mode
	i=atoi(PCstr+1);
	if(i<=7){
	 	if(SendPCChr(ACK)) goto Brk;
	 	WMode=i;
	}
	else
		if(SendPCChr(NAK)) goto Brk;
	goto Start;
}
else if(PCstr[0]=='m'){		// Set REN state
 	if(PCstr[1]=='0'){
 	 	flags|=RenState;
	 	if(SendPCChr(ACK)) goto Brk;
 	}
 	else if(PCstr[1]=='1'){
 	 	flags&=~RenState;
	 	if(SendPCChr(ACK)) goto Brk;
 	}
	else
		if(SendPCChr(NAK)) goto Brk;
	goto Start;
}
// End of new code

else if(PCstr[0]=='Q'){		// Enable/disable SRQ interrupt
 	if(PCstr[1]=='0'){
		SRQst=0;
	 	if(SendPCChr(ACK)) goto Brk;
 	}
 	else if(PCstr[1]=='1'){
		SRQst=SRQen;
	 	if(SendPCChr(ACK)) goto Brk;
 	}
	else
		if(SendPCChr(NAK)) goto Brk;
	goto Start;
}


else if(PCstr[0]=='I'){		// Read a controller's identification string
	switch(PCstr[1]){
		case '0': if(SendPCStr(StrIDN0)) goto Brk; break;
		case '1': if(SendPCStr(StrIDN1)) goto Brk; break;
		case '2': if(SendPCStr(StrIDN2)) goto Brk;
	}
	goto Start;
}

else if(PWR){  				// Power on if powered off
		PWR=0;  			// power on
		PORTIBctrl=0xfe+!!(flags&RenState);	// Set REN; V2.3 modified
		DDRIBctrl=DDRlstn;  //Set listen mode
		IFCout=0;    		// Clear interface
		PORTIB=0xff;
		delay_us(100);
		IFCout=1;
		SRQst=0;			// New in V2.4
}

if(PCstr[0]=='S'){				// Return state of control lines, new V2.4
	if(brk=SendPCChr(PINIBctrl)) goto Brk;
	goto Start;
}


if(PCstr[0]=='\r'){			// null command (power on and return ACK)
	if(SendPCChr(ACK)) goto Brk;
	goto Start;
}

timer=0;
timer_tot=0;
TMax=TMax_set;					// Enable timeouts
TMaxTot=TMaxTot_set;

if(!strncmpf(PCstr,StrDataSend,2)){	// SendData
	SetTalk();
	flags|=XmtBlkBrk;
	if(SendBinData(WMode<4)) goto BrkIB;
	else SendPCChr(ACK);
	flags&=~XmtBlkBrk;
	SetListen();
}


else if(PCstr[0]=='?'){				// Read data
	NRFDout=0;  // Set Not Ready For Data before releasing ATN to prevent No listener condition
	SetListen();
	if(RcvBinData()) goto BrkIB;
	NDACout=1;
	SendPCChr(ACK);
}


else if(PCstr[0]=='B'){				// Read one byte from the bus
	NRFDout=0;  // Set Not Ready For Data before releasing ATN to prevent No listener condition
	SetListen();
	if(RcvBinByte()) goto BrkIB;
	NDACout=1;
	SendPCChr(ACK);
}


else if(PCstr[0]=='C'||PCstr[0]=='c'){	// Send bus command
	SetTalk();
	if(SendCmd(PCstr[1])) goto BrkIB;
	SetListen();
	if(PCstr[0]=='C')
		ATNout=1;
	SendPCChr(ACK);

}
                 
else if(PCstr[0]=='Z'){  					// Interface Clear
	TE=0;
	IFCout=0;    						// Clear interface
	delay_us(100);
	IFCout=1;
	PORTIB=0xff;
	PORTIBctrl=0xfe+!!(flags&RenState);// Assert REN
	DDRIBctrl=DDRlstn;  //Set listen mode
	DDRIB=0;
	SendPCChr(ACK);
}

//******** Debug (low-level) commands

else if(PCstr[0]=='D'){					// Was a debug command received
	if(debug){
	 	if(PCstr[1]=='T'){					// Talk command
	 	 	if(PCstr[2]=='0') {SetListen(); brk=SendPCChr(ACK);}
	 	 	else if(PCstr[2]=='1') {SetTalk(); brk=SendPCChr(ACK);}
	 	}
 		else if(PCstr[1]=='M'){				// Write data (D7-D0)
 			PORTIB=~PCstr[2]; brk=SendPCChr(ACK);
		}
 		else if(PCstr[1]=='C'){				// Write control
 			PORTIBctrl=PCstr[2]; brk=SendPCChr(ACK);
		}
		else if(PCstr[1]=='?'){				// Read
		 	if(PCstr[2]=='C' || PCstr[2]=='?') brk=SendPCChr(PINIBctrl);	// control
		 	if(PCstr[2]=='M' || PCstr[2]=='?') brk=SendPCChr(~PINIB);	// data
		}
	}
	if(!strncmpf(PCstr,"DE\xAA",3)){
		debug=1;		// Enable debug mode.
		brk=SendPCChr(ACK);
	}
}

// End of debug commands


else{
	SendPCChr(NAK);							// The command was not recognised, return NAK
}

BrkIB:

TMax=0;					// Disable timeouts, V2.4
TMaxTot=0;
flags&=~UseFirst;		// Don't use timeout before first byte


if(brk&TimOutBrk){  	// Time Out Occured
	SetListen();
	if(!(brk&NoData)){  // Do not reset GPIB if no data were received (support for serial poll)
		PORTIBctrl=0xfe+!!(flags&RenState);
		IFCout=0; delay_us(100); IFCout=1;  // Clear interface
	}
	switch(brk&0x0f){  // Send ERROR character
		case NotRdyBrk: SendPCChr(1); break;
		case NotAccBrk: SendPCChr(2); break;
		case NoLstn: SendPCChr(8); break;
		case NoData: SendPCChr(9); break;
		case NotDAVrel: SendPCChr(3);
		case DataFrmtErr: SendPCChr(NAK); break;
	}
}


if(flags&XmtBlkBrk){  // This loop waits for binary data block to end (DLE ETX).
	flags&=~XmtBlkBrk;
	i=0;
	do{
		switch(i){
			case 0:
				if(PCDat==DLE) i=1; break;
			default:
				if(PCDat==DLE) i=0;
				else {i=0x80; if(PCDat!=ETX) brk|=DataFrmtErr;}
		}
		if(i!=0x80){
			while(RXF) {timer=0; if(brk&SleepBrk) break;}
			RD=0;
			timer=0;
			PCDat=PINUSB; RD=1;
		}
	}while(!(i&0x80));
}


Brk:

if(brk&SleepBrk||PWREN){  		// USB went sleep
	DDRUSB=0;
	DDRIB=0;
	DDRIBctrl=0;
	PORTIB=0;    			// disables pull-ups on GPIB
	PORTIBctrl=0;
	DC=0; TE=0;
	PWR=1;  				// power off
	while(!RXF){  			// Empty receiver FIFO
		RD=0; RD=1;
	}
	GIMSK=0x80;  			// Enable wake-up on RXF
//	#asm("sleep");  		// Go sleep
	TIMSK=0;  				// Disable timer interrupts
	powerdown();
}


brk=0;
//state=0;
timer=0;

} //while(1)

} //main