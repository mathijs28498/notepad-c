// TODO: Make this a system plugin

#pragma once
#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdint.h>

int32_t file_io_read(const char *path, char **buffer_out);

#endif // #ifndef FILE_IO_H