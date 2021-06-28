#pragma once

#include "ProductParameters.h"
#include <engines/geonames/Engine.h>
#include <engines/querydata/Engine.h>
#include <engines/sputnik/Engine.h>
#include <spine/HTTP.h>
#include <spine/Reactor.h>
#include <spine/SmartMetPlugin.h>
#include <libconfig.h++>
#include <set>
#include <string>

namespace SmartMet
{
class FmiNames;

namespace Plugin
{
namespace Autocomplete
{
class Autocomplete
{
 public:
  void requestHandler(Spine::Reactor& theReactor,
                      const Spine::HTTP::Request& theRequest,
                      Spine::HTTP::Response& theResponse);

  void complete(const Spine::HTTP::Request& theRequest, Spine::HTTP::Response& theResponse);

  // Thread for initializing the whole search system
  void init();

  void shutdown();

  // Constructor and destructor
  //
  Autocomplete(SmartMetPlugin* theParent, Spine::Reactor* theReactor, const char* theConfig);
  ~Autocomplete();

 private:
  std::atomic<bool> itsShutdownRequested;
  Engine::Querydata::Engine* itsQEngine = nullptr;   // does not own
  Engine::Geonames::Engine* itsGeoEngine = nullptr;  // does not own
  SmartMetPlugin* itsParent = nullptr;               // does not own
  libconfig::Config itsConfig;
  Spine::Reactor* itsReactor;  // does not own
  const char* itsConfigFile;

  std::string itsDefaultLanguage;   // default translation language
  std::string itsDefaultLocale;     // default locale for location name translations
  bool itsPrettyPrintFlag = false;  // pretty printing off by default
  std::string itsTimeFormat;        // iso, sql, xml, epoch, timestamp, http
  int itsMaxResults = 0;            // names per page, default page is always zero

  ProductParameters itsProductParameters;
};

}  // namespace Autocomplete
}  // namespace Plugin
}  // namespace SmartMet
