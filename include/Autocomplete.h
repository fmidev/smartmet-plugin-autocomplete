#pragma once

#include "ProductParameters.h"

#include <engines/geonames/Engine.h>
#include <engines/geoip/Engine.h>
#include <engines/querydata/Engine.h>
#include <engines/sputnik/Engine.h>
#include <spine/SmartMetPlugin.h>
#include <spine/Reactor.h>
#include <spine/HTTP.h>

#include <libconfig.h++>

#include <set>
#include <string>

namespace SmartMet
{
class FmiNames;
class GeoIPEngine;

namespace Plugin
{
namespace Autocomplete
{
class Autocomplete
{
 public:
  void requestHandler(SmartMet::Spine::Reactor& theReactor,
                      const SmartMet::Spine::HTTP::Request& theRequest,
                      SmartMet::Spine::HTTP::Response& theResponse);

  void complete(const SmartMet::Spine::HTTP::Request& theRequest,
                SmartMet::Spine::HTTP::Response& theResponse);

  // Thread for initializing the whole search system
  void init();

  // Constructor and destructor
  //
  Autocomplete(SmartMetPlugin* theParent,
               SmartMet::Spine::Reactor* theReactor,
               const char* theConfig);
  ~Autocomplete();

 private:
  SmartMet::Engine::Querydata::Engine* itsQEngine;   // does not own
  SmartMet::Engine::Geonames::Engine* itsGeoEngine;  // does not own
  SmartMet::Engine::Geoip::Engine* itsGeoIPEngine;   // does not own
  SmartMetPlugin* itsParent;                         // does not own
  libconfig::Config itsConfig;
  Reactor* itsReactor;  // does not own
  const char* itsConfigFile;

  ProductParameters itsProductParameters;
};

}  // namespace Autocomplete
}  // namespace Plugin
}  // namespace SmartMet
