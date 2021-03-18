#ifndef LF_LIST_H
#define LF_LIST_H

#include <linux/string.h>
//#include <linux/types.h>

const size_t STRING_ENTRY_LEN = 512

struct list_entry {
    char payload[STRING_ENTRY_LEN];
    struct list_entry * next;
};


struct string_buffer {
    struct list_entry * head;
    struct list_entry * tail;
    size_t length;
    size_t cur_position;
}


void list_entry_init(struct list_entry* le)
{
    le->payload[0] = '\0';
    le->next = NULL;
};

void string_buffer_init(struct string_buffer* sl) {
    //memset ?
    sl->head = NULL;
    sl->tail = NULL;
    sl->length = 0;
    sl->cur_position = 0;
}

/*
TODO

push_back()
pop_front()

trim_front(len)
append_string(const char*, len)

list_entry_new()
list_entry_free()

string_buffer_deinit

*/


#endif 