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
//hej Hopp (directory index)
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
      if(fileName.length() < 8){
        temp[3+fileName.length()] = '\0';
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
    returnValue = -4; //Current Directory Full?
  }

  if (name_is_path){
    this->changeDir(start);
  }
  return returnValue;
}

int FileSystem::write(std::string fileName, std::string data){
  std::string start = "";
  std::string path;
  //int blocks_required;
  //int directoryIndex_ofFile;
  //int blockIndex;
  bool name_is_path = contains_slash(fileName);
  if (name_is_path){
    start = this->getCurrentPath();
    path = fileName.substr(0, fileName.find_last_of('/'));
    fileName = fileName.substr(fileName.find_last_of('/')+1);
    this->changeDir(path);
  }
  if (start != this->getCurrentPath()) {
    int blocks_required = (data.length() / 500) + 1 ;
    int directoryIndex_ofFile = getIndex(fileName);
    int blockIndex = this->currentDir[directoryIndex_ofFile];
    std::string temp = this->mMemblockDevice.readBlock(blockIndex).toString();


    for (int i = 0; i < blocks_required; i++){

    }
    
    for (int i = 12; i < data.length()+12; i++){
      temp[i] = data[i -12];
    }
    temp[1] = data.length();
    this->mMemblockDevice.writeBlock(blockIndex, temp);
  }
  if (name_is_path){
    this->changeDir(start);
  }
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

//inte klar, fuckar ur
int FileSystem::removeFile(int directoryIndex_ofFile){
  std::string temp = this->currentDir.toString();
  //int directoryIndex_ofFile = getIndex(fileName);
  if ((directoryIndex_ofFile != -1) && (temp[1] > 2)) { //om filen finns i currentDir
    //temp[1] är mappens nrOf
    int index_of_last_element = 10 + (temp[1]);

    if(directoryIndex_ofFile != index_of_last_element){
      temp[directoryIndex_ofFile] = temp[index_of_last_element];
    }

    temp[1]--;
    //
    this->mMemblockDevice.writeBlock(int(temp[11]), temp);
    this->currentDir = this->mMemblockDevice.readBlock(int(temp[11]));
  }
  return directoryIndex_ofFile;
}

int FileSystem::removeFolder(int directoryIndex_ofDir){
  std::string temp = this->currentDir.toString();
  //int directoryIndex_ofFile = getIndex(fileName);
  std::string dirToBeRemoved = this->mMemblockDevice.readBlock(int(temp[directoryIndex_ofDir])).toString();
  if (dirToBeRemoved[1] <= 2){
    if ((directoryIndex_ofDir != -1) && (temp[1] > 2)) { //om filen finns i currentDir
      //temp[1] är mappens nrOf
      int index_of_last_element = 10 + (temp[1]);

      if(directoryIndex_ofDir != index_of_last_element){
        temp[directoryIndex_ofDir] = temp[index_of_last_element];
      }

      temp[1]--;
      //
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
  return output;
}


std::string FileSystem::listDir(){
  std::string temp = this->currentDir.toString();
  std::string returnValue = "";
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
    output += std::to_string(int(temp_header[1])) + "byte";
    //std::cout << output << int(temp_header[1]) << "byte" << std::endl;
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
    int nrOfEntries = int(this->currentDir[1]);
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
      std::cout << "Directory not found" << std::endl;
      this->currentDir = tempdir;
      subDir = "";
    }
    else{
      found = 1;
    }
  }
  return found;
}
/* Please insert your code
char* Filesystem::readHeader(Block* block){
  char* temp_header = new char[10];
  for(int i = 0; i < 10; i++){

  }
}
*/
