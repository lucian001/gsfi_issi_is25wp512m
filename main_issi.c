
#include "system.h"
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "operation_issi.h"


int mask_wip2 = 0x00000001; //set mask to check Write in Progress (WIP) bit to determine device busy or ready
int mask_sr2 = 0x0000003C; // set mask to check bit 5:2 of status register
int block_protect_sr_m = 0xF<<2; //set BP3-0 and TB bit in status register to protect all sectors
int mask_e_err2 = 0x7<<1; //set mask to check erase error bit in flag status register
int mask_wel2 = 0x00000002; //set mask to check write enable latch
int empty_data2 = 0xffffffff;


int Wait_WriteEnable(int timeout_ms)
{
	int loop = timeout_ms;

	write_enable();

	while(loop--)
	{
		if ( (read_status_register_issi()& mask_wel2) )
			return 1;
		else
			usleep(1000);
	}

	return -1;  // timeout
}



int Wait_Finish(int timeout_ms)
{
	int loop = timeout_ms;

	while(loop--)
	{
		if ( (read_status_register_issi() & mask_wip2) == 0 )
			return 1;
		else
			usleep(1000);
	}

	return -1;  // timeout
}


#define TIMEOUT_MS  5000

#define WAIT_WRITEENABLE(TO)  if( 0 > Wait_WriteEnable(TO) ) { printf("\n\n WaitEnable() timeout. at %s , LINE:%d \n", __FILE__, __LINE__);    exit(-5);  }


#define WAIT_FINISHED(TO)  if( 0 > Wait_Finish(TO) ) { printf("\n\n WaitFinish() timeout. at %s , LINE:%d \n", __FILE__, __LINE__);    exit(-7);  }


