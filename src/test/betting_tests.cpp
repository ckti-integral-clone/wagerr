// Copyright (c) 2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "betting/bet.h"
#include "random.h"
#include "uint256.h"
#include "test/test_wagerr.h"
#include <cstdlib>
#include <cstdio>

#include <vector>
#include <map>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem/operations.hpp>

static constexpr auto nMassRecordCount{333}; // must be greater then MaxReorganizationDepth

BOOST_FIXTURE_TEST_SUITE(betting_tests, TestingSetup)

BOOST_AUTO_TEST_CASE(betting_mappings_test)
{
    boost::filesystem::remove_all(CMappingsDB::GetDbName());
    int blockHeight{0};
    CMapping mapping{};
    CMappingsDB db{};
    MappingsIndex mappingsIndex{};

    BOOST_CHECK(!db.Read(teamMapping, mappingsIndex, blockHeight));
    BOOST_CHECK(mappingsIndex.size() == 0);
    BOOST_CHECK(!db.Read(sportMapping, mappingsIndex, blockHeight));
    BOOST_CHECK(mappingsIndex.size() == 0);
    BOOST_CHECK(!db.Read(roundMapping, mappingsIndex, blockHeight));
    BOOST_CHECK(mappingsIndex.size() == 0);
    BOOST_CHECK(!db.Read(tournamentMapping, mappingsIndex, blockHeight));
    BOOST_CHECK(mappingsIndex.size() == 0);

    mapping.nId = 1;
    mapping.nMType = teamMapping;
    mapping.sName = "Team1";
    mapping.nVersion = PROTOCOL_VERSION;
    BOOST_CHECK(db.Save(mapping, blockHeight));
    BOOST_CHECK(db.Read(teamMapping, mappingsIndex, blockHeight));
    BOOST_CHECK_EQUAL(mappingsIndex.size(), 1);
    BOOST_CHECK_EQUAL(mappingsIndex.count(mapping.nId), 1);
    BOOST_CHECK(!db.Read(sportMapping, mappingsIndex, blockHeight));
    BOOST_CHECK_EQUAL(mappingsIndex.size(), 1);
    BOOST_CHECK_EQUAL(mappingsIndex.count(mapping.nId), 1);
    BOOST_CHECK(!db.Read(roundMapping, mappingsIndex, blockHeight));
    BOOST_CHECK_EQUAL(mappingsIndex.size(), 1);
    BOOST_CHECK_EQUAL(mappingsIndex.count(mapping.nId), 1);
    BOOST_CHECK(!db.Read(tournamentMapping, mappingsIndex, blockHeight));
    BOOST_CHECK_EQUAL(mappingsIndex.size(), 1);
    BOOST_CHECK_EQUAL(mappingsIndex.count(mapping.nId), 1);

    mapping.nId = 2;
    mapping.nMType = sportMapping;
    mapping.sName = "Sport1";
    mapping.nVersion = PROTOCOL_VERSION;
    BOOST_CHECK(db.Save(mapping, blockHeight));
    BOOST_CHECK(db.Read(teamMapping, mappingsIndex, blockHeight));
    BOOST_CHECK_EQUAL(mappingsIndex.size(), 1);
    BOOST_CHECK_EQUAL(mappingsIndex.count(mapping.nId), 0);
    BOOST_CHECK(db.Read(sportMapping, mappingsIndex, blockHeight));
    BOOST_CHECK_EQUAL(mappingsIndex.size(), 2);
    BOOST_CHECK_EQUAL(mappingsIndex.count(mapping.nId), 1);
    BOOST_CHECK(!db.Read(roundMapping, mappingsIndex, blockHeight));
    BOOST_CHECK_EQUAL(mappingsIndex.size(), 2);
    BOOST_CHECK_EQUAL(mappingsIndex.count(mapping.nId), 1);
    BOOST_CHECK(!db.Read(tournamentMapping, mappingsIndex, blockHeight));
    BOOST_CHECK_EQUAL(mappingsIndex.size(), 2);
    BOOST_CHECK_EQUAL(mappingsIndex.count(mapping.nId), 1);

    mapping.nId = 4;
    mapping.nMType = roundMapping;
    mapping.sName = "Round1";
    mapping.nVersion = PROTOCOL_VERSION;
    BOOST_CHECK(db.Save(mapping, blockHeight));
    BOOST_CHECK(db.Read(teamMapping, mappingsIndex, blockHeight));
    BOOST_CHECK_EQUAL(mappingsIndex.size(), 2);
    BOOST_CHECK_EQUAL(mappingsIndex.count(mapping.nId), 0);
    BOOST_CHECK(db.Read(sportMapping, mappingsIndex, blockHeight));
    BOOST_CHECK_EQUAL(mappingsIndex.size(), 2);
    BOOST_CHECK_EQUAL(mappingsIndex.count(mapping.nId), 0);
    BOOST_CHECK(db.Read(roundMapping, mappingsIndex, blockHeight));
    BOOST_CHECK_EQUAL(mappingsIndex.size(), 3);
    BOOST_CHECK_EQUAL(mappingsIndex.count(mapping.nId), 1);
    BOOST_CHECK(!db.Read(tournamentMapping, mappingsIndex, blockHeight));
    BOOST_CHECK_EQUAL(mappingsIndex.size(), 3);
    BOOST_CHECK_EQUAL(mappingsIndex.count(mapping.nId), 1);

    mapping.nId = 8;
    mapping.nMType = tournamentMapping;
    mapping.sName = "Tournament1";
    mapping.nVersion = PROTOCOL_VERSION;
    BOOST_CHECK(db.Save(mapping, blockHeight));
    BOOST_CHECK(db.Read(teamMapping, mappingsIndex, blockHeight));
    BOOST_CHECK_EQUAL(mappingsIndex.size(), 3);
    BOOST_CHECK_EQUAL(mappingsIndex.count(mapping.nId), 0);
    BOOST_CHECK(db.Read(sportMapping, mappingsIndex, blockHeight));
    BOOST_CHECK_EQUAL(mappingsIndex.size(), 3);
    BOOST_CHECK_EQUAL(mappingsIndex.count(mapping.nId), 0);
    BOOST_CHECK(db.Read(roundMapping, mappingsIndex, blockHeight));
    BOOST_CHECK_EQUAL(mappingsIndex.size(), 3);
    BOOST_CHECK_EQUAL(mappingsIndex.count(mapping.nId), 0);
    BOOST_CHECK(db.Read(tournamentMapping, mappingsIndex, blockHeight));
    BOOST_CHECK_EQUAL(mappingsIndex.size(), 4);
    BOOST_CHECK_EQUAL(mappingsIndex.count(mapping.nId), 1);
}

