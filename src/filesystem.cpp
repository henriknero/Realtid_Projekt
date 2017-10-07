#include "filesystem.h"
#include <iostream>

//std::string initiation = ''

FileSystem::FileSystem() {

  this->mMemblockDevice = MemBlockDevice();
  std::string temp = (this->mMemblockDevice.readBlock(0)).toString();
  temp[0] = 1;
  temp[1] = 0;
  temp[2] = 'f';
  temp[3] = 'i';
  temp[4] = 'l';
  temp[5] = 'n';
  temp[6] = 'a';
  temp[7] = 'm';
  temp[8] = 'n';
  this->mMemblockDevice.writeBlock(0, temp);
  std::cout << temp << std::endl;
  std::cout << this->mMemblockDevice.readBlock(0) << std::endl;
  char* header = this->mMemblockDevice.readBlock(0).readHeader();
  std::cout << header << std::endl;
  //this->mMemblockDevice.writeBlock(0, []);

}





FileSystem::~FileSystem() {

}



/* Please insert your code
char* Filesystem::readHeader(Block* block){
  char* temp_header = new char[10];
  for(int i = 0; i < 10; i++){

  }
}
*/
