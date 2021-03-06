#ifndef _STRING_BUFFER_H
#define _STRING_BUFFER_H

#ifdef TEST_TYPES
#include <sys/types.h>
#define STRING_ENTRY_LEN 20
#else
#include <linux/types.h>
#define STRING_ENTRY_LEN 512
#endif

#ifndef NULL
#define NULL 0
#endif

#define TRUE     (1)
#define FALSE    (0)
#define SB_FAIL  (-1)

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
    void (*substring_free)(void* allocator, struct substring* ss);
};


void substring_init(struct substring* ss)
{
    ss->payload[0] = '\0';
    ss->next = NULL;
}

void string_buffer_init(struct string_buffer* sb) {
    sb->head = NULL;
    sb->tail = NULL;
    sb->capacity = 0;
    sb->f_cursor = 0;
    sb->r_cursor = STRING_ENTRY_LEN;
}

size_t string_buffer_capacity_available(const struct string_buffer* sb) {
    return sb->capacity == 0 ? 0 : sb->r_cursor;
}

size_t string_buffer_length(struct string_buffer* sb) {
    return sb->capacity != 0 ? sb->capacity - sb->r_cursor - sb->f_cursor : 0;
}


int string_buffer_push_back(struct string_buffer* sb) {
    struct substring* tmp = sb->substring_new(sb->allocator);
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
    sb->r_cursor = STRING_ENTRY_LEN;
    return TRUE;
}

int is_fail(int result) {
    return result == SB_FAIL;
}

int string_buffer_pop_front(struct string_buffer* sb) {
    if (sb->head) {
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

void string_buffer_clear(struct string_buffer* sb) {
    if (!sb->head) {
        return;
    }

    int pop_result = 0;
    while( pop_result = string_buffer_pop_front(sb) )
        ;

    return;
}

// Return zero for success
typedef int (*copy_to_buffer_callback_t)(char *to, const char *from, size_t count, void* context);

// return value of not readed byte;
// Negative if - error
ssize_t string_buffer_extract(struct string_buffer* from,
                              char* to, ssize_t count,
                              copy_to_buffer_callback_t copy_callback, void* context)
{
    ssize_t actual_length = string_buffer_length(from);
    if (actual_length == 0 || count == 0) {
        return count;
    }

    ssize_t result = count;

    if (count > actual_length) {
        count = actual_length;
    }

    while(count) {
        ssize_t rest_of_substring = STRING_ENTRY_LEN - from->f_cursor;
        if (rest_of_substring > count) {
            rest_of_substring = count;
        }

        char* current_payload_ptr = from->head->payload + from->f_cursor;

        if(copy_callback(to, current_payload_ptr, rest_of_substring, context)) {
            //Fail
            result *= -1;
            break;
        }
        to += rest_of_substring;
        count -= rest_of_substring;
        result -= rest_of_substring;
        from->f_cursor += rest_of_substring;

        if (from->f_cursor + 1 >= STRING_ENTRY_LEN) {
            string_buffer_pop_front(from);
        }
    }

    if (string_buffer_length(from) == 0) {
        from->f_cursor = 0;
        from->r_cursor = STRING_ENTRY_LEN;
    }

    return result;
}

// return value of unrecorded bytes;
// Negative if - system error
ssize_t string_buffer_append(struct string_buffer* to,
                             const char* from, ssize_t count,
                             copy_to_buffer_callback_t copy_callback, void* context)
{
    if (count == 0) {
        return 0;
    }

    while(count) {

        ssize_t capacity_available = string_buffer_capacity_available(to);

        if (count >= capacity_available) {
            if (capacity_available == 0) {
                if (!string_buffer_push_back(to)) {
                    //Fail
                    count *= -1;
                    break;
                }
                continue; // TODO OR goto?
            }

            char* current_payload_ptr = to->tail->payload + (STRING_ENTRY_LEN - to->r_cursor);

            if(copy_callback(current_payload_ptr, from, capacity_available, context)) {
                //Fail
                count *= -1;
                break;
            }

            count -= capacity_available;
            from += capacity_available;
            to->r_cursor = 0;
        } else {
            //count < capacity_available
            char* current_payload_ptr = to->tail->payload + (STRING_ENTRY_LEN - to->r_cursor);

            if(copy_callback(current_payload_ptr, from, count, context)) {
                //Fail
                count *= -1;
                break;
            }

            to->r_cursor -= count;
            count = 0;
        }
    }
    return count;
}

#undef TRUE
#undef FALSE

#endif
