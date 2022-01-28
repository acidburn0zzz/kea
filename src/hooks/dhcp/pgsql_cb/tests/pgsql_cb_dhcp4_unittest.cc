// Copyright (C) 2021-2022 Internet Systems Consortium, Inc. ("ISC")
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <config.h>
#include <database/db_exceptions.h>
#include <database/server.h>
#include <dhcpsrv/cfgmgr.h>
#include <dhcpsrv/config_backend_dhcp4_mgr.h>
#include <dhcpsrv/testutils/generic_cb_dhcp4_unittest.h>
#include <dhcpsrv/testutils/generic_cb_recovery_unittest.h>
#include <dhcpsrv/testutils/pgsql_generic_backend_unittest.h>
#include <dhcpsrv/testutils/test_utils.h>
#include <pgsql_cb_dhcp4.h>
#include <pgsql/testutils/pgsql_schema.h>
#include <testutils/multi_threading_utils.h>
#include <testutils/gtest_utils.h>

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <gtest/gtest.h>
#include <map>
#include <sstream>

using namespace isc;
using namespace isc::util;
using namespace isc::asiolink;
using namespace isc::db;
using namespace isc::db::test;
using namespace isc::data;
using namespace isc::dhcp;
using namespace isc::dhcp::test;
using namespace isc::process;
using namespace isc::test;
namespace ph = std::placeholders;

namespace {

/// @brief Test implementation of the PgSQL configuration backend.
///
/// It exposes protected members of the @c PgSqlConfigBackendDHCPv4.
class TestPgSqlConfigBackendDHCPv4 : public PgSqlConfigBackendDHCPv4 {
public:

    /// @brief Constructor.
    ///
    /// @param parameters A data structure relating keywords and values
    /// concerned with the database.
    explicit TestPgSqlConfigBackendDHCPv4(const DatabaseConnection::ParameterMap& parameters)
        : PgSqlConfigBackendDHCPv4(parameters) {
    }

    using PgSqlConfigBackendDHCPv4::base_impl_;
};

/// @brief Test fixture class for @c PgSqlConfigBackendDHCPv4.
class PgSqlConfigBackendDHCPv4Test : public GenericConfigBackendDHCPv4Test {
public:
    /// @brief Constructor.
    PgSqlConfigBackendDHCPv4Test() {};

    /// @brief Destructor.
    virtual ~PgSqlConfigBackendDHCPv4Test() {};

    /// @brief Creates the PostgreSQL back end schema
    virtual void createSchema() {
        createPgSQLSchema();
    }

    /// @brief Destroys the PostgreSQL back end schema
    virtual void destroySchema() {
        destroyPgSQLSchema();
    }

    /// @brief Returns a valid PostgreSQL back end specific connection
    /// string
    std::string validConnectionString() {
        return (validPgSQLConnectionString());
    }

    /// @brief Instantiates an instance of a PostgreSQL DHCPv4 configuration
    /// back end.
    ///
    /// @params Connection parameters describing the back end to create.
    ///
    /// @return Pointer to the newly created back end instance.
    ConfigBackendDHCPv4Ptr backendFactory(db::DatabaseConnection::ParameterMap&
                                          params) {

        return (ConfigBackendDHCPv4Ptr(new TestPgSqlConfigBackendDHCPv4(params)));
    }

    /// @brief Counts rows in a selected table in PgSQL database.
    ///
    /// This method can be used to verify that some configuration elements were
    /// deleted from a selected table as a result of cascade delete or a trigger.
    /// For example, deleting a subnet should trigger deletion of its address
    /// pools and options. By counting the rows on each table we can determine
    /// whether the deletion took place on all tables for which it was expected.
    ///
    /// @param table Table name.
    /// @return Number of rows in the specified table.
    size_t countRows(const std::string& table) const {
        auto p = boost::dynamic_pointer_cast<TestPgSqlConfigBackendDHCPv4>(cbptr_);
        if (!p) {
            ADD_FAILURE() << "cbptr_ does not cast to TestPgSqlConfigBackendDHCPv4";
            return (0);
        }

        // Reuse the existing connection of the backend.
        auto impl = boost::dynamic_pointer_cast<PgSqlConfigBackendImpl>(p->base_impl_);
        auto& conn = impl->conn_;

        return (PgSqlGenericBackendTest::countRows(conn, table));
    }
};

TEST_F(PgSqlConfigBackendDHCPv4Test, getType) {
    getTypeTest("postgresql");
}

TEST_F(PgSqlConfigBackendDHCPv4Test, getHost) {
    getHostTest();
}

TEST_F(PgSqlConfigBackendDHCPv4Test, getPort) {
    getPortTest();
}

TEST_F(PgSqlConfigBackendDHCPv4Test, createUpdateDeleteServer) {
    createUpdateDeleteServerTest();
}

TEST_F(PgSqlConfigBackendDHCPv4Test, getAndDeleteAllServersTest) {
    getAndDeleteAllServersTest();
}

TEST_F(PgSqlConfigBackendDHCPv4Test, createUpdateDeleteGlobalParameter4Test) {
    createUpdateDeleteGlobalParameter4Test();
}

TEST_F(PgSqlConfigBackendDHCPv4Test, globalParameters4WithServerTagsTest) {
    globalParameters4WithServerTagsTest();
}

TEST_F(PgSqlConfigBackendDHCPv4Test, getAllGlobalParameters4Test) {
    getAllGlobalParameters4Test();
}

TEST_F(PgSqlConfigBackendDHCPv4Test, getModifiedGlobalParameters4Test) {
    getModifiedGlobalParameters4Test();
}

TEST_F(PgSqlConfigBackendDHCPv4Test, nullKeyErrorTest) {
    nullKeyErrorTest();
}

/// @brief Test fixture for verifying database connection loss-recovery
/// behavior.
class PgSqlConfigBackendDHCPv4DbLostCallbackTest : public GenericConfigBackendDbLostCallbackTest {
public:
    /// @brief Constructor
    PgSqlConfigBackendDHCPv4DbLostCallbackTest() {};

