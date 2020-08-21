%define DIRNAME autocomplete
%define SPECNAME smartmet-plugin-%{DIRNAME}
Summary: SmartMet autocomplete plugin (provides pre-emptive text completion)
Name: %{SPECNAME}
Version: 20.8.21
Release: 1%{?dist}.fmi
License: MIT
Group: SmartMet/Plugins
URL: https://github.com/fmidev/smartmet-plugin-autocomplete
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: rpm-build
BuildRequires: gcc-c++
BuildRequires: make
BuildRequires: libconfig-devel
BuildRequires: boost169-devel
BuildRequires: smartmet-library-macgyver-devel >= 20.8.21
BuildRequires: jsoncpp-devel
BuildRequires: smartmet-library-spine-devel >= 20.8.21
BuildRequires: smartmet-engine-geonames-devel >= 20.8.21
BuildRequires: smartmet-engine-querydata-devel >= 20.8.21
BuildRequires: smartmet-engine-sputnik-devel
Requires: libconfig
Requires: smartmet-library-macgyver >= 20.8.21
Requires: smartmet-library-spine >= 20.8.21
Requires: smartmet-engine-geonames >= 20.8.21
Requires: smartmet-server >= 20.8.21
Requires: smartmet-engine-querydata >= 20.8.21
%if 0%{rhel} >= 7
Requires: boost169-date-time
Requires: boost169-filesystem
Requires: boost169-iostreams
Requires: boost169-system
Requires: boost169-thread
Requires: jsoncpp
%endif
Provides: %{SPECNAME}
Obsoletes: smartmet-brainstorm-autocomplete < 16.11.1
Obsoletes: smartmet-brainstorm-autocomplete-debuginfo < 16.11.1

%description
SmartMet autocomplete plugin

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q -n %{SPECNAME}
 
%build -q -n %{SPECNAME}
make %{_smp_mflags}

%install
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(0775,root,root,0775)
%{_datadir}/smartmet/plugins/%{DIRNAME}.so
%defattr(0664,root,root,0775)

%changelog
* Fri Aug 21 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.8.21-1.fmi
- Upgrade to fmt 6.2

* Wed May 13 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.5.13-1.fmi
- Repackaged since Spine Parameter class ABI changed

* Sat Apr 18 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.4.18-1.fmi
- Upgraded to Boost 1.69

* Tue Mar  3 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.3.3-1.fmi
- Repackaged due to base library changes

* Tue Feb 25 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.2.25-1.fmi
- Repackaged due to base library changes

* Wed Nov 20 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.11.20-1.fmi
- Repackaged since Spine::Parameter size changed

* Thu Sep 26 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.9.26-1.fmi
- Fixed thread safety issue

* Thu Sep  5 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.9.5-1.fmi
- Added CORS headers

* Wed Aug 28 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.8.28-1.fmi
- Repackaged since Spine::Location ABI changed

* Fri Aug  9 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.8.9-1.fmi
- Added tests for ascii_autocomplete mode

* Tue Feb 26 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.2.26-1.fmi
- Added option duplicates for keeping different feature types even if the names are the same

* Thu Feb 14 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.2.14-1.fmi
- Added client IP to exception reports

* Sun Aug 26 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.8.26-1.fmi
- debug=1 option will now print the sort score of the location
- Silenced CodeChecker warnings

* Sun Aug 19 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.8.19-1.fmi
- Added a NOLINT directive when deleting a C++ pointer obtained from extern "C" interface

* Mon Aug  6 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.8.6-1.fmi
- Silenced CodeChecker warnings

* Wed Jul 25 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.7.25-1.fmi
- Prefer nullptr over NULL

* Mon Jul 23 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.7.23-1.fmi
- Repackaged since spine ValueFormatter ABI changed

* Sat Apr  7 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.4.7-1.fmi
- Upgrade to boost 1.66

* Tue Mar 20 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.3.20-1.fmi
- Full recompile of all server plugins

* Tue Feb 27 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.2.27-1.fmi
- Querydata engine API was changed to be const correct

* Fri Feb  9 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.2.9-1.fmi
- Repackaged due to TimeZones API change

* Mon Aug 28 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.8.28-1.fmi
- Upgrade to boost 1.65

* Mon Jul 10 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.7.20-1.fmi
- Language, locale, timeformat, maxresults and pretty are now obligatory configuration variables

* Fri May  5 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.5.5-1.fmi
- Default locale can now be specified in the configuration file

* Sat Apr  8 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.4.8-1.fmi
- Simplified error reporting

