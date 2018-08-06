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

Plugin::Plugin(Reactor *theReactor, const char *theConfig)
    : itsShutdownRequested(false),
      itsReactor(theReactor),
      itsConfig(theConfig),
      itsAutoComplete(nullptr),
      itsModuleName("Autocomplete")
{
  try
  {
    if (theReactor->getRequiredAPIVersion() != SMARTMET_API_VERSION)
      throw SmartMet::Spine::Exception(BCP, "Autocomplete and Server API version mismatch");
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception::Trace(BCP, "Operation failed!");
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
    itsAutocomplete = new SmartMet::Plugin::Autocomplete::Autocomplete(this, itsReactor, itsConfig);
    itsAutocomplete->init();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception::Trace(BCP, "Operation failed!");
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

void Plugin::requestHandler(SmartMet::Spine::Reactor & /* theReactor */,
                            const HTTP::Request & /* theRequest */,
                            HTTP::Response & /* theResponse */)
{
}

// ----------------------------------------------------------------------
/*!
 * \brief Destructor
 */
// ----------------------------------------------------------------------

Plugin::~Plugin()
{
  delete itsAutocomplete;
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

bool Plugin::queryIsFast(const SmartMet::Spine::HTTP::Request & /* theRequest */) const
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
  delete us;
}

// ======================================================================
