/*
 * fault_handler.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include "trap.h"
#include <kern/proc/user_environment.h>
#include "../cpu/sched.h"
#include "../disk/pagefile_manager.h"
#include "../mem/memory_manager.h"

//2014 Test Free(): Set it to bypass the PAGE FAULT on an instruction with this length and continue executing the next one
// 0 means don't bypass the PAGE FAULT
uint8 bypassInstrLength = 0;

//===============================
// REPLACEMENT STRATEGIES
//===============================
//2020
void setPageReplacmentAlgorithmLRU(int LRU_TYPE) {
	assert(
			LRU_TYPE == PG_REP_LRU_TIME_APPROX || LRU_TYPE == PG_REP_LRU_LISTS_APPROX);
	_PageRepAlgoType = LRU_TYPE;
}
void setPageReplacmentAlgorithmCLOCK() {
	_PageRepAlgoType = PG_REP_CLOCK;
}
void setPageReplacmentAlgorithmFIFO() {
	_PageRepAlgoType = PG_REP_FIFO;
}
void setPageReplacmentAlgorithmModifiedCLOCK() {
	_PageRepAlgoType = PG_REP_MODIFIEDCLOCK;
}
/*2018*/void setPageReplacmentAlgorithmDynamicLocal() {
	_PageRepAlgoType = PG_REP_DYNAMIC_LOCAL;
}
/*2021*/void setPageReplacmentAlgorithmNchanceCLOCK(int PageWSMaxSweeps) {
	_PageRepAlgoType = PG_REP_NchanceCLOCK;
	page_WS_max_sweeps = PageWSMaxSweeps;
}

//2020
uint32 isPageReplacmentAlgorithmLRU(int LRU_TYPE) {
	return _PageRepAlgoType == LRU_TYPE ? 1 : 0;
}
uint32 isPageReplacmentAlgorithmCLOCK() {
	if (_PageRepAlgoType == PG_REP_CLOCK)
		return 1;
	return 0;
}
uint32 isPageReplacmentAlgorithmFIFO() {
	if (_PageRepAlgoType == PG_REP_FIFO)
		return 1;
	return 0;
}
uint32 isPageReplacmentAlgorithmModifiedCLOCK() {
	if (_PageRepAlgoType == PG_REP_MODIFIEDCLOCK)
		return 1;
	return 0;
}
/*2018*/uint32 isPageReplacmentAlgorithmDynamicLocal() {
	if (_PageRepAlgoType == PG_REP_DYNAMIC_LOCAL)
		return 1;
	return 0;
}
/*2021*/uint32 isPageReplacmentAlgorithmNchanceCLOCK() {
	if (_PageRepAlgoType == PG_REP_NchanceCLOCK)
		return 1;
	return 0;
}

//===============================
// PAGE BUFFERING
//===============================
void enableModifiedBuffer(uint32 enableIt) {
	_EnableModifiedBuffer = enableIt;
}
uint8 isModifiedBufferEnabled() {
	return _EnableModifiedBuffer;
}

void enableBuffering(uint32 enableIt) {
	_EnableBuffering = enableIt;
}
uint8 isBufferingEnabled() {
	return _EnableBuffering;
}

void setModifiedBufferLength(uint32 length) {
	_ModifiedBufferLength = length;
}
uint32 getModifiedBufferLength() {
	return _ModifiedBufferLength;
}

//===============================
// FAULT HANDLERS
//===============================

//Handle the table fault
void table_fault_handler(struct Env * curenv, uint32 fault_va) {
	//panic("table_fault_handler() is not implemented yet...!!");
	//Check if it's a stack page
	uint32* ptr_table;
#if USE_KHEAP
	{
		ptr_table = create_page_table(curenv->env_page_directory,
				(uint32) fault_va);
	}
#else
	{
		__static_cpt(curenv->env_page_directory, (uint32)fault_va, &ptr_table);
	}
#endif
}

//Handle the page fault
void placement_(struct Env * curenv, uint32 fault_va) {
	struct FrameInfo* new_frame;
	int res = allocate_frame(&new_frame);
	if (res != E_NO_MEM) {
		map_frame(curenv->env_page_directory, new_frame, fault_va,
		PERM_PRESENT | PERM_USER | PERM_WRITEABLE);
		int ret = pf_read_env_page(curenv, (void*) fault_va);
		if (ret == E_PAGE_NOT_EXIST_IN_PF) {
			if ((fault_va >= USTACKBOTTOM && fault_va < USTACKTOP)
					|| (fault_va >= USER_HEAP_START && fault_va < USER_HEAP_MAX)) {
				uint32 flag = 0;
				for (uint32 i = curenv->page_last_WS_index;
						i < curenv->page_WS_max_size; i++) {

					int ret_ = env_page_ws_is_entry_empty(curenv, i);
					if (ret_ == 1) {
						curenv->page_last_WS_index = i;
						env_page_ws_set_entry(curenv,
								curenv->page_last_WS_index, fault_va);
						int var = curenv->page_last_WS_index;
						var++;
						if (var > curenv->page_WS_max_size - 1) {
							curenv->page_last_WS_index = 0;
						} else {

							curenv->page_last_WS_index++;
						}

						flag = 1;
						break;
					}
				}

				if (flag == 0) {
					for (uint32 i = 0; i < curenv->page_last_WS_index; i++) {

						int ret_ = env_page_ws_is_entry_empty(curenv, i);
						if (ret_ == 1) {
							curenv->page_last_WS_index = i;
							env_page_ws_set_entry(curenv,
									curenv->page_last_WS_index, fault_va);
							int var = curenv->page_last_WS_index;
							var++;
							if (var > curenv->page_WS_max_size - 1) {
								curenv->page_last_WS_index = 0;
							} else {

								curenv->page_last_WS_index++;
							}

							flag = 1;
							break;
						}
					}
				}
			} else {
				unmap_frame(curenv->env_page_directory, fault_va);
				panic("ILLEGAL MEM ACCESS\n");

			}

		} else {
			uint32 flag = 0;
			for (uint32 i = curenv->page_last_WS_index;
					i < curenv->page_WS_max_size; i++) {

				int ret_ = env_page_ws_is_entry_empty(curenv, i);
				if (ret_ == 1) {
					curenv->page_last_WS_index = i;
					env_page_ws_set_entry(curenv, curenv->page_last_WS_index,
							fault_va);
					int var = curenv->page_last_WS_index;
					var++;
					if (var > curenv->page_WS_max_size - 1) {
						curenv->page_last_WS_index = 0;
					} else {

						curenv->page_last_WS_index++;
					}

					flag = 1;
					break;
				}
			}

			if (flag == 0) {
				for (uint32 i = 0; i < curenv->page_last_WS_index; i++) {

					int ret_ = env_page_ws_is_entry_empty(curenv, i);
					if (ret_ == 1) {
						curenv->page_last_WS_index = i;
						env_page_ws_set_entry(curenv,
								curenv->page_last_WS_index, fault_va);
						int var = curenv->page_last_WS_index;
						var++;
						if (var > curenv->page_WS_max_size - 1) {
							curenv->page_last_WS_index = 0;
						} else {

							curenv->page_last_WS_index++;
						}

						flag = 1;
						break;
					}
				}
			}
		}

	}

}



