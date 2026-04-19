#include "object.h"
#include "../vm/memory.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * Create a new Object instance
 */
Object* object_new(uint16_t class_id) {
    Object* obj;
    
    obj = (Object*)memory_alloc(sizeof(Object));
    if (obj == NULL) {
        return NULL;
    }
    
    obj->class_id = class_id;
    obj->ref_count = 1;  /* Start with ref count of 1 */
    
    return obj;
}

/**
 * Delete an Object instance
 */
void object_delete(Object* obj) {
    if (obj == NULL) {
        return;
    }
    
    memory_free(obj);
}

/**
 * Add a reference to an object
 */
void object_addref(Object* obj) {
    if (obj == NULL) {
        return;
    }
    
    obj->ref_count++;
}

/**
 * Release a reference to an object
 */
void object_release(Object* obj) {
    if (obj == NULL) {
        return;
    }
    
    if (obj->ref_count > 0) {
        obj->ref_count--;
    }
    
    /* If ref count reaches 0, delete the object */
    if (obj->ref_count == 0) {
        object_delete(obj);
    }
}

/**
 * Compare two objects for equality
 * Default implementation: reference equality
 */
uint8_t object_equals(Object* obj1, Object* obj2) {
    return (obj1 == obj2) ? 1 : 0;
}

/**
 * Get hash code for an object
 * Default implementation: address-based hash
 */
uint16_t object_hashcode(Object* obj) {
    if (obj == NULL) {
        return 0;
    }
    
    /* Use object address as hash code */
    return (uint16_t)((uintptr_t)obj & 0xFFFF);
}

/**
 * Get string representation of an object
 * Default implementation: "Object@<address>"
 */
char* object_tostring(Object* obj) {
    char* str;
    
    if (obj == NULL) {
        return NULL;
    }
    
    /* Allocate buffer for string */
    str = (char*)memory_alloc(32);
    if (str == NULL) {
        return NULL;
    }
    
    /* Format string */
    sprintf(str, "Object@%04X", (unsigned int)obj);
    
    return str;
}

/**
 * Get class ID of an object
 */
uint16_t object_getclass(Object* obj) {
    if (obj == NULL) {
        return 0;
    }
    
    return obj->class_id;
}

// Made with Bob
