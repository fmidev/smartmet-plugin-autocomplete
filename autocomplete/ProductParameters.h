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
class ProductParameters
{
 public:
  using ParameterList = std::list<Spine::Parameter>;
  using Parameters = std::map<std::string, ParameterList>;

  void add(const std::string& theProduct, const Spine::Parameter& theParameter);
  bool contains(const std::string& theProduct) const;
  const ParameterList& parameters(const std::string& theProduct) const;

 private:
  Parameters itsParameters;

};  // class ProductParameters

}  // namespace Autocomplete
}  // namespace Plugin
}  // namespace SmartMet
