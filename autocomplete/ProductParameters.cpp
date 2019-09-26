// ======================================================================

#include "ProductParameters.h"
#include <spine/Exception.h>

namespace SmartMet
{
namespace Plugin
{
namespace Autocomplete
{

namespace
{
  // to keep parameters() const correct
  ProductParameters::ParameterList my_empty_product;
}
  
// ----------------------------------------------------------------------
/*!
 * \brief Add a new parameter for a product
 */
// ----------------------------------------------------------------------

void ProductParameters::add(const std::string& theProduct, const Spine::Parameter& theParameter)
{
  try
  {
    itsParameters[theProduct].push_back(theParameter);
  }
  catch (...)
  {
    throw Spine::Exception::Trace(BCP, "Operation failed!");
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
    throw Spine::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Get the parameters for the given product
 */
// ----------------------------------------------------------------------

const ProductParameters::ParameterList& ProductParameters::parameters(const std::string& theProduct) const
{
  try
  {
    auto pos = itsParameters.find(theProduct);
    if(pos != itsParameters.end())
      return pos->second;
    return my_empty_product;
  }
  catch (...)
  {
    throw Spine::Exception::Trace(BCP, "Operation failed!");
  }
}

}  // namespace Autocomplete
}  // namespace Plugin
}  // namespace SmartMet
