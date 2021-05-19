/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */
#include "common/http/HttpClient.h"
#include "common/process/ProcessUtils.h"
#include "common/base/Cord.h"
#include <curl/curl.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

namespace nebula {
namespace http {

 StatusOr<std::string> HttpClient::get(const std::string& path, const std::string& /*options*/) {
    CURL *curl;
    CURLcode res;
    // create a new pointer of a newseesion;
    // path="http://wwww.baidu.com";
    // std::string mypath="http://www.baidu.luxian";
    FILE *f;
    curl = curl_easy_init();
    f = fopen("temp.txt", "w");
    if (curl) {
        // set the get url of the libcurl option;
        curl_easy_setopt(curl, CURLOPT_URL, path.c_str());
        // define the dedirect path of the session;
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
        // curl_easy_setopt(curl,CURLOPT_HEADERDATA,f);

        // perform the request,res will get the return code;
        res = curl_easy_perform(curl);
        LOG(INFO) << "HTTP return Code: " << res;
       // sleep(1);
        fclose(f);
        f = fopen("temp.txt", "rw");
        Cord out;
        char buf[1024];
        size_t len;
        do {
            len = fread(buf, 1, 1024, f);
            if (len > 0) {
                out.write(buf, len);
            }
        } while (len == 1024);
        fclose(f);
        StatusOr<std::string> result = out.str();
        curl_easy_cleanup(curl);
        if (result.ok()) {
            return result.value();
        } else {
            return Status::Error(folly::stringPrintf("Http Get Failed: %s", path.c_str()));
        }
    }
    return "";
}

StatusOr<std::string> HttpClient::post(const std::string& path, const std::string& header) {
    auto command =
        folly::stringPrintf("/usr/bin/curl -X POST %s \"%s\"", header.c_str(), path.c_str());
    LOG(INFO) << "HTTP POST Command: " << command;

    // auto result = nebula::ProcessUtils::runCommand(command.c_str());
    // if (result.ok()) {
    //     return result.value();
    // } else {
    //     return Status::Error(folly::stringPrintf("Http Post Failed: %s", path.c_str()));
    // }
    CURL *curl;
    CURLcode res;
    struct curl_slist *my_curl_list = nullptr;
    int len = header.length();
    int now_total = 0;
    std::string newString = "";
   for ( int i = 0 ; i < len ; i++ ) {
            if (header[i] == '\"') {
                now_total++;
                if (now_total%2 == 1) {
                    newString = "";
                    continue;
                }
                if (now_total%2 == 0) {
                    my_curl_list = curl_slist_append(my_curl_list, newString.c_str());
                }
                continue;
            }
            newString+=header[i];
        }
    curl = curl_easy_init();
    if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, path);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, my_curl_list);
    /* if we don't provide POSTFIELDSIZE, libcurl will strlen() by
       itself */
    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    curl_slist_free_all(my_curl_list);
    /* Check for errors */
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return "HttpClientHandler failed";
    }
    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  return "HttpClientHandler successfully";
}

StatusOr<std::string> HttpClient::post(const std::string& path,
                                       const std::unordered_map<std::string, std::string>& header) {
    folly::dynamic mapData = folly::dynamic::object;
    // Build a dynamic object from map
    for (auto const& it : header) {
        mapData[it.first] = it.second;
    }
    return post(path, mapData);
}

StatusOr<std::string> HttpClient::post(const std::string& path, const folly::dynamic& data) {
    return sendRequest(path, data, "POST");
}

StatusOr<std::string> HttpClient::put(const std::string& path,
                                       const std::unordered_map<std::string, std::string>& header) {
    folly::dynamic mapData = folly::dynamic::object;
    // Build a dynamic object from map
    for (auto const& it : header) {
        mapData[it.first] = it.second;
    }
    return put(path, mapData);
}

StatusOr<std::string> HttpClient::put(const std::string& path, const std::string& header) {
    auto command =
        folly::stringPrintf("/usr/bin/curl -X PUT %s \"%s\"", header.c_str(), path.c_str());
    LOG(INFO) << "HTTP PUT Command: " << command;
    // auto result = nebula::ProcessUtils::runCommand(command.c_str());
    // if (result.ok()) {
    //     return result.value();
    // } else {
    //     return Status::Error(folly::stringPrintf("Http Put Failed: %s", path.c_str()));
    // }
    CURL *curl;
    CURLcode res;
    struct curl_slist *my_curl_list = nullptr;
    int len = header.length();
    int now_total = 0;
    std::string newString = "";
   for ( int i = 0 ; i < len ; i++ ) {
            if (header[i] == '\"') {
                now_total++;
                if (now_total%2 == 1) {
                    newString = "";
                    continue;
                }
                if (now_total%2 == 0) {
                    my_curl_list = curl_slist_append(my_curl_list, newString.c_str());
                }
                continue;
            }
            newString+=header[i];
        }
    curl = curl_easy_init();
    if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, path);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, my_curl_list);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "put");
    /* if we don't provide POSTFIELDSIZE, libcurl will strlen() by
       itself */
    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    curl_slist_free_all(my_curl_list);
    /* Check for errors */
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return "HttpClientHandler failed";
    }
    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  return "HttpClientHandler successfully";
}

StatusOr<std::string> HttpClient::put(const std::string& path, const folly::dynamic& data) {
    return sendRequest(path, data, "PUT");
}

StatusOr<std::string> HttpClient::sendRequest(const std::string& path,
                                              const folly::dynamic& data,
                                              const std::string& reqType) {
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    FILE *f;
    f = fopen("tempput.txt", "w");
    if (curl) {
        struct curl_slist *headers = nullptr;
        std::string mydata = folly::toJson(data);
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string jsonObj = "{\"int64_test\":20}";
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, reqType.c_str());
        curl_easy_setopt(curl, CURLOPT_URL, path.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, mydata.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        res = curl_easy_perform(curl);
        fclose(f);
        f = fopen("tempput.txt", "rw");
        Cord out;
        char buf[1024];
        size_t len;
        do {
            len = fread(buf, 1, 1024, f);
            if (len > 0) {
                out.write(buf, len);
            }
        } while (len == 1024);
        fclose(f);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
        }
        StatusOr<std::string> result = out.str();
        if (result.ok()) {
            std:: cout<< "ok!!!!!!" << std::endl;
            return result.value();
        } else {
            return Status::Error(folly::stringPrintf("Http Get Failed: %s", path.c_str()));
        }
    }
    std::cout << "libcurl failed" << std::endl;
    return "libcurl failed";
}
}   // namespace http
}   // namespace nebula
