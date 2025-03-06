/*
    main.cpp
    Sandbox

    Created by alimovlex.
    Copyright (c) 2020 alimovlex. All rights reserved.
*/

#include "gemini.hpp"

using namespace std;

int main(int argc, char **argv) {
    string prompt = "";
    //cout << "What is your question to Gemini today?" << endl;
    //cin >> prompt;
    get_geminis_response(prompt);
    return 0;
}
