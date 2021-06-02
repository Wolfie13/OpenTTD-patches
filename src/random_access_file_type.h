/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

 /** @file random_access_file_type.h Class related to random access to files. */

#ifndef RANDOM_ACCESS_FILE_TYPE_H
#define RANDOM_ACCESS_FILE_TYPE_H

#include "fileio_type.h"
#include "core/endian_func.hpp"
#include <string>

/**
 * A file from which bytes, words and double words are read in (potentially) a random order.
 *
 * This is mostly intended to be used for things that can be read from GRFs when needed, so
 * the graphics but also the sounds. This also ties into the spritecache as it uses these
 * files to load the sprites from when needed.
 */
class RandomAccessFile {
	/** The number of bytes to allocate for the buffer. */
	static constexpr int BUFFER_SIZE = 4096;

	std::string filename;            ///< Full name of the file; relative path to subdir plus the extension of the file.
	std::string simplified_filename; ///< Simplified lowecase name of the file; only the name, no path or extension.

	FILE *file_handle;               ///< File handle of the open file.
	size_t pos;                      ///< Position in the file of the end of the read buffer.

	byte *buffer;                    ///< Current position within the local buffer.
	byte *buffer_end;                ///< Last valid byte of buffer.
	byte buffer_start[BUFFER_SIZE];  ///< Local buffer when read from file.

	byte ReadByteIntl();
	uint16 ReadWordIntl();
	uint32 ReadDwordIntl();

public:
	RandomAccessFile(const std::string &filename, Subdirectory subdir);
	RandomAccessFile(const RandomAccessFile&) = delete;
	void operator=(const RandomAccessFile&) = delete;

	virtual ~RandomAccessFile();

	const std::string &GetFilename() const;
	const std::string &GetSimplifiedFilename() const;

	size_t GetPos() const;
	void SeekTo(size_t pos, int mode);

	inline byte ReadByte()
	{
		if (likely(this->buffer != this->buffer_end)) return *this->buffer++;
		return this->ReadByteIntl();
	}

	inline uint16 ReadWord()
	{
		if (likely(this->buffer + 1 < this->buffer_end)) {
#if OTTD_ALIGNMENT == 0
			uint16 x = FROM_LE16(*((const unaligned_uint16*) this->buffer));
#else
			uint16 x = ((uint16)this->buffer[1] << 8) | this->buffer[0];
#endif
			this->buffer += 2;
			return x;
		}
		return this->ReadWordIntl();
	}

	inline uint32 ReadDword()
	{
		if (likely(this->buffer + 3 < this->buffer_end)) {
#if OTTD_ALIGNMENT == 0
			uint32 x = FROM_LE32(*((const unaligned_uint32*) this->buffer));
#else
			uint32 x = ((uint32)this->buffer[3] << 24) | ((uint32)this->buffer[2] << 16) | ((uint32)this->buffer[1] << 8) | this->buffer[0];
#endif
			this->buffer += 4;
			return x;
		}
		return this->ReadDwordIntl();
	}

	void ReadBlock(void *ptr, size_t size);
	void SkipBytes(int n);
};

#endif /* RANDOM_ACCESS_FILE_TYPE_H */