* Wed Mar 15 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.3.15-2.fmi
- Switched from json_spirit to jsoncpp to ease Docker builds

* Wed Mar 15 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.3.15-1.fmi
- Recompiled since Spine::Exception changed

* Tue Mar 14 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.3.14-1.fmi
- Switched to use macgyver StringConversion tools 

* Sat Feb 11 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.2.11-1.fmi
- Repackaged due to newbase API change

* Sun Feb  5 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.2.5-1.fmi
- Removed geoip for being outdated

* Fri Jan 27 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.1.27-1.fmi
- Geonames engine usage changed, now explicitly wait for it to report autocomplete is ready

* Wed Jan 11 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.1.11-1.fmi
- Test result changes due to limitations in dockerized test data
- mysql and mariadb dependencies removed; product configuration now in config file

* Wed Jan  4 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.1.4-1.fmi
- Changed to use renamed SmartMet base libraries

* Wed Nov 30 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.11.30-1.fmi
- Using test database in test configuration
- No installation for configuration

* Tue Nov 29 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.11.29-1.fmi
- Recompiled due to API changes

* Tue Nov  1 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.11.1-1.fmi
- Namespace changed

* Tue Sep  6 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.9.6-1.fmi
- New exception handler

* Tue Aug 30 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.8.30-1.fmi
- Base class API change

* Mon Aug 15 2016 Markku Koskela <markku.koskela@fmi.fi> - 16.8.15-1.fmi
- The init(),shutdown() and requestHandler() methods are now protected methods
- The requestHandler() method is called from the callRequestHandler() method

* Wed Jun 29 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.6.29-1.fmi
- QEngine API changed

* Tue Jun 14 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.6.14-1.fmi
- Full recompile

* Thu Jun  2 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.6.2-1.fmi
- Full recompile

* Wed Jun  1 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.6.1-1.fmi
- Added graceful shutdown

* Mon May 16 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.5.16-1.fmi
- Replaced TimeZoneFactory with TimeZones

* Tue Feb  9 2016 Tuomo Lauri <tuomo.lauri@fmi.fi> - 16.2.9-1.fmi
- Rebuilt against the new TimeSeries::Value definition

* Tue Feb  2 2016 Tuomo Lauri <tuomo.lauri@fmi.fi> - 16.2.2-1.fmi
- Missingvalue is now deprecated in Q-engine calls

* Sat Jan 23 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.1.23-1.fmi
- Fmi::TimeZoneFactory API changed

* Mon Jan 18 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.1.18-1.fmi
- newbase API changed, full recompile

* Wed Nov 18 2015 Tuomo Lauri <tuomo.lauri@fmi.fi> - 15.11.18-1.fmi
- SmartMetPlugin now receives a const HTTP Request

* Mon Oct 26 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.10.26-1.fmi
- Added proper debuginfo packaging

* Mon Aug 24 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.8.24-1.fmi
- Recompiled due to Convenience.h API changes

* Tue Aug 18 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.8.18-1.fmi
- Recompile forced by brainstorm API changes

* Mon Aug 17 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.8.17-1.fmi
- Use -fno-omit-frame-pointer to improve perf use

* Fri Aug 14 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.8.14-1.fmi
- Avoid boost::lexical_cast, Fmi::number_cast and std::ostringstream for speed

* Mon May 25 2015 Tuomo Lauri <tuomo.lauri@fmi.fi> - 15.5.25-1.fmi
- Rebuilt againts new spine

* Thu Apr  9 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.4.9-1.fmi
- newbase API changed

* Wed Apr  8 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.4.8-1.fmi
- Dynamic linking of smartmet libraries into use

* Thu Dec 18 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.12.18-1.fmi
- Recompiled due to spine API changes

* Mon Sep  8 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.9.8-1.fmi
- Recompiled due to geoengine API changes

* Wed May 14 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.5.14-1.fmi
- Use shared macgyver and locus libraries

* Tue May  6 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.5.6-1.fmi
- qengine API changed

* Mon Apr 28 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.4.28-1.fmi
- Full recompile due to large changes in spine etc APIs

* Mon Nov 25 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.11.25-1.fmi
- Updated Locus library

* Thu Nov 14 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.11.14-1.fmi
- Added necessary includes
- Updated regression test results (population changes, negative FMI indices, PPL to PPLA3)

* Tue Nov  5 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.11.5-1.fmi
- Major release

* Wed Oct  9 2013 Tuomo Lauri <tuomo.lauri@fmi.fi> - 13.10.9-1.fmi
- Now conforming with the new Reactor initialization API

