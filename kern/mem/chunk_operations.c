/*
 * chunk_operations.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include <kern/trap/fault_handler.h>
#include <kern/disk/pagefile_manager.h>
#include "kheap.h"
#include "memory_manager.h"
void create_table(uint32 source_va,uint32 source_end_va,uint32 counti,uint32 *page_directory,uint32 dest_va,uint32 dset_va_counter);

/******************************/
/*[1] RAM CHUNKS MANIPULATION */
/******************************/

//===============================
// 1) CUT-PASTE PAGES IN RAM:
//===============================
//This function should cut-paste the given number of pages from source_va to dest_va
void sum();
//if the page table at any destination page in the range is not exist, it should create it
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int cut_paste_pages(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 num_of_pages)
{
	uint32 *ptr_tset_source = NULL;
	uint32 *ptr_test_dest = NULL;
	int tst_source=get_page_table(page_directory, source_va, &ptr_tset_source);
	int tst_dst=get_page_table(page_directory, dest_va, &ptr_test_dest);
	struct FrameInfo* frame_source=get_frame_info(page_directory,source_va,&ptr_tset_source);
	struct FrameInfo* frame_dest=get_frame_info(page_directory,dest_va,&ptr_test_dest);
	sum();
	//---------------------------------------------------------------------------
	source_va=ROUNDDOWN(source_va,PAGE_SIZE);
	dest_va=ROUNDDOWN(dest_va,PAGE_SIZE);
	uint32 *ptr_page_table_source = NULL;
	uint32 *ptr_page_table_dest = NULL;
	uint32 source_end_va=source_va+(num_of_pages * PAGE_SIZE );
	uint32 dset_va_counter=dest_va;
	uint32 counti=PAGE_SIZE;
	for(uint32 i=source_va;i<source_end_va;i+=counti)
	{
		sum();
		uint32 *ptr_tset_source = NULL;
		uint32 *ptr_test_dest = NULL;
		int tst_source=get_page_table(page_directory, source_va, &ptr_tset_source);
		int tst_dst=get_page_table(page_directory, dest_va, &ptr_test_dest);
		struct FrameInfo* frame_source=get_frame_info(page_directory,source_va,&ptr_tset_source);
		struct FrameInfo* frame_dest=get_frame_info(page_directory,dest_va,&ptr_test_dest);
		//---------------------------------------------------------------------------
	    ptr_page_table_dest = NULL;
	    get_page_table(page_directory, dset_va_counter, &ptr_page_table_dest);
	    struct FrameInfo* info_dest = get_frame_info(page_directory,dset_va_counter,&ptr_page_table_dest);
	    if(info_dest!=NULL)
	    {
	    	return -1;
	    }
		dset_va_counter=dset_va_counter+counti;
	}

	dset_va_counter=dest_va;

	create_table(source_va,source_end_va,counti,page_directory,dest_va,dset_va_counter);

	dset_va_counter=dest_va;

	for(uint32 i=source_va;i<source_end_va;i+=counti){
		sum();
		uint32 *ptr_tset_source = NULL;
		uint32 *ptr_test_dest = NULL;
		int tst_source=get_page_table(page_directory, source_va, &ptr_tset_source);
		int tst_dst=get_page_table(page_directory, dest_va, &ptr_test_dest);
		struct FrameInfo* frame_source=get_frame_info(page_directory,source_va,&ptr_tset_source);
		struct FrameInfo* frame_dest=get_frame_info(page_directory,dest_va,&ptr_test_dest);
		//---------------------------------------------------------------------------
		ptr_page_table_source = NULL;
		ptr_page_table_dest = NULL;
		uint32 perms=pt_get_page_permissions(page_directory,i);
		get_page_table(page_directory, i, &ptr_page_table_source);
		get_page_table(page_directory, dset_va_counter, &ptr_page_table_dest);
		struct FrameInfo* info_source = get_frame_info(page_directory,i,&ptr_page_table_source);
		map_frame(page_directory,info_source,dset_va_counter,perms);
		unmap_frame(page_directory,i);
		dset_va_counter=dset_va_counter+counti;
	}
	return 0;
}

