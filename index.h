#ifndef INDEX_H
#define INDEX_H

//Function to compare word strings.
int compareStrings(void *c1, void *c2);

//Parses lines from files into separate words
void tokenize(char *string, char *fileName);

//Reads lines from a file
void readFile(char *filePath, char *fileName, char *subdirec);

//Moves through the filepath and any subdirectories taking note of files along the way
void readDirect(char *path, char *sub_path);

//Writes data from memory to file
void writeToFile(char *new_file);


#endif
