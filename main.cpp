#include <iostream>
#include <cstdlib>
#include <vector>
#include<fstream>
#include<sys/types.h>
#include<dirent.h>
#include<cerrno>
#include<unordered_map>
#include<functional>
using namespace std;

void norm(string &hstr);
void buildHashTable(ifstream &myFile, unordered_map<unsigned long,string> &comp, int chunkSize, string fname, vector<string> &hashTable);
int countCollisions(ifstream &myFile, unordered_map<unsigned long,string> &comp, int chunkSize, string fname, vector<string> &hashTable);
bool findInTable(unsigned long hashVal, vector<string> &hashTable);
long hashFcn(string);
struct data_out{
    int collisionCount;
    string file1;
    //string file2;
};

void printOutput(vector<data_out> &data, const int len);


int getdir (string dir, vector<string> &files){
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

int main(int argc, char *argv[]) {
    /*if(argc != 4){
        cout << "incorrect parameters!" << endl;
        return -1;
    }*/
    string dir = "testdir"; //argv[1];
    int chunkSize = 6; //atoi(argv[2]);
    int threshold = 200; //atoi(argv[3]);
    const int TABLE_SIZE = 900019;

    vector<string> files = vector<string>();
    vector<string> hashTable = vector<string>();
    hashTable.reserve(TABLE_SIZE);

    getdir(dir, files);
    /*for(unsigned int i = 0; i < files.size(); i ++){
        cout << i << files[i] << endl;
    }*/
    cout << endl;
    vector<string>::iterator it;
    it = files.begin();
    files.erase(it);
    it = files.begin();
    files.erase(it);

    vector<string>::iterator itComp;


    string fname;
    string compName;
    ifstream myFile;
    ifstream compFile;
    vector<data_out> output =  vector<data_out>();
    int compCount = 0;
    unordered_map<unsigned long,string> master;
    int collisions = 0;

    if(files.size() < 2){
        cout <<"not enough files!";
        return 0;
    }
    data_out temp;

    for(it = files.begin(); it!= files.end()-1; it++){
        fname = dir + "/" + *it;
        myFile.open(fname.c_str());
        buildHashTable(myFile, master, chunkSize, fname, hashTable);
        //cout << "\n\nCOMP TABLE\n\n";
        for(itComp = it + 1; itComp != files.end(); itComp++){
            compName = dir + "/" + *itComp;
            compFile.open(compName.c_str());
            collisions = countCollisions(compFile,master,chunkSize,compName, hashTable);
            if(collisions >= threshold) {
                cout << *it << " " << *itComp << " " << collisions << endl;
                temp.collisionCount = collisions;
                temp.file1 = *it + " " + *itComp;
                output.push_back(temp);
                compCount++;
            }
            compFile.close();
        }
        myFile.close();
        master.clear();
        hashTable.clear();
        hashTable.reserve(TABLE_SIZE);
    }

    cout << "SHOWING FILES WITH >" << threshold << " COLLISIONS OF SIZE " << chunkSize << " CHUNKS" << endl;
    //printOutput(output, compCount);
    return 0;
}

void norm(string &str){
    string::iterator it;
    for(it = str.begin(); it<str.end(); it++){
        if(*it >= 97 && *it <= 122){
            *it = *it - 32; //convert to upper
        }
        else if(*it < 65 || *it > 90){
            str.erase(it);
            it--; //delete non alphabet
        }
    }
}

void buildHashTable(ifstream &myFile, unordered_map<unsigned long,string> &comp, const int chunkSize, string fname, vector<string> &hashTable){
    unsigned long hashVal;
    vector<string> chunkQ;
    vector<string>::iterator it;
    if(myFile.good() && myFile.is_open()){
        string inWord;
        string chunkStr;
        string temp;

        while(chunkQ.size() < chunkSize && !myFile.eof()){
            myFile >> inWord;
            norm(inWord);
            if(!inWord.empty()) {
                chunkQ.push_back(inWord);
            }
        } //build initial chunk

        while(!myFile.eof()){

            for(int i = 0; i < chunkSize; i ++){
                chunkStr += chunkQ.at(i);
            }

            chunkQ.erase(chunkQ.begin());

            while(chunkQ.size() < chunkSize) {
                myFile >> inWord;
                norm(inWord);
                chunkQ.push_back(inWord);
            }

            //hashVal = hash<std::string>{}(chunkStr) % 900019;
            hashVal = hashFcn(chunkStr);
            //cout << hashVal << " " << chunkStr << "\n";
            comp.insert(make_pair(hashVal,chunkStr));
            //hashTable[hashVal] = chunkStr;
            chunkStr = "";
        }

    }

}

int countCollisions(ifstream &myFile, unordered_map<unsigned long,string> &comp, const int chunkSize, string fname, vector<string> &hashTable) {
    unsigned long hashVal;
    vector<string> chunkQ;
    vector<string>::iterator it;
    int collisionCount = 0;
    if(myFile.good() && myFile.is_open()){
        string inWord;
        string chunkStr;
        string temp;

        while(chunkQ.size() < chunkSize && !myFile.eof()){
            myFile >> inWord;
            norm(inWord);
            if(!inWord.empty()) {
                chunkQ.push_back(inWord);
            }
        } //build initial chunk

        while(!myFile.eof()){

            for(int i = 0; i < chunkSize; i ++){
                chunkStr += chunkQ.at(i);
            }

            chunkQ.erase(chunkQ.begin());

            while(chunkQ.size() < chunkSize) {
                myFile >> inWord;
                norm(inWord);
                chunkQ.push_back(inWord);
            }

            //hashVal = hash<std::string>{}(chunkStr) % 900019;
            hashVal = hashFcn(chunkStr);
            //cout << hashVal << " " << chunkStr << "\n";
            if(comp.find(hashVal) != comp.end()){
                collisionCount++;
            }
            /*if(findInTable(hashVal, hashTable)) {
                collisionCount++;
            }*/
            chunkStr = "";
        }
    }
    return collisionCount;

}

void printOutput(vector<data_out> &data, const int len){
    data_out temp;
    int maxIndex;

    for(int i = 0; i < len-1; i ++){
        maxIndex = i;
        for(int j = i+1; j < len; j++){
            if(data[j].collisionCount > data[maxIndex].collisionCount){
                maxIndex = j; //find index of max collision count
            }
        }
        //swap maxIndex and i
        if(maxIndex != i){
            temp.collisionCount = data[i].collisionCount;
            temp.file1 = data[i].file1;

            data[i].collisionCount = data[maxIndex].collisionCount;
            data[i].file1 = data[maxIndex].file1;

            data[maxIndex].collisionCount = temp.collisionCount;
            data[maxIndex].file1 = temp.file1;
        }
    }

    for(int i = 0; i < len; i ++){
        cout << data[i].collisionCount << " : " << data[i].file1 << " " << endl;
    }
}

bool findInTable(unsigned long hashVal, vector<string> &hashTable){
    return !hashTable[hashVal].empty();
}

long hashFcn(string s){
    const int TABLE_SIZE = 900019;
    long hash = 0;
    int base = 37;
    int exp = 1;
    for(char c : s){
        hash = (hash + c*exp) % TABLE_SIZE;
        exp = base * exp % TABLE_SIZE;
    }
    return hash;

}
