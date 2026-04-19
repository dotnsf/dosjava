#ifndef OBJECT_H
#define OBJECT_H

#include "../types.h"

/**
 * Base Object structure - foundation for all Java objects
 * 
 * This is the C representation of java.lang.Object
 */
typedef struct Object {
    uint16_t class_id;       /* Class identifier */
    uint16_t ref_count;      /* Reference count for GC */
} Object;

/**
 * Create a new Object instance
 * @param class_id Class identifier
 * @return Pointer to new Object, or NULL on error
 */
Object* object_new(uint16_t class_id);

/**
 * Delete an Object instance
 * @param obj Object to delete
 */
void object_delete(Object* obj);

/**
 * Add a reference to an object (increment ref count)
 * @param obj Object to reference
 */
void object_addref(Object* obj);

/**
 * Release a reference to an object (decrement ref count)
 * If ref count reaches 0, object is deleted
 * @param obj Object to release
 */
void object_release(Object* obj);

/**
 * Compare two objects for equality
 * Default implementation: reference equality
 * @param obj1 First object
 * @param obj2 Second object
 * @return 1 if equal, 0 otherwise
 */
uint8_t object_equals(Object* obj1, Object* obj2);

/**
 * Get hash code for an object
 * Default implementation: address-based hash
 * @param obj Object
 * @return Hash code
 */
uint16_t object_hashcode(Object* obj);

/**
 * Get string representation of an object
 * Default implementation: "Object@<address>"
 * @param obj Object
 * @return String representation (caller must free)
 */
char* object_tostring(Object* obj);

/**
 * Get class ID of an object
 * @param obj Object
 * @return Class ID
 */
uint16_t object_getclass(Object* obj);

#endif /* OBJECT_H */

// Made with Bob
