#include "filesystem.h"
#include <iostream>


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
  for(int i = 13; i < sizemap[int(temp[1])]+11; i++){
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

int FileSystem::fileOrDir(int blockIndex){
  int flag = this->mMemblockDevice.readBlock(blockIndex).toString()[0];
  return flag;
}

bool FileSystem::contains_slash(std::string name){
  bool slash = false;
  for(int i = 0; i < int(name.length()); i++){
    if(name[i] == '/'){
      slash = true;
      break;
    }
  }
  return slash;
}

FileSystem::FileSystem() {

  this->mMemblockDevice = MemBlockDevice();
  std::string temp = (this->mMemblockDevice.readBlock(0)).toString();
  temp[0] = 0; //F/D flagga
  temp[1] = 0; //self blockIndex
  temp[2] = 3; // read/write
  temp[3] = '/'; //element 3-10 utgör filnamn (max 8 tecken)
  temp[4] = '\0'; //nullterminator
  temp[11] = 0; // .  = link to currentDir
  temp[12] = 0; // .. = link to parentDir

  this->mMemblockDevice.writeBlock(0, temp);
  bitmap[0] = true;
  sizemap[0] = 2;
  this->currentDir = this->mMemblockDevice.readBlock(0);

}

FileSystem::~FileSystem() {

}

int FileSystem::createFile(std::string fileName, int privilege){
  int returnValue = -3;
  std::string start = "";
  std::string path;

  bool name_is_path = contains_slash(fileName);
  if (name_is_path){
    start = this->getCurrentPath();
    path = fileName.substr(0, fileName.find_last_of('/'));
    fileName = fileName.substr(fileName.find_last_of('/')+1);
    this->changeDir(path);
  }
  if ((!currentDir_is_full()) && (getIndex(fileName) == -1) && (start != this->getCurrentPath())) {
    returnValue = -3;
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
      temp[1] = blockIndex;
      temp[2] = privilege;
      temp[11] = 0;
      for (int i = 3; i < int(fileName.length()) + 3; i++) {
        temp[i] = fileName[i-3];
      }
      if(fileName.length() < 8){
        temp[3+fileName.length()] = '\0';
      }
      returnValue = this->mMemblockDevice.writeBlock(blockIndex, temp);
      if (returnValue == 1) {
        std::string tempdir = this->currentDir.toString();
        sizemap[int(tempdir[1])]++; //nrOf
        tempdir[10+sizemap[int(tempdir[1])]] = blockIndex;
        bitmap[blockIndex] = true;
        this->mMemblockDevice.writeBlock(int(tempdir[11]), tempdir);
        this->currentDir = this->mMemblockDevice.readBlock(int(tempdir[11]));
      }
    }
  }
  else{
    returnValue = -4;
  }
  if (name_is_path){
    this->changeDir(start);
  }
  return returnValue;
}

int FileSystem::createFolder(std::string name, int privilege){
  int returnValue = -3;
  std::string start = "";
  std::string path;
  bool name_is_path = contains_slash(name);
  if (name_is_path){
    start = this->getCurrentPath();
    path = name.substr(0, name.find_last_of('/'));
    name = name.substr(name.find_last_of('/')+1);
    this->changeDir(path);
  }
  if((!currentDir_is_full()) && (getIndex(name) == -1) && (start != this->getCurrentPath())) {
    returnValue = -3;
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
      temp[1] = blockIndex;
      sizemap[blockIndex] = 2;
      temp[2] = privilege;
      temp[11] = blockIndex;
      temp[12] = this->currentDir[11];
      for (int i = 3; i < int(name.length()) + 3; i++) {
        temp[i] = name[i-3];
      }
      if(name.length() < 8){
        temp[3+name.length()] = '\0';
      }
      returnValue = this->mMemblockDevice.writeBlock(blockIndex, temp);

      if (returnValue == 1) {
        std::string tempdir = this->currentDir.toString();
        sizemap[int(tempdir[1])]++; //nrOf
        tempdir[10+sizemap[int(tempdir[1])]] = blockIndex;
        bitmap[blockIndex] = true;
        this->mMemblockDevice.writeBlock(int(tempdir[11]), tempdir);
        this->currentDir = this->mMemblockDevice.readBlock(int(tempdir[11]));
      }
    }
  }
  else{
    returnValue = -4;
  }
  if (name_is_path){
    this->changeDir(start);
  }
  return returnValue;
}

