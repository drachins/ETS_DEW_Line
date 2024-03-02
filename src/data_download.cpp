
#include "data_download.h"

DataDownload::DataDownload(){};

/*void DataDownload::launch(){

    _curl_thread.emplace_back(&DataDownload::run, this);

}*/

void DataDownload::initialize(){

    curl_global_init(CURL_GLOBAL_ALL);   

    CURL* curl_1 = curl_easy_init();
    CURL* curl_2 = curl_easy_init();

    curl_easy_setopt(curl_1, CURLOPT_URL, "http://gtfs.edmonton.ca/TMGTFSRealTimeWebService/TripUpdate/TripUpdates.pb");
    curl_easy_setopt(curl_2, CURLOPT_URL, "http://gtfs.edmonton.ca/TMGTFSRealTimeWebService/Vehicle/VehiclePositions.pb");


    static std::ofstream output_1(filepath_trip, std::ostream::binary);
    curl_easy_setopt(curl_1, CURLOPT_WRITEFUNCTION, &DataDownload::write_data_1);
    curl_easy_setopt(curl_1, CURLOPT_WRITEDATA, reinterpret_cast<void*>(&output_1));
    curl_easy_setopt(curl_1, CURLOPT_FOLLOWLOCATION, 1L);


    static std::ofstream output_2(filepath_vehicle, std::ostream::binary);
    curl_easy_setopt(curl_2, CURLOPT_WRITEFUNCTION, &DataDownload::write_data_2);
    curl_easy_setopt(curl_2, CURLOPT_WRITEDATA, reinterpret_cast<void*>(&output_2));
    curl_easy_setopt(curl_2, CURLOPT_FOLLOWLOCATION, 1L);

}


void DataDownload::download(){

    transmission_complete = false;

    res_1 = curl_easy_perform(curl_1);
    res_2 = curl_easy_perform(curl_2);
 
    transmission_complete = true;


}
    






size_t DataDownload::write_data_1(void* buffer, size_t size, size_t nmemb, void* userp){

    std::lock_guard(_writer_lock);
    size_t real_size = size * nmemb;
    //auto file = reinterpret_cast<std::ofstream*>(userp);
    char* message = reinterpret_cast<char*>(buffer);
    std::ofstream file(filepath_trip, std::ios::binary);
    file.write(message, real_size);


    return real_size;


}


size_t DataDownload::write_data_2(void* buffer, size_t size, size_t nmemb, void* userp){

    std::lock_guard(_writer_lock);
    size_t real_size = size * nmemb;
    //auto file = reinterpret_cast<std::ofstream*>(userp);
    std::ofstream file(filepath_vehicle, std::ios::binary);
    file.write(reinterpret_cast<char*>(buffer), real_size);
    return real_size;

}

DataDownload::~DataDownload(){

    curl_easy_cleanup(curl_1);
    curl_easy_cleanup(curl_2);

    //_curl_thread[0].join();

}