#include "const.h"

/**
* @brief Helper function to compare whether two strings are equal
* @details This function takes pointers of two strings and sequentially
* check the character till one of reach the null terminating byte. If both
* the pointers are at the null termination byte then they are equal.
* @return 0 if both strings are equal or -1 if they are not equal
*/
int compare_strings(char* a, char* b)
{
    while(*a == *b && !(*a == '\0' || *b == '\0')){
        a++;
        b++;
    }

    if(*a == '\0' && *b == '\0'){
        return 0;
    }

    return -1;
}

/**
* @brief Helper function to write the header for every record
* @details This function writes the 16 byte header of each record
* before writing the actual data. Inputs are type of record, depth,
* size of the record
* @return 0 after successfully writing the 16 bytes of data
*/
int write_record_header(char type, uint32_t depth, uint64_t size){
    putchar(MAGIC0);
    putchar(MAGIC1);
    putchar(MAGIC2);
    putchar(type);
    putchar((depth >> 24) & 0xFF);
    putchar((depth >> 16) & 0xFF);
    putchar((depth >> 8) & 0xFF);
    putchar((depth) & 0xFF);
    putchar((size >> 56) & 0xFF);
    putchar((size >> 48) & 0xFF);
    putchar((size >> 40) & 0xFF);
    putchar((size >> 32) & 0xFF);
    putchar((size >> 24) & 0xFF);
    putchar((size >> 16) & 0xFF);
    putchar((size >> 8) & 0xFF);
    putchar((size) & 0xFF);
    return 0;
}

/**
* @brief Helper function to read header records only
* @details This function reads the 16 byte record headers of the records
* @return 0 if it reads all bytes and -1 if magic bytes are not equal
*/
int read_record_header(){
    uint64_t size = 0;
    uint32_t depth = 0;
    if(getchar() == MAGIC0 && getchar() == MAGIC1 && getchar() == MAGIC2){
        getchar();
        for(int j = 0;j<4;j++){
            depth = depth * 256 + getchar();
        }
        for(int i = 0;i<8;i++){
            size = size * 256 + getchar();
        }
        return 0;
    }

    return -1;
}

/**
* @brief Helper function to read the magic bytes
* @details Read the magic bytes of each record before going into types
* @return 0 if each byte matches and -1 if any mismatch with magic byte
*/
int read_magic_bytes(){
    if(getchar() == MAGIC0 && getchar() == MAGIC1 && getchar() == MAGIC2){
        return 0;
    }

     return -1;
}

/**
* @brief Helper function to read the depth of the record
* @details This function reads the consecutive 4 bytes to
* get the depth of the file or directory
* @return depth of the record
*/
uint32_t get_record_depth(){
    uint32_t depth = 0;
    for(int j = 0;j<4;j++){
        depth = depth * 256 + getchar();
    }

    return depth;
}

/**
* @brief Helper function to read the size of the record
* @details This function reads the consecutive 8 bytes to
* get the size of the file or directory
* @return size of the record
*/
uint64_t get_record_size(){
    uint64_t size = 0;
    for(int i = 0;i<8;i++){
        size = size * 256 + getchar();
    }

    return size;
}


/**
* @brief Helper function to store the name of the file in
* the name_buf variable
* @details This function initally clears the name_buf by putting
* null terminating byte and then loads it with the name of the file
* @return 0 if the name_buf is loaded with file name and -1 if length
* of the file name is greater than name_buf length
*/
int set_name_buf(int name_length){
    int i = 0;
    while(*(name_buf+i) != '\0'){
        *(name_buf+i) = '\0';
        i++;
    }

    i = 0;
    if(name_length>NAME_MAX){
        return -1;
    }
    while(i<name_length){
        *(name_buf+i) = getchar();
        i++;
    }

    return 0;
}