#ifndef ALLOC
#define ALLOC

#define HEAP_START 0x8200 // begin heap right above where kernel is loaded

#include <stdint.h>
#include <glob.h>

// this is a linked list allocator, where free chunks are stored in a linked list that is used before using more space.

/*
 * often, you'll see me typecast pointers to char* and then offset them by something. This is done in order to avoid
 * annoying pointer arithmetic rules, since ptr + 1 is actually ptr + (1 * sizeof(ptr)) which can get really annoying to
 * work with. I cast pointers to char pointers first in order to solve this since chars are 1 byte.
 */


struct chunk {
    struct chunk *fd; // fd will point to the next free chunk, only used when the chunk is free
    size_t size; // size of the chunk
};

uint64_t used_space = 0; // the space our heap is using so far
struct chunk free_bin = {.fd = 0, .size = 0};

/*
 * _find_free looks through the list of free chunks for a chunk that can be reused or shrunken to be reused. This is
 * preferred to making more space.
 */

void *_find_free(size_t size) {
    struct chunk *ptr = &free_bin;
    while (ptr->fd) { // loop while there are pointers in the free_bin list
        if (ptr->fd->size >= size) { // run code if we find a chunk bigger than what we requested
            ptr->fd->size -= size; // shrink that chunk
            return ((char *) ptr->fd) + size; // use the space that the free chunk is no longer using as new chunk
        }
        ptr = ptr->fd;
    }
    return 0;
}

void *alloc(size_t usable_size) {
    size_t new_size = usable_size + sizeof(struct chunk); // we need to accommodate for the metadata of the chunk
    struct chunk *ptr = _find_free(new_size);
    if (ptr) { // if we find a chunk in the free list use it
        ptr->fd = 0;
        ptr->size = usable_size;
        return ptr;
    }
    // since we did not find a chunk in the freelist, we must make space
    ptr = (struct chunk *)(HEAP_START + used_space); // get start of unused space
    ptr->fd = 0;
    ptr->size = usable_size;
    used_space += new_size; // increase the size we are using
    return ((char *) ptr) + sizeof(struct chunk); // return a pointer to free space after metadata
}

/*
 * free will add a chunk to the free list, so it may be reused.
 */

void free(void *chunk) {
    struct chunk *true_chunk = chunk - sizeof(struct chunk); // get actual chunk with metadata
    void *save = free_bin.fd; // first, we save the current fd pointer.
    free_bin.fd = true_chunk; // then, we unlink the current fd pointer, setting it to our new chunk
    true_chunk->fd = save; // we set our new chunk's fd pointer to the old chunk, completing the insertion
}

#endif