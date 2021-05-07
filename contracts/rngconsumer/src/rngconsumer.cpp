#include "../include/rngconsumer.hpp";

// using namespace requestor;

//======================== request actions ========================
ACTION rngconsumer::randreceipt(uint64_t request_id, name caller, uint64_t number) {
    require_auth(get_self());
}

ACTION rngconsumer::delrequest(uint64_t request_id) {
    require_auth(get_self());
    rngrequests_table rngrequests(get_self(), get_self().value);
    auto req_itr = rngrequests.find(request_id); 
    check(req_itr != rngrequests.end(), "Unable to find request");
    rngrequests.erase(req_itr);    
}

ACTION rngconsumer::requestrand(uint64_t seed, const name& caller, uint64_t min, uint64_t max) {
    //validate
    require_auth(caller);

    //open requests table, find request
    rngrequests_table rngrequests(get_self(), get_self().value);

    auto request_id = rngrequests.available_primary_key();

    action(
        {get_self(), "active"_n}, 
        ORACLE_CONTRACT, "requestrand"_n,
        std::tuple(request_id, seed, get_self()))
    .send();

    //emplace new request
    rngrequests.emplace(caller, [&](auto& col) {
        col.request_id = request_id;
        col.min = min;
        col.max = max;
        col.caller = caller;
    });

}

ACTION rngconsumer::receiverand(uint64_t caller_id, checksum256 random) {
    require_auth(ORACLE_CONTRACT);
    rngrequests_table rngrequests(get_self(), get_self().value);
    auto req_itr = rngrequests.find(caller_id);
    if (req_itr != rngrequests.end()) {
        auto byte_array = random.extract_as_byte_array();

        uint64_t random_int = 0;
        for (int i = 0; i < 8; i++) {
            random_int <<= 8;
            random_int |= (uint64_t)byte_array[i];
        }

        auto min = req_itr->min;
        auto max = req_itr->max;

        uint64_t number = min + ( random_int % ( max - min + 1 ) );

        action(
            {get_self(), "active"_n}, 
            get_self(), "randreceipt"_n,
            std::tuple(req_itr->request_id, req_itr->caller, number))
        .send();

        rngrequests.erase(req_itr);
    }
}