BOOST_AUTO_TEST_CASE(betting_events_test)
{
    boost::filesystem::remove_all(CEventsDB::GetDbName());
    int blockHeight{0};
    CEventsDB db{};
    EventsIndex eventsIndex{};
    CPeerlessEvent plEvent{};

    BOOST_CHECK(!db.Read(eventsIndex, blockHeight));

    plEvent.nEventId = 1;
    BOOST_CHECK(db.Save(plEvent, blockHeight));
    BOOST_CHECK(db.Read(eventsIndex, blockHeight));
    BOOST_CHECK(eventsIndex.size() == 1);
    BOOST_CHECK(eventsIndex.count(1) == 1);
}

BOOST_AUTO_TEST_CASE(betting_results_test)
{
    boost::filesystem::remove_all(CResultsDB::GetDbName());
    int blockHeight{0};
    CResultsDB db{};
    ResultsIndex resultsIndex{};
    CPeerlessResult plResult{};

    BOOST_CHECK(!db.Read(resultsIndex, blockHeight));

    plResult.nEventId = 1;
    BOOST_CHECK(db.Save(plResult, blockHeight));
    BOOST_CHECK(db.Read(resultsIndex, blockHeight));
    BOOST_CHECK(resultsIndex.size() == 1);
    BOOST_CHECK(resultsIndex.count(1) == 1);
}

