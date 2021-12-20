#include <io.h>
#include <stdio.h>

#include "system.h"
#include "operation_issi.h"




// 	Table 6.19 Advanced Sector/Block Protection Register (ASPR) Bit Definition
//   Read ASP (RDASP 2Bh) and Program ASP (PGASP 2Fh).
//	8.55 READ ASP OPERATION (RDASP, 2Bh)
/*
[15:12] = 0010	 # Number of data bytes
[11]	= 1 	 # Data type (0: write,  1:read)
[10:8]	= 000	 # address bytes
[7:0]	= 2B	 # opcode
*/

int ReadAdvanceBlockProtectionRegister()
{
	int regval;

	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x7,0x0000282B);
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x8,0x1);

	regval = IORD(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0xc);

	if(regval & (0x1<<1))
		printf(" [A1] Persistent Protection Mode not permanently enabled. \n");
	else
		printf(" [A2] Persistent Protection Mode permanently enabled. \n");

	if(regval & (0x1<<2))
		printf(" [B1] Password Protection Mode not permanently enabled. \n");
	else
		printf(" [B2] Password Protection Mode permanently enabled.  \n");

	if(regval & (0x1<<15))
		printf(" [C1] 4KB physical sectors at bottom (Low address) . \n");
	else
		printf(" [C2]  4KB physical sectors at top, (high address)   \n");


	return regval;
}



//Register access commands

//Applicable for all flashes
/* 
[15:12] = 0100  # Number of data bytes
[11] = 0x1      # Data type (0: write,  1:read)
[10:8] = 000    # address bytes
[7:0] = 1001 1111 # opcode
 */
int read_device_id(){
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x7,0x0000389F);
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x8,0x1);
	return IORD(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0xc);
}


/*
Read status register (RDSR , 0x5)
[15:12] = 0001  # Number of data bytes
[11] = 0x1      # Data type (0: write,  1:read)
[10:8] = 000    # address bytes
[7:0] = 0000 0101 # opcode
*/
int read_status_register_issi(){
	int regval;
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x7,0x00001805);
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x8,0x1);
	regval = 	IORD(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0xc);

/*
	printf("status register = 0x%X. \n", regval);

	if( regval & (0x1<<0) )
		printf("\t Device is busy. \n");

	if( regval & (0x1<<1) )
		printf("\t Write Enabled. \n");

	if( regval & (0xF<<2) )
		printf("\t Block is protected. BP[3:0] = 0x%X \n",  (regval & (0xf<<2)) >> 2);

	if( regval & (0x1<<6) )
		printf("\t Quad output is enabled. \n");

	if( regval & (0x1<<7) )
		printf("\t Status register can't write. \n");

	printf("\n");
//*/
	return regval;
}



// 8.29 READ EXTENDED READ PARAMETERS OPERATION (RDERP, 81h)
/*
[15:12] = 0001  # Number of data bytes
[11]    = 1     # Data type (0: write,  1:read)
[10:8]  = 000   # address bytes
[7:0]   = 0x81  # opcode
*/
int read_flag_status_register_issi(){
	int regval;
				IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x7,0x00001881);
				IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x8,0x1);
	regval =	IORD(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0xc);
//*
	printf("flag status register = 0x%X. \n", regval);

	if(regval & (0x1<<1))
		printf("\t [error_flag] protection error in an Erase or a Program operation . \n");

	if(regval & (0x1<<2))
		printf("\t [error_flag] Program operation failure or protection error . \n");

	if(regval & (0x1<<3))
		printf("\t [error_flag] Erase operation failure or protection error  . \n");
//*/

	return regval ;
}


 
void write_enable(){
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x7,0x00000006);
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x8,0x1);
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0xA,1);
}



// 8.48 ENTER 4-BYTE ADDRESS MODE OPERATION (EN4B, B7h) 
/*
[15:12] = 0000  # Number of data bytes
[11]    = 0     # Data type (0: write,  1:read)
[10:8]  = 000   # address bytes
[7:0]   = 0xB7  # opcode
*/