* Mon Sep 23 2013 Tuomo Lauri    <tuomo.lauri@fmi.fi>    - 13.9.23-1.fmi
- Recompiled due to QEngine API changes

* Wed Aug 28 2013 Tuomo Lauri    <tuomo.lauri@fmi.fi>    - 13.8.28-1.fmi
- Recompilation due to linking errors

* Tue Jul 23 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.7.23-1.fmi
- Recompiled due to thread safety fixes in newbase & macgyver

* Wed Jul  3 2013 mheiskan <mika.heiskanen@fmi.fi> - 13.7.3-1.fmi
- Update to boost 1.54

* Mon Jun  3 2013 lauri    <tuomo.lauri@fmi.fi>   - 13.6.3-1.fmi
- Rebuilt against the new Spine

* Mon Apr 22 2013 mheiskan <mika.heiskanen@fi.fi> - 13.4.22-1.fmi
- Brainstorm API changed

* Tue Apr 16 2013 mheiskan <mika.heiskanen@fmi.fi> - 13.4.16-1.fmi
- Use mysli.fmi.fi instead of base.weatherproof.fi

* Fri Apr 12 2013 lauri <tuomo.lauri@fmi.fi>       - 13.4.12-1.fmi
- Rebuild due to changes in Spine

* Wed Feb  6 2013 lauri    <tuomo.lauri@fmi.fi>    - 13.2.6-1.fmi
- Built against new Spine and Server

* Wed Nov  7 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.11.7-1.fmi
- Upgrade to boost 1.52
- Upgrade to refactored spine library

* Thu Aug 30 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.8.30-1.el6.fmi
- Validity of keyword is no longer tested

* Wed Aug 8  2012 lauri    <tuomo.lauri@fmi.fi>    - 12.8.8-1.el6.fmi
- Location API change

* Tue Jul 31 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.7.31-1.el6.fmi
- GeoEngine API changed

* Thu Jul 26 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.7.26-1.el6.fmi
- GeoEngine API changed

* Mon Jul 23 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.7.23-1.el6.fmi
- Common library changed

* Thu Jul 19 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.7.19-1.el6.fmi
- GeoNames API changed

* Thu Jul  5 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.7.5-1.el6.fmi
- Upgrade to boost 1.50

* Wed Apr  4 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.4.6-1.el6.fmi
- full recompile due to common lib change

* Wed Apr  4 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.4.4-1.el6.fmi
- qengine API changed

* Mon Apr  2 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.4.2-1.el6.fmi
- macgyver change forced recompile

* Sat Mar 31 2012 mheiskan <mika.heiskanen@fmi.fi> - 12.3.31-1.el5.fmi
- Upgrade to boost 1.49

* Tue Dec 27 2011 mheiskan <mika.heiskanen@fmi.fi> - 11.12.27-2.el5.fmi
- Table class changed, recompile forced

* Wed Dec 21 2011 mheiskan <mika.heiskanen@fmi.fi> - 11.12.21-1.el5.fmi
- RHEL6 release

* Tue Aug 16 2011 mheiskan <mika.heiskanen@fmi.fi> - 11.8.16-1.el5.fmi
- Upgrade to boost 1.47

* Wed Jun 22 2011 mheiskan <mika.heiskanen@fmi.fi> - 11.6.22-1.el5.fmi
- Disable autocomplete caching when client IP is used

* Thu Mar 24 2011 mheiskan <mika.heiskanen@fmi.fi> - 11.3.24-1.el5.fmi
- Upgrade to boost 1.46

* Mon Feb 14 2011 mheiskan <mika.heiskanen@fmi.fi> - 11.2.14-1.el5.fmi
- Link to mysqlclient_r for thread safety

* Tue Jan 18 2011 mheiskan <mika.heiskanen@fmi.fi> - 11.1.18-1.el5.fmi
- Refactored query string parsing

* Tue Dec 14 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.12.14-1.el5.fmi
- Added UTF8 support

* Mon Dec 13 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.12.13-1.el5.fmi
- Removed json_spirit wstring support as nonfunctional

* Fri Dec 10 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.12.10-1.el5.fmi
- Added Unicode support to JSON output

* Fri Nov 26 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.11.26-1.el5.fmi
- Fixed output to be in UTF-8

* Thu Nov 25 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.11.25-1.el5.fmi
- Weather is now printed for wall clock time, not next 15:00 local time

* Thu Oct 28 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.10.28-1.el5.fmi
- Now using json_spirit
- Added product, pretty and time options

* Wed Sep 22 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.9.22-1.el5.fmi
- Improved error messages to the terminal

