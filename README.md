# Projeto: Interface de Comunicação Serial com RP2040

## 🚀 Descrição
Este projeto implementa a comunicação serial no microcontrolador RP2040, utilizando a placa BitDogLab. As principais funcionalidades incluem a exibição de caracteres no display SSD1306, controle de LEDs RGB e uma matriz de LEDs WS2812, com interação através de botões.

## 🛠️ Componentes Utilizados
- **Microcontrolador:** RP2040 (BitDogLab)
- **Display:** SSD1306 (I2C - GPIO 14 e 15)
- **LED RGB:** GPIOs 11 e 12
- **Matriz 5x5 WS2812:** GPIO 7
- **Botões:**
  - 🔘 Botão A - GPIO 5
  - 🔘 Botão B - GPIO 6

## 🎯 Funcionalidades
1. **Entrada de caracteres via PC**
   - O Serial Monitor do VS Code permite a digitação de caracteres.
   - Cada caractere é exibido no display SSD1306.
   - Se um número entre 0 e 9 for digitado, o respectivo padrão será mostrado na matriz 5x5 WS2812.

2. **Interação com Botões**
   - **🔘 Botão A:** Alterna o estado do LED Verde e exibe uma mensagem no display SSD1306.
   - **🔘 Botão B:** Alterna o estado do LED Azul e exibe uma mensagem no display SSD1306.
   - Ambas as operações também são registradas no Serial Monitor.

## ✅ Requisitos Implementados
- ✅ **Uso de interrupções** para o acionamento dos botões.
- ✅ **Debouncing via software** para evitar leituras incorretas.
- ✅ **Controle de LEDs WS2812 e LEDs comuns.**
- ✅ **Exibição de caracteres no display SSD1306** (maiúsculas e minúsculas adicionadas na `font.h`).
- ✅ **Envio de informações via UART** para o Serial Monitor.
- ✅ **Código bem estruturado e comentado**.

## ▶️ Como Executar o Código
1. **🔧 Configurar o ambiente:**
   - Instale o SDK do Raspberry Pi Pico.
   - Configure o CMake e a toolchain do Pico.
   - Clone o repositório e navegue até a pasta do projeto.

2. **💻 Compilar e carregar o código:**

3. **🔍 Abrir o Serial Monitor:**
   - Use o Serial Monitor VS Code para monitorar a comunicação.

## 🎥 Demonstração em Vídeo
📌 [Veja a demonstração aqui](https://drive.google.com/file/d/1ZytfPPYa_Kfk8jPyoE3uOQt6GCZDceW5/view?usp=sharing)