int main()
{
	//unsigned int regval;
	int test_addr = 0x0;
	int data1 = 0xabcd1234;
	int sector_size = 4*1024;
	int block_size = 64*1024;


	//IDENTIFY FLASH DEVICE IDENTITY
	//int issi_flash = 0x1a709d;
	int devid = read_device_id();
	printf("Device ID: %x\n", devid);
	printf("Flash Device: ISSI flash IS25WP512 \n");

	printf("Enter 4-byte mode. \n");
	enter_4byte_addressing_mode();


	printf("\n ========= Program START ========= \n\n\n");


	for(int i=0; i<10; i++)
	{
		//test_addr = i*block_size;
		test_addr = i*sector_size;
		data1 = 0xABCD0000 + i;

		printf("\n\n\n--- target address = 0x%08X --- \n", test_addr);

		//clear_flag_status_register();
		clear_flag_status_register_issi();

		//PERFORM SECTOR PROTECT CONFIGURATION DEVICE
		ReadAdvanceBlockProtectionRegister();

	    // check if any sector of the flash is in protecting
		if ((read_status_register_issi() & mask_sr2) == 0 )
		{
			//printf("\t All sectors in this configuration device is not protected\n");
			printf("\t Now performing sector protection...\n");
			WAIT_WRITEENABLE(TIMEOUT_MS);
			write_status_register_for_block_protect_issi();
			WAIT_FINISHED(TIMEOUT_MS);
		}


		//Check Status Register to confirm whether all of its sector already protected
		if ((read_status_register_issi() & block_protect_sr_m)!= block_protect_sr_m)
		{
			printf("\n\t Setor protection failed due to error in setting status register\n");
			printf("\t Status Register: %08x\n",read_status_register_issi());
			printf("\t Check datasheet to find out\n");
			return 0;
		}
		
		printf("\t All sectors in this configuration device is now successfully protected\n");
		printf("\t Trying to erase sector ...\n");


		WAIT_WRITEENABLE(TIMEOUT_MS);
		erase_sector_issi(test_addr);
		WAIT_FINISHED(TIMEOUT_MS);

		read_flag_status_register_issi();


		//start of skip
		if ((read_flag_status_register_issi()& mask_e_err2) == 0) {
			printf("\n\n ERASE ERROR do not occur. Check status register for more details. \n Exit\n\n");
			return 0;
		}


		//printf("\t Erase Error as erase is not allow during sector is protected!\n");
		clear_flag_status_register_issi(); // clear erase error bit


		//UNPROTECT ALL SECTORS IN CONFIGURATION DEVICE
			printf("Now perform sector unprotect...\n");
			WAIT_WRITEENABLE(TIMEOUT_MS);
			write_register_for_sector_unprotect_issi();
			WAIT_FINISHED(TIMEOUT_MS);


		if ((read_status_register_issi() & mask_sr2) > 0 ) {
			printf("Sector unprotect not successful! :(\n");
			printf("Status Register: %08x\n",read_status_register_issi());
			printf("Flag Register: %08x\n",read_flag_status_register_issi());
			return 0;
		}

		printf("Sector unprotect successfully! :)\n");


		if ((read_flag_status_register_issi()& mask_e_err2) )
		{
			printf("\n Program/Erase has error. \n");
			return 0;
		}



		//READ AND WRTIE DATA

		printf("\t Reading data at address : 0x%X...\n", test_addr);
		printf("\t Memory content at address 0x%x: %x\n", test_addr, read_memory(test_addr));


		if (read_memory(test_addr)!= empty_data2)
		{
			//PERFORM SECTOR ERASE to clear sector 0
			printf("\t Address 0x%X containing data, it is not empty.\n", test_addr);
			printf("\t Trying to erase sector ...\n");

			WAIT_WRITEENABLE(TIMEOUT_MS);
			erase_sector_issi(test_addr);
			WAIT_FINISHED(TIMEOUT_MS);
			printf("[f0] Status reg=0x%X. \n", read_status_register_issi() );

			if (read_memory(test_addr)!= empty_data2) {
				 printf("\t Sector erase failed. Sector is still having data.\n Exit \n\n");
				 return 0;
			}
			printf("\t Sector erase successfully. Sector is now empty.\n");
		}

		//WRITE DATA INTO EMPTY MEMORY

		printf("\t Address 0x%x not containing data...\n", test_addr);
		printf("\t Writing data to address 0x%X...\n", test_addr);

		write_memory_issi(test_addr, data1);
		WAIT_FINISHED(TIMEOUT_MS);
		printf("[g3] Status reg=0x%X. \n", read_status_register_issi() );

		//READ BACK DATA

		printf("\t Read back data from address 0x%X...\n", test_addr);


		if(read_memory(test_addr) != data1) {
			printf("\t Current memory in address 0x%X: %x\n", test_addr, read_memory(test_addr) );
			printf("\n\n  Something is wrong... \n\n Exit \n");
			return 0;
		}
		printf("\t Current memory in address 0x%X: %x\n", test_addr, read_memory(test_addr));
		printf("\t Read data match with data written. Write memory successful.\n");

		printf("\t Now performing sector protection...\n");
			WAIT_WRITEENABLE(TIMEOUT_MS);
			write_status_register_for_block_protect_issi();
			WAIT_FINISHED(TIMEOUT_MS);

		//Check Status Register to see whether it is set to perform sector protect
		if ((read_status_register_issi() & block_protect_sr_m) != block_protect_sr_m) {
			printf("\t Setor protection failed due to error in setting status register\n");
			printf("\t Status Register: %08x\n",read_status_register_issi());
			printf("\t Check datasheet to find out \n");
			return 0;
		}

		printf("\t All sectors in this configuration device is now successfully protected\n");

		//PERFORM SECTOR ERASE ON PROTECTED SECTOR
			printf("Trying to erase sector ...\n");
			WAIT_WRITEENABLE(TIMEOUT_MS);
			erase_sector_issi(test_addr);
			WAIT_FINISHED(TIMEOUT_MS);


		//int mask_e_err = 0x00000020; //set mask to check protection error bit
		if ((read_status_register_issi()& mask_e_err2) == 0) {
			printf("\t ERASE ERROR do not occur. Check status register for more details.\n Exit \n");
			return 0;
		}
		printf("\t ERASE ERROR as sector is protected!\n");
		clear_flag_status_register_issi(); //clear erase error bit

		if(read_memory(test_addr) == data1) {
			printf("\t Current memory in address 0x%x: %x\n", test_addr, read_memory(test_addr));
			printf("\t Read data match with data written previously. Sector erase does not perform during sector is protected.\n\n");

		} else {
			printf("\t Current memory in address 0x%x: %x\n", test_addr, read_memory(test_addr));
			printf("\t Something is wrong...\n Exit \n");
			return 0;
		}


	}


	printf("\n ========= Program end ========= \n\n\n");

}









