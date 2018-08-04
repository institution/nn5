#include "source-reader.hpp"


void SourceReader::open(Path const& path)
{
	this->path = path;
	fp = file_open(path, "rb");
	c = file_get(fp);

	line_pos = 0;
	line_num = 1;

	mark_atom_pos = 0;
	mark_line_pos = 0;
	mark_line_num = 0;
}

size_t SourceReader::tell() const
{
	assert(fp);
	return file_tell(fp);
}

char SourceReader::get() {
	assert(fp);
	auto t = c;
	if (t != EOF) {
		c = file_get(fp);
	}
	else {
		file_close(fp);
		fp = nullptr;
	}
	return t;
}

bool SourceReader::eof() const {
	return (fp == nullptr or c == EOF);
}

char SourceReader::peek() const {
	assert(fp);
	return c;
}

void SourceReader::close() {
	file_close(fp);
	fp = nullptr;
}

void SourceReader::mark() {
	assert(fp);
	mark_atom_pos = file_tell(fp) - 1;
	mark_line_pos = line_pos;
	mark_line_num = line_num;
}

void SourceReader::unread() {
	assert(fp);
	file_seek(fp, mark_atom_pos);
	c = file_get(fp);
	line_pos = mark_line_pos;
	line_num = mark_line_num;
}
