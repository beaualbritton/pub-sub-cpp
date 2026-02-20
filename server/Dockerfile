FROM alpine:latest

RUN apk add build-base cmake ninja boost-dev

WORKDIR /app

COPY . .

RUN cmake -B build -G Ninja && cmake --build build --target server

CMD ["./build/server"]
