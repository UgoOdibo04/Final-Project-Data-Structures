#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main(){
    
    
    ifstream inFile;
    inFile.open("tmdb_5000_credits.csv");
    
    string text;
    
    if(!inFile.is_open()){
        cout << "Unable to open file" << endl;
        return 0;
    }
    
    
    string keyword = "job"; 

    
    while (inFile) {
        char ch = ',';
        text.clear();  

        
        while (inFile.get(ch) && ch != '}') {
            text += ch;  
        }

        
        if (ch == '}') {
            text += ch;
        }

        
        if (text.find(keyword) != string::npos) {
            
            continue;
        }

        
        cout << text << endl;
        
    }

    
    return 0;
}