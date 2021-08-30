# MongoDB CPP playground

Simple playground to try out mongodb cpp driver and what can be done.
It is not meant anyhow as project with a specific goal or purpose.
It has NOTHING to do with Runtastic GmbH or their Backend and Apps. 
I only selected the json-input files from the runtastic export, as i just
exported recently and had that data in my hands when starting this.


# Current functionality 

* Weights
    * Import weights from runtastic export into mongodb (files have to be placed at "data/Weights")

* Sessions
    * Import sport-sessions from runtastic export into mongodb (at "data/Sport-sessions")
    * Import sessions from garmin csv export (constraint: language: en, number format: 1,234.56)
    * Import sessions from gpx files (at "data/gpx")
    * Find/Read those documents
    * Check for existance of documents
    * List sessions, filter by from and to date, default to 30 days ago and now.
    * Delete a single session
    * Delete many (with e.g. start/end-time range)

* Calculations
    * Elevation refinement using SRTM3
    * Timezone calculation for GPX parser (via lat/lng and geonames)

* Statistics
    * Calculate track based stats (overall_distance, average_pace,...)
    * Distance, ElevationGain, Duration per year graphs
    * Sessions per weekday
    * Sessions per hour of day
    * Distance Buckets statistics

* Other
    * Simple menu for selecting some actions
    * Allow command line usage (ls, show, stats..)
    * Usage output (`./run help` and `./run help <cmd>`)
    * Multithreading (e.g. insert / parsing)
    * Configuration via config file (db name / host,..): `config.json`

## Next ideas

* Index for text + use this when searching
* Edit sessions
* Make pause detection threshold configurable

## Setup

Install the mongo c++ driver as described on the mongodb c++ page:

http://mongocxx.org/mongocxx-v3/installation/

Install docker + docker-compose to start a mongodb using this
repositories docker-compose.yml file. Or install a local mongo
or adapt the mongo connection uri to match your setup.

Necessary librares: 
* boost (on ubuntu it is: libboost-date-time-dev, libboost-dev)
* mongocxx + bsoncxx

## JSON Parsing

For json parsing I am using the json parser from nlohmann:

https://github.com/nlohmann/json

## Elevation refinment and SRTM3 data

Elevations are refined when parsing GPX data if the necessary .hgt file is 
available at `data/srtm3`. If necessary files are missing
it is printed afterwards. The elevation is not refined then for these
points. 

SRTM3 hgt files Can be downloaded there:

http://lic2.apemap.at/touren/hgt3/

https://e4ftl01.cr.usgs.gov/MEASURES/SRTMGL3.003/2000.02.11/index.html

## Geonames parsing

If a geonames city file is present (defaults to: "/data/cities1000.txt"), it checks
the timezone of a gpx parsed file by it's corrdinates to determine
the right timezone offset, it falls back to UTC.

You can also import cities to mongodb using the cities_import with the -file 
argument. The cities files can be downloaded there:

http://download.geonames.org/export/dump/

http://download.geonames.org/export/dump/cities500.zip

http://download.geonames.org/export/dump/cities1000.zip

It is recommended to import the cities to mongo:

```
./run cities_import -file=data/cities500.txt
```

As this would be much faster than reading the huge file on each import.

## Compilation

This project provides a simple makefile to compile the source files
and link it together into the `run` executable.

```
$ make all
```

Run it:

```
$ ./run
```

## Structure

I decided to split the files up in some sub-folders a bit to have at
least a bit of structure. As it is mainly a playground this does not follow
any specific patterns or rules. Currently i have the following folders:

* `data`, where the input files should be placed in
* `ext`, for external dependencies which are locally added (json.hpp)
* `helper`, for helper classes, like TimeConverter
* `models`, for the data classes
* `use_case`, for the various use-cases i want to provider (RunSessionImport, WeightImport,...)
* `mongo_db`, mongo related classes, e.g. connection or sessions and cities repositories.
* `parser`, various parser classes, e.g. json, gpx,..

## Questions 

If you have any questions regarding this "project" or this code,
don't hesitate to contact me.

## Contribution

If you want to contribute to this, and add some specific functionality,
don't hesitate to contact me or also just open an issue or pull-request. 
