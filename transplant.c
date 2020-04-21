#include "const.h"
#include "transplant.h"
#include "debug.h"
#include "helper.h"
#include <errno.h>

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

/*
 * You may modify this file and/or move the functions contained here
 * to other source files (except for main.c) as you wish.
 *
 * IMPORTANT: You MAY NOT use any array brackets (i.e. [ and ]) and
 * you MAY NOT declare any arrays or allocate any storage with malloc().
 * The purpose of this restriction is to force you to use pointers.
 * Variables to hold the pathname of the current file or directory
 * as well as other data have been pre-declared for you in const.h.
 * You must use those variables, rather than declaring your own.
 * IF YOU VIOLATE THIS RESTRICTION, YOU WILL GET A ZERO!
 *
 * IMPORTANT: You MAY NOT use floating point arithmetic or declare
 * any "float" or "double" variables.  IF YOU VIOLATE THIS RESTRICTION,
 * YOU WILL GET A ZERO!
 */

/*
 * A function that returns printable names for the record types, for use in
 * generating debugging printout.
 */
static char *record_type_name(int i) {
    switch(i) {
    case START_OF_TRANSMISSION:
    return "START_OF_TRANSMISSION";
    case END_OF_TRANSMISSION:
    return "END_OF_TRANSMISSION";
    case START_OF_DIRECTORY:
    return "START_OF_DIRECTORY";
    case END_OF_DIRECTORY:
    return "END_OF_DIRECTORY";
    case DIRECTORY_ENTRY:
    return "DIRECTORY_ENTRY";
    case FILE_DATA:
    return "FILE_DATA";
    default:
    return "UNKNOWN";
    }
}

/*
 * @brief  Initialize path_buf to a specified base path.
 * @details  This function copies its null-terminated argument string into
 * path_buf, including its terminating null byte.
 * The function fails if the argument string, including the terminating
 * null byte, is longer than the size of path_buf.  The path_length variable
 * is set to the length of the string in path_buf, not including the terminating
 * null byte.
 *
 * @param  Pathname to be copied into path_buf.
 * @return 0 on success, -1 in case of error
 */
int path_init(char *name) {
    // To be implemented
    int i = 0;
    int length = sizeof(path_buf)/sizeof(char);
    path_length = -1;
    while((*(name+i)) != '\0' && (i<length)){
        path_length++;
        *(path_buf+path_length) = *(name+i);
        i++;
    }

    // if(*(path_buf+path_length) != '/'){
    //     path_length++;
    //     *(path_buf+path_length) = '/';
    // }

    if(i>=(length)){
        return -1;
    }
    return 0;
}

/*
 * @brief  Append an additional component to the end of the pathname in path_buf.
 * @details  This function assumes that path_buf has been initialized to a valid
 * string.  It appends to the existing string the path separator character '/',
 * followed by the string given as argument, including its terminating null byte.
 * The length of the new string, including the terminating null byte, must be
 * no more than the size of path_buf.  The variable path_length is updated to
 * remain consistent with the length of the string in path_buf.
 *
 * @param  The string to be appended to the path in path_buf.  The string must
 * not contain any occurrences of the path separator character '/'.
 * @return 0 in case of success, -1 otherwise.
 */
int path_push(char *name) {
    // To be implemented.
    int i = 0;
    if(*(name) == '\0'){
        return 0;
    }

    //Appending '/' if current path_buf doesn't have it as it's last character
    if(*(path_buf+path_length) != '/'){
        path_length++;
        *(path_buf+path_length) = '/';
    }

    int length = sizeof(path_buf)/sizeof(char);
    while(*(name+i) != '\0'  && i<length){
        path_length++;
        if(*(name+i) == '/'){
            return -1;
        }
        *(path_buf+path_length) = *(name+i);
        i++;
    }

    if(i>=(length)){
        return -1;
    }
    return 0;
}

/*
 * @brief  Remove the last component from the end of the pathname.
 * @details  This function assumes that path_buf contains a non-empty string.
 * It removes the suffix of this string that starts at the last occurrence
 * of the path separator character '/'.  If there is no such occurrence,
 * then the entire string is removed, leaving an empty string in path_buf.
 * The variable path_length is updated to remain consistent with the length
 * of the string in path_buf.  The function fails if path_buf is originally
 * empty, so that there is no path component to be removed.
 *
 * @return 0 in case of success, -1 otherwise.
 */
