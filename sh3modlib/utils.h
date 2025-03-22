#ifndef SH3ML_UTILS_H
#define SH3ML_UTILS_H

#include <stdint.h>

struct membuf
{
	void *buf;
	size_t size;
	size_t capacity;
	size_t pos;
};

enum membuf_setmode
{
	MB_MODE_SET,
	MB_MODE_CUR,
	MB_MODE_END,
};

void membuf_init(struct membuf *buffer);
void membuf_free(struct membuf *buffer);
void membuf_set_capacity(struct membuf *buffer, size_t new_cap);
void membuf_set_size(struct membuf *buffer, size_t new_size);
void membuf_add_capacity(struct membuf *buffer, size_t add_cap);
void membuf_add_size(struct membuf *buffer, size_t add_size);
void *membuf_at_pos(struct membuf *buffer);
void membuf_move_fwd(struct membuf *buffer, size_t off);
void membuf_move_back(struct membuf *buffer, size_t off);
void membuf_seek(struct membuf *buffer, int pos, enum membuf_setmode mode);
int membuf_is_pos_valid(struct membuf *buffer);
void membuf_read(struct membuf *buffer, void *dest, size_t size);

#endif