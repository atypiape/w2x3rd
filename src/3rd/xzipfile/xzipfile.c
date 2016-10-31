/* zipfread.c
   Version 0.1, 2016-10-26
*/

#include "xzipfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
//#include <string.h>
//#include <errno.h>

#include "../zlib/minizip/unzip.h"

#ifdef _WIN32
#  define USEWIN32IOAPI
#  include "../zlib/minizip/iowin32.h"
#endif

#define WRITEBUFFERSIZE (8192)
#define MAXFILENAME     (256)


typedef struct {
	unsigned int size;
	void *buf;
} DefaultUserData;


static int do_concat_buffer(
	void **_dst, 
	unsigned int* _dst_size,
	void *_src, 
	unsigned int _src_size,
	unsigned int _total_size)
{
	int ext_size = 0;

	assert(_dst);
	if (NULL == _dst) {
		perror("[XZipFile] pointer to hold file buffer is null\n");
		return kXZipFileParamError;
	}

	assert(_dst_size);
	if (NULL == _dst) {
		perror("[XZipFile] pointer to file buffer size is null\n");
		return kXZipFileParamError;
	}

	assert(_src_size <= _total_size);
	if (_src_size > _total_size) {
		_src_size = _total_size;
	}
	if (*_dst_size + _src_size >= _total_size) {
		ext_size = 1;
	}

	if (NULL == *_dst) {
		*_dst = malloc(_src_size + ext_size);
		*_dst_size = 0;
	}
	else {
		*_dst = realloc(*_dst, *_dst_size + _src_size + ext_size);
	}

	if (NULL == *_dst) {
		*_dst_size = 0;
		perror("[XZipFile] error allocating %d bytes memory\n");
		return kXZipFileInternalError;
	}
	memcpy((char *)*_dst + *_dst_size, _src, _src_size);
	*_dst_size += _src_size;

	if (0 < ext_size) {
		((char *)*_dst)[*_dst_size] = 0;
	}

	return kXZipFileOK;
}


static int default_callback(
	void *_buf,
	unsigned int _size_buf,
	unsigned int _size_file,
	void *_user_data)
{
	DefaultUserData *user_data = NULL;

	if (0 == _size_file) {
		return kXZipFileOK;
	}

	assert(_user_data);
	if (NULL == _user_data) {
		return kXZipFileParamError;
	}
	user_data = (DefaultUserData *)_user_data;

	return do_concat_buffer(
		&(user_data->buf), &(user_data->size), _buf, _size_buf, _size_file);
}


static int do_read_file_by_callback(
	XZipFile _zip,
	const char *_password,
	XZipFileCallback _callback,
	void *_user_data)
{
	int ret = 0;
	int err_close = UNZ_OK;
	void* buf = NULL;
	uInt size_buf = WRITEBUFFERSIZE;
	char file_name[256] = {0};
	unz_file_info64 file_info = {0};

	assert(_callback);
	if (NULL == _callback) {
		perror("[XZipFile] pointer to callback function is null\n");
		return kXZipFileParamError;
	}

	ret = unzGetCurrentFileInfo64((unzFile)_zip, &file_info, 
		file_name, sizeof(file_name), NULL, 0, NULL, 0);
	if (UNZ_OK != ret) {
		fprintf(stderr, "[XZipFile] get file info in zip error %d\n", ret);
		return kXZipFileInternalError;
	}

	if (0 == file_info.uncompressed_size) {
		ret = _callback(NULL, 0, 0, _user_data);
		return ret;
	}

	buf = (void*)malloc(size_buf);
	if (NULL == buf) {
		perror("[XZipFile] error allocating memory for read temp buffer\n");
		return kXZipFileInternalError;
	}

	if (NULL != _password && 0 != _password[0]) {
		ret = unzOpenCurrentFilePassword((unzFile)_zip, _password);
		if (UNZ_OK != ret) {
			fprintf(stderr, "[XZipFile] open file in zip with password error %d\n", ret);
		}
	}

	do {
		ret = unzReadCurrentFile((unzFile)_zip, buf, size_buf);
		if (0 > ret) {
			fprintf(stderr, "[XZipFile] read file in zip error %d\n", ret);
			break;
		}
		if (0 == ret) {
			break;
		}
		ret = _callback(buf, ret, (unsigned int)(file_info.uncompressed_size), _user_data);
	} while (kXZipFileOK == ret);

	err_close = unzCloseCurrentFile((unzFile)_zip);
	if (UNZ_OK != err_close) {
		fprintf(stderr, "[XZipFile] close read file error %d\n", err_close);
	}

	free(buf);
	return ret;
}


