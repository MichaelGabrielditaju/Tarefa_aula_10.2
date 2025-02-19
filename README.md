Controle de LEDs e Display com Joystick no RP2040

Descrição do Projeto

Este projeto tem como objetivo consolidar os conceitos sobre o uso de conversores analógico-digitais (ADC) no RP2040, utilizando a placa de desenvolvimento BitDogLab. O sistema utiliza um joystick para controlar a intensidade de LEDs RGB e mover um quadrado no display SSD1306, além de interagir com botões para alterar estados do sistema.

Adicional: Optei por adicionar também a funcionalidade de quando apertar o botão B, o quadrado aumentar de tamanho até um limite pré estabelecido, após isso ele volta ao tamanho inicial.

Objetivos

Compreender o funcionamento do conversor analógico-digital (ADC) no RP2040.

Utilizar o PWM para controlar a intensidade de dois LEDs RGB com base nos valores do joystick.

Representar a posição do joystick no display SSD1306 por meio de um quadrado móvel.

Aplicar o protocolo de comunicação I2C na integração com o display.

Funcionalidades Implementadas

Controle dos LEDs RGB

LED Azul: O brilho é ajustado com base no eixo Y do joystick. Na posição central, o LED permanece apagado. Movimentos para cima ou para baixo aumentam gradualmente a intensidade, atingindo o máximo nos extremos.

LED Vermelho: Segue o mesmo princípio, mas baseado no eixo X do joystick. Na posição central, o LED está apagado, e seu brilho aumenta ao mover o joystick para a esquerda ou direita.

Os LEDs são controlados via PWM para proporcionar uma transição suave de intensidade.

Exibição no Display SSD1306

Um quadrado de 8x8 pixels representa a posição do joystick.

O movimento do quadrado é proporcional aos valores do joystick.

O display é atualizado via comunicação I2C.

Funcionalidade dos Botões

Botão do Joystick:

Alterna o estado do LED Verde a cada acionamento.

Modifica a borda do display para indicar a ativação, alternando estilos de borda.

Botão A:

Ativa ou desativa os LEDs RGB controlados por PWM.

Componentes Utilizados

LED RGB (conectado às GPIOs 11, 12 e 13).

Botão do Joystick (conectado à GPIO 22).

Joystick (conectado às GPIOs 26 e 27).

Botão A (conectado à GPIO 5).

Botão B (conectado à GPIO 6).

Display OLED SSD1306 (conectado via I2C nas GPIOs 14 e 15).

Requisitos Técnicos

Uso de interrupções: As funcionalidades dos botões são implementadas via interrupções (IRQ).

Debouncing: Implementação do tratamento de bouncing dos botões via software.

Utilização do Display 128x64: Implementação gráfica para manipulação de dados e exibição de informações.

Código bem estruturado e comentado: Facilita o entendimento e manutenção.

Entrega do Projeto

Código-fonte: Disponível neste repositório.

Vídeo de demonstração: Explicação e demonstração prática do projeto.

Link: https://youtube.com/shorts/hThVZhI1TlY?feature=share
