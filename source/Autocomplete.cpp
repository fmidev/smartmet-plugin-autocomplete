#include "Autocomplete.h"

#include <spine/Exception.h>
#include <spine/Convenience.h>
#include <spine/ParameterFactory.h>
#include <spine/Reactor.h>
#include <spine/TimeSeries.h>
#include <spine/TimeSeriesOutput.h>
#include <engines/geoip/Engine.h>
#include <engines/geonames/Engine.h>

#include <macgyver/CharsetTools.h>
#include <macgyver/String.h>
#include <macgyver/TimeFormatter.h>
#include <macgyver/TimeParser.h>

#include <mysql++/mysql++.h>

#include <json_spirit/json_spirit.h>

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
 * \brief Determine if the response can be cached
 */
// ----------------------------------------------------------------------

bool is_cache_ok(const HTTP::Request &theRequest)
{
  try
  {
    // Cannot cache if we check for client IP

    string locateflag = SmartMet::Spine::optional_string(theRequest.getParameter("locate"), "");
    if (!locateflag.empty())
    {
      string ip = SmartMet::Spine::optional_string(theRequest.getParameter("ip"), "");
      if (ip.empty())
        return false;
    }

    return true;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

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

    const libconfig::Setting &host = theConfig.lookup("database.host");
    const libconfig::Setting &user = theConfig.lookup("database.user");
    const libconfig::Setting &pass = theConfig.lookup("database.pass");

    mysqlpp::Connection mysql;

    if (!mysql.connect("autocomplete", host, user, pass))
      throw SmartMet::Spine::Exception(BCP, "Failed to connect to autocomplete database");

    auto query = mysql.query();
    query << "SELECT product,param FROM parameters";

    auto res = query.store();
    if (!res)
      return result;

    for (size_t i = 0; i < res.num_rows(); ++i)
    {
      string product = res.at(i).at(0).c_str();
      string param = res.at(i).at(1).c_str();
      result.add(product, ParameterFactory::instance().parse(param));
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

void append_forecast(json_spirit::Object &theResult,
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
      BOOST_FOREACH (const Parameter &param, theParameters)
        theResult.push_back(json_spirit::Pair(param.name(), json_spirit::Value::null));
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
      theResult.push_back(json_spirit::Pair(param.name(), ss.str()));
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

      bool cacheok = is_cache_ok(theRequest);

      boost::shared_ptr<TimeFormatter> tformat(TimeFormatter::create("http"));

      if (cacheok)
      {
        ptime t_expires = t_now + seconds(expires_seconds);

        // The headers themselves

        std::string cachecontrol = "public, max-age=" + Fmi::to_string(expires_seconds);
        std::string expiration = tformat->format(t_expires);
        std::string modification = tformat->format(t_now);

        theResponse.setHeader("Cache-Control", cachecontrol.c_str());
        theResponse.setHeader("Expires", expiration.c_str());
        theResponse.setHeader("Last-Modified", modification.c_str());
      }
      else
      {
        theResponse.setHeader("Cache-Control", "no-cache, must-revalidate");
        theResponse.setHeader("Pragma", "no-cache");

        // Expired already at some past date

        std::string expiration = tformat->format(t_now - years(1));
        theResponse.setHeader("Expires", expiration.c_str());
      }
    }
    catch (...)
    {
      SmartMet::Spine::Exception exception(BCP, "Request processing exception!", NULL);
      exception.addParameter("URI", theRequest.getURI());

      std::cerr << exception.getStackTrace();

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
    string format = SmartMet::Spine::optional_string(theRequest.getParameter("format"), "json");
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

    json_spirit::Object json;
    json_spirit::Object jautocomplete;
    json_spirit::Array jresult;

    // Proof-of-Concept IP tracking

    string locateflag = SmartMet::Spine::optional_string(theRequest.getParameter("locate"), "");
    if (!locateflag.empty())
    {
      string ip = SmartMet::Spine::optional_string(theRequest.getParameter("ip"), "");
      if (ip.empty())
      {
        // No IP specified in query string, first check proxy header
        auto headerip = theRequest.getHeader("X-Forwarded-For");

        if (headerip)
        {
          ip = *headerip;
          // Remove the possible upstream information
          string::size_type npos = ip.find(',');
          if (npos != string::npos)
            ip.erase(npos);
        }
        else
        {
          // If no proxy forward header in the request, use the requesters IP
          ip = theRequest.getClientIP();
        }
      }

      // Consult the GeoIP
      boost::shared_ptr<SmartMet::Engine::Geoip::GeoIPLocation> location(
          itsGeoIPEngine->locationByIP(ip.c_str()));

      if (!location)
      {
        jautocomplete.push_back(json_spirit::Pair("found-results", 0));
        jautocomplete.push_back(json_spirit::Pair("max-results", int(maxresults)));
        jautocomplete.push_back(json_spirit::Pair("error-message", "GeoIP query failed"));
        json.push_back(json_spirit::Pair("autocomplete", jautocomplete));

        if (pretty)
          theResponse.appendContent(write_formatted(json).c_str());
        else
          theResponse.appendContent(write(json).c_str());

        return;
      }

      // Find the nearest Location for this lat, lon with this Keyword

      SmartMet::Spine::LocationPtr nearestLocation =
          itsGeoEngine->keywordSearch(location->lon, location->lat, -1, lang, keyword);

      if (!nearestLocation)
      {
        jautocomplete.push_back(json_spirit::Pair("found-results", 0));
        jautocomplete.push_back(json_spirit::Pair("max-results", int(maxresults)));
        jautocomplete.push_back(
            json_spirit::Pair("error-message", "Found no near location for IP"));
        json.push_back(json_spirit::Pair("autocomplete", jautocomplete));

        if (pretty)
          theResponse.appendContent(write_formatted(json).c_str());
        else
          theResponse.appendContent(write(json).c_str());

        return;  // No nearest place from fminames/geonames found.
      }

      // Possible character set conversions
      std::string name = nearestLocation->name;
      std::string area = nearestLocation->area;

      // Send GeoIP tracking result

      // TODO: Add weather

      json_spirit::Object j;
      j.push_back(json_spirit::Pair("id", int(nearestLocation->geoid)));
      j.push_back(json_spirit::Pair("name", name));
      j.push_back(json_spirit::Pair("country", nearestLocation->iso2));
      j.push_back(json_spirit::Pair("feature", nearestLocation->feature));
      j.push_back(json_spirit::Pair("area", area));
      j.push_back(json_spirit::Pair("population", nearestLocation->population));
      j.push_back(json_spirit::Pair("lon", nearestLocation->longitude));
      j.push_back(json_spirit::Pair("lat", nearestLocation->latitude));
      j.push_back(json_spirit::Pair("timezone", nearestLocation->timezone));
      append_forecast(j,
                      itsProductParameters.parameters(product),
                      nearestLocation,
                      *itsQEngine,
                      *itsGeoEngine,
                      valueformatter,
                      *timeformatter,
                      stamp,
                      lang,
                      outlocale);

      jresult.push_back(j);

      jautocomplete.push_back(json_spirit::Pair("result", jresult));
      jautocomplete.push_back(json_spirit::Pair("found-results", 1));
      jautocomplete.push_back(json_spirit::Pair("max-results", int(maxresults)));
      json.push_back(json_spirit::Pair("autocomplete", jautocomplete));

      if (pretty)
        theResponse.appendContent(write_formatted(json).c_str());
      else
        theResponse.appendContent(write(json).c_str());

      // GeoIP tracking done
      return;
    }

    // ---- NORMAL AUTOCOMPLETION ROUTINES --------------------------------

    // If the pattern is empty or too big it makes no point to do search.
    if (pattern.size() == 0 || pattern.size() >= 32)
    {
      // Send error message

      jautocomplete.push_back(json_spirit::Pair("found-results", 0));
      jautocomplete.push_back(json_spirit::Pair("max-results", int(maxresults)));
      jautocomplete.push_back(json_spirit::Pair("error-message", "Invalid pattern for search"));
      json.push_back(json_spirit::Pair("autocomplete", jautocomplete));

      if (pretty)
        theResponse.appendContent(write_formatted(json).c_str());
      else
        theResponse.appendContent(write(json).c_str());

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

      json_spirit::Object j;

      j.push_back(json_spirit::Pair("id", int(ptr->geoid)));
      j.push_back(json_spirit::Pair("name", name));
      j.push_back(json_spirit::Pair("country", ptr->iso2));
      j.push_back(json_spirit::Pair("feature", ptr->feature));
      j.push_back(json_spirit::Pair("area", area));
      j.push_back(json_spirit::Pair("population", ptr->population));
      j.push_back(json_spirit::Pair("lon", ptr->longitude));
      j.push_back(json_spirit::Pair("lat", ptr->latitude));
      j.push_back(json_spirit::Pair("timezone", ptr->timezone));
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

      jresult.push_back(j);
    }

    jautocomplete.push_back(json_spirit::Pair("result", jresult));
    jautocomplete.push_back(json_spirit::Pair("found-results", jresult.size()));
    jautocomplete.push_back(json_spirit::Pair("max-results", int(maxresults)));
    json.push_back(json_spirit::Pair("autocomplete", jautocomplete));

    if (pretty)
      theResponse.appendContent(write_formatted(json).c_str());
    else
      theResponse.appendContent(write(json).c_str());

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

    // Connect to GeoIP Engine
    engine = itsReactor->getSingleton("Geoip", NULL);
    if (engine == NULL)
      throw SmartMet::Spine::Exception(BCP, "Geoip engine unavailable");

    itsGeoIPEngine = reinterpret_cast<SmartMet::Engine::Geoip::Engine *>(engine);

    // Connect to QEngine

    engine = itsReactor->getSingleton("Querydata", NULL);
    if (engine == NULL)
      throw SmartMet::Spine::Exception(BCP, "Querydata engine unavailable");

    itsQEngine = reinterpret_cast<SmartMet::Engine::Querydata::Engine *>(engine);

    // Load places from fminames

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
