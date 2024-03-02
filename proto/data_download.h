#ifndef DATA_DOWNLOAD_H
#define DATA_DOWNLOAD_H

#include<curl/curl.h>
#include<iostream>
#include<fstream>
#include<thread>
#include<memory>
#include<vector>
#include<string>
#include<chrono>
#include<mutex>


class DataDownload{

    public:

        DataDownload();
        ~DataDownload();

        void download();
        void initialize();
        void launch();

        void SetWriteDataObj1(std::ofstream* output);
        void SetWriteDataObj2(std::ofstream* output);


        bool operating{true};
        bool transmission_complete{true};


        

    private:

        void save_to_file1(CURL* curl);
        void save_to_file2(CURL* curl);
        static size_t write_data_1(void* buffer, size_t size, size_t nmemb, void* userp);
        static size_t write_data_2(void* buffer, size_t size, size_t nmemb, void* userp);

        CURL* curl_1;
        CURL* curl_2;
        CURLcode res_1;
        CURLcode res_2;

        std::mutex _writer_lock;

        std::vector<std::thread> _curl_thread;

        const char* url_trip = "http://gtfs.edmonton.ca/TMGTFSRealTimeWebService/TripUpdate/TripUpdates.pb";
        const char* url_vehicle = "http://gtfs.edmonton.ca/TMGTFSRealTimeWebService/Vehicle/VehiclePositions.pb";





};



#endif