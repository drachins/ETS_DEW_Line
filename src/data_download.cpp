
#include "data_download.h"

DataDownload::DataDownload(){};

void DataDownload::launch(){

    std::cout << "echo0" << std::endl;
    _curl_thread.emplace_back(&DataDownload::run, this);

}

void DataDownload::run(){


    std::cout << "echo1" << std::endl;

    curl_global_init(CURL_GLOBAL_ALL);   

    std::cout << "echo2" << std::endl;

    curl_1 = curl_easy_init();
    curl_2 = curl_easy_init();

    std::cout << "echo3" << std::endl;    

    curl_easy_setopt(curl_1, CURLOPT_URL, "http://gtfs.edmonton.ca/TMGTFSRealTimeWebService/TripUpdate/TripUpdates.pb");
    std::cout << "echo7" << std::endl;   
    curl_easy_setopt(curl_2, CURLOPT_URL, "http://gtfs.edmonton.ca/TMGTFSRealTimeWebService/Vehicle/VehiclePositions.pb");

    std::cout << "echo4" << std::endl;   

    static std::ofstream output_1(filepath_trip, std::ostream::binary);
    curl_easy_setopt(curl_1, CURLOPT_WRITEFUNCTION, &DataDownload::write_data_1);
    curl_easy_setopt(curl_1, CURLOPT_WRITEDATA, reinterpret_cast<void*>(&output_1));
    curl_easy_setopt(curl_1, CURLOPT_FOLLOWLOCATION, 1L);

    std::cout << "echo5" << std::endl;   

    static std::ofstream output_2(filepath_vehicle, std::ostream::binary);
    curl_easy_setopt(curl_2, CURLOPT_WRITEFUNCTION, &DataDownload::write_data_2);
    curl_easy_setopt(curl_2, CURLOPT_WRITEDATA, reinterpret_cast<void*>(&output_2));
    curl_easy_setopt(curl_2, CURLOPT_FOLLOWLOCATION, 1L);

    std::cout << "echo6" << std::endl;   

    while(operating){

        std::cout << "echo8" << std::endl;   
        res_1 = curl_easy_perform(curl_1);
        std::cout << "echo9" << std::endl;  
        res_2 = curl_easy_perform(curl_2);
 

        transmission_complete = true;

        std::this_thread::sleep_for(std::chrono::milliseconds(30000));

        transmission_complete = false;

    }




}

size_t DataDownload::write_data_1(void* buffer, size_t size, size_t nmemb, void* userp){

    std::cout << "echo10" << std::endl;
    size_t real_size = size * nmemb;
    std::cout << size << " " << nmemb << std::endl;
    //auto file = reinterpret_cast<std::ofstream*>(userp);
    std::cout << "echo11" << std::endl;
    std::cout << "echo12" << std::endl;
    char* message = reinterpret_cast<char*>(buffer);
    std::ofstream file(filepath_trip, std::ios::binary);
    file.write(message, real_size);


    return real_size;


}


size_t DataDownload::write_data_2(void* buffer, size_t size, size_t nmemb, void* userp){

    std::cout << "echo13" << std::endl;
    size_t real_size = size * nmemb;
    //auto file = reinterpret_cast<std::ofstream*>(userp);
    std::ofstream file(filepath_vehicle, std::ios::binary);
    std::cout << "echo14" << std::endl;
    file.write(reinterpret_cast<char*>(buffer), real_size);
    return real_size;

}

DataDownload::~DataDownload(){

    curl_easy_cleanup(curl_1);
    curl_easy_cleanup(curl_2);

    _curl_thread[0].join();

}