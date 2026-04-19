/**
 * Memory Manager Test Program
 * 
 * Tests the memory allocation and deallocation functions
 */

#include "vm/memory.h"
#include "runtime/object.h"
#include "runtime/string.h"
#include <stdio.h>

void test_basic_allocation(void) {
    void* p1;
    void* p2;
    void* p3;
    
    printf("Test: Basic Allocation\n");
    
    p1 = memory_alloc(100);
    printf("  Allocated 100 bytes: %s\n", p1 ? "OK" : "FAIL");
    
    p2 = memory_alloc(200);
    printf("  Allocated 200 bytes: %s\n", p2 ? "OK" : "FAIL");
    
    p3 = memory_alloc(50);
    printf("  Allocated 50 bytes: %s\n", p3 ? "OK" : "FAIL");
    
    memory_stats();
    
    memory_free(p2);
    printf("  Freed 200 bytes\n");
    
    memory_free(p1);
    printf("  Freed 100 bytes\n");
    
    memory_free(p3);
    printf("  Freed 50 bytes\n");
    
    memory_stats();
    printf("\n");
}

void test_object_creation(void) {
    Object* obj1;
    Object* obj2;
    
    printf("Test: Object Creation\n");
    
    obj1 = object_new(1);
    printf("  Created object 1: %s\n", obj1 ? "OK" : "FAIL");
    printf("  Class ID: %u, Ref count: %u\n", obj1->class_id, obj1->ref_count);
    
    obj2 = object_new(2);
    printf("  Created object 2: %s\n", obj2 ? "OK" : "FAIL");
    
    memory_stats();
    
    object_release(obj1);
    printf("  Released object 1\n");
    
    object_release(obj2);
    printf("  Released object 2\n");
    
    memory_stats();
    printf("\n");
}

void test_string_operations(void) {
    String* s1;
    String* s2;
    String* s3;
    
    printf("Test: String Operations\n");
    
    s1 = string_new("Hello");
    printf("  Created string: '%s'\n", s1 ? s1->data : "FAIL");
    printf("  Length: %u\n", string_length(s1));
    
    s2 = string_new(" World");
    printf("  Created string: '%s'\n", s2 ? s2->data : "FAIL");
    
    s3 = string_concat(s1, s2);
    printf("  Concatenated: '%s'\n", s3 ? s3->data : "FAIL");
    
    printf("  Equals test: %s\n", 
           string_equals(s1, s2) ? "FAIL (should be false)" : "OK");
    
    memory_stats();
    
    object_release((Object*)s1);
    object_release((Object*)s2);
    object_release((Object*)s3);
    
    memory_stats();
    printf("\n");
}

void test_reference_counting(void) {
    Object* obj;
    
    printf("Test: Reference Counting\n");
    
    obj = object_new(1);
    printf("  Created object, ref count: %u\n", obj->ref_count);
    
    object_addref(obj);
    printf("  Added reference, ref count: %u\n", obj->ref_count);
    
    object_addref(obj);
    printf("  Added reference, ref count: %u\n", obj->ref_count);
    
    object_release(obj);
    printf("  Released reference, ref count: %u\n", obj->ref_count);
    
    object_release(obj);
    printf("  Released reference, ref count: %u\n", obj->ref_count);
    
    memory_stats();
    
    object_release(obj);
    printf("  Released final reference (object should be deleted)\n");
    
    memory_stats();
    printf("\n");
}

void test_memory_defrag(void) {
    void* p1;
    void* p2;
    void* p3;
    void* p4;
    uint16_t coalesced;
    
    printf("Test: Memory Defragmentation\n");
    
    p1 = memory_alloc(100);
    p2 = memory_alloc(100);
    p3 = memory_alloc(100);
    p4 = memory_alloc(100);
    
    printf("  Allocated 4 blocks of 100 bytes\n");
    memory_stats();
    
    memory_free(p2);
    memory_free(p4);
    printf("  Freed blocks 2 and 4\n");
    memory_stats();
    
    coalesced = memory_defrag();
    printf("  Defragmented: %u blocks coalesced\n", coalesced);
    memory_stats();
    
    memory_free(p1);
    memory_free(p3);
    printf("  Freed remaining blocks\n");
    memory_stats();
    printf("\n");
}

int main(void) {
    printf("=== DOS Java Memory Manager Test ===\n\n");
    
    /* Initialize memory manager */
    if (memory_init(0) != 0) {
        printf("ERROR: Failed to initialize memory manager\n");
        return 1;
    }
    
    printf("Memory manager initialized\n");
    memory_stats();
    printf("\n");
    
    /* Run tests */
    test_basic_allocation();
    test_object_creation();
    test_string_operations();
    test_reference_counting();
    test_memory_defrag();
    
    /* Check heap integrity */
    printf("Test: Heap Integrity Check\n");
    if (memory_check()) {
        printf("  Heap is valid: OK\n");
    } else {
        printf("  Heap is corrupted: FAIL\n");
    }
    printf("\n");
    
    /* Shutdown */
    memory_shutdown();
    printf("Memory manager shut down\n");
    
    printf("\n=== All Tests Complete ===\n");
    
    return 0;
}

// Made with Bob
