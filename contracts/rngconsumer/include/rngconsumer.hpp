// Telos Random is...
//
// @author Craig Branscom
// @contract requestor
// @version v0.1.0

#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/crypto.hpp>

using namespace std;
using namespace eosio;

static constexpr eosio::name ORACLE_CONTRACT = name("rng.oracle");

CONTRACT rngconsumer: public contract {

public:

    rngconsumer(name self, name code, datastream<const char*> ds) : contract(self, code, ds) {};

    ~rngconsumer() {};

    //request a random value
    ACTION requestrand(uint64_t seed, const name& caller, uint64_t min, uint64_t max);

    ACTION receiverand(uint64_t caller_id, checksum256 random);

    ACTION randreceipt(uint64_t request_id, name caller, uint64_t number);

    ACTION delrequest(uint64_t request_id);

    //TODO: other types of requests

    //======================== contract tables ========================

    //request entry
    //scope: self
    TABLE rngrequest {
        uint64_t request_id;
        uint64_t min;
        uint64_t max;
        name caller;

        uint64_t primary_key() const { return request_id; }
        EOSLIB_SERIALIZE(rngrequest, (request_id)(caller)(min)(max))
    };
    typedef multi_index<name("rngrequests"), rngrequest> rngrequests_table;

};