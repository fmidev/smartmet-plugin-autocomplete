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

The following variables must be set in the configuration file. They are the defaults of the
corresponding query string options described in the [README](../README.md).

### language
The default language of the location names as a 2-character ISO code (query string option `lang`). If no specific translation exists for a location, the primary name for the location will be returned.

```
language = "fi";
```

### locale
The default locale used for formatting locale specific forecast parameters (query string option `locale`).

```
locale = "fi_FI";
```

### maxresults
The default number of results to return in one page (query string option `max`).

```
maxresults = 15;
```

### pretty
The default of pretty printing, which is used mostly for debugging purposes (and regression tests to spot changes more easily).

```
pretty = false;
```

### timeformat
The default output time format as supported by MacGyver::TimeFormatter (iso, xml, timestamp, sql).

```
timeformat = "iso";
```

### products
The optional `products` group lists the products which can be selected with the query string option `product`, and the forecast parameters of each product, as shown above. The default is not to include a forecast in the output.

3. Test the plugin. The URL of the HTTP request contains parameters that have to be delivered to the autocomplete plugin. 

http://hostname:8080/autocomplete?pattern=Hel&keyword=ajax_fi_fi

Test with the weather forecast:

http://hostname:8080/autocomplete?pattern=Kuop&keyword=ajax_fi_fi&product=test&time=2017-06-30T14:00

**Note:** Replace hostname with your host machine name, by localhost or by host-ip. This depends on the machine that you are using (Cloud server/Native Linux/Mac or Windows).
