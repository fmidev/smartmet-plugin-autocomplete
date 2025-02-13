# Tutorial

This tutorial explains how to configure the Autocomplete plugin when using Docker.

## Prereqs

Docker software has been installed on some Linux server where you have access to and the smartmetserver docker container is up and running.
The Autocomplete plugin and the configuration file it uses have been defined in the main configuration file smartmet.conf already.

### File autocomplete.conf

The purpose of the file autocomplete.conf is to define some default parameter values.

If you followed the “SmartMet Server Tutorial (Docker)” you have your configuration folders and files in the host machine at $HOME/docker-smartmetserver/smartmetconf but inside Docker they show up under /etc/smartmet. 

1. Go to the correct directory and enter command below to review the file:

```
$ less autocomplete.conf
```
You will see something like this:

```
# Product settings

products:
{
        rich = [ "Temperature", "WeatherSymbol3" ];
        test = [ "sunelevation", "Temperature", "WeatherSymbol3" ];
}
```

2. Use Nano or some other editor to enable/disable configuration attributes or to add some more attributes if needed.

## Configuration attributes

The following variables must be set in the configuration file:
* language
* locale
* maxresults
* pretty
* timeformat

### keyword
Keyword is used to identify the set of locations enabled in autocomplete.
```
keyword =
```
The allowed keywords are listed in table `fminames.keywords` and the locations for each keyword are listed in table `fminames.keywords_has_geonames`.

There is a special case `keyword=all` which defines the set of places as *all places which belong to some known keyword*.

### pattern
Pattern is used to define the beginning of the location name usually typed by the user. It gets always changed in practice so it is not very useful to provide a default value for it in the config file. 

```
pattern = 
```

### lang
Lang is used to define the selected language using a 2-character language ISO-code. If no specific translation exists for a location, the primary name for the location will be returned

```
lang = fi
```

### maxresults
Maxresults is used to define the number of results to return in one page.

```
maxresults = 15
```

### page
Page is used to define the page number. This option works in conjuction with the maxresults option. It will only be given if you do not want to have the first page of hits.

```
page = 0
```

### pretty
Pretty print is used mostly for debugging purposes (and regression tests to spot changes more easily).

```
pretty = 0
```

### product
Product is used to identify a product name to add localized forecast data to the output. The allowed product names are listed in a table **autocomplete.products** and the forecast parameters for each product in table **autocomplete.parameters**. The default is not to include a forecast in the output as it is only given if needed.

```
product = 
```

### timeformat
This is used to define the output format as supported by MacGyver::TimeFormatter (iso, xml, timestamp, sql).

```
timeformat = iso
```

### locale
Locale is used to define formatting of locale specific forecast parameters.

```
locale = fi_FI 
```

### time
Time is used to define the date and time for the request. Time is always changed in practice so it is not very useful to provide a default value for it in the config file.

```
time =
```

3. Test the plugin. The URL of the HTTP request contains parameters that have to be delivered to the autocomplete plugin. 

http://hostname:8080/autocomplete?pattern=Hel&keyword=ajax_fi_fi

Test with the weather forecast:

http://hostname:8080/autocomplete?pattern=Kuop&keyword=ajax_fi_fi&product=test&time=2017-06-30T14:00

**Note:** Replace hostname with your host machine name, by localhost or by host-ip. This depends on the machine that you are using (Cloud server/Native Linux/Mac or Windows).