int FileSystem::write(std::string fileName, std::string data){
  int returnValue = -1;
  std::string start = "";
  std::string path;
  bool name_is_path = contains_slash(fileName);
  if (name_is_path){
    start = this->getCurrentPath();
    path = fileName.substr(0, fileName.find_last_of('/'));
    fileName = fileName.substr(fileName.find_last_of('/')+1);
    this->changeDir(path);
  }
  if (start != this->getCurrentPath()) {
    returnValue = 1;
    int blocks_required = (data.length() / 500) + 1 ;
    if(data.length() % 500 == 0){
      blocks_required--;
    }
    int directoryIndex_ofFile = getIndex(fileName);
    int blockIndex = this->currentDir[directoryIndex_ofFile];
    std::string temp = this->mMemblockDevice.readBlock(blockIndex).toString();

    if(blocks_required > 1){
      int availableBlocks = 0;
      for (int i = 1; i < 250; i++) {
        if (!bitmap[i]) {
          availableBlocks++;
        }
      }

      if(availableBlocks < blocks_required-1){
        blocks_required = availableBlocks+1;
      }

      //Lägger indexens hos filens fortsättningsblock i en array
      int following_blockIndexes[blocks_required-1] = {0};
      for (int i = 0; i < blocks_required-1; i++){
        int tempIndex = -1;
        for (int j = 1; j < 250; j++) {
          if (!bitmap[j]) {
            tempIndex = j;
            bitmap[tempIndex] = true;
            break;
          }
        }
        following_blockIndexes[i] = tempIndex;
      }
      //---------------------------------------------


      temp[11] = following_blockIndexes[0];

      std::string temp_followingBlock;
      for(int i = 0; i < blocks_required-2; i++){
        temp_followingBlock = this->mMemblockDevice.readBlock(following_blockIndexes[i]).toString();
        temp_followingBlock[11] = following_blockIndexes[i+1];
        this->mMemblockDevice.writeBlock(following_blockIndexes[i], temp_followingBlock);
      }

      for (int i = 12; i < 512; i++){
        temp[i] = data[i -12];
      }

      for (int i = 0; i < blocks_required-2; i++){
        temp_followingBlock = this->mMemblockDevice.readBlock(following_blockIndexes[i]).toString();
        for (int j = 12; j < 512; j++){
          temp_followingBlock[j] = data[(j-12)+(500*(i+1))];
        }
        this->mMemblockDevice.writeBlock(following_blockIndexes[i], temp_followingBlock);
      }

      int rest = data.length() % 500;
      temp_followingBlock = this->mMemblockDevice.readBlock(following_blockIndexes[blocks_required-2]).toString();
      for (int i = 0; i < rest; i++){
        temp_followingBlock[i+12] = data[i + ((blocks_required-1)*500)];
      }
      this->mMemblockDevice.writeBlock(following_blockIndexes[blocks_required-2], temp_followingBlock);

    }

    else{
      for (size_t i = 12; i < data.length()+12; i++){
        temp[i] = data[i -12];
      }
    }
    sizemap[int(temp[1])] = data.length();


    this->mMemblockDevice.writeBlock(blockIndex, temp);
  }
  if (name_is_path){
    this->changeDir(start);
  }
  return returnValue;
}

std::string FileSystem::read(std::string fileName){
  std::string start = "";
  std::string path;
  std::string returnValue = "";
  bool name_is_path = contains_slash(fileName);
  if (name_is_path){
    start = this->getCurrentPath();
    path = fileName.substr(0, fileName.find_last_of('/'));
    fileName = fileName.substr(fileName.find_last_of('/')+1);
    this->changeDir(path);
  }
    if (start != this->getCurrentPath()) {
      int blockIndex = int(this->currentDir[getIndex(fileName)]);
      int fileSize = sizemap[int(this->mMemblockDevice[blockIndex][1])];
      int nrOfBlocks = (fileSize/500) + 1;
      if (fileSize%500 == 0){
        nrOfBlocks--;
      }
      for(int i = 0; i < nrOfBlocks; i++){
        for(int j = 12; j < 512; j++){
          returnValue += this->mMemblockDevice[blockIndex][j];
        }
        blockIndex = this->mMemblockDevice[blockIndex][11];
      }
      int rest = fileSize%500;

      for(int i; i < rest; i++){
        returnValue += this->mMemblockDevice[blockIndex][11];
      }
    }
  if (name_is_path){
    this->changeDir(start);
  }
  return returnValue;
}

int FileSystem::remove(std::string name){

  int blockIndex;
  int returnValue = -1;
  std::string start = this->getCurrentPath();
  std::string path;
  bool name_is_path = contains_slash(name);
  if (name_is_path){
    path = name.substr(0, name.find_last_of('/'));
    name = name.substr(name.find_last_of('/')+1);
    this->changeDir(path);
  }
  int directoryIndex = getIndex(name);
  if (directoryIndex != -1){
    int flag = fileOrDir(this->currentDir[directoryIndex]);
    blockIndex = this->currentDir[directoryIndex];
    if (flag == 1){
      returnValue = this->removeFile(directoryIndex);
    }

    else if (flag == 0){
      returnValue = this->removeFolder(directoryIndex);
    }
    bitmap[blockIndex] = false;

  }
  if (name_is_path){
    this->changeDir(start);
  }
  return returnValue;
}

