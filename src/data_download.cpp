

size_t write_data_1(void* buffer, size_t size, size_t nmemb, void* userp){

    size_t real_size = size * nmemb;
    auto file = static_cast<std::ofstream*>(userp);
    file->write(reinterpret_cast<char*>(buffer), real_size);

    return real_size;


}

void save_to_file1(CURL* curl){

    static std::ofstream output("/Users/davidrachinsky/the_workspace/realtime_transit/build/TripUpdates.pb", std::ios::binary);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data_1);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, reinterpret_cast<void*>(&output));
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
}

size_t write_data_2(void* buffer, size_t size, size_t nmemb, void* userp){

    size_t real_size = size * nmemb;
    auto file = static_cast<std::ofstream*>(userp);
    file->write(reinterpret_cast<char*>(buffer), real_size);
    return real_size;

}

void save_to_file2(CURL* curl){

    static std::ofstream output("/Users/davidrachinsky/the_workspace/realtime_transit/build/VehiclePositions.pb", std::ios::binary);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data_2);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, reinterpret_cast<void*>(&output));
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);


}

{

    //curl_global_init(CURL_GLOBAL_ALL);

    /*if(argc < 2){
        std::cerr << "Usage: " << "GTFS_FEED" << std::endl;
        return -1;
    }*/

    /*CURL* curl_1 = curl_easy_init();
    CURL* curl_2 = curl_easy_init();

    curl_easy_setopt(curl_1, CURLOPT_URL, "http://gtfs.edmonton.ca/TMGTFSRealTimeWebService/TripUpdate/TripUpdates.pb");
    curl_easy_setopt(curl_2, CURLOPT_URL, "http://gtfs.edmonton.ca/TMGTFSRealTimeWebService/Vehicle/VehiclePositions.pb");

    save_to_file1(curl_1);
    save_to_file2(curl_2);

    CURLcode res1 = curl_easy_perform(curl_1);
    CURLcode res2 = curl_easy_perform(curl_2);

    curl_easy_cleanup(curl_1);
    curl_easy_cleanup(curl_2);*/

}