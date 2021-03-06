// ======================================================================
/*!
 * \brief SmartMet Autocomplete plugin interface
 */
// ======================================================================

#pragma once

#include "Autocomplete.h"
#include <boost/move/unique_ptr.hpp>
#include <spine/HTTP.h>
#include <spine/Reactor.h>
#include <spine/SmartMetPlugin.h>

namespace SmartMet
{
namespace Plugin
{
namespace Autocomplete
{
class Plugin : public SmartMetPlugin
{
 public:
  Plugin(Spine::Reactor* theReactor, const char* theConfig);
  virtual ~Plugin() = default;

  const std::string& getPluginName() const;
  int getRequiredAPIVersion() const;
  bool queryIsFast(const Spine::HTTP::Request& theRequest) const;

 protected:
  void init();
  void shutdown();
  void requestHandler(Spine::Reactor& theReactor,
                      const Spine::HTTP::Request& theRequest,
                      Spine::HTTP::Response& theResponse);

 private:
  boost::atomic<bool> itsShutdownRequested;
  Spine::Reactor* itsReactor;
  const char* itsConfig;
  boost::movelib::unique_ptr<Autocomplete> itsAutocomplete;
  const std::string itsModuleName;

};  // class Plugin

}  // namespace Autocomplete
}  // namespace Plugin
}  // namespace SmartMet

// ======================================================================