int path_pop() {
    // To be implemented.
    if(!path_length){
        return -1;
    }
    while(*(path_buf+path_length) != '/' && path_length>=0){
        *(path_buf+path_length) = '\0';
        path_length--;
    }

    if(*(path_buf+path_length) == '/'){
        *(path_buf+path_length) = '\0';
        path_length--;
    }
    return 0;
}

/*
 * @brief Deserialize directory contents into an existing directory.
 * @details  This function assumes that path_buf contains the name of an existing
 * directory.  It reads (from the standard input) a sequence of DIRECTORY_ENTRY
 * records bracketed by a START_OF_DIRECTORY and END_OF_DIRECTORY record at the
 * same depth and it recreates the entries, leaving the deserialized files and
 * directories within the directory named by path_buf.
 *
 * @param depth  The value of the depth field that is expected to be found in
 * each of the records processed.
 * @return 0 in case of success, -1 in case of an error.  A variety of errors
 * can occur, including depth fields in the records read that do not match the
 * expected value, the records to be processed to not being with START_OF_DIRECTORY
 * or end with END_OF_DIRECTORY, or an I/O error occurs either while reading
 * the records from the standard input or in creating deserialized files and
 * directories.
 */
int deserialize_directory(int depth) {
    // To be implemented.
    uint32_t record_depth = 0;
    uint64_t size = 0;
    uint32_t file_mode = 0;
    int name_length = 0;
    char record_type = 0;
    int exit = -1;

    if(mkdir(path_buf,0700) && errno!= EEXIST){
        return -1;
    }

    if(read_magic_bytes()){
        return -1;
    }

    record_type = getchar();
    record_depth = get_record_depth();
    size = get_record_size();
    struct stat stat_buf;

    //Checking for START_OF_DIRECTORY record for deserializing a directory
    if(record_type != START_OF_DIRECTORY || record_depth != depth){
        return -1;
    }

    //Magic bytes check for DIRECTORY_ENTRY record
    if(read_magic_bytes()){
        return -1;
    }

    record_type = getchar();
    while(record_type != END_OF_DIRECTORY && record_type == DIRECTORY_ENTRY){
        record_depth = get_record_depth();
        size = get_record_size();
        file_mode = get_record_depth();
        uint64_t file_size = get_record_size();
        name_length = size-12-HEADER_SIZE;
        set_name_buf(name_length);
        path_push(name_buf);

        //If clobber is not set and file exists returning -1
        if(!(global_options&0x08) && !stat(path_buf,&stat_buf)){
            return -1;
        }

        //Deserializing based on a file record and directory record
        if(S_ISREG(file_mode)){
            if(deserialize_file(depth)){
                return -1;
            }
            chmod(path_buf,file_mode & 0777);
            path_pop();
        }else if(S_ISDIR(file_mode)){
            if(deserialize_directory(depth+1)){
                return -1;
            }
            chmod(path_buf,file_mode & 0777);
            path_pop();
        }

        if(read_magic_bytes()){
            return -1;
        }
        record_type = getchar();
    }

    //Checking for the END_OF_DIRECTORY record
    record_depth = get_record_depth();
    size = get_record_size();
    if(record_type != END_OF_DIRECTORY || record_depth != depth){
        return -1;
    }
    return 0;
}

/*
 * @brief Deserialize the contents of a single file.
 * @details  This function assumes that path_buf contains the name of a file
 * to be deserialized.  The file must not already exist, unless the ``clobber''
 * bit is set in the global_options variable.  It reads (from the standard input)
 * a single FILE_DATA record containing the file content and it recreates the file
 * from the content.
 *
 * @param depth  The value of the depth field that is expected to be found in
 * the FILE_DATA record.
 * @return 0 in case of success, -1 in case of an error.  A variety of errors
 * can occur, including a depth field in the FILE_DATA record that does not match
 * the expected value, the record read is not a FILE_DATA record, the file to
 * be created already exists, or an I/O error occurs either while reading
 * the FILE_DATA record from the standard input or while re-creating the
 * deserialized file.
 */
