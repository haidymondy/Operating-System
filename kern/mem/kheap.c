#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"

//==================================================================//
//==================================================================//
//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)//
//==================================================================//
//==================================================================//

void initialize_dyn_block_system()
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] initialize_dyn_block_system
	// your code is here, remove the panic and write your code
	//kpanic_into_prompt("initialize_dyn_block_system() is not implemented yet...!!");

	//[1] Initialize two lists (AllocMemBlocksList & FreeMemBlocksList) [Hint: use LIST_INIT()]
#if STATIC_MEMBLOCK_ALLOC
	//DO NOTHING
#else

	 LIST_INIT(&FreeMemBlocksList);
	 LIST_INIT(&AllocMemBlocksList);
	 uint32 num_of_pages = (KERNEL_HEAP_MAX - KERNEL_HEAP_START)/PAGE_SIZE;
	 MAX_MEM_BLOCK_CNT =  num_of_pages;
	 uint32 size_of_mem_block = ROUNDUP(num_of_pages* sizeof(struct MemBlock),PAGE_SIZE) ;
	 allocate_chunk(ptr_page_directory, KERNEL_HEAP_START,size_of_mem_block ,PERM_WRITEABLE);

	/*[2] Dynamically allocate the array of MemBlockNodes
	 * 	remember to:
	 * 		1. set MAX_MEM_BLOCK_CNT with the chosen size of the array
	 * 		2. allocation should be aligned on PAGE boundary
	 * 	HINT: can use alloc_chunk(...) function
	 */
	 MemBlockNodes = (struct MemBlock *)KERNEL_HEAP_START;


#endif
    	//[3] Initialize AvailableMemBlocksList by filling it with the MemBlockNodes
	 //[4] Insert a new MemBlock with the remaining heap size into the FreeMemBlocksList
	 initialize_MemBlocksList(num_of_pages);
	 	     struct MemBlock * first_in_available = LIST_FIRST(&AvailableMemBlocksList);
	 	     uint32 avail_size = KERNEL_HEAP_MAX - KERNEL_HEAP_START- ROUNDUP(size_of_mem_block,PAGE_SIZE);
	 	     first_in_available->size = avail_size ;
	 	     uint32 avail_sva = ROUNDUP(size_of_mem_block,PAGE_SIZE);
	 	     first_in_available->sva = KERNEL_HEAP_START + avail_sva ;
	 	     LIST_REMOVE(&AvailableMemBlocksList,first_in_available);
	 	     LIST_INSERT_HEAD(&FreeMemBlocksList,first_in_available);



}


void* kmalloc(unsigned int size)
{
	int var = 0;
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");

	//TODO: [PROJECT MS2] [KERNEL HEAP] kmalloc
	// your code is here, remove the panic and write your code

	//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
	//refer to the project presentation and documentation for details
	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy

	//change this "return" according to your answer
	struct MemBlock * block;
	size=ROUNDUP(size,PAGE_SIZE);


	if(isKHeapPlacementStrategyFIRSTFIT())
	{
		block=alloc_block_FF(size);

		if(block!=NULL){
			insert_sorted_allocList(block);
			allocate_chunk(ptr_page_directory,block->sva,size,PERM_WRITEABLE);
			var = 1 ;
		}
		else
			return NULL;
	}
	else if(isKHeapPlacementStrategyBESTFIT())
	{
		//	kpanic_into_prompt("kmalloc() is not implemented yet...!!");
		block=alloc_block_BF(size);
			if(block!=NULL){
				insert_sorted_allocList(block);
				allocate_chunk(ptr_page_directory,block->sva,size,PERM_WRITEABLE);
				var = 1 ;
			}
		   else
			   return NULL;
	}
	else if(isKHeapPlacementStrategyNEXTFIT()){
		block=alloc_block_NF(size);
		if(block!=NULL){
			insert_sorted_allocList(block);
			allocate_chunk(ptr_page_directory,block->sva,size,PERM_WRITEABLE);
			var = 1;
		}
		else
			return NULL;
	}
	if(var == 1 ){
		return (void*)block->sva;
	}
	else
		return NULL ;
	//return 0 ;
}


