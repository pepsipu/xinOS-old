#define HEAP_START 0x8900 // begin heap right above where kernel is loaded

/*
 * often, you'll see me typecast pointers to char* and then offset them by something. This is done in order to avoid
 * annoying pointer arithmetic rules, since ptr + 1 is actually ptr + (1 * sizeof(ptr)) which can get really annoying to
 * work with. I cast pointers to char pointers first in order to solve this since chars are 1 byte.
 */

struct chunk {
    struct chunk *fd;
    unsigned int size;
};

unsigned int used_space = 0;
struct chunk free_bin = {.fd = 0, .size = 0};

void *_find_free(unsigned int size) {
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

void *alloc(unsigned int usable_size) {
    int new_size = usable_size + sizeof(struct chunk);
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
    return ((char *) ptr) + sizeof(struct chunk); // return pointer to free space after metadata
}

void free(void *chunk) {
    struct chunk *true_chunk = chunk - sizeof(struct chunk); // get actual chunk with metadata
    void *save = free_bin.fd;
    free_bin.fd = true_chunk;
    true_chunk->fd = save;
}