//===============================
// 2) COPY-PASTE RANGE IN RAM:
//===============================
//This function should copy-paste the given size from source_va to dest_va
//if the page table at any destination page in the range is not exist, it should create it
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int copy_paste_chunk(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 size)
{
	uint32 *ptr_tset_source = NULL;
	uint32 *ptr_test_dest = NULL;
	int tst_source=get_page_table(page_directory, source_va, &ptr_tset_source);
	int tst_dst=get_page_table(page_directory, dest_va, &ptr_test_dest);
	struct FrameInfo* frame_source=get_frame_info(page_directory,source_va,&ptr_tset_source);
	struct FrameInfo* frame_dest=get_frame_info(page_directory,dest_va,&ptr_test_dest);
	//---------------------------------------------------------------------------
	uint32 s=source_va;
	uint32 d=dest_va;
	uint32 dif = source_va-ROUNDDOWN(source_va,PAGE_SIZE);
	uint32 dif2 = dest_va-ROUNDDOWN(dest_va,PAGE_SIZE);
	//size=size+dif;
	source_va=ROUNDDOWN(source_va,PAGE_SIZE);
	dest_va=ROUNDDOWN(dest_va,PAGE_SIZE);
	uint32 source_end_va=source_va+size;
	source_end_va=ROUNDUP(source_end_va,PAGE_SIZE);
	uint32 *ptr_page_table_source = NULL;
	uint32 *ptr_page_table_dest = NULL;
	uint32 dset_va_counter=dest_va;
	uint32 counti=PAGE_SIZE;

	for(uint32 i=source_va;i<source_end_va;i+=counti)
	{
		sum();
		uint32 *ptr_tset_source = NULL;
		uint32 *ptr_test_dest = NULL;
		int tst_source=get_page_table(page_directory, source_va, &ptr_tset_source);
		int tst_dst=get_page_table(page_directory, dest_va, &ptr_test_dest);
		struct FrameInfo* frame_source=get_frame_info(page_directory,source_va,&ptr_tset_source);
		struct FrameInfo* frame_dest=get_frame_info(page_directory,dest_va,&ptr_test_dest);
		//---------------------------------------------------------------------------
		ptr_page_table_dest = NULL;
		uint32 perms=(pt_get_page_permissions(page_directory,dset_va_counter));
		struct FrameInfo* info_dest = get_frame_info(page_directory,dset_va_counter,&ptr_page_table_dest);
		if(info_dest!=NULL)
		{
			if((perms & PERM_WRITEABLE) == 0)
			{
				return -1;
			}
		}
		dset_va_counter=dset_va_counter+counti;
	}
	dset_va_counter=dest_va;
	create_table(source_va,source_end_va,counti,page_directory,dest_va,dset_va_counter);
	dset_va_counter=dest_va;
	for(uint32 i=source_va;i<source_end_va;i+=counti){
		sum();
		uint32 *ptr_tset_source = NULL;
		uint32 *ptr_test_dest = NULL;
		int tst_source=get_page_table(page_directory, source_va, &ptr_tset_source);
		int tst_dst=get_page_table(page_directory, dest_va, &ptr_test_dest);
		struct FrameInfo* frame_source=get_frame_info(page_directory,source_va,&ptr_tset_source);
		struct FrameInfo* frame_dest=get_frame_info(page_directory,dest_va,&ptr_test_dest);
		//---------------------------------------------------------------------------
		int *ptr = (int*)(i) ;
		int *ptr2 = (int*)(dset_va_counter) ;
		ptr_page_table_source = NULL;
		ptr_page_table_dest = NULL;
		get_page_table(page_directory, i, &ptr_page_table_source);
		get_page_table(page_directory, dset_va_counter, &ptr_page_table_dest);
		struct FrameInfo* info_source = get_frame_info(page_directory,i,&ptr_page_table_source);
		struct FrameInfo* info_dest = get_frame_info(page_directory,dset_va_counter,&ptr_page_table_dest);
		struct FrameInfo *ptr_frame_info =NULL;
		int perm=(pt_get_page_permissions(page_directory,i)&PERM_USER)|PERM_WRITEABLE;

		if(info_dest==NULL)
		{
			allocate_frame(&ptr_frame_info);
			map_frame(page_directory,ptr_frame_info,dset_va_counter,perm);
			for(int i=0;i<4096/4;i++)
			{
				*ptr2=*ptr;
				ptr++;
				ptr2++;
			}

		}
		else
		{
			int count=0;
			for(int i=0;i<size;i++){
				int *ptr = (int*)(s+count) ;
				int *ptr2 = (int*)(d+count) ;
				*ptr2=*ptr;
				count++;
			}
		}
		dset_va_counter=dset_va_counter+counti;
	}
	return 0;
}
//===============================
// 3) SHARE RANGE IN RAM:
//===============================
//This function should share the given size from dest_va with the source_va
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int share_chunk(uint32* page_directory, uint32 source_va,uint32 dest_va, uint32 size, uint32 perms)
{
	sum();
	uint32 *ptr_tset_source = NULL;
	uint32 *ptr_test_dest = NULL;
	int tst_source=get_page_table(page_directory, source_va, &ptr_tset_source);
	int tst_dst=get_page_table(page_directory, dest_va, &ptr_test_dest);
	struct FrameInfo* frame_source=get_frame_info(page_directory,source_va,&ptr_tset_source);
	struct FrameInfo* frame_dest=get_frame_info(page_directory,dest_va,&ptr_test_dest);
	//---------------------------------------------------------------------------
	uint32 dif = source_va-ROUNDDOWN(source_va,PAGE_SIZE);
	size=size+dif;
	source_va=ROUNDDOWN(source_va,PAGE_SIZE);
	dest_va=ROUNDDOWN(dest_va,PAGE_SIZE);
	uint32 source_end_va=source_va+size;
	source_end_va=ROUNDUP(source_end_va,PAGE_SIZE);
	uint32 *ptr_page_table_source = NULL;
	uint32 *ptr_page_table_dest = NULL;
	uint32 dset_va_counter=dest_va;
	uint32 counti=PAGE_SIZE;

	for(uint32 i=source_va;i<source_end_va;i+=counti){
		sum();
		uint32 *ptr_tset_source = NULL;
		uint32 *ptr_test_dest = NULL;
		int tst_source=get_page_table(page_directory, source_va, &ptr_tset_source);
		int tst_dst=get_page_table(page_directory, dest_va, &ptr_test_dest);
		struct FrameInfo* frame_source=get_frame_info(page_directory,source_va,&ptr_tset_source);
		struct FrameInfo* frame_dest=get_frame_info(page_directory,dest_va,&ptr_test_dest);
		//---------------------------------------------------------------------------
		ptr_page_table_dest = NULL;
		get_page_table(page_directory, dset_va_counter, &ptr_page_table_dest);
		struct FrameInfo* info_dest = get_frame_info(page_directory,dset_va_counter,&ptr_page_table_dest);
		if(info_dest!=NULL){
			return -1;
		}
		dset_va_counter=dset_va_counter+counti;
	}

	dset_va_counter=dest_va;

	create_table(source_va,source_end_va,counti,page_directory,dest_va,dset_va_counter);

	dset_va_counter=dest_va;

	for(uint32 i=source_va;i<source_end_va;i+=counti){
		sum();
		uint32 *ptr_tset_source = NULL;
		uint32 *ptr_test_dest = NULL;
		int tst_source=get_page_table(page_directory, source_va, &ptr_tset_source);
		int tst_dst=get_page_table(page_directory, dest_va, &ptr_test_dest);
		struct FrameInfo* frame_source=get_frame_info(page_directory,source_va,&ptr_tset_source);
		struct FrameInfo* frame_dest=get_frame_info(page_directory,dest_va,&ptr_test_dest);
		//---------------------------------------------------------------------------
		ptr_page_table_source = NULL;
		ptr_page_table_dest = NULL;
		get_page_table(page_directory, i, &ptr_page_table_source);
		get_page_table(page_directory, dset_va_counter, &ptr_page_table_dest);
		struct FrameInfo* info_source = get_frame_info(page_directory,i,&ptr_page_table_source);
		map_frame(page_directory,info_source,dset_va_counter,perms);
		dset_va_counter=dset_va_counter+counti;
	}
	return 0;
}
void create_table(uint32 source_va,uint32 source_end_va,uint32 counti,uint32 *page_directory,uint32 dest_va,uint32 dset_va_counter)
{
	uint32 *ptr_page_table_dest;
	for(uint32 i=source_va;i<source_end_va;i+=counti){
		uint32 *ptr_tset_source = NULL;
		uint32 *ptr_test_dest = NULL;
		int tst_source=get_page_table(page_directory, source_va, &ptr_tset_source);
		int tst_dst=get_page_table(page_directory, dest_va, &ptr_test_dest);
		struct FrameInfo* frame_source=get_frame_info(page_directory,source_va,&ptr_tset_source);
		struct FrameInfo* frame_dest=get_frame_info(page_directory,dest_va,&ptr_test_dest);
		//---------------------------------------------------------------------------
		sum();
		 ptr_page_table_dest = NULL;
		int ret=get_page_table(page_directory, dset_va_counter, &ptr_page_table_dest);
		if(ret != TABLE_IN_MEMORY){
			create_page_table(page_directory,dset_va_counter);
		}
		dset_va_counter=dset_va_counter+counti;
	}
}

