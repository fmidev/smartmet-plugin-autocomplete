
#include "Plugin.h"

#include <spine/PluginTest.h>

void prelude(SmartMet::Spine::Reactor& reactor)
{
#if 1
  auto handlers = reactor.getURIMap();
  while (handlers.find("/autocomplete") == handlers.end())
  {
    sleep(1);
    handlers = reactor.getURIMap();
  }
#endif

  std::cout << "\nTesting autocomplete plugin\n===========================\n";
}

int main()
{
  SmartMet::Spine::Options options;
  options.configfile = "cnf/reactor.conf";
  options.defaultlogging = false;
  options.quiet = true;

  std::setlocale(LC_ALL, "");  // for iconv to work
  return SmartMet::Spine::PluginTest::test(options, prelude, false, 10);
}
