#include "filesystem.h"
#include <iostream>

//std::string initiation = ''

FileSystem::FileSystem() {

  this->mMemblockDevice = MemBlockDevice();
  std::string temp = (this->mMemblockDevice.readBlock(0)).toString();
  temp[0] = 0; //F/D flagga
  temp[1] = 2; //nrOf
  temp[2] = 3; // read/write
  temp[3] = '/';
  temp[4] = '\0';
  temp[11] = 0; // .  = link to currentDir
  temp[12] = 0; // .. = link to parentDir

//  temp[13] = 1;

  //temp[5] = 'l';
  //temp[6] = 'n';
  //temp[7] = 'a';
  //temp[8] = 'm';
  //temp[9] = 'n';
  //temp[10] = 'x';

  this->mMemblockDevice.writeBlock(0, temp);
  bitmap[0] = true;
  this->currentDir = this->mMemblockDevice.readBlock(0);

}





FileSystem::~FileSystem() {

}

int FileSystem::createFile(std::string fileName, int privilege){
  int returnValue;
  if(!currentDir_is_full()){
    returnValue = -3; //-3 = inappropriate filename length, 1 = successfully written to hdd
    int blockIndex = -1;
    for (int i = 1; i < 250; i++) {
      if (!bitmap[i]) {
        blockIndex = i;
        break;
      }
    }
    std::string temp = this->mMemblockDevice.readBlock(blockIndex).toString();
    if(fileName.length()<9){
      temp[0] = 1;
      temp[1] = 0;
      temp[2] = privilege;
      for (int i = 3; i < int(fileName.length()) + 3; i++) {
        temp[i] = fileName[i-3];
      }
      returnValue = this->mMemblockDevice.writeBlock(blockIndex, temp);
      if (returnValue == 1) {
        std::string tempdir = this->currentDir.toString();
        tempdir[1]++; //nrOf
        tempdir[10+int(tempdir[1])] = blockIndex;
        bitmap[blockIndex] = true;
        this->mMemblockDevice.writeBlock(int(tempdir[11]), tempdir);
        this->currentDir = this->mMemblockDevice.readBlock(int(tempdir[11]));
      }
    }
  }
  else{
    returnValue = -4;
  }
  return returnValue;
}

void FileSystem::listDir(){
  std::cout << "Type\tName\tPermissions\tSize" << std::endl;
  std::string temp = currentDir.toString();
  for (int i = 13; i < int(temp[1]) + 11; i++) {
    std::string temp_header = this->mMemblockDevice.readBlock(temp[i]).toString();
    std::string output = "";
    if(temp_header[0] == 1){
      output += "FILE\t";
    }
    else{
      output += "DIR\t";
    }
    for (size_t j = 3; j < 11; j++) {
      if(temp_header[j] != '\0'){
        output += temp_header[j];
      }
    }
    output +="\t";
    if(temp_header[2] == 3){
      output += "rw";
    }
    else if (temp_header[2] == 2) {
      output += 'w';
    }
    else if(temp_header[2] == 1){
      output += 'r';
    }
    output +="\t\t";
    std::cout << output << int(temp_header[1]) << "byte" << std::endl;
  }
}

int FileSystem::createFolder(std::string name, int privilege){
  int returnValue;
  if(!currentDir_is_full()){
    returnValue = -3; //-3 = inappropriate filename length, 1 = successfully written to hdd
    int blockIndex = -1;
    for (int i = 1; i < 250; i++) {
      if (!bitmap[i]) {
        blockIndex = i;
        break;
      }
    }
    std::string temp = this->mMemblockDevice.readBlock(blockIndex).toString();
    if(name.length()<9){
      temp[0] = 0;
      temp[1] = 0;
      temp[2] = privilege;
      temp[11] = blockIndex;
      temp[12] = this->currentDir.toString()[11];
      for (int i = 3; i < int(name.length()) + 3; i++) {
        temp[i] = name[i-3];
      }
      returnValue = this->mMemblockDevice.writeBlock(blockIndex, temp);
      if (returnValue == 1) {
        std::string tempdir = this->currentDir.toString();
        tempdir[1]++; //nrOf
        tempdir[10+int(tempdir[1])] = blockIndex;
        bitmap[blockIndex] = true;
        this->mMemblockDevice.writeBlock(int(tempdir[11]), tempdir);
        this->currentDir = this->mMemblockDevice.readBlock(int(tempdir[11]));
      }
    }
  }
  else{
    returnValue = -4;
  }
  return returnValue;
}
/* Please insert your code
char* Filesystem::readHeader(Block* block){
  char* temp_header = new char[10];
  for(int i = 0; i < 10; i++){

  }
}
*/
