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

void norm(string &str);
void buildHashTable(ifstream &myFile, unordered_map<unsigned long,string> &comp, int chunkSize, string fname);
int countCollisions(ifstream &myFile, unordered_map<unsigned long,string> &comp, int chunkSize, string fname);



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
    if(argc != 4){
        cout << "incorrect parameters!" << endl;
        return -1;
    }
    string dir = argv[1];
    int chunkSize = atoi(argv[2]);
    int threshold = atoi(argv[3]);


    vector<string> files = vector<string>();


    getdir(dir, files);
    for(unsigned int i = 0; i < files.size(); i ++){
        cout << i << files[i] << endl;
    }
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

    unordered_map<unsigned long,string> master;
    int collisions = 0;

    if(files.size() < 2){
        cout <<"not enough files!";
        return 0;
    }

    for(it = files.begin(); it!= files.end()-1; it++){
        fname = dir + "/" + *it;
        myFile.open(fname.c_str());
        buildHashTable(myFile, master, chunkSize, fname);
        //cout << "\n\nCOMP TABLE\n\n";
        for(itComp = it + 1; itComp != files.end(); itComp++){
            compName = dir + "/" + *itComp;
            compFile.open(compName.c_str());
            collisions = countCollisions(compFile,master,chunkSize,compName);
            if(collisions >= threshold) {
                cout << *it << " " << *itComp << " " << collisions << endl;
            }
            compFile.close();
        }
        myFile.close();
        master.clear();
    }


/*
    myFile.open(fname.c_str());
    compFile.open(compName.c_str());

    unordered_map<unsigned long,string> comp;

    buildHashTable(myFile, master, chunkSize, fname);
    int coll = countCollisions(compFile, master, chunkSize, compName);
    cout << coll << "\n";
    unordered_map<unsigned long,string>::iterator iter;

    myFile.close();
    compFile.close(); */

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

void buildHashTable(ifstream &myFile, unordered_map<unsigned long,string> &comp, const int chunkSize, string fname){
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

            hashVal = hash<std::string>{}(chunkStr);
            //cout << hashVal << " " << chunkStr << "\n";
            comp.insert(make_pair(hashVal,chunkStr));
            chunkStr = "";
        }

    }

}

int countCollisions(ifstream &myFile, unordered_map<unsigned long,string> &comp, const int chunkSize, string fname) {
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

            hashVal = hash<std::string>{}(chunkStr);
            //cout << hashVal << " " << chunkStr << "\n";
            if(comp.find(hashVal) != comp.end()){
                collisionCount++;
            }
            chunkStr = "";
        }
    }
    return collisionCount;

}

