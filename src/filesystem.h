#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "memblockdevice.h"

class FileSystem
{
private:
    MemBlockDevice mMemblockDevice;
    Block currentDir;
    bool bitmap[250] = {false};
    // Here you can add your own data structures
    std::string getFileName(int blockIndex);
    bool currentDir_is_full(){
      return (this->currentDir.toString()[1] > 499);
    }
public:
    FileSystem();
    ~FileSystem();


    /* These API functions need to be implemented
	   You are free to specify parameter lists and return values
    */

    /* This function creates a file in the filesystem */
    int createFile(std::string fileName, int privilege = 3);

    int write(std::string);

    /* Creates a folder in the filesystem */
    // createFolderi(...);

    /* Removes a file in the filesystem */
    // removeFile(...);

    /* Removes a folder in the filesystem */
    // removeFolder(...);

    /* Function will move the current location to a specified location in the filesystem */
    // goToFolder(...);

    void printCurrentPath();

    /* This function will get all the files and folders in the specified folder */
    void listDir();

    /* Add your own member-functions if needed */
};

#endif // FILESYSTEM_H
