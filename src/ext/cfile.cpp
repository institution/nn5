#include "fail.hpp"
#include "cfile.hpp"

namespace ext{

	FILE * file_open(char const* path, char const* mode)
	{
		FILE * fp = fopen(path, mode);
		if (fp == nullptr) {
			ext::fail("ERROR: cannot open file: {}\n", path);
		}
		return fp;
	}

	FILE * file_open(Path const& path, char const* mode) {
		return file_open(path.c_str(), mode);
	}

	size_t file_tell(FILE * fp)
	{
		auto c = ftell(fp);
		if (c == -1L) {
			ext::fail("ERROR: ftell\n");
		}
		return c;
	}

	void file_seek(FILE * fp, size_t offset)
	{
		if (fseek(fp, offset, SEEK_SET)) {
			ext::fail("ERROR: fseek\n");
		}
	}

	void file_seek_rel(FILE * fp, size_t offset)
	{
		if (fseek(fp, offset, SEEK_CUR)) {
			ext::fail("ERROR: fseek\n");
		}
	}

	void file_close(FILE * fp)
	{
		if (fp != nullptr) {
			if (fclose(fp) == EOF) {
				ext::fail("ERROR: while closing file\n");
			}
		}
	}

	char file_get(FILE * fp)
	{
		auto c = getc(fp);
		if (c == EOF)
		{
			if (ferror(fp)) {
				ext::fail("ERROR: while reading file\n");
			}
			if (not feof(fp)) {
				ext::fail("ERROR: EOF reached but eof bit not set\n");
			}
		}

		return c;
	}

}
