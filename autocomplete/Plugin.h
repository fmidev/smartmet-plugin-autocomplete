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

  const std::string& getPluginName() const override;
  int getRequiredAPIVersion() const override;
  bool queryIsFast(const Spine::HTTP::Request& theRequest) const override;

 protected:
  void init() override;
  void shutdown() override;
  void requestHandler(Spine::Reactor& theReactor,
                      const Spine::HTTP::Request& theRequest,
                      Spine::HTTP::Response& theResponse) override;

 private:
  Spine::Reactor* itsReactor;
  const char* itsConfig;
  std::unique_ptr<Autocomplete> itsAutocomplete;
  const std::string itsModuleName;

};  // class Plugin

}  // namespace Autocomplete
}  // namespace Plugin
}  // namespace SmartMet

// ======================================================================
