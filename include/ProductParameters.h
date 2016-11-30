// ======================================================================
/*!
 * \brief Hold information on extra parameters for each product
 */
// ======================================================================

#pragma once

#include <spine/Parameter.h>

#include <list>
#include <map>
#include <string>

namespace SmartMet
{
namespace Plugin
{
namespace Autocomplete
{
using namespace SmartMet::Spine;
class ProductParameters
{
 public:
  typedef std::list<Parameter> ParameterList;
  typedef std::map<std::string, ParameterList> Parameters;

  void add(const std::string& theProduct, const Parameter& theParameter);
  bool contains(const std::string& theProduct) const;
  const ParameterList& parameters(const std::string& theProduct);

 private:
  Parameters itsParameters;

};  // class ProductParameters

}  // namespace Autocomplete
}  // namespace Plugin
}  // namespace SmartMet