    /// @brief Destructor
    virtual ~PgSqlConfigBackendDHCPv4DbLostCallbackTest() {};

    /// @brief Creates the PostgreSQL CB schema.
    virtual void createSchema() {
        createPgSQLSchema();
    }

    /// @brief Destroys the PostgreSQL CB schema.
    virtual void destroySchema() {
        destroyPgSQLSchema();
    }

    /// @brief Method which returns a valid back end specific connection
    /// string
    virtual std::string validConnectionString() {
        return (validPgSQLConnectionString());
    }

    /// @brief Method which returns an invalid back end specific connection
    /// string.
    virtual std::string invalidConnectionString() {
        return (connectionString(PGSQL_VALID_TYPE, INVALID_NAME, VALID_HOST,
                                 VALID_USER, VALID_PASSWORD));
    }

    /// @brief Registers PostgreSQL as a CB backend type.
    virtual void registerBackendType() {
        isc::dhcp::PgSqlConfigBackendDHCPv4::registerBackendType();
    }

    /// @brief Unregisters PostgreSQL as a CB backend type.
    virtual void unregisterBackendType() {
        isc::dhcp::PgSqlConfigBackendDHCPv4::unregisterBackendType();
    }

    /// @brief Sets the IOService instance in the CB implementation object.
    ///
    /// @param io_service pointer to the IOService instance to use. It may be
    /// an empty pointer.
    virtual void setConfigBackendImplIOService(isc::asiolink::IOServicePtr io_service) {
        isc::dhcp::PgSqlConfigBackendImpl::setIOService(io_service);
    }

    /// @brief  Attempts to add a backend instance to the CB manager.
    ///
    /// @param access Connection access string containing the database
    /// connetion parameters.
    virtual void addBackend(const std::string& access) {
        ConfigBackendDHCPv4Mgr::instance().addBackend(access);
    }

    /// @brief Fetches a collection of all the servers currently in
    /// the CB database.  This function is used to check the operability
    /// of the CB backend.
    ServerCollection getAllServers() {
        return (ConfigBackendDHCPv4Mgr::instance().getPool()->getAllServers4(BackendSelector()));
    }
};

TEST_F(PgSqlConfigBackendDHCPv4DbLostCallbackTest, testNoCallbackOnOpenFailure) {
    MultiThreadingTest mt(false);
    testNoCallbackOnOpenFailure();
}

TEST_F(PgSqlConfigBackendDHCPv4DbLostCallbackTest, testNoCallbackOnOpenFailureMultiThreading) {
    MultiThreadingTest mt(true);
    testNoCallbackOnOpenFailure();
}

TEST_F(PgSqlConfigBackendDHCPv4DbLostCallbackTest, testDbLostAndRecoveredCallback) {
    MultiThreadingTest mt(false);
    testDbLostAndRecoveredCallback();
}

TEST_F(PgSqlConfigBackendDHCPv4DbLostCallbackTest, testDbLostAndRecoveredCallbackMultiThreading) {
    MultiThreadingTest mt(true);
    testDbLostAndRecoveredCallback();
}

TEST_F(PgSqlConfigBackendDHCPv4DbLostCallbackTest, testDbLostAndFailedCallback) {
    MultiThreadingTest mt(false);
    testDbLostAndFailedCallback();
}

TEST_F(PgSqlConfigBackendDHCPv4DbLostCallbackTest, testDbLostAndFailedCallbackMultiThreading) {
    MultiThreadingTest mt(true);
    testDbLostAndFailedCallback();
}

TEST_F(PgSqlConfigBackendDHCPv4DbLostCallbackTest, testDbLostAndRecoveredAfterTimeoutCallback) {
    MultiThreadingTest mt(false);
    testDbLostAndRecoveredAfterTimeoutCallback();
}

TEST_F(PgSqlConfigBackendDHCPv4DbLostCallbackTest, testDbLostAndRecoveredAfterTimeoutCallbackMultiThreading) {
    MultiThreadingTest mt(true);
    testDbLostAndRecoveredAfterTimeoutCallback();
}

TEST_F(PgSqlConfigBackendDHCPv4DbLostCallbackTest, testDbLostAndFailedAfterTimeoutCallback) {
    MultiThreadingTest mt(false);
    testDbLostAndFailedAfterTimeoutCallback();
}

TEST_F(PgSqlConfigBackendDHCPv4DbLostCallbackTest, testDbLostAndFailedAfterTimeoutCallbackMultiThreading) {
    MultiThreadingTest mt(true);
    testDbLostAndFailedAfterTimeoutCallback();
}

}
