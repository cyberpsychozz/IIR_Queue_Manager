# Конфигурация библиотеки (Unix):
```
 cd libs/sqlite-autoconf-3500400
 ./configure
 make
```

# Конфигурация makefile:
```
    cd build
    rm -rf *  # Очистить старую конфигурацию
    cmake ..
    make
``` 

# Сборка Docker-контейнера
```
docker build -t iir-queue-bot .
```

# Запуск контейнера
```
docker run --rm -e BOT_TOKEN="8271031556:AAEnbmv-r2nN78h84EwUKmzq91dRBUNnSek" iir-queue-bot
```