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
sudo docker build -t iir-queue-bot .
```

# Запуск контейнера (одноразового)
```
sudo docker run --rm -e BOT_TOKEN="8271031556:AAEnbmv-r2nN78h84EwUKmzq91dRBUNnSek" -v "$(pwd)/data:/data" iir-queue-bot
```

# Запуск контейнера (start/stop)
```
sudo docker run -d \
  --name iir-queue-bot \
  -e BOT_TOKEN="8271031556:AAEnbmv-r2nN78h84EwUKmzq91dRBUNnSek" \
  -v "$(pwd)/data:/data" \
  iir-queue-bot
```

# Удаление старого контейнера
```
sudo docker rm iir-queue-bot
```