void kfree(void* virtual_address)
{

	struct MemBlock* block = find_block(&AllocMemBlocksList, (uint32)virtual_address);
	if (block!= NULL)
	{
		LIST_REMOVE(&AllocMemBlocksList, block);
		uint32 end = block->size + (uint32)virtual_address;
		for(uint32 iter = (uint32)virtual_address ; iter < end ; iter += PAGE_SIZE)
			unmap_frame(ptr_page_directory, iter);

		insert_sorted_with_merge_freeList(block);
	}

}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] kheap_virtual_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");
	struct FrameInfo* f  = to_frame_info(physical_address);
	unsigned int output = f->va ;
return output;

}


unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] kheap_physical_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");
	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details
	uint32 *ptr_page_table=NULL;
	int ret=get_page_table(ptr_page_directory,(uint32)virtual_address,&ptr_page_table);
	if(ret==TABLE_NOT_EXIST)
	{
		return 0;
	}

	int phy=((ptr_page_table[PTX(virtual_address)])>>12)*PAGE_SIZE;
	return phy;

}



void kfreeall()
{
	panic("Not implemented!");

}

void kshrink(uint32 newSize)
{
	panic("Not implemented!");
}

void kexpand(uint32 newSize)
{
	panic("Not implemented!");
}




//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
 int check = 0;
 struct MemBlock * block ;
 struct MemBlock * block_new ;

	//TODO: [PROJECT MS2 - BONUS] [KERNEL HEAP] krealloc
	// Write your code here, remove the panic and write your code
	//panic("krealloc() is not implemented yet...!!");
	if(virtual_address == NULL ) {
		block = kmalloc((unsigned int)new_size);
		check = 1 ;
	}
	else if(new_size == 0){
		kfree(virtual_address);
		check = 0;
	}
	else{
		 block = find_block(&AllocMemBlocksList,(uint32)virtual_address);
		uint32 sva_after_block = block->sva + block->size;
		struct MemBlock * block_af = find_block(&AllocMemBlocksList,sva_after_block);
		 if(block_af == NULL){
			struct MemBlock * block_af_free = find_block(&FreeMemBlocksList,sva_after_block);
                   uint32 diff_for_new_size = new_size - block->size;
                   if(block_af_free->size >=diff_for_new_size ){
                	   if(block_af_free->size == diff_for_new_size){
                		   LIST_REMOVE(&FreeMemBlocksList,block_af_free);
                		   block->size = new_size;
                			check = 1 ;
                		  // return block;
                	   }
                	   else if(block_af_free->size > diff_for_new_size){
                		   block->size = new_size;
                		   block_af_free->size = (block_af_free->size - diff_for_new_size);
                		   block_af_free->sva = block_af_free->sva + diff_for_new_size ;
                			check = 1 ;
                		  // return block;

                	   }
                   }
                   else{
                                  // was allocated
                   					   block_new = alloc_block_BF(new_size);
                   					   if(block_new != NULL){
                               		  // LIST_REMOVE(&AllocMemBlocksList,block);
                               		  // LIST_INSERT_HEAD(&FreeMemBlocksList,block);
                                          check = 2;
                   					   }
                                      }


		}
         else{
               // was allocated
			 block_new = alloc_block_BF(new_size);
				if(block_new != NULL){
            	// LIST_REMOVE(&AllocMemBlocksList,block);
            	// LIST_INSERT_HEAD(&FreeMemBlocksList,block);
                       check = 2;
					   }
            }




	}
struct MemBlock * b ;
if(check == 1){
	b = block ;
}

else if(check == 2 ){
	b = block_new ;
}

else{
	b = NULL ;

}



     return b;
}