//===============================
// 4) ALLOCATE CHUNK IN RAM:
//===============================
//This function should allocate in RAM the given range [va, va+size)
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int allocate_chunk(uint32* page_directory, uint32 va, uint32 size, uint32 perms)
{
	int flag = 0 ;
	uint32 new_upper_range = ROUNDUP((va + size),PAGE_SIZE);
	va = ROUNDDOWN(va,PAGE_SIZE);

	for(int i = va ; i < new_upper_range ;i+=PAGE_SIZE ){
		uint32 * ptr_page_table=NULL;
		int ret = get_page_table(page_directory,i,&ptr_page_table);
		if(ret == TABLE_IN_MEMORY){
			uint32 * ptr_page=NULL;
			struct FrameInfo* info = get_frame_info(page_directory,i,&ptr_page);
			if(info!=NULL){
				flag = -1;
				break ;
			}
			else {
				struct FrameInfo * ptr_frame  ;
				int res = allocate_frame(&ptr_frame);
				if(res!=E_NO_MEM){
					res = map_frame(page_directory,ptr_frame,i,perms);
					ptr_frame->va = i;
				}
			}
		}

		else{
			create_page_table(page_directory,i);
			struct FrameInfo * ptr_frame = NULL ;
			int res  = allocate_frame(&ptr_frame);
			if(res!=E_NO_MEM){
				res = map_frame(page_directory,ptr_frame,i,perms);
				ptr_frame->va = i;
			}
		}
	}

	return flag ;
}
/*BONUS*/
//=====================================
// 5) CALCULATE ALLOCATED SPACE IN RAM:
//=====================================
void calculate_allocated_space(uint32* page_directory, uint32 sva, uint32 eva, uint32 *num_tables, uint32 *num_pages)
{
	sum();
	uint32 *ptr_tset_source = NULL;
	uint32 *ptr_test_dest = NULL;
	int tst_source=get_page_table(page_directory, sva, &ptr_tset_source);
	int tst_dst=get_page_table(page_directory, eva, &ptr_test_dest);
	struct FrameInfo* frame_source=get_frame_info(page_directory,sva,&ptr_tset_source);
	struct FrameInfo* frame_dest=get_frame_info(page_directory,eva,&ptr_test_dest);
	//---------------------------------------------------------------------------
	int count =PAGE_SIZE*(PAGE_SIZE/4);
	uint32 evaa=ROUNDUP(eva,count);
	sva=ROUNDDOWN(sva,PAGE_SIZE);
	eva=ROUNDUP(eva,PAGE_SIZE);
	int x_tables=0;
	int y_pages=0;
	for(uint32 i = sva ; i <= evaa ; i+=count)
	{
		uint32 * ptr_page_table=NULL;
		int ret = get_page_table(page_directory,i,&ptr_page_table);
		if(ret != TABLE_NOT_EXIST)
		{
			x_tables++;
		}
	}
	for(uint32 i = sva ; i < eva ; i+=PAGE_SIZE){
		uint32 * ptr_page_table=NULL;
		int ret = get_page_table(page_directory,i,&ptr_page_table);
		struct FrameInfo* info = get_frame_info(page_directory,i,&ptr_page_table);
		if(info!=NULL){
			y_pages++;
		}
	}
	*num_tables=x_tables;
	*num_pages=y_pages;
}
/*BONUS*/
//=====================================
// 6) CALCULATE REQUIRED FRAMES IN RAM:
//=====================================
// calculate_required_frames:
// calculates the new allocation size required for given address+size,
// we are not interested in knowing if pages or tables actually exist in memory or the page file,
// we are interested in knowing whether they are allocated or not.
uint32 calculate_required_frames(uint32* page_directory, uint32 sva, uint32 size)
{
	sum();
	uint32 *ptr_tset_source = NULL;
	uint32 *ptr_test_dest = NULL;
	int tst_source=get_page_table(page_directory, sva, &ptr_tset_source);
	//int tst_dst=get_page_table(page_directory, eva, &ptr_test_dest);
	struct FrameInfo* frame_source=get_frame_info(page_directory,sva,&ptr_tset_source);
	//struct FrameInfo* frame_dest=get_frame_info(page_directory,eva,&ptr_test_dest);
	//---------------------------------------------------------------------------
	int count =PAGE_SIZE*(PAGE_SIZE/4);
	uint32 c1=0,c2=0;
	uint32 dif=sva-ROUNDDOWN(sva,PAGE_SIZE);
	size=size+dif;
	sva=ROUNDDOWN(sva,PAGE_SIZE);
	uint32 eva=sva+size;
	uint32 evaa=ROUNDUP(eva,count);
	eva=ROUNDUP(eva,PAGE_SIZE);
	for(uint32 i = sva ; i < evaa ; i+=count){
		uint32 * ptr_page_table=NULL;
		int ret = get_page_table(page_directory,i,&ptr_page_table);
		if(ret != TABLE_IN_MEMORY){
			c1++;
		}
	}
	for(uint32 i = sva ; i < eva ; i+=PAGE_SIZE){
		uint32 * ptr_page_table=NULL;
		int ret = get_page_table(page_directory,i,&ptr_page_table);
		struct FrameInfo* info = get_frame_info(page_directory,i,&ptr_page_table);
		if(info==NULL){
			c2++;
		}
	}
	int required=c1+c2;
	return required;
}
void sum(){
	int c=0;

}
//=================================================================================//
//===========================END RAM CHUNKS MANIPULATION ==========================//
//=================================================================================//

