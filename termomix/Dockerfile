#    docker build -t artifactory.hi.inet/analytics/analytics-speech-text .
# to be run with:
#   docker run -it -v /var/run/docker.sock:/var/run/docker.sock -v /home/zoraida/transcripts:/opt/analytics-text-data artifactory.hi.inet/analytics/analytics-speech-text
FROM centos:7

# Installing docker 1.7.1 to be compatible with default RHEL7 docker
ADD https://get.docker.com/builds/Linux/i386/docker-1.7.1 /opt/docker/docker
RUN \
  chmod +x /opt/docker/docker && \
  ln -s /opt/docker/docker /usr/bin/docker

# Install dependencies for python packages
RUN \
	yum install -y gcc-c++ && \
        yum install -y lapack-devel && \
        yum install -y blas-devel && \
	yum install -y freetype-devel && \
	yum install -y libpng-devel && \
	yum install -y epel-release && \
	yum install -y python-pip && \
	yum install -y python-devel 

# Install freeling dependencies
RUN \

        yum install -y boost-devel && \
	yum install -y libicu-devel && \
	yum install -y zlib-devel && \
	yum install -y automake && \
	yum install -y autoconf && \
	yum install -y libtool && \
        yum groupinstall -y "Development Tools"

COPY requirements.txt .

# Install python dependencies
RUN \
        pip install -r requirements.txt

# Install Freeling(at the moment of building this, 4.0)
# docker build --build-arg FREELING_DIR=/other/directory
ARG FREELING_DIR=/usr/local/freeling
RUN \
        mkdir /freeling-4 && \
        git clone https://github.com/TALP-UPC/FreeLing.git freeling-4 && \
        cd /freeling-4 && \
        git checkout -b freeling-4 4.0 && \
        autoreconf --install && \
        ./configure --prefix=$FREELING_DIR && \
        make && \
        make install

# Build and install python wrapper for Freeling
RUN \ 
        cd /freeling-4/APIs/python && \
        sed -i -e "s|FREELINGDIR = /usr/local|FREELINGDIR = $FREELING_DIR|g" Makefile && \
        sed -i -e "s|PYTHONVER = python3.4|PYTHONVER = python2.7|g" Makefile && \
        sed -i -e "s|-py3||g" Makefile && \
	make && \
	cp _freeling.so /usr/lib64/python2.7/lib-dynload && \
	cp freeling.py /usr/lib64/python2.7/site-packages

# Clean
RUN \
	cd / && \
	rm -rf freeling-4 && \
	rm requirements.txt	

COPY . /opt/analytics-speech-text

RUN \
      	pip install jupyter
# Define default command.
CMD ["ipython notebook"]

ENV LD_LIBRARY_PATH $FREELING_DIR/lib
