#include "../front.h"

#include <assert.h>
#include <crk/string.h>
#include <crk/vector.h>
#include <crk/memstream.h>
#include <cstring>
#include <fstream>
#include <map>

#include "types.h"
#include "../types.h"

static constexpr u64 Arc_tag = COM_4CHAR_TAG('a', 'r', 'c', '2');

struct str_less
{
	bool operator() (const char * lhs, const char * rhs) const
	{
		return strcmp(lhs, rhs) < 0;
	}
};

template <typename T>
using string_map = std::map<const char*, T, str_less, mem_Allocator<std::pair<const char* const, T>, Arc_tag>>;

struct _impl_arc_Context
{
	crk::string<crk::string_tag> gameDataPath;
	bool isTableLoaded {false};
	_impl_arc_Table table;
	string_map<size_t> cluster_name_table;
	string_map<size_t> vfile_name_table;
	mem_Array<arc_Cluster> cached_clusters {nullptr, 0};
	mem_Array<arc_Vfile> cached_vfiles {nullptr, 0};
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

void arc_SetDataDirectory(const char *path)
{
	g_CurrentContext->gameDataPath = path;
}

_impl_arc_Table createTableFromStream(std::istream &in)
{
	assert((g_CurrentContext != nullptr));

	crk::MemoryStream datastream;
	decompress_file(datastream, in);

	int cluster_it = 0;
	int vfile_it = 0;
	int strings_it = 0;
	int string_size = 0;
	_impl_arc_TableItem item;

	_impl_arc_Table table;
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

			datastream.Read(temp_string.data(), string_size);
			table.strings.AddString(temp_string.c_str());

			cluster_it++;
			break;
		case 3:
			datastream.Read<4>(table.vfiles[vfile_it]);

			string_size = item.size - 8;
			temp_string.resize(string_size);
			table.vfiles[vfile_it].string_index = table.strings.Size();

			datastream.Read(temp_string.data(), string_size);
			table.strings.AddString(temp_string.c_str());

			vfile_it++;
			break;
		}
	}

	return table;
}

void arc_LoadData()
{
	auto file_path = g_CurrentContext->gameDataPath + "/arc.arc";
	std::ifstream arc_file(file_path.c_str(), std::ios_base::binary);
	if (!arc_file.is_open())
	{
		printf("Could not load arc.arc file! Check if you set correctly the data directory.\n");
		return;
	}
	g_CurrentContext->table = createTableFromStream(arc_file);
	arc_file.close();
	g_CurrentContext->isTableLoaded = true;

	auto &table = g_CurrentContext->table;
	g_CurrentContext->cached_clusters = mem_AllocArray<arc_Cluster>(table.clusters.size(), Arc_tag);
	g_CurrentContext->cached_vfiles = mem_AllocArray<arc_Vfile>(table.vfiles.size(), Arc_tag);

	// generate name maps & arrays
	u32 count = 0;
	for (auto &cluster : table.clusters)
	{
		g_CurrentContext->cluster_name_table[table.strings[cluster.string_index]] = count;
		g_CurrentContext->cached_clusters[count] = {
			.id = count,
			.vfile_count = cluster.vfile_count,
			.name = table.strings[cluster.string_index],
		};
		count++;
	}
	count = 0;
	for (auto &vfile : table.vfiles)
	{
		g_CurrentContext->vfile_name_table[table.strings[vfile.string_index]] = count;
		g_CurrentContext->cached_vfiles[count] = {
			.id = count,
			.index = vfile.index,
			.cluster_id = vfile.cluster_index,
			.name = table.strings[vfile.string_index],
		};
		count++;
	}
}

bool arc_IsDataLoaded()
{
	return g_CurrentContext->isTableLoaded;
}

void arc_FreeData()
{
	g_CurrentContext->table = {};
	mem_FreeArray(g_CurrentContext->cached_clusters, Arc_tag);
	mem_FreeArray(g_CurrentContext->cached_vfiles, Arc_tag);
	g_CurrentContext->cluster_name_table.clear();
	g_CurrentContext->vfile_name_table.clear();
	g_CurrentContext->isTableLoaded = false;
}

arc_Cluster arc_GetCluster(const char *name)
{
	assert(arc_IsDataLoaded() && "The arc data must be loaded first!");
	return g_CurrentContext->cached_clusters[g_CurrentContext->cluster_name_table[name]];
}

mem_Array<arc_Cluster> arc_GetClusters()
{
	return g_CurrentContext->cached_clusters;
}

arc_Vfile arc_GetVfile(const char *name)
{
	assert(arc_IsDataLoaded() && "The arc data must be loaded first!");
	return g_CurrentContext->cached_vfiles[g_CurrentContext->vfile_name_table[name]];
}

mem_Array<arc_Vfile> arc_GetVfiles()
{
	return g_CurrentContext->cached_vfiles;
}

mem_Chunk arc_LoadVfile(const char *name)
{
	auto &ctx = g_CurrentContext;
	auto &table = ctx->table;

	arc_Vfile vfile = ctx->cached_vfiles[ctx->vfile_name_table[name]];
	arc_Cluster cluster = ctx->cached_clusters[vfile.cluster_id];
	crk::string cluster_filename = cluster.name;
	cluster_filename += ".arc";
        cluster_filename = ctx->gameDataPath + "/" + cluster_filename;

        std::ifstream cluster_file(cluster_filename.c_str(), std::ios_base::binary);
        if (!cluster_file.is_open())
            return {nullptr, 0};

        uint32_t magic{0}, vfile_count{0}, data_offset{0};
        cluster_file.read((char*)&magic, sizeof(magic));
        if (magic != 0x20030507)
            return {nullptr, 0};

        cluster_file.read((char*)&vfile_count, sizeof(vfile_count));
        cluster_file.read((char*)&data_offset, sizeof(data_offset));
        uint32_t vfile_index = vfile.index;
        cluster_file.seekg(0x10 + (vfile_index * 0x10), std::ios_base::beg);
        struct vfile_entry
        {
            uint32_t offset{0};
            uint32_t some_number{0};
            uint32_t length{0};
            uint32_t length_again{0};
        } entry;
        cluster_file.read((char*)&entry, sizeof(entry));
	mem_Chunk vfile_data = mem_AllocChunk(entry.length, Arc_tag);
        cluster_file.seekg(entry.offset, std::ios_base::beg);
        cluster_file.read((char*)vfile_data.data, entry.length);
        cluster_file.close();

        return vfile_data;
}

void arc_FreeVfile(mem_Chunk chunk)
{
	mem_FreeChunk(chunk, Arc_tag);
}