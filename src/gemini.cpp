#include <curl/curl.h>
#include <json.h>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <regex>

#include <sysexits.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#include "gemini.hpp"
using namespace std;

size_t callback(void* contents, size_t size, size_t nmemb, void* user)
{
    auto chunk = reinterpret_cast<char*>(contents);
    auto buffer = reinterpret_cast<vector<char>*>(user);

    size_t priorSize = buffer->size();
    size_t sizeIncrease = size * nmemb;

    buffer->resize(priorSize + sizeIncrease);
    copy(chunk, chunk + sizeIncrease, buffer->data() + priorSize);

    return sizeIncrease;
}

size_t callBack(const char* in, size_t size, size_t num, string* out)
{
    const size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}

vector<char> download(string url, long* responseCode)
{
    vector<char> data;

    curl_global_init(CURL_GLOBAL_ALL);
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    CURLcode result = curl_easy_perform(handle);
    if (responseCode != nullptr)
        curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, responseCode);
    curl_easy_cleanup(handle);
    curl_global_cleanup();

    if (result != CURLE_OK)
    {
        stringstream err;
        err << "Error downloading from URL \"" << url << "\": " << curl_easy_strerror(result);
        throw runtime_error(err.str());
    }
    for (auto i: data)
        cout<< i;
    return data;
}

int get_geminis_response(string& prompt, const string& key)
{
    const string base_url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key=";
    const string url = base_url + key;
    CURL* curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    string jsonStr = "{"
                     "\"contents\": [{"
                     "\"parts\": [{"
                     "\"text\": \"" + prompt + "\""
                                               "}]"
                                               "}]"
                                               "}";
    // Set remote URL.
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Don't bother trying IPv6, which would increase DNS resolution time.
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    // Don't wait forever, time out after 30 seconds.
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);

    // Follow HTTP redirects if necessary.
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Response information.
    long httpCode(0);
    unique_ptr<string> httpData(new string());

    // Hook up data handling function.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callBack);

    // Hook up data container (will be passed as the last parameter to the
    // callback handling function).  Can be any pointer type, since it will
    // internally be passed as a void pointer.
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());

    // Run our HTTP GET command, capture the HTTP response code, and clean up.
    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);

    if (httpCode == 200)
    {
        json_object* resp_root = json_tokener_parse(httpData->c_str());
        if (!resp_root) {
            cerr << "Failed to parse JSON response" << endl;
            return -1;
        }
        format_response(resp_root);
    }
    else
    {
        cout << "Couldn't GET from " << url << " - exiting" << endl;
        return 1;
    }
    return 0;
}

void format_response(json_object *resp_root) {

    // Extract the text field: candidates[0].content.parts[0].text
    json_object* candidates_arr = nullptr;
    json_object_object_get_ex(resp_root, "candidates", &candidates_arr);
    if (candidates_arr && json_object_get_type(candidates_arr) == json_type_array) {
        json_object* first_candidate = json_object_array_get_idx(candidates_arr, 0);
        json_object* content_obj = nullptr;
        json_object_object_get_ex(first_candidate, "content", &content_obj);
        json_object* parts_arr = nullptr;
        json_object_object_get_ex(content_obj, "parts", &parts_arr);
        if (parts_arr && json_object_get_type(parts_arr) == json_type_array) {
            json_object* first_part = json_object_array_get_idx(parts_arr, 0);
            json_object* text_obj = nullptr;
            json_object_object_get_ex(first_part, "text", &text_obj);
            if (text_obj) {
                const char* text = json_object_get_string(text_obj);
                cout << "Response: " << text << endl;
                //prompt = text;   // optionally replace prompt with the answer
            }
        }
    }
    json_object_put(resp_root);   // free parsed tree
}
