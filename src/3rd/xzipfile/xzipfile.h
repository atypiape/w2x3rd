/******************************************************************************
fle:     xzipfile.h
desc:    Read file from ZIP package into memory, or process file data by callback.
authod:  Atypiape
e-mail:  atypiape@163.com
created: 2016-10-26
*******************************************************************************/

#ifndef __W2X_XZIP_FILE_H__
#define __W2X_XZIP_FILE_H__

#ifdef __cplusplus
extern "C" {
#endif


enum {
	kXZipFileOK		       = 0,
	kXZipFileParamError    = 1,
	kXZipFileCanNotOpen    = 2,
	kXZipFileNotFound	   = 3,
	kXZipFileInternalError = 4,
};


typedef void *XZipFile;

/**
 * The callback function type for process read file in zip.
 *
 * @param _buf       - pointer to pair of file is read.
 * @param _size_buf  - size of pair of file is read, in bytes.
 * @param _size_file - total size of file, in bytes.
 * @param _user_data - user data to pass to callback function.

 *
 * @return If successful, return kXZipFileOK.
 */
typedef int (*XZipFileCallback)(
	void *_buf,
	unsigned int _size_buf,
	unsigned int _size_file,
	void *_user_data
	);

/**
 * Opens a zip file by file name and password.
 * 
 * @param _zip       - use to hold the zip file pointer.
 * @param _file_name - the zip file name used to locate it.
 *
 * @return If successful, the return value is equal kXZipFileOK and
 *         returns a pointer to the opened zip file by _zip paramater.
 */
int xzipfile_open(XZipFile *_zip, const char *_file_name);

/**
 * Close the opened zip file.
 *
 * @param _zip - the pointer to a opened zip file.
 */
void xzipfile_close(XZipFile _zip);

/**
 * Read file from ZIP package into memory buffer, and add a null at end of buffer.
 *
 * @param _zip       - pointer to a opened zip file.
 * @param _file_name - the file name for read in zip.
 * @param _password  - the encryption password, can be null, if no encryption.
 * @param _data      - the pointer to read file is stored.
 * @param _size      - size of read file in bytes, not include the last null.
 *
 * @return If successful, the return value is equal kXZipFileOK.
 */
int xzipfile_read(
	XZipFile _zip,
	const char *_file_name,
	const char *_password,
	void **_data,
	unsigned int *_size
	);

/**
 * Read file from ZIP package and process it by callback.
 *
 * @param _zip       - pointer to a opened zip file.
 * @param _file_name - the file name for read in zip.
 * @param _password  - the encryption password, can be null, if no encryption.
 * @param _callback  - the callback function to process file in zip.
 * @param _user_data - user data to pass to the callback function.
 *
 * @return If successful, the return value is equal kXZipFileOK.
 */
int xzipfile_read_by_callback(
	XZipFile _zip,
	const char *_file_name,
	const char *_password,
	XZipFileCallback _callback,
	void *_user_data
	);

/** 
 * Deallocates the space previously allocated by xzipfile_read().
 * If _ptr is a null pointer, the function does nothing.
 *
 * @param _ptr - pointer to the memory to deallocate.
 */
void xzipfile_free(void *_ptr);


#ifdef __cplusplus
} // extern "C" {
#endif

#endif /* __W2X_XZIP_FILE_H__ */