FROM ubuntu:18.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
	apt-get upgrade -y && \
	apt-get install -y texlive-lang-cjk xdvik-ja evince texlive-fonts-recommended texlive-fonts-extra

ENTRYPOINT [ "/bin/bash" ]
