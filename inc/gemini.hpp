#pragma once
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <json.h>

using namespace std;

vector<char> download(string url, long* responseCode);
int get_geminis_response(string& prompt, const string& key);
void format_response(json_object *json);
