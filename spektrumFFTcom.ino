// ---------------- НАСТРОЙКИ ----------------
#define FFT_N 256                                       // ширина спектра х2
#define samplingFrequency 38400                         // частота дискретизации
// ---------------- НАСТРОЙКИ ----------------

// ---------------------- ПИНЫ ----------------------
#define AUDIO_IN 0                                      // пин, куда подключен звук
// ---------------------- ПИНЫ ----------------------

// --------------- БИБЛИОТЕКИ ---------------
#define LIN_OUT 1
#include <FFT.h>                                        // преобразование Фурье
#include <Wire.h>
#define printByte(args) write(args);
double prevVolts = 100.0;
// --------------- БИБЛИОТЕКИ ---------------
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

int Freq = 0;

void setup() {
  // поднимаем частоту опроса аналогового порта до 38.4 кГц, по теореме
  // Котельникова (Найквиста) частота дискретизации будет 19 кГц
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  sbi(ADCSRA, ADPS0);

  // для увеличения точности уменьшаем опорное напряжение,
  // выставив EXTERNAL и подключив Aref к выходу 3.3V на плате через делитель
  // GND ---[2х10 кОм] --- REF --- [10 кОм] --- 3V3
  analogReference(EXTERNAL);

  Serial.begin(9600);
  delay(2000);
}

void loop() {
  analyzeAudio();                                        // функция FFT, забивает массив fft_log_out[] величинами по спектру

  Serial.println("Computed magnitudes:");
  for (int pos = 0; pos < FFT_N / 2; pos++) {
    Freq = (pos * samplingFrequency) / FFT_N;
    Serial.print(Freq);
    Serial.print(" ");
    Serial.println(fft_lin_out[pos]);
  }
  Serial.println();
}

void analyzeAudio() {
  for (int i = 0 ; i < FFT_N ; i++) {
    int sample = analogRead(AUDIO_IN);
    fft_input[i] = sample;                              // помещаем данные в массив
  }
  fft_window();                                         // окно данных для лучшей частотной характеристики
  fft_reorder();                                        // переупорядочивание данных перед выполнением БПФ
  fft_run();                                            // обработка данных БПФ
  fft_mag_lin();                                        // вывод данных БПФ
}
