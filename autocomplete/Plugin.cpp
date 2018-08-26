// ======================================================================
/*!
 * \brief SmartMet Autocompelete plugin implementation
 */
// ======================================================================

#include "Plugin.h"
#include "Autocomplete.h"

#include <spine/Exception.h>
#include <spine/Reactor.h>
#include <spine/SmartMet.h>
#include <sstream>
#include <stdexcept>

namespace SmartMet
{
namespace Plugin
{
namespace Autocomplete
{
// ----------------------------------------------------------------------
/*!
 * \brief Plugin constructor
 */
// ----------------------------------------------------------------------

Plugin::Plugin(Spine::Reactor *theReactor, const char *theConfig)
    : itsShutdownRequested(false),
      itsReactor(theReactor),
      itsConfig(theConfig),
      itsModuleName("Autocomplete")
{
  try
  {
    if (theReactor->getRequiredAPIVersion() != SMARTMET_API_VERSION)
      throw Spine::Exception(BCP, "Autocomplete and Server API version mismatch");
  }
  catch (...)
  {
    throw Spine::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Initializator
 */
// ----------------------------------------------------------------------
void Plugin::init()
{
  try
  {
    itsAutocomplete = boost::movelib::make_unique<Autocomplete>(this, itsReactor, itsConfig);
    itsAutocomplete->init();
  }
  catch (...)
  {
    throw Spine::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Shutdown the plugin
 */
// ----------------------------------------------------------------------

void Plugin::shutdown()
{
  std::cout << "  -- Shutdown requested (autocomplete)\n";
  itsShutdownRequested = true;
  itsAutocomplete->shutdown();
}

// ----------------------------------------------------------------------
/*!
 * \brief requestHandler is a dummy method for this plugin
 */
// ----------------------------------------------------------------------

void Plugin::requestHandler(Spine::Reactor & /* theReactor */,
                            const Spine::HTTP::Request & /* theRequest */,
                            Spine::HTTP::Response & /* theResponse */)
{
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the plugin name
 */
// ----------------------------------------------------------------------

const std::string &Plugin::getPluginName() const
{
  return itsModuleName;
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the required version
 */
// ----------------------------------------------------------------------

int Plugin::getRequiredAPIVersion() const
{
  return SMARTMET_API_VERSION;
}
// ----------------------------------------------------------------------
/*!
 * \brief Autocomplete queries are always fast
 */
// ----------------------------------------------------------------------

bool Plugin::queryIsFast(const Spine::HTTP::Request & /* theRequest */) const
{
  return true;
}

}  // namespace Autocomplete
}  // namespace Plugin
}  // namespace SmartMet

/*
 * Server knows us through the 'SmartMetPlugin' virtual interface, which
 * the 'Plugin' class implements.
 */

extern "C" SmartMetPlugin *create(SmartMet::Spine::Reactor *them, const char *config)
{
  return new SmartMet::Plugin::Autocomplete::Plugin(them, config);
}

extern "C" void destroy(SmartMetPlugin *us)
{
  // This will call 'Plugin::~Plugin()' since the destructor is virtual
  delete us;  // NOLINT(cppcoreguidelines-owning-memory)
}

// ======================================================================
