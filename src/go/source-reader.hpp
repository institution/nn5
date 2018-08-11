#pragma once

#include "ext/ext.hpp"

using namespace ext;

struct SourceReader
{
	Path path;
	FILE * fp{nullptr};
	char c{EOF};             // next char

	size_t mark_atom_pos;
	size_t mark_line_pos;
	size_t mark_line_num;

	size_t line_pos;
	size_t line_num;

	void open(Path const& path) ;

	size_t tell() const;

	char get();

	bool eof() const;
	char peek() const;

	void close();

	bool is_open() { return fp != nullptr; }

	void mark();

	void unread();

	~SourceReader() { close(); }

};