int xzipfile_open(XZipFile *_zip, const char *_file_name)
{
	unzFile unzfile = NULL;
#ifdef USEWIN32IOAPI
	zlib_filefunc64_def ffunc = {0};
#endif

	assert(_zip);
	if (NULL == _zip) {
		perror("[XZipFile] the pointer is null that used to hold zip file pointer\n");
		return kXZipFileParamError;
	}
	*_zip = NULL;

	if (NULL == _file_name) {
		perror("[XZipFile] zip file name to open is null\n");
		return kXZipFileParamError;
	}

#ifdef USEWIN32IOAPI
	fill_win32_filefunc64A(&ffunc);
	unzfile = unzOpen2_64(_file_name, &ffunc);
#else
	unzfile = unzOpen64(_file_name);
#endif

	if (NULL == unzfile) {
		fprintf(stderr, "[XZipFile] cannot open zip file %s\n", _file_name);
		return kXZipFileCanNotOpen;
	}
	*_zip = (XZipFile)unzfile;

	return kXZipFileOK;
}


void xzipfile_close(XZipFile _zip)
{
	if (NULL != _zip) {
		unzClose((unzFile)_zip);
	}
}


int xzipfile_read_by_callback(
	XZipFile _zip,
	const char *_file_name,
	const char *_password,
	XZipFileCallback _callback,
	void *_user_data
	)
{
	assert(_zip);
	if (NULL == _zip) {
		perror("[XZipFile] zip file pointer is null\n");
		return kXZipFileParamError;
	}

	assert(_file_name && 0 != _file_name[0]);
	if (NULL == _file_name || 0 == _file_name[0]) {
		perror("[XZipFile] file name in zip is null\n");
		return kXZipFileParamError;
	}

	if (UNZ_OK != unzLocateFile((unzFile)_zip, _file_name, NULL)) {
		printf("[XZipFile] file %s not found in the zip\n", _file_name);
		return kXZipFileNotFound;
	}

	return do_read_file_by_callback((unzFile)_zip, _password, _callback, _user_data);
}


int xzipfile_read(
	XZipFile _zip,
	const char *_file_name,
	const char *_password,
	void **_data,
	unsigned int *_size)
{
	int ret = kXZipFileOK;
	DefaultUserData *user_data = NULL;

	if (NULL != _data) {
		*_data = NULL;
	}
	if (NULL != _size) {
		*_size = 0;
	}

	assert(_data);
	if (NULL == _data) {
		perror("[XZipFile] pointer to read file is null \n");
		return kXZipFileParamError;
	}
	
	assert(_size);
	if (NULL == _size) {
		perror("[XZipFile] pointer to file size is null\n");
		return kXZipFileParamError;
	}
	
	user_data = (DefaultUserData *)malloc(sizeof(DefaultUserData));
	if (NULL == user_data) {
		perror("[XZipFile] error allocating memory for default user data\n");
		return kXZipFileInternalError;
	}
	memset(user_data, 0x0, sizeof(DefaultUserData));

	ret = xzipfile_read_by_callback(_zip, _file_name, 
		_password, default_callback, user_data);
	if (kXZipFileOK == ret) {
		*_data = user_data->buf;
		*_size = user_data->size;
	}

	free(user_data);

	return ret;
}
