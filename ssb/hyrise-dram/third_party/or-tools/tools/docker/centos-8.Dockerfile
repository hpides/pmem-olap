FROM centos:8 AS env

#############
##  SETUP  ##
#############
RUN dnf -y update \
&& dnf -y groupinstall 'Development Tools' \
&& dnf -y install wget redhat-lsb-core pkgconfig autoconf libtool cmake zlib-devel which \
&& dnf clean all \
&& rm -rf /var/cache/dnf
#pkgconfig

# Install Swig
RUN dnf -y update \
&& dnf -y install swig \
&& dnf clean all \
&& rm -rf /var/cache/dnf

# Install Java 8 SDK
RUN dnf -y update \
&& dnf -y install java-1.8.0-openjdk  java-1.8.0-openjdk-devel \
&& dnf clean all \
&& rm -rf /var/cache/dnf

# Install dotnet
# see https://docs.microsoft.com/en-us/dotnet/core/install/linux-package-manager-centos8
RUN dnf -y update \
&& dnf -y install dotnet-sdk-3.1 \
&& dnf clean all \
&& rm -rf /var/cache/dnf
# Trigger first run experience by running arbitrary cmd
RUN dotnet --info

ENV TZ=America/Los_Angeles
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

################
##  OR-TOOLS  ##
################
FROM env AS devel
# Copy the snk key
COPY or-tools.snk /root/or-tools.snk
ENV DOTNET_SNK=/root/or-tools.snk

ARG SRC_GIT_BRANCH
ENV SRC_GIT_BRANCH ${SRC_GIT_BRANCH:-master}
ARG SRC_GIT_SHA1
ENV SRC_GIT_SHA1 ${SRC_GIT_SHA1:-unknown}

# Download sources
# use SRC_GIT_SHA1 to modify the command
# i.e. avoid docker reusing the cache when new commit is pushed
WORKDIR /root
RUN git clone -b "${SRC_GIT_BRANCH}" --single-branch https://github.com/google/or-tools \
&& echo "sha1: $(cd or-tools && git rev-parse --verify HEAD)" \
&& echo "expected sha1: ${SRC_GIT_SHA1}"

# Build third parties
FROM devel AS third_party
WORKDIR /root/or-tools
RUN make detect && make third_party

# Build project
FROM third_party AS build
RUN make detect_cc && make cc
RUN make detect_java && make java
RUN make detect_dotnet && make dotnet