/*******************************/
/*[2] USER CHUNKS MANIPULATION */
/*******************************/

//======================================================
/// functions used for USER HEAP (malloc, free, ...)
//======================================================

//=====================================
// 1) ALLOCATE USER MEMORY:
//=====================================
void allocate_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	// Write your code here, remove the panic and write your code
	panic("allocate_user_mem() is not implemented yet...!!");
}
//=====================================
// 2) FREE USER MEMORY:
//=====================================
void free_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	//TODO: [PROJECT MS3] [USER HEAP - KERNEL SIDE] free_user_mem
	// Write your code here, remove the panic and write your code
	//panic("free_user_mem() is not implemented yet...!!");

	//This function should:
	//1. Free ALL pages of the given range from the Page File
	//2. Free ONLY pages that are resident in the working set from the memory
	//3. Removes ONLY the empty page tables (i.e. not used) (no pages are mapped in the table)
	//	struct MemBlock*block=find_block(&AllocMemBlocksList,(uint32)virtual_address);

	//size=ROUNDUP(size,PAGE_SIZE);
	for(uint32 i=virtual_address;i<virtual_address+size;i+=PAGE_SIZE)
	{

		int bool =1;
		pf_remove_env_page(e,i);
		unmap_frame(e->env_page_directory,i);
		env_page_ws_invalidate(e,i);
		uint32 *ptr_page_table=NULL;
		get_page_table(e->env_page_directory,i,&ptr_page_table);
		if(ptr_page_table !=NULL)
		{
			for(int j=0; j<PAGE_SIZE/4; j++)
			{
				if(ptr_page_table[j] != 0)
				{
					bool=0;
					break;
				}
			}
			if(bool)
			{
				kfree((void*)ptr_page_table);
				pd_clear_page_dir_entry(e->env_page_directory,i);
			}
		}
	}
}
//=====================================
// 2) FREE USER MEMORY (BUFFERING):
//=====================================
void __free_user_mem_with_buffering(struct Env* e, uint32 virtual_address, uint32 size)
{
	// your code is here, remove the panic and write your code
	panic("__free_user_mem_with_buffering() is not implemented yet...!!");

	//This function should:
	//1. Free ALL pages of the given range from the Page File
	//2. Free ONLY pages that are resident in the working set from the memory
	//3. Free any BUFFERED pages in the given range
	//4. Removes ONLY the empty page tables (i.e. not used) (no pages are mapped in the table)
}

//=====================================
// 3) MOVE USER MEMORY:
//=====================================
void move_user_mem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
{
	//TODO: [PROJECT MS3 - BONUS] [USER HEAP - KERNEL SIDE] move_user_mem
	//your code is here, remove the panic and write your code
	panic("move_user_mem() is not implemented yet...!!");

	// This function should move all pages from "src_virtual_address" to "dst_virtual_address"
	// with the given size
	// After finished, the src_virtual_address must no longer be accessed/exist in either page file
	// or main memory

	/**/
}

//=================================================================================//
//========================== END USER CHUNKS MANIPULATION =========================//
//=================================================================================//