int deserialize_file(int depth){
    //Reading magic bytes from stdin for FILE_DATA
    if(read_magic_bytes()){
        return -1;
    }

    uint64_t i = 0;
    char record_type = getchar();
    uint32_t record_depth = get_record_depth();
    uint64_t size = get_record_size()-16;

    //Checking whether record is of type FILE_DATA and record depth matches with expected depth
    if(record_type == FILE_DATA && depth == record_depth){
        FILE *f = fopen(path_buf,"w");
        if(f==NULL && errno){
            return -1;
        }
        while(i<size){
            fputc(getchar(),f);
            i++;
        }
        fclose(f);
        return 0;
    }else
        return -1;
}

/*
 * @brief  Serialize the contents of a directory as a sequence of records written
 * to the standard output.
 * @details  This function assumes that path_buf contains the name of an existing
 * directory to be serialized.  It serializes the contents of that directory as a
 * sequence of records that begins with a START_OF_DIRECTORY record, ends with an
 * END_OF_DIRECTORY record, and with the intervening records all of type DIRECTORY_ENTRY.
 *
 * @param depth  The value of the depth field that is expected to occur in the
 * START_OF_DIRECTORY, DIRECTORY_ENTRY, and END_OF_DIRECTORY records processed.
 * Note that this depth pertains only to the "top-level" records in the sequence:
 * DIRECTORY_ENTRY records may be recursively followed by similar sequence of
 * records describing sub-directories at a greater depth.
 * @return 0 in case of success, -1 otherwise.  A variety of errors can occur,
 * including failure to open files, failure to traverse directories, and I/O errors
 * that occur while reading file content and writing to standard output.
 */
int serialize_directory(int depth) {
    // To be implemented.

    uint64_t size = 16;
    int exit = -1;

    //START_OF_DIRECTORY record for serializing a directory
    write_record_header(START_OF_DIRECTORY, depth, size);

    DIR *dir = opendir(path_buf);
    struct dirent *de;
    if(dir == NULL){
        return -1;
    }
    struct stat stat_buf;
    int i=0;
    char *name;
    while((de = readdir(dir)) != NULL && !errno){
        if(compare_strings(de->d_name,".") == -1 && compare_strings(de->d_name,"..") == -1){
            i = 0;
            path_push((de->d_name));
            name = de->d_name;
            stat(path_buf, &stat_buf);

            //Finding the length of the file name
            while(*(name+i) != '\0'){
                i++;
            }
            size = 16+12+i;
            i = 0;
            // writing DIRECTORY_ENTRY record
            write_record_header(4,depth, size);

            // writing the mode of the file in directory entry record
            putchar((stat_buf.st_mode >> 24) & 0xFF);
            putchar((stat_buf.st_mode >> 16) & 0xFF);
            putchar((stat_buf.st_mode >> 8) & 0xFF);
            putchar((stat_buf.st_mode) & 0xFF);

            //writing the size of the file in directory entry record
            putchar((stat_buf.st_size >> 56) & 0xFF);
            putchar((stat_buf.st_size >> 48) & 0xFF);
            putchar((stat_buf.st_size >> 40) & 0xFF);
            putchar((stat_buf.st_size >> 32) & 0xFF);
            putchar((stat_buf.st_size >> 24) & 0xFF);
            putchar((stat_buf.st_size >> 16) & 0xFF);
            putchar((stat_buf.st_size >> 8) & 0xFF);
            putchar((stat_buf.st_size) & 0xFF);

            //writing the name of the file in directory entry record
            while(*(name+i) != '\0'){
                putchar(*(name+i));
                i++;
            }
            if(S_ISREG(stat_buf.st_mode)){
                exit = serialize_file(depth,stat_buf.st_size);
            }else if(S_ISDIR(stat_buf.st_mode)){
                exit = serialize_directory(depth+1);
            }

            if(exit !=0){
                return -1;
            }
            path_pop();
        }
    }

    //writing END_OF_DIRECTORY record for serializing a directory
    write_record_header(END_OF_DIRECTORY, depth, HEADER_SIZE);
    return 0;
}

/*
 * @brief  Serialize the contents of a file as a single record written to the
 * standard output.
 * @details  This function assumes that path_buf contains the name of an existing
 * file to be serialized.  It serializes the contents of that file as a single
 * FILE_DATA record emitted to the standard output.
 *
 * @param depth  The value to be used in the depth field of the FILE_DATA record.
 * @param size  The number of bytes of data in the file to be serialized.
 * @return 0 in case of success, -1 otherwise.  A variety of errors can occur,
 * including failure to open the file, too many or not enough data bytes read
 * from the file, and I/O errors reading the file data or writing to standard output.
 */
