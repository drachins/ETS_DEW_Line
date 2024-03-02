
#include "data_download.h"

DataDownload::DataDownload(){};

void DataDownload::launch(){

    _curl_thread.emplace_back(&DataDownload::download, this);

}

void DataDownload::initialize(){

    curl_global_init(CURL_GLOBAL_ALL);   

    curl_1 = curl_easy_init();
    curl_2 = curl_easy_init();

    curl_easy_setopt(curl_1, CURLOPT_URL, url_trip);
    curl_easy_setopt(curl_2, CURLOPT_URL, url_vehicle);


    //static std::ofstream output_1;
    curl_easy_setopt(curl_1, CURLOPT_WRITEFUNCTION, DataDownload::write_data_1);
    //curl_easy_setopt(curl_1, CURLOPT_WRITEDATA, reinterpret_cast<void*>(&output_1));
    curl_easy_setopt(curl_1, CURLOPT_FOLLOWLOCATION, 1L);


    //static std::ofstream output_2;
    curl_easy_setopt(curl_2, CURLOPT_WRITEFUNCTION, DataDownload::write_data_2);
    //curl_easy_setopt(curl_2, CURLOPT_WRITEDATA, reinterpret_cast<void*>(&output_2));
    curl_easy_setopt(curl_2, CURLOPT_FOLLOWLOCATION, 1L);

}

void DataDownload::SetWriteDataObj1(std::ofstream* output){

    curl_easy_setopt(curl_1, CURLOPT_WRITEDATA, reinterpret_cast<void*>(output));

}

void DataDownload::SetWriteDataObj2(std::ofstream* output){
    
    curl_easy_setopt(curl_2, CURLOPT_WRITEDATA, reinterpret_cast<void*>(output));

}


void DataDownload::download(){

    transmission_complete = false;

    res_1 = curl_easy_perform(curl_1);
    res_2 = curl_easy_perform(curl_2);
 
    transmission_complete = true;


}
    

size_t DataDownload::write_data_1(void* buffer, size_t size, size_t nmemb, void* userp){

    size_t real_size = size * nmemb;
    auto file = reinterpret_cast<std::ofstream*>(userp);
    file->write(reinterpret_cast<char*>(buffer), real_size);

    return real_size;


}


size_t DataDownload::write_data_2(void* buffer, size_t size, size_t nmemb, void* userp){

    size_t real_size = size * nmemb;
    auto file = reinterpret_cast<std::ofstream*>(userp);
    file->write(reinterpret_cast<char*>(buffer), real_size);

    return real_size;

}

DataDownload::~DataDownload(){

    curl_easy_cleanup(curl_1);
    curl_easy_cleanup(curl_2);

    _curl_thread[0].join();

}