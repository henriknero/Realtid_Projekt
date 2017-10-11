#include "filesystem.h"
#include <iostream>

//std::string initiation = ''

std::string FileSystem::getFileName(int blockIndex){
  std::string temp = this->mMemblockDevice.readBlock(blockIndex).toString();
  std::string output = "";
  for (size_t i = 3; i < 11; i++) {
    if(temp[i] != '\0'){
      output += temp[i];
    }
  }
  return output;
}

int FileSystem::getIndex(std::string name){
  int returnValue = -1;
  std::string temp = this->currentDir.toString();
  for(int i = 13; i < temp[1]+11; i++){
    if(this->getFileName(temp[i]) == name){
      returnValue = i;
      break;
    }
  }
  return returnValue;
}
std::string FileSystem::getHeader(int blockIndex){
  std::string temp = this->mMemblockDevice.readBlock(blockIndex).toString();
  std::string output = "";
  for (size_t i = 0; i < 11; i++) {
    output += temp[i];
  }
  return output;
}


std::string FileSystem::getHeader(int blockIndex){
  std::string temp = this->mMemblockDevice.readBlock(blockIndex).toString();
  std::string output = "";
  for (size_t i = 0; i < 11; i++) {
    output += temp[i];
  }
  return output;
}


FileSystem::FileSystem() {

  this->mMemblockDevice = MemBlockDevice();
  std::string temp = (this->mMemblockDevice.readBlock(0)).toString();
  temp[0] = 0; //F/D flagga
  temp[1] = 2; //nrOf
  temp[2] = 3; // read/write
  temp[3] = '/'; //element 3-10 utgör filnamn (max 8 tecken)
  temp[4] = '\0'; //nullterminator
  temp[11] = 0; // .  = link to currentDir
  temp[12] = 0; // .. = link to parentDir

  this->mMemblockDevice.writeBlock(0, temp);
  bitmap[0] = true;
  this->currentDir = this->mMemblockDevice.readBlock(0);

}

FileSystem::~FileSystem() {

}

int FileSystem::createFile(std::string fileName, int privilege){
  int returnValue;
  if ((!currentDir_is_full()) && (getIndex(fileName) == -1)) {
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
      temp[11] = 0;
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

int FileSystem::createFolder(std::string name, int privilege){
  int returnValue;
  if((!currentDir_is_full()) && (getIndex(name) == -1)){
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


//inte klar, fuckar ur
int FileSystem::removeFile(std::string fileName){
  std::string temp = this->currentDir.toString();
  int directoryIndex_ofFile = getIndex(fileName);
  if (directoryIndex_ofFile != -1){
    //temp[1] är mappens nrOf
    int index_of_last_element = 13 + (temp[1]-2);
    temp[directoryIndex_ofFile] = index_of_last_element;
    temp[-1]--;
    //
  }
  return directoryIndex_ofFile;
}

void FileSystem::printCurrentPath(){
  //int start;
  int blockIndex;
  std::string output = "";
  std::string temp_dir_name = "";
  std::string temp_header = this->currentDir.toString();
  blockIndex = int(temp_header[11]);
  //start = blockIndex;

  while(temp_header[11] != 0){
    temp_dir_name = this->getFileName(blockIndex);
    output = temp_dir_name + "/" + output;
    blockIndex = temp_header[12];
    temp_header = this->mMemblockDevice.readBlock(blockIndex).toString();
  }
  output = "/" + output;
  std::cout << output << std::endl;
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


    /*
    for (size_t j = 3; j < 11; j++) {
      if(temp_header[j] != '\0'){
        output += temp_header[j];
      }
    }
    */
    output += this->getFileName(temp[i]);
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
      temp[1] = 2;
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

int FileSystem::changeDir(std::string path){
  int found = 0;
  std::string subDir = path;
  Block tempdir = currentDir;
  std::string dirToFind = "";
  while (subDir != ""){
    if(subDir.find("/") != std::string::npos){
      dirToFind = subDir.substr(0,subDir.find("/"));
      int nrOfEntries = int(tempdir[1]);
      for (int i = 11; i < nrOfEntries + 11; i++) {
        std::string header = this->getHeader(tempdir[i]);
        if (this->getFileName(tempdir[i]) == dirToFind and int(header[0]) == 0) {
          std::cout << this->getFileName(int(tempdir[i])) << std::endl;
          tempdir = this->mMemblockDevice.readBlock(int(tempdir[i]));
        }
      }
      subDir = subDir.substr(subDir.find("/")+1);
    //found = changeDir(path.substr(path.find("/")+1));
    }
  }

  //path.substr(path.find("/"));

  return 0;
}
/* Please insert your code
char* Filesystem::readHeader(Block* block){
  char* temp_header = new char[10];
  for(int i = 0; i < 10; i++){

  }
}
*/
