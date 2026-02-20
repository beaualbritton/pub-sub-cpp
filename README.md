# c++ pub-sub message broker

> [!IMPORTANT]
> hi :trollface:

### run with docker
* only requirement is docker & docker compose! :^)

```
docker compose build
docker compose up
```


### run locally w/ cmake
* requires gcc (or any c compiler), cmake 4.1 + boost headers installed on system

```
cmake -S . -B build -G Ninja

cmake --build build --parallel
```

files found in ./build
