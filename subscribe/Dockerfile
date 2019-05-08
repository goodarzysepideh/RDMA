FROM ubuntu:16.04
RUN apt-get update --fix-missing
RUN apt-get install -y python
RUN apt-get install -y python-dev
RUN apt-get install -y python-pip
RUN apt-get install -y python-pycurl
RUN pip install sysv_ipc
COPY subscribe.py /
CMD ["python","/subscribe.py"]
