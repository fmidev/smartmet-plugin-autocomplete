// ======================================================================
/*!
 * \brief SmartMet Autocomplete plugin interface
 */
// ======================================================================

#pragma once

#include <spine/SmartMetPlugin.h>
#include <spine/Reactor.h>
#include <spine/HTTP.h>

#include "Autocomplete.h"

namespace SmartMet
{
namespace Plugin
{
namespace Autocomplete
{
class Plugin : public SmartMetPlugin
{
 public:
  Plugin(Reactor* theReactor, const char* theConfig);
  virtual ~Plugin();

  const std::string& getPluginName() const;
  int getRequiredAPIVersion() const;
  bool queryIsFast(const SmartMet::Spine::HTTP::Request& theRequest) const;

 protected:
  void init();
  void shutdown();
  void requestHandler(SmartMet::Spine::Reactor& theReactor,
                      const HTTP::Request& theRequest,
                      HTTP::Response& theResponse);

 private:
  SmartMet::Spine::Reactor* itsReactor;
  const char* itsConfig;
  Autocomplete* itsAutocomplete;
  const std::string itsModuleName;

};  // class Plugin

}  // namespace Autocomplete
}  // namespace Plugin
}  // namespace SmartMet

// ======================================================================
