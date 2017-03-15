#include "Autocomplete.h"

#include <spine/Exception.h>
#include <spine/Convenience.h>
#include <spine/ParameterFactory.h>
#include <spine/Reactor.h>
#include <spine/TimeSeries.h>
#include <spine/TimeSeriesOutput.h>
#include <engines/geonames/Engine.h>

#include <macgyver/CharsetTools.h>
#include <macgyver/StringConversion.h>
#include <macgyver/TimeFormatter.h>
#include <macgyver/TimeParser.h>

#include <json/json.h>

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <map>
#include <unistd.h>

using namespace std;

using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::local_time;

namespace SmartMet
{
namespace Plugin
{
namespace Autocomplete
{
// ----------------------------------------------------------------------
/*!
 * \brief Read product parameters from the database
 */
// ----------------------------------------------------------------------

ProductParameters read_product_parameters(const libconfig::Config &theConfig)
{
  try
  {
    ProductParameters result;

    const char *optProducts = "products";

    if (!theConfig.exists(optProducts))
      return result;

    libconfig::Setting &products = theConfig.lookup(optProducts);

    if (!products.isGroup())
      throw SmartMet::Spine::Exception(BCP,
                                       "Products must be stored as a group of arrays: line " +
                                           Fmi::to_string(products.getSourceLine()));

    for (int i = 0; i < products.getLength(); i++)
    {
      libconfig::Setting &product = products[i];

      if (!product.isArray())
        throw SmartMet::Spine::Exception(BCP,
                                         "Product must be stored as an array of parameters: line " +
                                             Fmi::to_string(product.getSourceLine()));

      string productName = product.getName();

      for (int j = 0; j < product.getLength(); j++)
      {
        try
        {
          string param = product[j];
          result.add(productName, ParameterFactory::instance().parse(param));
        }
        catch (libconfig::ParseException &e)
        {
          throw SmartMet::Spine::Exception(BCP,
                                           string("Configuration error ' ") + e.getError() +
                                               "' with variable '" + productName + "' on line " +
                                               Fmi::to_string(e.getLine()));
        }
        catch (libconfig::ConfigException &)
        {
          throw SmartMet::Spine::Exception(BCP,
                                           string("Configuration error with variable '") +
                                               productName + "' on line " +
                                               Fmi::to_string(product[j].getSourceLine()));
        }
        catch (std::exception &e)
        {
          throw SmartMet::Spine::Exception(BCP,
                                           e.what() + string(" (line number ") +
                                               Fmi::to_string(product[j].getSourceLine()) + ")");
        }
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Add forecast to the location
 */
// ----------------------------------------------------------------------

void append_forecast(Json::Value &theResult,
                     const ProductParameters::ParameterList &theParameters,
                     const SmartMet::Spine::LocationPtr theLoc,
                     SmartMet::Engine::Querydata::Engine &theEngine,
                     SmartMet::Engine::Geonames::Engine &theGeoEngine,
                     const ValueFormatter &theValueFormatter,
                     const Fmi::TimeFormatter &theTimeFormatter,
                     const string &theStamp,
                     const string &theLang,
                     const std::locale &theLocale)
{
  try
  {
    // Establish default producer for the coordinate

    auto producer = theEngine.find(theLoc->longitude, theLoc->latitude);

    // Should happen only if GFS is not in use:

    if (producer.empty())
    {
      Json::Value nulljson;
      BOOST_FOREACH (const Parameter &param, theParameters)
      {
        theResult[param.name()] = nulljson;
      }
      return;
    }

    // Establish local time now at the location

    auto timezone = theGeoEngine.getTimeZones().time_zone_from_string(theLoc->timezone);

    ptime utc = second_clock::universal_time();
    local_date_time t(utc, timezone);

    // Regression tests may override the date
    if (!theStamp.empty())
      t = Fmi::TimeParser::parse(theStamp, timezone);

    // Model, querydata and some default settins

    auto q = theEngine.get(producer);

    int precision = 0;
    const bool findnearest = false;
    NFmiPoint nearestpoint, lastpoint;
    const string timestring = "";

    // And process all parameters

    ostringstream ss;
    TimeSeries::OStreamVisitor val_visitor(ss, theValueFormatter, precision);

    BOOST_FOREACH (const Parameter &param, theParameters)
    {
      SmartMet::Engine::Querydata::ParameterOptions qparams(param,
                                                            producer,
                                                            *theLoc,
                                                            theLoc->iso2,
                                                            theLoc->name,
                                                            theTimeFormatter,
                                                            timestring,
                                                            theLang,
                                                            theLocale,
                                                            theLoc->timezone,
                                                            findnearest,
                                                            nearestpoint,
                                                            lastpoint);

      auto tmp = q->value(qparams, t);
      boost::apply_visitor(val_visitor, tmp);
      theResult[param.name()] = ss.str();
      ss.str("");
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Handle incoming request
 *
 * 1. We set a fixed expiration time of 60 seconds
 * 2. We handle IF_MODIFIED_SINCE queries
 * 3. We set expiration headers properly
 *
 * theReactor is a dummy argument to conform to the SmartMetPlugin
 * interface.
 */
// ----------------------------------------------------------------------

void Autocomplete::requestHandler(SmartMet::Spine::Reactor & /* theReactor */,
                                  const HTTP::Request &theRequest,
                                  HTTP::Response &theResponse)

{
  try
  {
    using namespace boost::posix_time;
    using namespace Fmi::TimeParser;
    using Fmi::TimeFormatter;

    // Default expiration time

    const int expires_seconds = 60;

    // Now

    ptime t_now = second_clock::universal_time();

    // Handle If-Modified-Since queries

    auto if_modified_since = theRequest.getHeader("If-Modified-Since");

    if (if_modified_since)
    {
      try
      {
        ptime t_modified = parse_http(*if_modified_since);
        if (t_now - t_modified < seconds(expires_seconds))
        {
          theResponse.setStatus(HTTP::Status::not_modified);
          return;
        }
      }
      catch (...)
      {
        // We choose to ignore all invalid If-Modified-Since headers
      }
    }

    // Process the query normally, If-Modified-Since check failed

    try
    {
      complete(theRequest, theResponse);
      theResponse.setStatus(HTTP::Status::ok);
      theResponse.setHeader("Content-Type", "application/json; charset=UTF-8");

      // Build cache expiration time info

      boost::shared_ptr<TimeFormatter> tformat(TimeFormatter::create("http"));

      ptime t_expires = t_now + seconds(expires_seconds);

      // The headers themselves

      std::string cachecontrol = "public, max-age=" + Fmi::to_string(expires_seconds);
      std::string expiration = tformat->format(t_expires);
      std::string modification = tformat->format(t_now);

      theResponse.setHeader("Cache-Control", cachecontrol.c_str());
      theResponse.setHeader("Expires", expiration.c_str());
      theResponse.setHeader("Last-Modified", modification.c_str());
    }
    catch (...)
    {
      SmartMet::Spine::Exception exception(BCP, "Request processing exception!", NULL);
      exception.addParameter("URI", theRequest.getURI());

      if (!exception.stackTraceDisabled())
        std::cerr << exception.getStackTrace();
      else
        std::cerr << "Error: " << exception.what() << std::endl;

      string msg = string("Error: ") + exception.what();
      theResponse.setStatus(HTTP::Status::ok);

      // Remove newlines, make sure length is reasonable
      boost::algorithm::replace_all(msg, "\n", " ");
      msg = msg.substr(0, 100);
      theResponse.setHeader("X-Autocomplete-Error", msg.c_str());
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Build response to incoming request without headers / error handling
 */
// ----------------------------------------------------------------------

void Autocomplete::complete(const HTTP::Request &theRequest, HTTP::Response &theResponse)
{
  try
  {
    string theURI(theRequest.getURI());

    // Parse query strings
    string keyword = SmartMet::Spine::optional_string(theRequest.getParameter("keyword"), "");
    string pattern = SmartMet::Spine::optional_string(theRequest.getParameter("pattern"), "");
    string lang = SmartMet::Spine::optional_string(theRequest.getParameter("lang"), "fi");
    unsigned long maxresults =
        SmartMet::Spine::optional_unsigned_long(theRequest.getParameter("max"), 15);
    unsigned long page =
        SmartMet::Spine::optional_unsigned_long(theRequest.getParameter("page"), 0);
    bool pretty = SmartMet::Spine::optional_unsigned_long(theRequest.getParameter("pretty"), false);
    string product = SmartMet::Spine::optional_string(theRequest.getParameter("product"), "");
    string timeformat =
        SmartMet::Spine::optional_string(theRequest.getParameter("timeformat"), "iso");
    string localename =
        SmartMet::Spine::optional_string(theRequest.getParameter("locale"), "fi_FI");
    string stamp = SmartMet::Spine::optional_string(theRequest.getParameter("time"), "");

    if (!product.empty() && !itsProductParameters.contains(product))
      throw SmartMet::Spine::Exception(BCP, "Product " + product + " has no associated parameters");

    ValueFormatter valueformatter(theRequest);
    boost::shared_ptr<Fmi::TimeFormatter> timeformatter(Fmi::TimeFormatter::create(timeformat));

    std::locale outlocale = locale(localename.c_str());

    Json::Value json;
    Json::Value jautocomplete;
    Json::Value jresult;

    std::unique_ptr<Json::Writer> writer;
    if (pretty)
      writer.reset(new Json::StyledWriter);
    else
      writer.reset(new Json::FastWriter);

    // ---- NORMAL AUTOCOMPLETION ROUTINES --------------------------------

    // If the pattern is empty or too big it makes no point to do search.
    if (pattern.size() == 0 || pattern.size() >= 32)
    {
      // Send error message

      jautocomplete["found-results"] = 0;
      jautocomplete["max-results"] = int(maxresults);
      jautocomplete["error-message"] = "Invalid pattern for search";
      json["autocomplete"] = jautocomplete;

      theResponse.appendContent(writer->write(json));

      return;
    }

    // Query the Suggestor engine
    SmartMet::Spine::LocationList suggestions =
        itsGeoEngine->suggest(pattern, lang, keyword, page, maxresults);

    // Loop through the Locations

    BOOST_FOREACH (SmartMet::Spine::LocationPtr &ptr, suggestions)
    {
      string name = ptr->name;
      string area = ptr->area;

      Json::Value j;

      j["id"] = int(ptr->geoid);
      j["name"] = name;
      j["country"] = ptr->iso2;
      j["feature"] = ptr->feature;
      j["area"] = area;
      j["population"] = ptr->population;
      j["lon"] = ptr->longitude;
      j["lat"] = ptr->latitude;
      j["timezone"] = ptr->timezone;
      append_forecast(j,
                      itsProductParameters.parameters(product),
                      ptr,
                      *itsQEngine,
                      *itsGeoEngine,
                      valueformatter,
                      *timeformatter,
                      stamp,
                      lang,
                      outlocale);

      jresult.append(j);
    }

    jautocomplete["result"] = jresult;
    jautocomplete["found-results"] = jresult.size();
    jautocomplete["max-results"] = int(maxresults);
    json["autocomplete"] = jautocomplete;

    theResponse.appendContent(writer->write(json));

    return;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief The constructor
 *
 * The constructor starts a separate thread for connecting to the database
 * and loading all the location information. The service is exceptionally
 * not registered by the constructor, but by init when the required data
 * has been loaded and processed.
 */
// ----------------------------------------------------------------------

Autocomplete::Autocomplete(SmartMetPlugin *theParent,
                           SmartMet::Spine::Reactor *theReactor,
                           const char *theConfig)
    : itsReactor(theReactor), itsConfigFile(theConfig)
{
  // Banner

  itsParent = theParent;
}

// ----------------------------------------------------------------------
/*!
 * \brief Initialize data from database
 *
 * This method registers to the server when the data has been loaded.
 * This method is run in a separate thread due to the slow speed
 * of downloading data from the database.
 */
// ----------------------------------------------------------------------

void Autocomplete::init()
{
  try
  {
    // Connect to GeoEngine
    auto engine = itsReactor->getSingleton("Geonames", NULL);
    if (engine == NULL)
      throw SmartMet::Spine::Exception(BCP, "Geonames engine unavailable");

    itsGeoEngine = reinterpret_cast<SmartMet::Engine::Geonames::Engine *>(engine);

    // Connect to QEngine

    engine = itsReactor->getSingleton("Querydata", NULL);
    if (engine == NULL)
      throw SmartMet::Spine::Exception(BCP, "Querydata engine unavailable");

    itsQEngine = reinterpret_cast<SmartMet::Engine::Querydata::Engine *>(engine);

    try
    {
      itsConfig.readFile(itsConfigFile);
      itsProductParameters = read_product_parameters(itsConfig);
    }
    catch (libconfig::ParseException &e)
    {
      throw SmartMet::Spine::Exception(BCP,
                                       string("autocomplete configuration error '") + e.getError() +
                                           "' on line " + Fmi::to_string(e.getLine()));
    }
    catch (libconfig::ConfigException &)
    {
      throw SmartMet::Spine::Exception(BCP, "autocomplete configuration error");
    }

    // Cannot register until geonames suggest is ready

    while (!itsGeoEngine->isSuggestReady())
    {
      boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }

    // Ready for service now

    if (!itsReactor->addContentHandler(
            itsParent,
            "/autocomplete",
            boost::bind(&Autocomplete::requestHandler, this, _1, _2, _3)))
    {
      throw SmartMet::Spine::Exception(BCP, "Failed to register autocomplete content handler");
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Destructor
 */
// ----------------------------------------------------------------------

Autocomplete::~Autocomplete()
{
  // Banner
  cout << "\t+ Autocomplete plugin shutting down" << endl;
}

}  // namespace Autocomplete
}  // namespace Plugin
}  // namespace SmartMet
