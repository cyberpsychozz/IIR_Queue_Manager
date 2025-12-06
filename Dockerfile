FROM ubuntu:24.04

# 1. Устанавливаем всё необходимое (Этот слой кэшируется, пока не изменится)
RUN apt-get update && apt-get install -y \
    cmake g++ make git \
    libboost-all-dev libssl-dev libcurl4-openssl-dev zlib1g-dev libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

# 2. Создаем директорию для данных (Кэшируется)
RUN mkdir -p /data
VOLUME /data

# 3. СБОРКА tgbot-cpp ИЗ ИСХОДНИКОВ (3+ минуты. Теперь он будет кэшироваться!)
# Мы временно переходим в /tmp для сборки библиотеки
WORKDIR /tmp
RUN git clone https://github.com/reo7sp/tgbot-cpp.git /tmp/tgbot-cpp && \
    cd /tmp/tgbot-cpp && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc) && \
    make install && \
    ldconfig && \
    rm -rf /tmp/tgbot-cpp

# --- СЕКЦИЯ, СБРОС КЭША В КОТОРОЙ БУДЕТ БЫСТРЫМ ---

# 4. Копируем проект и переходим в рабочую директорию (Сброс кэша только здесь)
COPY . /app
WORKDIR /app

# 5. Теперь собираем твой проект (Быстрый шаг, 3-4 секунды)
RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make -j$(nproc)

# 6. Запуск
CMD ["/app/build/iir_queue_manager"]