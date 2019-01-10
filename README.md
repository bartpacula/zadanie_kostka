### Zadanie - Kostka
## Autor: Bartosz Pacuła 
* email: <mailto:bartpacula@yahoo.com>
* LinkedIn: <https://www.linkedin.com/in/bartoszpacula>
## O programie
* Data: Styczeń 2019r. 
* Wersja: 1.0
* Wersja biblioteki OpenCV: 3.2.0
* Wersja Visual Studio: Microsoft Visual Studio Community 2017 v.15.9.4
* Aplikacja konsoli Windows (architektura: win32)

Program wyświetla kostkę (sześcian) z nałożonymi na ściany teksturami (przy pomocy mapowania perspektywicznego) i umożliwia obracanie kostki wokół trzech osi za pomocą klawiszy sterujących.
## Dodatkowe informacje

Sterowanie:

(przy aktywnym oknie wyświetlającym obraz kostki)

klawisze a" i "z" - obrót kostki względem osi x

klawisze "s" i "x" - obrót kostki względem osi y

klawisze "d" i "c" - obrót kostki względem osi z

klawisz "q" - wyjście z programu

Obrót z krokiem 5 stopni.

W celu poprawnego działania programu w zmiennej systemowej PATH musi być ustawiona ścieżka do plików .dll biblioteki OpenCV używanych przez program.

Pliki te to:

opencv_core320.dll

opencv_imgproc320.dll

opencv_highgui320.dll

opencv_imgcodecs320.dll

opencv_calib3d320.dll

W razie potrzeby mogę dodać je do repozytorium.

## Autorskie pliki programu:

kostka.hpp

zadanie_kostka.cpp
