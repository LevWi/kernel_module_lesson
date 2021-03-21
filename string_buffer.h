#ifndef _STRING_BUFFER_H
#define _STRING_BUFFER_H

#include <linux/types.h>

#define TRUE   (1 == 1)
#define FALSE  (!TRUE)
#define FAIL   (-1)

const size_t STRING_ENTRY_LEN = 512;
struct substring {
    char payload[STRING_ENTRY_LEN];
    struct substring * next;
};

struct string_buffer {
    struct substring * head;
    struct substring * tail;
    size_t capacity;
    size_t r_cursor; //reverse_cursor
    size_t f_cursor; //forward_cursor

    void* allocator;
    struct substring* (*substring_new)(void* allocator);
    void (*substring_free)(void* allocator, struct substring* ss) {
}


void substring_init(struct substring* ss)
{
    ss->payload[0] = '\0';
    ss->next = NULL;
};

void string_buffer_init(struct string_buffer* sb) {
    //memset ?
    sb->head = NULL;
    sb->tail = NULL;
    sb->capacity = 0;
    sb->f_cursor = 0;
    sb->r_cursor = STRING_ENTRY_LEN;
}

void string_buffer_deinit(struct string_buffer* sb) {
    //TODO
}

int string_buffer_push_back(struct string_buffer* sb, struct substring* tmp) {
    if(!tmp) {
        return FALSE;
    }

    if (!sb->head) {
        sb->head = tmp;
        sb->tail = tmp;
    } else {
        sb->tail->next = tmp;
        sb->tail = tmp;
    }

    sb->capacity += STRING_ENTRY_LEN;
    return TRUE;
}

//TODO return error
int string_buffer_pop_front(struct string_buffer* sb) {
    ssize_t t;
    if (!sb->head) { 
        struct substring* tmp = sb->head;
        if (sb->head == sb->tail) {
            string_buffer_init(sb);
        } else {
            sb->head = sb->head->next;
            sb->capacity -= STRING_ENTRY_LEN;
            sb->f_cursor = 0;
        }
        sb->substring_free(sb->allocator, tmp);
        return TRUE;
    } else {
        return FALSE;
    }
}

size_t string_buffer_actual_length(struct string_buffer* sb) {
    //Todo error here
    return sb->capacity != 0 ? sb->capacity - sb->r_cursor - sb->f_cursor : 0;
}

// Return zero for success
typedef int (*copy_to_buffer_callback_t)(char *to, const char *from, size_t count, void* context);

//TODO how to return error ?
ssize_t move_string_to_buffer(struct string_buffer* from, 
                              char* to, size_t count, 
                              copy_to_buffer_callback_t copy_callback, void* context)
{
    size_t actual_length = string_buffer_actual_length(from);
    if (actual_length == 0 || count == 0) {
        return 0;
    }

    if (count > actual_length) {
        count = actual_length;
    }

    ssize_t result = count;

    while(count) {
        size_t rest_of_substring = STRING_ENTRY_LEN - from->f_cursor;
        if (rest_of_substring > count) {
            rest_of_substring = count;
        }

        char* current_payload_ptr = from->head->payload + from->f_cursor;

        if(copy_callback(to, current_payload_ptr, rest_of_substring, context)) {
            goto fail_;    
        }
        to += rest_of_substring;
        count -= rest_of_substring;
        from->f_cursor += rest_of_substring;

        if (from->f_cursor + 1 >= STRING_ENTRY_LEN && 
                !string_buffer_pop_front(from)) {
            goto fail_;
        }
    }

fail_:
    result -= count; //TODO handling error
    return result;
}

size_t string_buffer_capacity_available(conts struct string_buffer* sb) {
    return sb->capacity == 0 ? 0 : sb->r_cursor;
}

ssize_t string_buffer_append(struct string_buffer* to, 
                             const char* from, ssize_t count, 
                             copy_to_buffer_callback_t copy_callback, void* context) 
{
    if (count == 0) {
        return 0;
    }
    
    ssize_t capacity_needed =  count - string_buffer_capacity_available(to);
    if (capacity_needed > 0) {
        int push_backs_needed = capacity_needed / STRING_ENTRY_LEN + capacity_needed % STRING_ENTRY_LEN ? 1 : 0;
        for (size_t i = 0; i < push_backs_needed; i++)
        {
            /* code */
        }
        
    } 
    // Fill to the end of rest
}

/*
TODO

validate_string_buffer

trim_front(len)
append_string(const char*, len)

*/

#undef TRUE
#undef FALSE

#endif 