BOOST_AUTO_TEST_CASE(betting_massmappings_test)
{
    boost::filesystem::remove_all(CMappingsDB::GetDbName());
    std::srand(std::time(nullptr));
    int nMappingCount{0};
    int blockHeight{0};

    for (auto i{0}; i < 2; i++) {
        CMappingsDB db{};
        CMapping mapping{};
        MappingsIndex mappingsIndex{};

        if (i == 0) {
            nMappingCount = nMassRecordCount + std::rand() % nMassRecordCount;
            for (auto j{static_cast<int>(sportMapping)}; j <= tournamentMapping; j++) {
                const auto mappingType{static_cast<MappingTypes>(j)};
                BOOST_CHECK(!db.Read(mappingType, mappingsIndex, blockHeight));
                for (auto k{0}; k < nMappingCount; k++) {
                    mapping.nId = k;
                    mapping.nMType = mappingType;
                    mapping.nVersion = PROTOCOL_VERSION;
                    BOOST_CHECK(db.Save(mapping, blockHeight));
                }
            }

        } else {
            for (auto j{static_cast<int>(sportMapping)}; j <= tournamentMapping; j++) {
                const auto mappingType{static_cast<MappingTypes>(j)};
                mappingsIndex.clear();
                for (auto k{0}; k < nMappingCount; k++) {
                    BOOST_CHECK(db.Read(mappingType, mappingsIndex, blockHeight));
                    BOOST_CHECK(mappingsIndex.count(k) == 1);
                    BOOST_CHECK(mappingsIndex[k].nId == k);
                    BOOST_CHECK(mappingsIndex[k].nMType == mappingType);
                    BOOST_CHECK(mappingsIndex[k].nVersion == PROTOCOL_VERSION);
                }
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(betting_massevents_test)
{
    boost::filesystem::remove_all(CEventsDB::GetDbName());
    std::srand(std::time(nullptr));
    int nEventCount{0};
    int blockHeight{0};

    for (auto i{0}; i < 2; i++) {
        CEventsDB db{};
        CPeerlessEvent plEvent{};
        EventsIndex eventsIndex{};

        if (i == 0) {
            nEventCount = nMassRecordCount + std::rand() % nMassRecordCount;
            BOOST_CHECK(!db.Read(eventsIndex, blockHeight));
            for (auto j{0}; j < nEventCount; j++) {
                plEvent.nEventId = j;
                plEvent.nVersion = PROTOCOL_VERSION;
                BOOST_CHECK(db.Save(plEvent, blockHeight));
            }
        } else {
            BOOST_CHECK(db.Read(eventsIndex, blockHeight));
            for (auto j{0}; j < nEventCount; j++) {
                BOOST_CHECK(eventsIndex.count(j) == 1);
                BOOST_CHECK(eventsIndex[j].nEventId == j);
                BOOST_CHECK(eventsIndex[j].nVersion == PROTOCOL_VERSION);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(betting_massresults_test)
{
    boost::filesystem::remove_all(CResultsDB::GetDbName());
    std::srand(std::time(nullptr));
    int nResultCount{0};
    int blockHeight{0};

    for (auto i{0}; i < 2; i++) {
        CResultsDB db{};
        CPeerlessResult plResult{};
        ResultsIndex resultsIndex{};

        if (i == 0) {
            nResultCount = nMassRecordCount + std::rand() % nMassRecordCount;
            BOOST_CHECK(!db.Read(resultsIndex, blockHeight));
            for (auto j{0}; j < nResultCount; j++) {
                plResult.nEventId = j;
                plResult.nVersion = PROTOCOL_VERSION;
                BOOST_CHECK(db.Save(plResult, blockHeight));
            }
        } else {
            BOOST_CHECK(db.Read(resultsIndex, blockHeight));
            for (auto j{0}; j < nResultCount; j++) {
                BOOST_CHECK(resultsIndex.count(j) == 1);
                BOOST_CHECK(resultsIndex[j].nEventId == j);
                BOOST_CHECK(resultsIndex[j].nVersion == PROTOCOL_VERSION);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(betting_massall_test)
{
    boost::filesystem::remove_all(CMappingsDB::GetDbName());
    boost::filesystem::remove_all(CEventsDB::GetDbName());
    boost::filesystem::remove_all(CResultsDB::GetDbName());

    CMappingsDB mdb{};
    CEventsDB edb{};
    CResultsDB rdb{};
    CMapping mapping{};
    CPeerlessEvent plEvent{};
    CPeerlessResult plResult{};
    MappingsIndex mappingsIndex{};
    EventsIndex eventsIndex{};
    ResultsIndex resultsIndex{};
    int blockHeight{0};

    for (auto i{0}; i < nMassRecordCount; i++) {
        for (auto j{static_cast<int>(sportMapping)}; j <= tournamentMapping; j++) {
            BOOST_CHECK(!mdb.Read(static_cast<MappingTypes>(j), mappingsIndex, blockHeight));
        }
        BOOST_CHECK(!edb.Read(eventsIndex, blockHeight));
        BOOST_CHECK(!rdb.Read(resultsIndex, blockHeight));
    }

    for (auto i{0}; i < nMassRecordCount; i++) {
        mapping.nId = i;
        mapping.nVersion = PROTOCOL_VERSION;
        plEvent.nEventId = i;
        plEvent.nVersion = PROTOCOL_VERSION;
        plResult.nEventId = i;
        plResult.nVersion = PROTOCOL_VERSION;

        for (auto j{static_cast<int>(sportMapping)}; j <= tournamentMapping; j++) {
            mapping.nMType = static_cast<MappingTypes>(j);
            BOOST_CHECK(mdb.Save(mapping, blockHeight));
        }

        BOOST_CHECK(edb.Save(plEvent, blockHeight));
        BOOST_CHECK(rdb.Save(plResult, blockHeight));
    }

    for (auto i{0}; i < nMassRecordCount; i++) {
        for (auto j{static_cast<int>(sportMapping)}; j <= tournamentMapping; j++) {
            BOOST_CHECK(mdb.Read(static_cast<MappingTypes>(j), mappingsIndex, blockHeight));
            BOOST_CHECK_EQUAL(mappingsIndex.size(), nMassRecordCount);
            BOOST_CHECK_EQUAL(mappingsIndex.count(i), 1);
        }

        BOOST_CHECK(edb.Read(eventsIndex, blockHeight));
        BOOST_CHECK(eventsIndex.size() == nMassRecordCount);
        BOOST_CHECK(eventsIndex.count(i) == 1);

        BOOST_CHECK(rdb.Read(resultsIndex, blockHeight));
        BOOST_CHECK(resultsIndex.size() == nMassRecordCount);
        BOOST_CHECK(resultsIndex.count(i) == 1);
    }
}

BOOST_AUTO_TEST_CASE(betting_blockheight_test)
{
    boost::filesystem::remove_all(CMappingsDB::GetDbName());
    boost::filesystem::remove_all(CEventsDB::GetDbName());
    boost::filesystem::remove_all(CResultsDB::GetDbName());

    CMappingsDB mdb{};
    CEventsDB edb{};
    CResultsDB rdb{};
    CMapping mapping{};
    CPeerlessEvent plEvent{};
    CPeerlessResult plResult{};
    MappingsIndex mappingsIndex{};
    EventsIndex eventsIndex{};
    ResultsIndex resultsIndex{};

    mapping.nId = 11;
    mapping.nVersion = PROTOCOL_VERSION;
    plEvent.nEventId = 42;
    plEvent.nVersion = PROTOCOL_VERSION;
    plResult.nEventId = 42;
    plResult.nVersion = PROTOCOL_VERSION;

    for (auto i{0}; i < nMassRecordCount; i++) {
        for (auto j{static_cast<int>(sportMapping)}; j <= tournamentMapping; j++) {
            mapping.nMType = static_cast<MappingTypes>(j);
            BOOST_CHECK(mdb.Save(mapping, i));
        }
        BOOST_CHECK(edb.Save(plEvent, i));
        BOOST_CHECK(rdb.Save(plResult, i));
    }

    for (auto i{0}; i < nMassRecordCount - Params().MaxReorganizationDepth() - 1; i++) {
        for (auto j{static_cast<int>(sportMapping)}; j <= tournamentMapping; j++) {
            BOOST_CHECK(!mdb.Read(static_cast<MappingTypes>(j), mappingsIndex, i));
        }
        BOOST_CHECK(!edb.Read(eventsIndex, i));
        BOOST_CHECK(!rdb.Read(resultsIndex, i));
    }

    for (auto i{nMassRecordCount - Params().MaxReorganizationDepth() - 1}; i < nMassRecordCount; i++) {
        for (auto j{static_cast<int>(sportMapping)}; j <= tournamentMapping; j++) {
            BOOST_CHECK(mdb.Read(static_cast<MappingTypes>(j), mappingsIndex, i));
            BOOST_CHECK_EQUAL(mappingsIndex.size(), 1);
            BOOST_CHECK_EQUAL(mappingsIndex.count(11), 1);
        }

        BOOST_CHECK(edb.Read(eventsIndex, i));
        BOOST_CHECK(eventsIndex.size() == 1);
        BOOST_CHECK(eventsIndex.count(42) == 1);

        BOOST_CHECK(rdb.Read(resultsIndex, i));
        BOOST_CHECK(resultsIndex.size() == 1);
        BOOST_CHECK(resultsIndex.count(42) == 1);
    }

    for (auto i{static_cast<int>(sportMapping)}; i <= tournamentMapping; i++) {
        BOOST_CHECK(mdb.Read(static_cast<MappingTypes>(i), mappingsIndex, nMassRecordCount * nMassRecordCount));
    }
    BOOST_CHECK(edb.Read(eventsIndex, nMassRecordCount * nMassRecordCount));
    BOOST_CHECK(rdb.Read(resultsIndex, nMassRecordCount * nMassRecordCount));
}

BOOST_AUTO_TEST_SUITE_END()
