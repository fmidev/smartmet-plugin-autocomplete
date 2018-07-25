// ======================================================================

#include "ProductParameters.h"
#include <spine/Exception.h>

namespace SmartMet
{
namespace Plugin
{
namespace Autocomplete
{
// ----------------------------------------------------------------------
/*!
 * \brief Add a new parameter for a product
 */
// ----------------------------------------------------------------------

void ProductParameters::add(const std::string& theProduct, const Parameter& theParameter)
{
  try
  {
    itsParameters[theProduct].push_back(theParameter);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Test if the given product has extra parameters
 */
// ----------------------------------------------------------------------

bool ProductParameters::contains(const std::string& theProduct) const
{
  try
  {
    return (itsParameters.find(theProduct) != itsParameters.end());
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Get the parameters for the given product
 */
// ----------------------------------------------------------------------

const ProductParameters::ParameterList& ProductParameters::parameters(const std::string& theProduct)
{
  try
  {
    // If the product didn't have any contents prior to this call,
    // and empty list will be created at this point
    return itsParameters[theProduct];
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception::Trace(BCP, "Operation failed!");
  }
}

}  // namespace Autocomplete
}  // namespace Plugin
}  // namespace SmartMet
