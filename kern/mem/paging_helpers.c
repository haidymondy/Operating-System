/*
 * paging_helpers.c
 *
 *  Created on: Sep 30, 2022
 *      Author: HP
 */
#include "memory_manager.h"

/*[2.1] PAGE TABLE ENTRIES MANIPULATION */
inline void pt_set_page_permissions(uint32* page_directory, uint32 virtual_address, uint32 permissions_to_set, uint32 permissions_to_clear)
{
	uint32 *ptr_page_table = NULL;
	   get_page_table(page_directory, virtual_address, &ptr_page_table);

		if (ptr_page_table != NULL)
		{
		ptr_page_table[PTX(virtual_address)] =
		ptr_page_table[PTX(virtual_address)] & (~permissions_to_clear);

		ptr_page_table[PTX(virtual_address)] =
			ptr_page_table[PTX(virtual_address)] | (permissions_to_set);
		}
		else
			panic("Invalid VA\n");

		tlb_invalidate((void *)NULL, (void *)virtual_address);}


inline int pt_get_page_permissions(uint32* page_directory, uint32 virtual_address )
{
	uint32 *ptr_page_table = NULL;
	uint32 directory_entry=ptr_page_directory[PDX(virtual_address )];
	int ret = get_page_table(page_directory, virtual_address, &ptr_page_table);
	if (ret == TABLE_IN_MEMORY)
	{
		uint32 table_entry=ptr_page_table[PTX(virtual_address )];
		int permission_num_table_entry=table_entry<<20	;
		permission_num_table_entry=permission_num_table_entry>>20;
		return 	permission_num_table_entry;
	}
	else {
		return -1;
	}
}

inline void pt_clear_page_table_entry(uint32* page_directory, uint32 virtual_address)
{
	//TODO: [PROJECT MS2] [PAGING HELPERS] pt_clear_page_table_entry
		// Write your code here, remove the panic and write your code
		//panic("pt_clear_page_table_entry() is not implemented yet...!!");

		uint32 *ptr_page_table=NULL;
		int table_exist = get_page_table(page_directory,virtual_address,&ptr_page_table);
		if(table_exist == TABLE_IN_MEMORY)
		{
			uint32 table_entry = ptr_page_table[PTX(virtual_address)]=0;

		}

		else
		{
			panic("Invalid VA\n");
		}

		tlb_invalidate((void *)NULL,(void *)virtual_address);
}

/***********************************************************************************************/

/*[2.2] ADDRESS CONVERTION*/
inline int virtual_to_physical(uint32* page_directory, uint32 virtual_address)
{
	uint32 PAGE_PRESENT = 0x00000FFF;
			uint32 MASK_right_10_bits = 0x3FF;
	//		uint32 PAGE_MASK = 0xFFF;
			uint32 PAGE_MASK_left_20_bits = 0xFFFFF000;
		    uint32 *ptr_page_table = NULL;

		if (get_page_table(page_directory, virtual_address, &ptr_page_table) == TABLE_IN_MEMORY)
	    {
				uint32 directory = (virtual_address) >> 22; // directory
				// get first 10 bits to left
				uint32 first_10_bits = directory & MASK_right_10_bits;

				uint32 page_dir = (virtual_address) >> 12; // page
				// get second 10 bits from the left
				uint32 second_10_bits = page_dir & MASK_right_10_bits;

				if (page_directory[first_10_bits]) // access with first 10
				{
					// take only the 20 bits without the offset
					uint32 x = page_directory[first_10_bits] & PAGE_MASK_left_20_bits;
					// physical
					uint32 *page_table = STATIC_KERNEL_VIRTUAL_ADDRESS(x);
					// point to physical with first 20 bits & take them without the offset
					return page_table[second_10_bits] & PAGE_MASK_left_20_bits;
				}
				return 0;
	    }
			return -1;
}

/***********************************************************************************************/

/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/
/***********************************************************************************************/

///============================================================================================
/// Dealing with page directory entry flags

inline uint32 pd_is_table_used(uint32* page_directory, uint32 virtual_address)
{
	return ( (page_directory[PDX(virtual_address)] & PERM_USED) == PERM_USED ? 1 : 0);
}

inline void pd_set_table_unused(uint32* page_directory, uint32 virtual_address)
{
	page_directory[PDX(virtual_address)] &= (~PERM_USED);
	tlb_invalidate((void *)NULL, (void *)virtual_address);
}

inline void pd_clear_page_dir_entry(uint32* page_directory, uint32 virtual_address)
{
	page_directory[PDX(virtual_address)] = 0 ;
	tlbflush();
}