* Tue Sep 21 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.9.21-2.el5.fmi
- Fixed bug in JSON output when there are no results
- Fixed geoip dependencies

* Tue Sep 14 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.9.14-1.el5.fmi
- Upgrade to boost 1.44

* Mon Aug  9 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.8.9-1.el5.fmi
- GeoEngine replaces FmiNames

* Mon Jun  7 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.6.7-1.el5.fmi
- Autocomplete now reads allowed keywords from the database

* Wed Apr 14 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.4.14-1.el5.fmi
- Fixed charset to ISO-8859-1

* Tue Apr 13 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.4.13-1.el5.fmi
- Added expiration dates, cache-control and If-Modified-Since handling

* Fri Jan 15 2010 mheiskan <mika.heiskanen@fmi.fi> - 10.1.15-1.el5.fmi
- Upgrade to boost 1.41

* Thu Dec  3 2009 tervo <roope.tervo@fmi.fi> - 9.12.3-1.el5.fmi
- Enabled keyword 'mobile_press'

* Wed Dec  2 2009 mheiskan <mika.heiskanen@fmi.fi> - 9.12.2-1.el5.fmi
- Enabled keyword 'arava-asunnot'

* Fri Nov 20 2009 mheiskan <mika.heiskanen@fmi.fi> - 9.11.20-1.el5.fmi
- Added exception handling

* Mon Aug 31 2009 mheiskan <mika.heiskanen@fmi.fi> - 9.8.31-1.el5.fmi
- Added new keywords

* Tue Aug  4 2009 mheiskan <mika.heiskanen@fmi.fi> - 9.8.4-1.el5.fmi
- Added validity check for input keyword

* Tue Jul 21 2009 mheiskan <mika.heiskanen@fmi.fi> - 9.7.21-1.el5.fmi
- PHP serialization simplified

* Tue Jul 14 2009 mheiskan <mika.heiskanen@fmi.fi> - 9.7.14-1.el5.fmi
- Upgrade to boost 1.39

* Thu Jul 02 2009 westerba <antti.westerberg@fmi.fi> - 9.7.2-2.el5.fmi
- Search pattern normalization, GeoIP works with fminames engine

* Thu Jul  2 2009 mheiskan <mika.heiskanen@fmi.fi> - 9.7.2-1.el5.fmi
- Updated geoip handling, UTF8 fixes

* Wed Jul  1 2009 mheiskan <mika.heiskanen@fmi.fi> - 9.7.1-1.el5.fmi
- Rewritten to use fminames engine

* Fri Jun 26 2009 westerba <antti.westerberg@fmi.fi> - 9.6.26-2.el5.fmi
- Default configuration included with RPM

* Fri Jun 26 2009 westerba <antti.westerberg@fmi.fi> - 9.6.26-1.el5.fmi
- First prototype with GeoIP engine (still using old geoengine)

* Tue Jun 23 2009 westerba <antti.westerberg@fmi.fi> - 9.6.23-1.el5.fmi
- Supports many same name places by appending GeoID to all keywords internally

* Mon Jun 22 2009 westerba <antti.westerberg@fmi.fi> - 9.6.22-1.el5.fmi
- Launch loading thread seperately when starting. Plugin available after load.

* Mon Jun 15 2009 westerba <antti.westerberg@fmi.fi> - 9.6.15-2.el5.fmi
- Accent removing from the search patterns (discard accent in suggestions)

* Mon Jun 15 2009 mheiskan <mika.heiskanen@fmi.fi> - 9.6.15-1.el5.fmi
- Removed extra commas from JSON output

* Wed Jun 10 2009 westerba <antti.westerberg@fmi.fi> - 9.6.10-1.el5.fmi
- Suggestion behavior modifying

* Tue Jun 09 2009 westerba <antti.westerberg@fmi.fi> - 9.6.9-2.el5.fmi
- Changed the result names to be outputted in iso-8859-1 encoding

* Tue Jun 09 2009 westerba <antti.westerberg@fmi.fi> - 9.6.9-1.el5.fmi
- Fixed couple of bugs that could lead to memory leaks

* Mon Jun 08 2009 westerba <antti.westerberg@fmi.fi> - 9.6.8-1.el5.fmi
- Improved output area text logic. (Done when initially building the TRIEs)

* Fri Jun 05 2009 westerba <antti.westerberg@fmi.fi> - 9.6.5-1.el5.fmi
- JSON formatting improved

* Thu May 07 2009 westerba <antti.westerberg@fmi.fi> - 9.5.7-1.el5.fmi
- Initial release
