#include "filesys.hpp"

namespace ext{
	FILE * file_open(char const* path, char const* mode);
	FILE * file_open(Path const& path, char const* mode);
	size_t file_tell(FILE * fp);
	void file_seek(FILE * fp, size_t offset);
	void file_seek_rel(FILE * fp, size_t offset);
	void file_close(FILE * fp);
	char file_get(FILE * fp);
}
