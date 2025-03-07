/*
    main.cpp
    Sandbox

    Created by alimovlex.
    Copyright (c) 2020 alimovlex. All rights reserved.
*/

#include "gemini.hpp"


using namespace std;

int main(int argc, char **argv) {

    string prompt;
    const string key = "";

    if (key.empty()) {
        cerr << "ERROR!!! NO API KEY PROVIDED!!!" << endl;
        return -1;
    } else {
        cout << "What is your question to Gemini today?" << endl;
    
        while (prompt.empty()) {
        getline(cin, prompt);
        }
    
        get_geminis_response(prompt, key);
        format_response(prompt);

        return 0;
    } 
    
}
