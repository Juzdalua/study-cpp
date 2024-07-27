#include <stdio.h>
#include <iostream>
#include <set>

using namespace std;

int main()
{
    set<int> intSet;
    intSet.insert(10);
    intSet.insert(99);
    intSet.insert(4);
    intSet.insert(1);

    set<int>::iterator intSetIter;
    for(intSetIter = intSet.begin(); intSetIter != intSet.end(); intSetIter++){
        cout << *intSetIter << endl;
    }
    
    set<char> charSet;
    charSet.insert('A');
    charSet.insert('a');
    charSet.insert('C');
    charSet.insert('f');

    set<char>::iterator charSetIter;
    for(charSetIter = charSet.begin(); charSetIter != charSet.end(); charSetIter++){
        cout << *charSetIter << endl;
    }

    if(charSet.find('A') != charSet.end()){
        charSet.erase('A');
    }

    return 0;
}