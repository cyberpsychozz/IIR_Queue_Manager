# Dockerfile — ПОЛНОСТЬЮ РАБОЧИЙ ВАРИАНТ (tgbot-cpp собирается из исходников)
FROM ubuntu:24.04

# Устанавливаем всё необходимое
RUN apt-get update && apt-get install -y \
    cmake g++ make git \
    libboost-all-dev libssl-dev libcurl4-openssl-dev zlib1g-dev libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

# Копируем проект (build/ игнорируется через .dockerignore)
COPY . /app
WORKDIR /app

# --- СБОРКА tgbot-cpp ИЗ ИСХОДНИКОВ (единственный надёжный способ) ---
RUN git clone https://github.com/reo7sp/tgbot-cpp.git /tmp/tgbot-cpp && \
    cd /tmp/tgbot-cpp && \
    mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc) && \
    make install && \
    ldconfig && \
    rm -rf /tmp/tgbot-cpp

# --- Теперь собираем твой проект ---
RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make -j$(nproc)

# Запуск
CMD ["./build/start"]