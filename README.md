# Snake auf dem Arduino
Autor: Florian Königsmann
Beschreibung: Ein simples Snake Spiel, welches auf einer Neopixel Matrix gespielt werden kann.

## Spielablauf
Das Spiel wird gestartet, indem auf den Joystick von Spieler 1 gedrückt wird.

Die Schlange bewegt sich ab nun immer in Kopfrichtung (zu Beginn nach oben). Um die Richtung zu ändern,
kann mit dem Joystick nach links und rechts gelenkt werden (jeweils aus der Perspektive der Schlange).

Würde sich die Schlange außerhalb der Matrix bewegen oder in sich selbst fahren, ist das Spiel vorbei.
Das Spielfeld wird nun zurückgesetzt, indem auf den Joystick von Spieler 1 gedrückt wird. Bei erneutem
Druck startet das nächste Spiel.

In unregelmäßigen Abständen spawnt zufällig auf dem Spielfeld verteilt ein Item. Insgesamt kann es maximal
fünf Items geben. Wird das Item von der Schlange gefressen, so wird diese um einen Pixel Länger, und die Punktzahl
des Spielers erhöht sich. Alle fünf gefressene Items erhöht sich die Geschwindigkeit der Schlange ein wenig,
um das Spiel spannend zu erhalten.

## Hardware
* Arduino Uno Rev 3
* 16x2 [LCD Display HD44780](https://www.informatik.htw-dresden.de/~jvogt/mc/mc-lcd.html)
* Joystickmodul (+ analoger Joystick mittels [Gameport](https://de.wikipedia.org/wiki/Gameport))
* 16x16 Matrix [WS2812B](https://www.mikrocontroller.net/articles/WS2812_Ansteuerung) inklusive Spannungsversorgung mittels 50W Netzteil

## Software
* Arduino IDE
* LCD Bibliothek [LiquidCrystal](https://www.arduino.cc/reference/en/libraries/liquidcrystal/)
* WS2812B Bibliothek [FastLed](https://fastled.io/)

## Programmierung
* Mittels C++, Objektorientierter Ansatz
* Aufteilung in folgende Klassen
  * Snake.ino (Hauptprogramm)
  * Item (von Schlange fressbares Spielitem, welches auf der Matrix angezeigt wird)
  * game_settings.h (Spieleinstellungen)
  * hardware_settings.h (Hardwarekonfiguration)
  * Matrix (verwalten der LED Matrix)
  * Player (Spieler, welcher seine Schlange steuert)
  * Snake (Schlange, welche auf der Matrix angezeigt wird "Spielfigur")
  * Statistics (Statistikverwaltung, die auf dem LCD angezeigt wird)