void page_fault_handler(struct Env * curenv, uint32 fault_va) {

	//TODO: [PROJECT MS3] [FAULT HANDLER] page_fault_handler
	// Write your code here, remove the panic and write your code
	//panic("page_fault_handler() is not implemented yet...!!");
	//refer to the project presentation and documentation for details
	//fault_va = ROUNDDOWN(fault_va,PAGE_SIZE);
	if (env_page_ws_get_size(curenv) < curenv->page_WS_max_size) {
		placement_(curenv, fault_va);
	} else {

		int flag = 1;
		for (int i = curenv->page_last_WS_index; i < curenv->page_WS_max_size;
				i++) {

			int perm_ = pt_get_page_permissions(curenv->env_page_directory,
					curenv->ptr_pageWorkingSet[i].virtual_address);
			//int new_perm = perm_ &(111111011111);
			int modified_ = (perm_ & PERM_MODIFIED);
			int result = (perm_ & PERM_USED);
			if (result == 0) {
				if (modified_ == 0) {
					unmap_frame(curenv->env_page_directory,
							curenv->ptr_pageWorkingSet[i].virtual_address);
					env_page_ws_clear_entry(curenv, i);
					flag = 0;
					curenv->page_last_WS_index = i;
					placement_(curenv, fault_va);
					break;

				} else {
					uint32 * ptr_page_table = NULL;
					int ret = get_page_table(curenv->env_page_directory,
							curenv->ptr_pageWorkingSet[i].virtual_address,
							&ptr_page_table);
					struct FrameInfo* info = get_frame_info(
							curenv->env_page_directory,
							curenv->ptr_pageWorkingSet[i].virtual_address,
							&ptr_page_table);
					pf_update_env_page(curenv,
							curenv->ptr_pageWorkingSet[i].virtual_address,
							info);
					unmap_frame(curenv->env_page_directory,
							curenv->ptr_pageWorkingSet[i].virtual_address);
					env_page_ws_clear_entry(curenv, i);
					flag = 0;
					curenv->page_last_WS_index = i;
					placement_(curenv, fault_va);
					break;

				}

			} else {
				pt_set_page_permissions(curenv->env_page_directory,
						curenv->ptr_pageWorkingSet[i].virtual_address, 0,
						PERM_USED);

			}

		}

		if (flag == 1) {

			for (int i = 0; i < curenv->page_WS_max_size; i++) {
				int perm_ = pt_get_page_permissions(curenv->env_page_directory,
						curenv->ptr_pageWorkingSet[i].virtual_address);
				int modified_ = (perm_ & PERM_MODIFIED);
				int result = (perm_ & PERM_USED);
				if (result == 0) {
					if (modified_ == 0) {

						unmap_frame(curenv->env_page_directory,
								curenv->ptr_pageWorkingSet[i].virtual_address);
						env_page_ws_clear_entry(curenv, i);
						curenv->page_last_WS_index = i;
						placement_(curenv, fault_va);
						break;

					} else {

						uint32 * ptr_page_table = NULL;
						int ret = get_page_table(curenv->env_page_directory,
								curenv->ptr_pageWorkingSet[i].virtual_address,
								&ptr_page_table);
						struct FrameInfo* info = get_frame_info(
								curenv->env_page_directory,
								curenv->ptr_pageWorkingSet[i].virtual_address,
								&ptr_page_table);
						pf_update_env_page(curenv,
								curenv->ptr_pageWorkingSet[i].virtual_address,
								info);
						unmap_frame(curenv->env_page_directory,
								curenv->ptr_pageWorkingSet[i].virtual_address);
						env_page_ws_clear_entry(curenv, i);
						curenv->page_last_WS_index = i;
						placement_(curenv, fault_va);
						break;

					}

				}

			}
		}
	}

}




void __page_fault_handler_with_buffering(struct Env * curenv, uint32 fault_va) {
	// Write your code here, remove the panic and write your code
	panic("__page_fault_handler_with_buffering() is not implemented yet...!!");

}
