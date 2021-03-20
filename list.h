#ifndef LF_LIST_H
#define LF_LIST_H

#include <linux/string.h>
//#include <linux/types.h>

const size_t STRING_ENTRY_LEN = 512

struct substring {
    char payload[STRING_ENTRY_LEN];
    struct substring * next;
};


struct string_buffer {
    struct substring * head;
    struct substring * tail;
    size_t length;
    size_t cur_position;

    void* allocator;
    struct substring* (*substring_new)(void* allocator);
    void (*substring_free)(void* allocator, struct substring* ss) {
}


void substring_init(struct substring* ss)
{
    ss->payload[0] = '\0';
    ss->next = NULL;
};

void string_buffer_init(struct string_buffer* sl) {
    //memset ?
    sl->head = NULL;
    sl->tail = NULL;
    sl->length = 0;
    sl->cur_position = 0;
}

int string_buffer_push_back(struct string_buffer* sl, struct substring* ss) {

}
/*
TODO

push_back()
pop_front()

trim_front(len)
append_string(const char*, len)

substring_new()
substring_free()

string_buffer_deinit

*/


#endif 
