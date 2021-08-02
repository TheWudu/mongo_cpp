# MongoDB CPP playground

Simple playground to try out mongodb cpp driver and what can be done.
It is not meant anyhow as project with a specific goal or purpose.
It has NOTHING to do with Runtastic GmbH or their Backend and Apps. 
I only selected the json-input files from the runtastic export, as i just
exported recently and had that data in my hands when starting this.


# Current functionality 

* Import weights from runtastic export into mongodb (files have to be placed at "data/Weights")
* Import sport-sessions from runtastic export into mongodb (at "data/Sport-sessions")
* Import sessions from garmin csv export (constraint: language: en, number format: 1,234.56)
* Import sessions from gpx files (at "data/gpx")
* Find/Read those documents and convert to models
* Check for existance of documents
* Aggregations to calculate some statistics
* Simple menu for selecting some actions
* List sessions, filter by from and to date, default to 30 days ago and now.
* Allow command line usage (ls, show, stats..)
* Delete a single session
* Delete many (with e.g. start/end-time range)
* Usage output (`./run help` and `./run help <cmd>`)

## Next ideas

* Edit sessions
* Elevation refinement
* Timezone calculation for GPX parser (via lat/lng)

## Setup

Install the mongo c++ driver as described on the mongodb c++ page:

http://mongocxx.org/mongocxx-v3/installation/

Install docker + docker-compose to start a mongodb using this
repositories docker-compose.yml file. Or install a local mongo
or adapt the mongo connection uri to match your setup.

## JSON Parsing

For json parsing I am using the json parser from nlohmann:

https://github.com/nlohmann/json

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

If i add more and more stuff to mongo_db class, I maybe will split it up in multiple mongo classes,
and move them into some "repository" folder or so, let's see.

## Questions 

If you have any questions regarding this "project" or this code,
don't hesitate to contact me.

## Contribution

If you want to contribute to this, and add some specific functionality,
don't hesitate to contact me or also just open an issue or pull-request. 
