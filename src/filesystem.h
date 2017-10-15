#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "memblockdevice.h"

class FileSystem
{
private:
    MemBlockDevice mMemblockDevice;
    Block currentDir;
    bool bitmap[250] = {false};
    int sizemap[250] = {0};
    // Here you can add your own data structures
    std::string getFileName(int blockIndex);
    std::string getHeader(int blockIndex);
    int getIndex(std::string name);
    bool currentDir_is_full(){
      return (sizemap[int(this->currentDir.toString()[1])] > 499);
    }
    int fileOrDir(int blockIndex);
    bool contains_slash(std::string name);

public:
    FileSystem();
    ~FileSystem();

    /* These API functions need to be implemented
	   You are free to specify parameter lists and return values
    */
    bool createImage(std::string filepath);
    bool restoreImage(std::string filepath);
    /* This function creates a file in the filesystem */
    int createFile(std::string fileName, int privilege = 3);


    /* Creates a folder in the filesystem */
    int createFolder(std::string name, int privilege = 3);

    int write(std::string fileName, std::string data, int privilege = 3);

    int writeContinue(std::string fileOne, std::string fileTwo, int privilege = 3);

    std::string read(std::string fileName, int privilege = 3);

    int move(std::string source, std::string destination, int privilege = 3);

    int copy(std::string source, std::string destination, int privilege = 3);

    int remove(std::string name, int privilege = 3);
    /* Removes a file in the filesystem */
    int removeFile(int directoryIndex_ofFile);

    /* Removes a folder in the filesystem */
    int removeFolder(int directoryIndex_ofDir);


    std::string getCurrentPath();

    /* This function will get all the files and folders in the specified folder */
    std::string listDir(std::string path = "");
    /* Function will move the current location to a specified location in the filesystem */
    bool changeDir(std::string path);
    /* Add your own member-functions if needed */
};

#endif // FILESYSTEM_H
