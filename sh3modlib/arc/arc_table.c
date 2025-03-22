#include "arc_table.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <zlib.h>

#include "../utils.h"

#define ZLIB_CHUNK (1024 * 128)

void decompress_file(FILE *file, struct membuf *out_buf)
{
	const char *error_code = NULL;

	int ret;
	unsigned have;
	z_stream strm;
	uint8_t *in_buf = (uint8_t *)malloc(ZLIB_CHUNK); // 128K buffer
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
		strm.avail_in = fread(in_buf, 1, ZLIB_CHUNK, file);
		if (ferror(file))
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
			membuf_add_capacity(out_buf, ZLIB_CHUNK);
			strm.avail_out = ZLIB_CHUNK;
			strm.next_out = membuf_at_pos(out_buf);
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
				return;
			}
			have = ZLIB_CHUNK - strm.avail_out;
			membuf_add_size(out_buf, have);
			membuf_seek(out_buf, have, MB_MODE_CUR);
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
	return;
}

struct arc_table_item
{
	uint16_t type;
	uint16_t size;
};

int arc_table_load(struct arc_ctx *ctx, struct arc_table *table)
{
	assert(ctx != NULL);
	assert(table != NULL);

	char arctable_path[SH3ML_PATH_SIZE + 64];
	strcpy(arctable_path, ctx->data_path);
	strcat(arctable_path, "/arc.arc");

	FILE *arcfile = fopen(arctable_path, "rb");
	if (arcfile == NULL)
	{
		ctx->msg = "invalid data path or non-existent arc.arc file";
		return -1;
	}

	struct membuf decomp_buf;
	membuf_init(&decomp_buf);
	decompress_file(arcfile, &decomp_buf);
	fclose(arcfile);

	int cluster_it = 0;
	int vfile_it = 0;
	int strings_it = 0;
	int string_size = 0;
	struct arc_table_item item;

	membuf_seek(&decomp_buf, 16, MB_MODE_SET);
	while (decomp_buf.pos < decomp_buf.size)
	{
		membuf_read(&decomp_buf, &item, sizeof(item));

		switch (item.type)
		{
		case 1:
			uint32_t cluster_count;
			uint32_t vfile_count;
			membuf_read(&decomp_buf, &cluster_count, sizeof(cluster_count));
			membuf_read(&decomp_buf, &vfile_count, sizeof(vfile_count));
			table->cluster_count = cluster_count;
			table->vfile_count = vfile_count;
			table->clusters = malloc(table->cluster_count * sizeof(struct arc_table_cluster));
			table->vfiles = malloc(table->vfile_count * sizeof(struct arc_table_vfile));
			break;
		case 2:
			membuf_read(&decomp_buf, &table->clusters[cluster_it], 4);

			string_size = item.size - 8;
			strings_it = table->strings_length;
			table->strings_length += string_size;
			table->strings = realloc(table->strings, table->strings_length);

			table->clusters[cluster_it].string_index = strings_it;
			cluster_it++;

			membuf_read(&decomp_buf, table->strings + strings_it, string_size);
			break;
		case 3:
			membuf_read(&decomp_buf, &table->vfiles[vfile_it], 4);

			string_size = item.size - 8;
			strings_it = table->strings_length;
			table->strings_length += string_size;
			table->strings = realloc(table->strings, table->strings_length);

			table->vfiles[vfile_it].string_index = strings_it;
			vfile_it++;

			membuf_read(&decomp_buf, table->strings + strings_it, string_size);
			break;
		}
	}

	membuf_free(&decomp_buf);
	return 0;
}

int arc_table_get_cluster_index(struct arc_table *table, const char *cluster)
{
	assert(table != NULL);
	assert(cluster != NULL);
	int i;
	for (i = 0; i < table->cluster_count; i++)
		if (strcmp(table->strings + table->clusters[i].string_index, cluster) == 0)
			break;
	return i >= table->cluster_count ? -1 : i; 
}