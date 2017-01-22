/**
 * Ideal Indirection Lab
 * CS 241 - Fall 2016
 */
#include "kernel.h"
#include "mmu.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>

MMU *MMU_create() {
  MMU *mmu = calloc(1, sizeof(MMU));
  mmu->tlb = TLB_create();
  mmu->curr_pid = 0;
  return mmu;
}

void *MMU_get_physical_address(MMU *mmu, void *virtual_address, size_t pid) {
  assert(pid < MAX_PROCESS_ID);

  if (mmu->curr_pid != pid) {
    TLB_flush(&(mmu->tlb));
    mmu->curr_pid = pid;
  }

  size_t offset = (size_t)(((uintptr_t)virtual_address) & 0xFFF);

  void * offset_virtual_address = (void*)(((uintptr_t)virtual_address) & 0x7FFF);
  void *TLB_result = TLB_get_physical_address(&(mmu->tlb), offset_virtual_address);
  if (TLB_result != NULL) {
    TLB_result = (void*)(((size_t)TLB_result) + offset);
    return TLB_result;
  }
  MMU_tlb_miss(mmu, virtual_address, pid);
  size_t address1 = (size_t)((((uintptr_t)virtual_address) >> 15) & 0xFFF);
  size_t address2 = (size_t)((((uintptr_t)virtual_address) >> 27) & 0xFFF);
  size_t address3 = (size_t)((((uintptr_t)virtual_address) >> 39) & 0xFFF);
  //size_t offset = (size_t)(((uintptr_t)virtual_address) & 0xFFF);

  void *page2 = PageTable_get_entry(mmu->base_pts[pid], address1);

  if (page2 == NULL) {
    MMU_raise_page_fault(mmu, virtual_address, pid);
    PageTable *a = PageTable_create();
    PageTable_set_entry(mmu->base_pts[pid], address1, a);
    page2 = PageTable_get_entry(mmu->base_pts[pid], address1);
  }

  void *page3 = PageTable_get_entry(page2, address2);

  if (page3 == NULL) {
    MMU_raise_page_fault(mmu, virtual_address, pid);
    PageTable *b = PageTable_create();
    PageTable_set_entry(page2, address2, b);
    page3 = PageTable_get_entry(page2, address2);
  }

  void *phys_address = PageTable_get_entry(page3, address3);

  if (phys_address == NULL) {
    MMU_raise_page_fault(mmu, virtual_address, pid);
    phys_address = ask_kernel_for_frame();
    PageTable_set_entry(page3, address3, phys_address);
  }

  TLB_add_physical_address(&(mmu->tlb), offset_virtual_address, phys_address);
  phys_address = (void*)(((size_t)phys_address) + offset);
  return phys_address;

}

void MMU_tlb_miss(MMU *mmu, void *address, size_t pid) {
  assert(pid < MAX_PROCESS_ID);
  mmu->num_tlb_misses++;
  printf("Process [%lu] tried to access [%p] and it couldn't find it in the "
         "TLB so the MMU has to check the PAGE TABLES\n",
         pid, address);
}

void MMU_raise_page_fault(MMU *mmu, void *address, size_t pid) {
  assert(pid < MAX_PROCESS_ID);
  mmu->num_page_faults++;
  printf(
      "Process [%lu] tried to access [%p] and the MMU got an invalid entry\n",
      pid, address);
}

void MMU_add_process(MMU *mmu, size_t pid) {
  assert(pid < MAX_PROCESS_ID);
  mmu->base_pts[pid] = PageTable_create();
}

void MMU_free_process_tables(MMU *mmu, size_t pid) {
  assert(pid < MAX_PROCESS_ID);
  PageTable *base_pt = mmu->base_pts[pid];
  Pagetable_delete_tree(base_pt);
}

void MMU_delete(MMU *mmu) {
  for (size_t i = 0; i < MAX_PROCESS_ID; i++) {
    MMU_free_process_tables(mmu, i);
  }
  TLB_delete(mmu->tlb);
  free(mmu);
}
