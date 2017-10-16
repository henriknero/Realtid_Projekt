#include <iostream>
#include <sstream>
#include "filesystem.h"

const int MAXCOMMANDS = 8;
const int NUMAVAILABLECOMMANDS = 16;

std::string availableCommands[NUMAVAILABLECOMMANDS] = {
    "quit","format","ls","create","cat","createImage","restoreImage",
    "rm","cp","append","mv","mkdir","cd","pwd","help","chmod"
};

/* Takes usercommand from input and returns number of commands, commands are stored in strArr[] */
int parseCommandString(const std::string &userCommand, std::string strArr[]);
int findCommand(std::string &command);
bool quit();
std::string help();

/* More functions ... */

int main(void) {
	std::string userCommand, commandArr[MAXCOMMANDS];
	std::string user = "HenrikNero+JohanNaslund";    // Change this if you want another user to be displayed
	std::string currentDir = "/";    // current directory, used for output
  std::string output;
  int returnValue;

  FileSystem *fileSystem;
    bool bRun = true;

    do {
        commandArr[2] = "";
        commandArr[1] = "";
        std::cout << user << ":" << currentDir << "$ ";
        getline(std::cin, userCommand);

        int nrOfCommands = parseCommandString(userCommand, commandArr);
        if (nrOfCommands > 0) {

            int cIndex = findCommand(commandArr[0]);
            switch(cIndex) {

			case 0: //quit
				bRun = quit();
                break;
            case 1: // format
                fileSystem = new FileSystem();
                break;
            case 2: // ls
                std::cout << fileSystem->listDir(commandArr[1]) << std::endl;
                break;
            case 3: // create
                if(fileSystem->createFile(commandArr[1]) == 1){
                  std::string tempString;
                  getline(std::cin,tempString);
                  fileSystem->write(commandArr[1], tempString);
                }
                break;
            case 4: // cat
                std::cout << fileSystem->read(commandArr[1]) << std::endl;
                break;
            case 5: // createImage
                fileSystem->createImage(commandArr[1]);
                break;
            case 6: // restoreImage
                fileSystem->restoreImage(commandArr[1]);
                break;
            case 7: // rm
                fileSystem->remove(commandArr[1]);
                break;
            case 8: // cp
              {
                returnValue = fileSystem->copy(commandArr[1], commandArr[2]);
                if(returnValue == -2){
                  std::cout << "cp: File does not exist or is Empty" << std::endl;
                }
                if (returnValue == -1) {
                  std::cout << "cp: Path or file does not exist" << std::endl;
                }
                if (returnValue == -3) {
                  std::cout << "cp: Directory is Full" << std::endl;
                }
                if (returnValue == -4) {
                  std::cout << "cp: Filename Already Exists" << std::endl;
                }
                if (returnValue == -5){
                  std::cout << "cp: Insufficient rights" << std::endl;
                }
                break;
              }
            case 9: // append
                returnValue = fileSystem->append(commandArr[1], commandArr[2]);
                if (returnValue == -1) {
                  std::cout << "append: Path or file does not exist" << std::endl;
                }
                if (returnValue == -5){
                  std::cout << "append: Insufficient rights" << std::endl;
                }

                break;
            case 10: // mv
            {
                returnValue = fileSystem->move(commandArr[1], commandArr[2]);
                if (returnValue == -4) {
                  std::cout << "Destination filename already exists" << std::endl;
                }
                if (returnValue == -3) {
                  std::cout << "Directory is full" << std::endl;
                }
                if(returnValue == -2){
                  std::cout << "Source file not found or Insufficient privilege" << std::endl;
                }
                if (returnValue == -1) {
                  std::cout << "mv: Path or file does not exist" << std::endl;
                }
                break;
            }
            case 11: // mkdir
                fileSystem->createFolder(commandArr[1]);
                break;
            case 12: // cd
                if(fileSystem->changeDir(commandArr[1])){
                  currentDir = fileSystem->getCurrentPath();
                }
                else{
                  std::cout << "cd: Directory not found" << std::endl;
                }
                break;
            case 13: // pwd
                std::cout << fileSystem->getCurrentPath() <<std::endl;
                break;
            case 14: // help
                std::cout << help() << std::endl;
                break;
            case 15: // chmod
                returnValue = fileSystem->changePrivilege(commandArr[1], commandArr[2]);
                if (returnValue == -1){
                  std::cout << "chmod: Path or file does not exist" << std::endl;
                }
                if (returnValue == -2){
                  std::cout << "chmod: incorrect privilege: Privilege codes allowed: 0, 1, 2, or 3" << std::endl;
                }
                break;

            default:
                std::cout << "Unknown command: " << commandArr[0] << std::endl;
            }
        }
    } while (bRun == true);
    delete fileSystem;

    return 0;
}

int parseCommandString(const std::string &userCommand, std::string strArr[]) {
    std::stringstream ssin(userCommand);
    int counter = 0;
    while (ssin.good() && counter < MAXCOMMANDS) {
        ssin >> strArr[counter];
        counter++;
    }
    if (strArr[0] == "") {
        counter = 0;
    }
    return counter;
}
int findCommand(std::string &command) {
    int index = -1;
    for (int i = 0; i < NUMAVAILABLECOMMANDS && index == -1; ++i) {
        if (command == availableCommands[i]) {
            index = i;
        }
    }
    return index;
}

bool quit() {
	std::cout << "Exiting\n";
	return false;
}

std::string help() {
    std::string helpStr;
    helpStr += "OSD Disk Tool .oO Help Screen Oo.\n";
    helpStr += "-----------------------------------------------------------------------------------\n" ;
    helpStr += "* quit:                             Quit OSD Disk Tool\n";
    helpStr += "* format;                           Formats disk\n";
    helpStr += "* ls     <path>:                    Lists contents of <path>.\n";
    helpStr += "* create <path>:                    Creates a file and stores contents in <path>\n";
    helpStr += "* cat    <path>:                    Dumps contents of <file>.\n";
    helpStr += "* createImage  <real-file>:         Saves disk to <real-file>\n";
    helpStr += "* restoreImage <real-file>:         Reads <real-file> onto disk\n";
    helpStr += "* rm     <file>:                    Removes <file>\n";
    helpStr += "* cp     <source> <destination>:    Copy <source> to <destination>\n";
    helpStr += "* append <source> <destination>:    Appends contents of <source> to <destination>\n";
    helpStr += "* mv     <old-file> <new-file>:     Renames <old-file> to <new-file>\n";
    helpStr += "* mkdir  <directory>:               Creates a new directory called <directory>\n";
    helpStr += "* cd     <directory>:               Changes current working directory to <directory>\n";
    helpStr += "* pwd:                              Get current working directory\n";
    helpStr += "* help:                             Prints this help screen\n";
    helpStr += "* chmod <accessrights> <filepath>:  Changes the access rights (0-3) of <filepath>\n";
    return helpStr;
}

/* Insert code for your shell functions and call them from the switch-case */
