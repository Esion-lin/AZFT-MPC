FROM i386/ubuntu
WORKDIR /MPC_HW

COPY . /MPC_HW

RUN sed -i s@/archive.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list && apt-get update && apt-get install -y autoconf automake libtool build-essential cmake openssl libssl-dev \
	&& rm CMakeCache.txt \
	&& cmake -DCMAKE_BUILD_TYPE=Release \
	&& make

WORKDIR /MPC_HW
