Projekt FreeRTOS ESP32 - Inteligentna Sygnalizacja Świetlna
===========================================================

Projekt demonstrujący użycie systemu czasu rzeczywistego FreeRTOS w środowisku Arduino. Program realizuje system inteligentnej sygnalizacji świetlnej, który po wykryciu pojazdu (lub ruchu) odlicza czas na wyświetlaczu 7-segmentowym, a następnie sekwencyjnie zmienia światła i komunikaty na wyświetlaczu LCD.

Wykorzystane biblioteki
-----------------------
- Arduino.h - główna biblioteka środowiska
- Wire.h - obsługa magistrali I2C (wymagana dla wyświetlacza)
- LiquidCrystal_I2C.h - obsługa wyświetlacza LCD znakowego
- task.h i semphr.h - wbudowane biblioteki FreeRTOS dla ESP32 do obsługi wielozadaniowości

Zmienne globalne i struktury danych
-----------------------------------
- Piny rejestru przesuwnego - stałe przypisujące piny ESP32 (19, 18, 17) do układu 74HC595 (data, latch, clock), sterującego wyświetlaczem 7-segmentowym.
- Piny systemowe - stałe przypisujące piny dla czujnika ruchu (26) oraz trzech diod sygnalizatora (27 - czerwona, 32 - żółta, 33 - zielona).
- lcd - instancja obiektu wyświetlacza LCD o adresie 0x27 i rozmiarze 16x2.
- sygnal - semafor binarny informujący system o wykryciu ruchu.
- taskhandle - uchwyt zadania umożliwiający bezpośrednie powiadomienie modułu sterującego światłami.
- digits[10] - tablica bajtów przechowująca stany logiczne potrzebne do wyświetlenia cyfr od 0 do 9 na wyświetlaczu 7-segmentowym ze wspólną anodą/katodą.
- digitOff - zmienna bajtowa przechowująca stan wygaszenia wyświetlacza 7-segmentowego (wszystkie segmenty wyłączone).

Funkcje pomocnicze
------------------
- wyswietlLiczby(byte wartosc) - funkcja manipulująca pinem latch oraz używająca wbudowanej funkcji shiftOut do szeregowego wysłania bajtu danych do rejestru przesuwnego.

Opis zadań (Tasks)
------------------

TaskCzujnik:
- Działa w nieskończonej pętli z interwałem 1000 ms.
- Odczytuje stan wejścia cyfrowego czujnika ruchu.
- Po wykryciu sygnału wysokiego (HIGH) wypisuje komunikat na port szeregowy i podnosi semafor (sygnal).
- Po wykryciu ruchu zadanie usypia się na 25 sekund, co stanowi mechanizm zabezpieczający przed ciągłym wyzwalaniem cyklu.

TaskOdliczanie:
- Oczekuje w zawieszeniu (bez obciążania procesora) na podniesienie semafora przez czujnik.
- Po wybudzeniu uruchamia pętlę od 0 do 9.
- W każdej iteracji wysyła do rejestru przesuwnego kolejną cyfrę z tablicy digits i usypia zadanie na 1000 ms (1 sekunda).
- Po zakończeniu 10-sekundowego odliczania wygasza wyświetlacz 7-segmentowy.
- Wysyła bezpośrednie powiadomienie (xTaskNotifyGive) do zadania TaskLampki.

TaskLampki:
- Działa w nieskończonej pętli, oczekując na bezpośrednie powiadomienie systemowe (ulTaskNotifyTake).
- Po wybudzeniu rozpoczyna sekwencję zmiany świateł:
  1. Wyłącza diodę czerwoną i czyści pierwszy wiersz wyświetlacza LCD.
  2. Włącza diodę żółtą i wyświetla na LCD komunikat "przygotuj sie". Czeka 1 sekundę.
  3. Wyłącza diodę żółtą i czyści LCD.
  4. Włącza diodę zieloną i wyświetla w drugim wierszu LCD komunikat "mozesz jechac". Czeka 10 sekund.
  5. Wyłącza diodę zieloną, czyści LCD, włącza diodę czerwoną i ponownie wyświetla komunikat "czerwone swiatlo".

Konfiguracja główna
-------------------

Setup:
- Uruchamia komunikację szeregową (baud rate 9600).
- Konfiguruje piny wyjściowe dla diod oraz rejestru przesuwnego.
- Konfiguruje pin czujnika jako wejście z wewnętrznym rezystorem ściągającym do masy (INPUT_PULLDOWN).
- Inicjalnie wygasza wyświetlacz 7-segmentowy, wysyłając same zera do rejestru.
- Inicjalizuje wyświetlacz LCD, włącza podświetlenie i ustawia domyślny stan początkowy (zapalona czerwona dioda, napis "czerwone swiatlo").
- Tworzy semafor binarny w pamięci.
- Rejestruje we FreeRTOS trzy opisane wyżej zadania, przydzielając każdemu stos 2048 bajtów oraz priorytet 1.

Loop:
- Pętla pozostaje pusta. Zarządzanie poszczególnymi funkcjami sygnalizacji i odliczania realizowane jest w 100% asynchronicznie przez planistę (schedulera) FreeRTOS.
