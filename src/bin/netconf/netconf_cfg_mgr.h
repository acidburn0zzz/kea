// Copyright (C) 2018 Internet Systems Consortium, Inc. ("ISC")
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef NETCONF_CFG_MGR_H
#define NETCONF_CFG_MGR_H

#include <cc/data.h>
#include <hooks/hooks_config.h>
#include <process/d_cfg_mgr.h>
#include <boost/pointer_cast.hpp>
#include <map>
#include <string>

namespace isc {
namespace netconf {

class NetconfCfgContext;
/// @brief Pointer to a configuration context.
typedef boost::shared_ptr<NetconfCfgContext> NetconfCfgContextPtr;

/// @brief Netconf Configuration Context.
///
/// Implement the storage container for configuration context.
/// It provides a single enclosure for the storage of configuration parameters
/// and any other Netconf specific information that needs to be accessible
/// during configuration parsing as well as to the application as a whole.
/// It is derived from the context base class, DCfgContextBase.
class NetconfCfgContext : public process::DCfgContextBase {
public:

    /// @brief Default constructor
    NetconfCfgContext();

    /// @brief Creates a clone of this context object.
    ///
    /// @return A pointer to the new clone.
    virtual process::DCfgContextBasePtr clone() {
        return (process::DCfgContextBasePtr(new NetconfCfgContext(*this)));
    }

    /// @brief Returns non-const reference to configured hooks libraries.
    ///
    /// @return non-const reference to configured hooks libraries.
    isc::hooks::HooksConfig& getHooksConfig() {
        return (hooks_config_);
    }

    /// @brief Returns const reference to configured hooks libraries.
    ///
    /// @return const reference to configured hooks libraries.
    const isc::hooks::HooksConfig& getHooksConfig() const {
        return (hooks_config_);
    }

    /// @brief Unparse a configuration object
    ///
    /// Returns an element which must parse into the same object, i.e.
    /// @code
    /// for all valid config C parse(parse(C)->toElement()) == parse(C)
    /// @endcode
    ///
    /// @return a pointer to a configuration which can be parsed into
    /// the initial configuration object
    virtual isc::data::ElementPtr toElement() const;

private:

    /// @brief Private copy constructor
    ///
    /// It is private to forbid anyone outside of this class to make copies.
    /// The only legal way to copy a context is to call @ref clone().
    ///
    /// @param orig the original context to copy from
    NetconfCfgContext(const NetconfCfgContext& orig);

    /// @brief Private assignment operator to avoid potential for slicing.
    ///
    /// @param rhs Context to be assigned.
    NetconfCfgContext& operator=(const NetconfCfgContext& rhs);

    /// @brief Configured hooks libraries.
    isc::hooks::HooksConfig hooks_config_;
};

/// @brief Ctrl Netconf Configuration Manager.
///
/// Provides the mechanisms for managing the Netconf application's
/// configuration.
class NetconfCfgMgr : public process::DCfgMgrBase {
public:

    /// @brief Constructor.
    NetconfCfgMgr();

    /// @brief Destructor
    virtual ~NetconfCfgMgr();

    /// @brief Convenience method that returns the Netconf configuration
    /// context.
    ///
    /// @return returns a pointer to the configuration context.
    NetconfCfgContextPtr getNetconfCfgContext() {
        return (boost::dynamic_pointer_cast<NetconfCfgContext>(getContext()));
    }

    /// @brief Returns configuration summary in the textual format.
    ///
    /// @param selection Bitfield which describes the parts of the configuration
    /// to be returned. This parameter is ignored for Netconf.
    ///
    /// @return Summary of the configuration in the textual format.
    virtual std::string getConfigSummary(const uint32_t selection);

protected:

    /// @brief Parses configuration of Netconf.
    ///
    /// @param config Pointer to a configuration specified for netconf.
    /// @param check_only Boolean flag indicating if this method should
    /// only verify correctness of the provided configuration.
    /// @return Pointer to a result of configuration parsing.
    virtual isc::data::ConstElementPtr
    parse(isc::data::ConstElementPtr config, bool check_only);

    /// @brief Creates a new, blank NetconfCfgContext context.
    ///
    ///
    /// This method is used at the beginning of configuration process to
    /// create a fresh, empty copy of a NetconfCfgContext. This new context
    /// will be populated during the configuration process and will replace the
    /// existing context provided the configuration process completes without
    /// error.
    ///
    /// @return Returns a DCfgContextBasePtr to the new context instance.
    virtual process::DCfgContextBasePtr createNewContext();
};

/// @brief Defines a shared pointer to NetconfCfgMgr.
typedef boost::shared_ptr<NetconfCfgMgr> NetconfCfgMgrPtr;

} // namespace isc::netconf
} // namespace isc

#endif // NETCONF_CFG_MGR_H
