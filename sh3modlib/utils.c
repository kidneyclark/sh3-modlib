#include "utils.h"

#include <stdlib.h>
#include <string.h>

void membuf_init(struct membuf *buffer)
{
	buffer->buf = NULL;
	buffer->capacity = 0;
	buffer->size = 0;
	buffer->pos = 0;
}

void membuf_free(struct membuf *buffer)
{
	free(buffer->buf);
	membuf_init(buffer);
}

void membuf_set_capacity(struct membuf *buffer, size_t new_cap)
{
	buffer->capacity = new_cap;
	buffer->buf = realloc(buffer->buf, new_cap);
	if (buffer->size < buffer->capacity)
		buffer->size = buffer->capacity;
}

void membuf_set_size(struct membuf *buffer, size_t new_size)
{
	buffer->size = new_size;
	if (buffer->size > buffer->capacity)
		membuf_set_capacity(buffer, buffer->size);
}

void membuf_add_capacity(struct membuf *buffer, size_t add_cap)
{
	membuf_set_capacity(buffer, buffer->capacity + add_cap);
}

void membuf_add_size(struct membuf *buffer, size_t add_size)
{
	membuf_set_size(buffer, buffer->size + add_size);
}

void *membuf_at_pos(struct membuf *buffer)
{
	return (void *)(((uint8_t *)buffer->buf) + buffer->pos);
}

void membuf_seek(struct membuf *buffer, int pos, enum membuf_setmode mode)
{
	switch (mode)
	{
	case MB_MODE_SET:
		buffer->pos = pos;
		break;
	case MB_MODE_CUR:
		buffer->pos += pos;
		break;
	case MB_MODE_END:
		buffer->pos = buffer->capacity - pos;
		break;
	}
}

int membuf_is_pos_valid(struct membuf *buffer)
{
	return (buffer->pos < buffer->capacity);
}

void membuf_read(struct membuf *buffer, void *dest, size_t size)
{
	memcpy(dest, membuf_at_pos(buffer), size);
	membuf_seek(buffer, size, MB_MODE_CUR);
}