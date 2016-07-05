FROM ubuntu:trusty
RUN apt-get -qq update && apt-get -qq install gcc-4.8
ADD . /code

