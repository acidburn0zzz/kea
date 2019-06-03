// Copyright (C) 2018-2019 Internet Systems Consortium, Inc. ("ISC")
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef STAMPED_ELEMENT_H
#define STAMPED_ELEMENT_H

#include <cc/data.h>
#include <cc/server_tag.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scoped_ptr.hpp>
#include <cstdint>
#include <string>

namespace isc {
namespace data {

/// @brief This class represents configuration element which is
/// associated with database identifier and the modification
/// timestamp.
///
/// Classes storing Kea configuration should derive from this object
/// to track ids and modification times of the configuration objects.
/// This is specifically required by the Kea Configuration Backend
/// feature which stores and fetches configuration from the database.
/// The configuration elements must be accessible by their database
/// identifiers and modification times.
///
/// @note This class is not derived from @c Element and should not
/// be confused with the classes being derived from @c Element class.
/// Those classes are used to represent JSON structures, whereas this
/// class represents data fetched from the database.
///
/// @todo Find a better name for @c StamepedElement.
class StampedElement {
public:

    /// @brief Constructor.
    ///
    /// Sets timestamp to the current time.
    StampedElement();

    /// @brief Sets element's database identifier.
    ///
    /// @param id New id.
    void setId(const uint64_t id) {
        id_ = id;
    }

    /// @brief Returns element's database identifier.
    uint64_t getId() const {
        return (id_);
    }

    /// @brief Sets timestamp to the explicitly provided value.
    ///
    /// @param timestamp New timestamp value.
    void setModificationTime(const boost::posix_time::ptime& timestamp) {
        timestamp_ = timestamp;
    }

    /// @brief Sets timestmp to the current time.
    void updateModificationTime();

    /// @brief Returns timestamp.
    boost::posix_time::ptime getModificationTime() const {
        return (timestamp_);
    }

    /// @brief Sets new server tag.
    ///
    /// @param server_tag new server tag.
    /// @throw BadValue if the server tag length exceeds 256 characters.
    void setServerTag(const std::string& server_tag) {
        server_tag_ = ServerTag(server_tag);
    }

    /// @brief Returns server tag.
    ///
    /// @return Server tag as string.
    std::string getServerTag() const;

    /// @brief Checks if the stamped element is for all servers.
    ///
    /// @return true if the stamped element is associated with all servers,
    /// false otherwise.
    bool allServers() const;

    /// @brief Returns an object representing metadata to be returned
    /// with objects from the configuration backend.
    ///
    /// @return Pointer to the metadata element.
    isc::data::ElementPtr getMetadata() const;

private:

    /// @brief Database identifier of the configuration element.
    ///
    /// The default value of 0 indicates that the identifier is
    /// not set. 
    uint64_t id_;

    /// @brief Holds timestamp value.
    boost::posix_time::ptime timestamp_;

    /// @brief Holds server tag.
    ServerTag server_tag_;
};

} // end of namespace isc::data
} // end of namespace isc

#endif
