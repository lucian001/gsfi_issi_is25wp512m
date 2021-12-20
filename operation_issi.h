#ifndef _OPERATION_ISSI_H_
#define _OPERATION_ISSI_H_


//#define DUMMY_CYCLE   10
#define DUMMY_CYCLE   0


// External function declaration
int read_device_id();
int ReadAdvanceBlockProtectionRegister();

int read_status_register_issi();
int read_flag_status_register_issi();
void write_enable();
void enter_4byte_addressing_mode();
void clear_flag_status_register_issi();
void erase_sector_issi(int address );
int read_memory(int address);
void write_memory_issi(int address, int data);
void write_register_for_sector_unprotect_issi();
void write_status_register_for_block_protect_issi();



#endif //_OPERATION_ISSI_H_

