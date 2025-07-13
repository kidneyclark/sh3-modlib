#include "../front.h"
#include <assert.h>
#include <crk/string.h>
#include <crk/vector.h>
#include <crk/memstream.h>

struct _impl_arc_Context
{
	crk::string<crk::string_tag> gameDataPath;
};

COM_CONTEXT_IMPL(arc)

#include <zlib.h>

#define ZLIB_CHUNK (1024 * 128)

void decompress_file(crk::MemoryStream &ostream, std::istream &in)
{
	const char *error_code = NULL;

	int ret;
	unsigned have;
	z_stream strm;
	u8 *in_buf = (u8 *)malloc(ZLIB_CHUNK); // 128K buffer
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;

	ret = inflateInit2(&strm, 16 + MAX_WBITS);
	if (ret != Z_OK)
	{
		error_code = "could not init zlib's inflation.";
		goto out;
	}

	do
	{
		strm.avail_in = in.readsome((char *)in_buf, ZLIB_CHUNK);
		// strm.avail_in = fread(in_buf, 1, ZLIB_CHUNK, file);
		if (!in.good())
		{
			error_code = "file read error.";
			inflateEnd(&strm);
			goto out;
		}
		if (strm.avail_in == 0)
			break;
		strm.next_in = in_buf;
		do
		{
			ostream.AddCapacity(ZLIB_CHUNK);
			strm.avail_out = ZLIB_CHUNK;
			strm.next_out = ostream.AtPos<Bytef*>();
			ret = inflate(&strm, Z_NO_FLUSH);
			assert((ret != Z_STREAM_ERROR) &&
			       "zlib's inflate() stream error!");
			switch (ret)
			{
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				inflateEnd(&strm);
				goto out;
			}
			have = ZLIB_CHUNK - strm.avail_out;
			ostream.AddSize(have);
			ostream.MovePos(have);
		} while (strm.avail_out == 0);
	} while (ret != Z_STREAM_END);

	free(in_buf);
	inflateEnd(&strm);
out:
	if (error_code != NULL)
	{
		printf("error: %s\n", error_code);
		fflush(stdout);
	}
}

struct arc_table_item
{
	uint16_t type;
	uint16_t size;
};

arc_Table arc_CreateTableFromStream(std::istream &in)
{
	assert((g_CurrentContext != nullptr));

	crk::MemoryStream datastream;
	decompress_file(datastream, in);

	int cluster_it = 0;
	int vfile_it = 0;
	int strings_it = 0;
	int string_size = 0;
	arc_table_item item;

	arc_Table table;
	crk::string temp_string;

	datastream.SetPos(16);
	while (datastream.IsPosValid())
	{
		datastream.Read(item);
		temp_string.clear();

		switch (item.type)
		{
		case 1:
			uint32_t cluster_count;
			uint32_t vfile_count;
			datastream.Read(cluster_count);
			datastream.Read(vfile_count);
			table.clusters.resize(cluster_count);
			table.vfiles.resize(vfile_count);
			break;
		case 2:
			datastream.Read<4>(table.clusters[cluster_it]);

			string_size = item.size - 8;
			temp_string.resize(string_size);
			table.clusters[cluster_it].string_index = table.strings.Size();
			cluster_it++;

			datastream.Read(temp_string.data(), string_size);
			table.strings.AddString(temp_string.c_str());
			break;
		case 3:
			datastream.Read<4>(table.vfiles[vfile_it]);

			string_size = item.size - 8;
			temp_string.resize(string_size);
			table.vfiles[vfile_it].string_index = table.strings.Size();
			vfile_it++;

			datastream.Read(temp_string.data(), string_size);
			table.strings.AddString(temp_string.c_str());
			break;
		}
	}

	return table;
}

#include <cstring>

size_t arc_GetTableClusterIndex(arc_Table &table, const char *cluster)
{
	assert(cluster != NULL);
	size_t index = 0;
	for (auto &cl : table.clusters)
	{
		if (strcmp(table.strings[cl.string_index], cluster) == 0)
			break;
		index++;
	}
	return index;
}