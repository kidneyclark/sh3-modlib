#ifndef UTILS_MEMBUF_H
#define UTILS_MEMBUF_H

#include <com/types.h>
#include <mem/types.h>

namespace crk
{
struct MemoryStream
{
	MemoryStream();
	~MemoryStream();

	void SetCapacity(size_t new_capacity);
	void SetSize(size_t new_size);
	void AddCapacity(size_t new_capacity);
	void AddSize(size_t new_size);
	void *AtPos();

	template <typename T> T AtPos()
	{
		return (T)(((u8 *)chunk.data) + pos);
	}

	void MovePos(size_t off, bool forward = true);
	void SetPos(size_t off, bool from_start = true);
	bool IsPosValid();

	template <typename T> void Read(T &dest)
	{
		memcpy(&dest, AtPos(), sizeof(T));
		MovePos(sizeof(T));
	}

	template <typename U, typename T> void Read(T &dest)
	{
		memcpy((U *)&dest, AtPos(), sizeof(U));
		MovePos(sizeof(U));
	}

	template <size_t R, typename T> void Read(T &dest)
	{
		memcpy(&dest, AtPos(), R);
		MovePos(R);
	}

	template <size_t R, typename T> void Read(T *dest)
	{
		memcpy(dest, AtPos(), R);
		MovePos(R);
	}

	// This one is the most unsafe of the Read(...) funcs :)
	template <typename T> void Read(T &dest, size_t length)
	{
		memcpy(&dest, AtPos(), length);
		MovePos(length);
	}

	// This one is the most unsafe of the Read(...) funcs :)
	template <typename T> void Read(T *dest, size_t length)
	{
		memcpy(dest, AtPos(), length);
		MovePos(length);
	}

	mem_Chunk chunk;
	size_t pos{0};
	size_t size{0};
};
} // namespace crk

#endif // SH3_CORE_H