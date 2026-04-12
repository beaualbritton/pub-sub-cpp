FROM alpine:latest

RUN apk add build-base cmake ninja boost-dev postgresql-dev git netcat-openbsd

WORKDIR /app

COPY . .

RUN cmake -B build -G Ninja -DBROKER=ON 
RUN cmake --build build --target broker

CMD ["./build/broker"]
