Autor programu: Krzysztof Kulewski
Temat: Gra dla dwóch graczy - "Statki"


1. Krótki opis programu

Gra w "Statki" dla wielu graczy, na kwadratowej planszy.
Wygrywa ten, kto pierwszy zniszczy wszystkie statki przeciwnika.
Rozmiar planszy oraz liczba statków może zostać zmieniona - wydzielono stałe.


2. Merytoryczne uzasadnienie wyboru mechanizmu komunikacji międzyprocesowej

Procesy graczy komunikują się ze sobą przy użyciu pamięci współdzielonej.
Za synchronizację ruchów graczy odpowiada współdzielona struktura, zawierająca informacje
o aktualnej turze oraz ilości graczy.

Ponadto, zastosowanie mechanizmu monitorowania (select) obserwującego zarówno:
- zdarzenia okna Xlib,
- zdarzenia oparte o interwał czasowy (timeval),
pozwoliło automatyczne reagowanie na ruchy przeciwnika, tj. odświeżanie okna.


3. Opis użytkowania programu

Kompilacja:
gcc client.c -o client -lX11

Każdy z graczy uruchamia osobną instancję programu.
Po uruchomieniu, w dolnej części okna wyświetlane są dalsze instrukcje postępowania.
Gracze najpierw rozmieszczają swoje statki, a następnie naprzemiennie strzelają do przeciwnika.
Każdy statek składa się z dwóch przylegających do siebie części (w poziomie lub pionie).
Gra kończy się po zniszczeniu wszystkich statków bądź zamknięciu programu przez jednego z graczy.


4. Sytuacje wyjątkowe

Sytuacja: Serwer X jest niedostępny lub zmienna DISPLAY nie jest ustawiona.
Zachowanie: Wypisanie komunikatu błędu na konsolę oraz zakończenie działania programu.

Sytuacja: Otrzymanie sygnału SIGINT (CTRL+C).
Zachowanie: Okno zostaje zamknięte. Okno przeciwnika wyświetla wiadomość o wygranej.

Sytuacja: Przedwczesne zamknięcie okna przez gracza.
Zachowanie: Okno zostaje zamknięte. Okno przeciwnika wyświetla wiadomość o wygranej.

Sytuacja: Próba dołączenia do gry przez trzeciego gracza.
Zachowanie: Wypisanie stosownej informacji na konsolę oraz zakończenie działania programu.

Sytuacja: Próba ustawnienia statku poza własną planszą.
Zachowanie: Wyświetlenie stosownej wiadomości w oknie oraz zignorowanie ruchu.

Sytuacja: Próba wykonania ruchu podczas nieswojej kolejki.
Zachowanie: Wyświetlenie stosownej wiadomości w oknie oraz zignorowanie ruchu.

UWAGA: pamięć współdzielona jest czyszczona niezależnie od wystąpienia błędu.