void enter_4byte_addressing_mode(){
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x7,0x000000B7);
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x8,0x1);
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0xA,1);
	
}


// 8.30 CLEAR EXTENDED READ REGISTER OPERATION (CLERP, 82h) 
/*
[15:12] = 0000  # Number of data bytes
[11]    = 0     # Data type (0: write,  1:read)
[10:8]  = 000   # address bytes
[7:0]   = 0x82  # opcode
*/
void clear_flag_status_register_issi(){
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x7,0x00000082);
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x8,0x1);
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0xA,1);
}

 

// 8.14 BLOCK ERASE OPERATION (BER32K:52h or 4BER32K:5Ch, BER64K/256K:D8h or 4BER64K/256K:DCh)
/* 
[15:12] = 0000   # Number of data bytes
[11]    = 0      # Data type (0: write,  1:read)
[10:8]  = 100    # address bytes
[7:0]   = D8     # opcode
*/

void erase_block_issi(int address ){
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x7,0x000004DC);
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x9,address);	// # offset = 0x0
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x8,0x1);
}


// 8.13 SECTOR ERASE OPERATION (SER, D7h/20h or 4SER, 21h)
/*
[15:12] = 0000   # Number of data bytes
[11]    = 0      # Data type (0: write,  1:read)
[10:8]  = 100    # address bytes
[7:0]   = 21     # opcode
*/

void erase_sector_issi(int address ){
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x7,0x00000421);
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x9,address);	// # offset = 0x0
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x8,0x1);
}

//Read Memory Commands
// NORMAL READ OPERATION (NORD, 03h or 4NORD, 13h)  
/* 
[15:12] = 0100   # Number of data bytes = 4
[11]    = 1      # Data type (0: write,  1:read)
[10:8]  = 100    # address bytes = 4
[7:0]   = 13     # opcode
*/

int read_memory(int address){
	int regval;
	int opcode = 0x13;
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x4,0x00000000);		// [1:0] 0: standard spi
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x0,0x00000101);		// [8]= 1:  4-byte addr mode, [1] enabled

	regval = DUMMY_CYCLE << 8 | opcode;
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x5, regval);		// [12:8] dummy cycle, [7:0] read opcode
	return IORD(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_MEM_BASE, address);
}


//Page Program Commands

//4byte addr page program
// 8.29 READ EXTENDED READ PARAMETERS OPERATION (RDERP, 81h)
// 8.10 PAGE PROGRAM OPERATION (PP, 02h or 4PP, 12h)
void write_memory_issi(int address, int data){
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x4,0x00000000);		// [1:0] 0: standard spi
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x0,0x00000101);		// [8]= 1:  4-byte addr mode, [1] enabled
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x6,0x00000502);		// [7:0] opcode for write,  [15:8] opcode for polling
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_MEM_BASE, address, data);
}


//Sector Protection Commands


// 8.19 WRITE STATUS REGISTER OPERATION (WRSR, 01h)
/* 
[15:12] = 0001   # Number of data bytes = 0
[11]    = 0      # Data type (0: write,  1:read)
[10:8]  = 000    # address bytes =
[7:0]   = 0x01     # opcode
*/

void write_register_for_sector_unprotect_issi(){
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x7,0x00001001);
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0xA,0x00000000);
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x8,0x1);
}



// 8.19 WRITE STATUS REGISTER OPERATION (WRSR, 01h)
/*
[15:12] = 0001   # Number of data bytes = 0
[11]    = 0      # Data type (0: write,  1:read)
[10:8]  = 000    # address bytes =
[7:0]   = 0x01     # opcode

DATA [5:2] =   1111  (0x3C)
             00111100

*/
void write_status_register_for_block_protect_issi(){
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x7,0x00001001);
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0xA,0x0000003c);
	IOWR(INTEL_GENERIC_SERIAL_FLASH_INTERFACE_TOP_0_AVL_CSR_BASE,0x8,0x1);
}




