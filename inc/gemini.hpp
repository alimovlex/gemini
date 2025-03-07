//
// Created by alimovlex
//

//
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>

using namespace std;

vector<char> download(string url, long* responseCode);
int get_geminis_response(string& prompt, const string& key);
int format_response(string& response);