int serialize_file(int depth, off_t size) {
    // To be implemented.
    FILE *f = fopen(path_buf, "r");
    if(f==NULL && errno){
        return -1;
    }
    off_t i = 0;

    uint64_t file_size = (uint64_t) size+16;

    write_record_header(FILE_DATA,depth,file_size);
    while(i<size){
        putchar(fgetc(f));
        i++;
    }

    fclose(f);
    return 0;
}

/**
 * @brief Serializes a tree of files and directories, writes
 * serialized data to standard output.
 * @details This function assumes path_buf has been initialized with the pathname
 * of a directory whose contents are to be serialized.  It traverses the tree of
 * files and directories contained in this directory (not including the directory
 * itself) and it emits on the standard output a sequence of bytes from which the
 * tree can be reconstructed.  Options that modify the behavior are obtained from
 * the global_options variable.
 *
 * @return 0 if serialization completes without error, -1 if an error occurs.
 */
int serialize() {
    // To be implemented.
    uint32_t depth = 0;
    uint64_t size = 16;
    int exit = -1;

    //writing the START_OF_TRANSMISSION record
    write_record_header(0,depth,size);

    if(serialize_directory(1)){
        return -1;
    }

    // Writing the END_OF_TRANSMISSION record
    write_record_header(1, depth, size);

    return 0;
}

/**
 * @brief Reads serialized data from the standard input and reconstructs from it
 * a tree of files and directories.
 * @details  This function assumes path_buf has been initialized with the pathname
 * of a directory into which a tree of files and directories is to be placed.
 * If the directory does not already exist, it is created.  The function then reads
 * from from the standard input a sequence of bytes that represent a serialized tree
 * of files and directories in the format written by serialize() and it reconstructs
 * the tree within the specified directory.  Options that modify the behavior are
 * obtained from the global_options variable.
 *
 * @return 0 if deserialization completes without error, -1 if an error occurs.
 */
int deserialize() {
    // To be implemented.
    uint32_t depth = 0;
    uint64_t size = 16;

    //reading the START_OF_TRANSMISSION record
    if(read_record_header()){
        return -1;
    }

    if(deserialize_directory(1)){
        return -1;
    }

    //reading the END_OF_TRANSMISSION record
    if(read_record_header()){
        return -1;
    }

    return 0;
}

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 0 if validation succeeds and -1 if validation fails.
 * Upon successful return, the selected program options will be set in the
 * global variable "global_options", where they will be accessible
 * elsewhere in the program.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 0 if validation succeeds and -1 if validation fails.
 * Refer to the homework document for the effects of this function on
 * global variables.
 * @modifies global variable "global_options" to contain a bitmap representing
 * the selected options.
 */
int validargs(int argc, char **argv)
{
    // To be implemented.
    if(argc == 1){
        return -1;
    }

    if(!compare_strings(*(argv+1),"-h")){
        global_options = 0x01;
        return 0;
    }

    if(!compare_strings(*(argv+1),"-s")){
        if(argc==2){
            global_options = 2;
            path_init(".");
            return 0;
        }else if(argc == 4 && !compare_strings(*(argv+2),"-p")){
            global_options = 2;
            path_init(*(argv+3));
            return 0;
        }
    }

    if(!compare_strings(*(argv+1),"-d")){
        if(argc==2){
            global_options = 4;
            path_init(".");
            return 0;
        }else if(argc == 3 && !compare_strings(*(argv+2),"-c")){
            global_options = 12;
            path_init(".");
            return 0;
        }else if(argc == 4 && !compare_strings(*(argv+2),"-p") && compare_strings(*(argv+3),"-c")){
            global_options = 4;
            path_init(*(argv+3));
            return 0;
        }else if(argc == 5 && !compare_strings(*(argv+2),"-p") && !compare_strings(*(argv+4),"-c")){
            global_options = 12;
            path_init(*(argv+3));
            return 0;
        }else if(argc == 5 && !compare_strings(*(argv+2),"-c") && !compare_strings(*(argv+3),"-p")){
            global_options = 12;
            path_init(*(argv+4));
            return 0;
        }
    }

    return -1;
}