int FileSystem::removeFile(int directoryIndex_ofFile){
  std::string temp = this->currentDir.toString();
  if ((directoryIndex_ofFile != -1) && (sizemap[int(temp[1])] > 2)) { //om filen finns i currentDir

    int index_of_last_element = 10 + (sizemap[int(temp[1])]);

    if(directoryIndex_ofFile != index_of_last_element){
      temp[directoryIndex_ofFile] = temp[index_of_last_element];
    }

    sizemap[int(temp[1])]--;
    //
    this->mMemblockDevice.writeBlock(int(temp[11]), temp);
    this->currentDir = this->mMemblockDevice.readBlock(int(temp[11]));
  }
  return directoryIndex_ofFile;
}

int FileSystem::removeFolder(int directoryIndex_ofDir){
  std::string temp = this->currentDir.toString();
  std::string dirToBeRemoved = this->mMemblockDevice.readBlock(int(temp[directoryIndex_ofDir])).toString();
  if (dirToBeRemoved[1] <= 2){
    if ((directoryIndex_ofDir != -1) && (sizemap[int(temp[1])] > 2)) { //om filen finns i currentDir
      int index_of_last_element = 10 + (sizemap[int(temp[1])]);

      if(directoryIndex_ofDir != index_of_last_element){
        temp[directoryIndex_ofDir] = temp[index_of_last_element];
      }

      sizemap[int(temp[1])]--;
      this->mMemblockDevice.writeBlock(int(temp[11]), temp);
      this->currentDir = this->mMemblockDevice.readBlock(int(temp[11]));
    }
    return directoryIndex_ofDir;
  }
  else{
    return 0;
  }
}

std::string FileSystem::getCurrentPath(){
  int blockIndex;
  std::string output = "";
  std::string temp_dir_name = "";
  std::string temp_header = this->currentDir.toString();
  blockIndex = int(temp_header[11]);

  while(temp_header[11] != 0){
    temp_dir_name = this->getFileName(blockIndex);
    output = temp_dir_name + "/" + output;
    blockIndex = temp_header[12];
    temp_header = this->mMemblockDevice.readBlock(blockIndex).toString();
  }
  output = "/" + output;
  return output;
}


std::string FileSystem::listDir(){
  std::string temp = this->currentDir.toString();
  std::string returnValue = "";
  for (int i = 13; i < int(sizemap[int(temp[1])]) + 11; i++) {
    std::string temp_header = this->mMemblockDevice.readBlock(temp[i]).toString();
    std::string output = "";
    if(temp_header[0] == 1){
      output += "FILE\t";
    }
    else{
      output += "DIR\t";
    }

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
    output += std::to_string(int(sizemap[int(temp_header[1])])) + "byte";
    output += "\n";
    returnValue += output;
  }
  return returnValue;
}


bool FileSystem::changeDir(std::string path){
  int found = 0;
  std::string subDir = path;
  Block tempdir = this->currentDir;
  if (subDir[0] == '/'){
    this->currentDir = this->mMemblockDevice.readBlock(0);
    subDir = subDir.substr(1);
  }
  std::string dirToFind = "";
  while (subDir != ""){
    found = 0;
    while (subDir.substr(0,2) == "..") {
      this->currentDir = this->mMemblockDevice.readBlock(this->currentDir[12]);
      subDir = subDir.substr(2);
      if (subDir[0] == '/' && subDir.length() > 1) {
        subDir = subDir.substr(1);
      }
      else{
        subDir = "";
        found = 1;
      }
    }
    while (subDir.substr(0,1) == ".") {
      subDir = subDir.substr(1);
      found = 1;
    }

    if(subDir.find("/") != std::string::npos){
      dirToFind = subDir.substr(0,subDir.find("/"));
      subDir = subDir.substr(subDir.find("/")+1);
    }
    else{
      dirToFind = subDir;
      subDir = "";
    }
    int nrOfEntries = sizemap[int(this->currentDir[1])];
    for (int i = 11; i < nrOfEntries + 11; i++) {
      std::string entryName = this->getFileName(this->currentDir[i]);
      int entryBIndex = int(this->currentDir[i]);
      if ((entryName == dirToFind) && (this->mMemblockDevice[entryBIndex][0] == 0)) {
        this->currentDir = this->mMemblockDevice.readBlock(int(this->currentDir[i]));
        found = 1;
        break;
      }
    }
    if(found == 0){
      this->currentDir = tempdir;
      subDir = "";
    }
    else{
      found = 1;
    }
  }
  return found;
}
