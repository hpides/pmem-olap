FROM ubuntu:16.04

RUN apt update \
&& apt install -y -q build-essential zlib1g-dev default-jdk \
&& apt clean \
&& rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

#ENV TZ=America/Los_Angeles
#RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

WORKDIR /root
ADD or-tools_ubuntu-16.04_v*.tar.gz .

RUN cd or-tools_*_v* && make test_java
