// Copyright (C) 2018-2019 Internet Systems Consortium, Inc. ("ISC")
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cc/stamped_element.h>

namespace isc {
namespace data {

StampedElement::StampedElement()
    /// @todo Change it to microsec_clock once we transition to subsecond
    /// precision.
    : id_(0), timestamp_(boost::posix_time::second_clock::local_time()),
      server_tag_() {
}

void
StampedElement::updateModificationTime() {
    /// @todo Change it to microsec_clock once we transition to subsecond
    /// precision.
    setModificationTime(boost::posix_time::second_clock::local_time());
}

std::string
StampedElement:: getServerTag() const {
    return (server_tag_.get());
}

bool
StampedElement::allServers() const {
    return (server_tag_.amAll());
}

ElementPtr
StampedElement::getMetadata() const {
    ElementPtr metadata = Element::createMap();
    metadata->set("server-tag", Element::create(getServerTag()));
    return (metadata);
}

} // end of namespace isc::data
} // end of namespace isc
