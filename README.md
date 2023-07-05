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
* 16x2 [LCD HD44780](https://www.informatik.htw-dresden.de/~jvogt/mc/mc-lcd.html)
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

### game_settings
| Name | Bedeutung | Standardwert | Anmerkung
| --- | --- | --- | --- |
| PLAYERS | Anzahl der Spieler | 1 | :warning: Aktuell wird nur ein Spieler unterstützt, diese Definition hat damit keinerlei Wirkung
| MAX_ITEMS |  Maximale Anzahl der Items, die auf dem Spielfeld generiert werden können | 5 | -
| DEFAULT_ITEM_SPAWN_TICKS | Gibt an, aller wieviel Ticks ein Item standardmäßig generiert werden soll | 100 | :warning: Es handelt sich lediglich um den Standardwert, der tatsächliche Wert wird mittels einer Zufallsfunktion in Abhängigkeit vom Standardwert generiert, siehe [Itemgenerierung](https://github.com/HTWDD-RN/s83676-mc-beleg/edit/main/README.md#gametick)
| BLINK_TICKS | Anzahl an Ticks, nach denen die Animation weiter fortschreitet | 10 | -

### hardware_settings
| Name | Bedeutung | Standardwert | Anmerkung
| --- | --- | --- | --- |
|LEDPIN | PIN, an dem die LED Matrix angeschlossen ist | 8 | -
|LEDCOUNT | Anzahl der LEDS in der Matrix | 256 | :warning: Aktuell werden nur quadratische Matrizen unterstützt (z.B. hier 16x16 -> 256)
|BUTTONPIN | Pin, an dem der Button angeschlossen ist, mit dem das Spiel gestartet bzw. neu gestartet werden kann | 7 | Im Normalfall wird der Joystick Knopf genommen, es kann aber auch ein anderer Knopf verwendet werden. Dann allerdings unbedingt prüfen, ob ein pullup Widerstand erforderlich ist!
|JOYSTICKX | Pin, an dem die X-Achse des Joysticks angeschlossen ist | A2 | Je nach verwendetem Joystick muss der Auslösebereich manuell eingestellt werden. Dafür gibt es bisher keine Einstellung, dies muss um Quellcode gemacht werden, siehe [Kommt noch](#todo)


### Snake.ino
* einbinden aller Bibliotheken
* LED Matrix, LCD, Spieler, Statistiken, Items (sowie dafür notwendige Parameter) initialisieren
* *ticksSinceLastItem*... Anzahl der Spielticks, die seit der Generierung des letzten Items vergangen sind.
* *nextItemTickCount*... Anzahl der Spielticks, die vergehen sollen, bis das nächste Item generiert wird.
* *ticksSinceLastAnimation*... Anzahl der Spielticks, die seit der letzten änderung der Animation vergangen sind. Dies wird benötigt, um die Blinkgeschwindigkeit der Schlange nach Spielende zu steuern.
* *gameState*... Aktueller Zustand des Spiels:
  * 0: Spiel läuft nicht, kann gestartet werden.
  * 1: Spiel wurde beendet, kann neu gestartet werden.
  * 2: Spiel läuft.
* *lastGameTick*... Zeitstempel (in ms) des letzten Spielticks
* *buttonPressed*... Wurde der Knopf gedrückt
  * Wird auf true gesetzt, sobald Knopf gedrückt wird.
  * Wird auf false gesetzt, sobald Knopf losgelassen wird.

#### setup()
* Serielle Schnitstelle öffnen mit Baudrate 9600 (nur für Debugging Zwecke)
* PIN des Knopfes als Eingang definieren (Pull Up Widerstand vermutlich bereits auf Joystick vorhanden, sonst wäre hier INPUT_PULLUP besser)
* Spielern Verweis auf Items (Array) übergeben
* Matrix zeichnen, Schlange nun erkennbar
* Standardwerte festlegen

### loop()
Wird der Knopf losgelassen, wird der Eingang des Button LOW. Es wird sofort *buttonPressed* auf false gesetzt, um auf weitere Knopfeingaben reagieren zu können.
Das wird benötigt, da der Knopf aufgrund der Trägheit des Nutzes in der Regel mehrere loop Durchgänge gedrückt bleibt. Es soll verhindert werden, dass in diesem Fall das Spiel beispielsweise mehrfach zurückgesetzt wird oder direkt nach dem zurücksetzen startet. Jede Knopfabfrage, die dieses Feature benötigt, muss neben der digitalen Abfrage des Knopfzustandes auch noch prüfen, ob *buttonPressed* == false ist. 
```C++
if(!digitalRead(BUTTONPIN)) {
  buttonPressed = false;
}
```

Befindet sich das Spiel im Status 0, kann also gestartet werden, und der Knopf wurde gedrückt, wird *lastGameTick* auf die aktuelle Zeit zurückgesetzt, sowie der Spielstatus auf 2 gesetzt. Um mehrfache Eingaben zu verhindern, wird *buttonPressed* auf true gesetzt. Wie oben erläutert, dient dies dazu die Knopfeingabe zu "konsumieren", also für die weitere Verarbeitung zu sperren, bis der Knopf losgelassen wird.
```C++
if(gameState == 0 && digitalRead(BUTTONPIN) && !buttonPressed) {
  lastGameTick = millis();
  gameState = 2;
  buttonPressed = true;
}
```
Wird der Knopf im Spielzustand 1 gedrückt, wird der Spieler und die Statistik zurückgesetzt und angezeigt. Außerdem wird der Spielstatus auf 0 gesetzt, sodass das Spiel beim nächsten Knopfdruck gestartet werden kann.
Außerdem werden alle generierten Items gelöscht, indem das Array durchlaufen und für jedes Item *clear()* aufgerufen wird.
```C++
player.reset(); stats.reset(); stats.printPlayers();
gameState = 0;
for(int i = 0; i < MAX_ITEMS; i++) {
  items[i].clear();
}
```
Nun wird der X-Wert des Joysticks ausgelesen und zur Verarbeitung an die Player Funktion *handleJoystick(int joyStickX)* übergeben. Die Verarbeitung der Eingabe erfolgt nun in dieser Klasse.
```C++
int readX = analogRead(JOYSTICKX);
player.handleJoyStick(readX);
```
Damit das Spiel gleichmäßig läuft, wird es in Ticks unterteilt. Alle 50ms wird in der Hauptschleife ein Tick ausgelöst, in welchem für das Spiel relevante Berechnungen und Aktionen durchgeführt werden. Die Spielfunktionalität wird in der Funktion *gameTick()* untergebracht. Außerdem wird der Zeitstempel *lastGameTick* auf die aktuelle Zeit zurückgesetzt, da immer geprüft wird, ob die Differenz aus der aktuellen Zeit und diesem Zeistempel größer als 50 (ms) ist.
```C++
if(millis() - lastGameTick > 50) {
  lastGameTick = millis();
  gameTick();
}
```
#### gameTick()
In dieser Funktion werden alle Spielrelevanten Aktionen durchgeführt. Dazu zählen das erstellen von Items, sowie das Fortbewegen der Schlange. Es wird aber auch geprüft, ob der Spieler noch lebt und die entsprechende Statistik gespeichert und angezeigt.

Als erstes wird (falls nötig) das Item generiert. Dafür wird geprüft, ob mehr als *nextItemTickCount* Ticks vergangen sind. Ist dies nicht der Fall, wird kein Item erzeugt. Um zu verhindern, dass bei einem pausiertem Spiel Items generiert werden, wird *ticksSinceLastItem* in diesem Fall immer zurückgesetzt, sodass die Bedingung nicht wahr werden kann.
Tritt die Bedingung ein, wird *ticksSinceLastItem* ebenfalls zurückgesetzt, da ja nun versucht wird ein Item zu generieren. Nach der Funktion wird *ticksSinceLastItem* um eins inkrementiert, da ein weiterer Tick seit der letzten Item generierung vergangen ist.

Für die X, sowie Y Position wird ein zufälliger Wert im Intervall [0; 15] erstellt. Anschließend wird die Liste der Items durchlaufen. Für das Item an der aktuellen Position wird geprüft, ob es existiert und falls ja wird dieser Schleifendurchlauf übersprungen, da für ein bereits vorhandenes Item kein neues generiert werden soll. Existiert ein Item noch nicht, so wird versucht das Item an der randomisierten Position zu erstellen. Ist die Erstellung erfolgt, gibt die Funktion true zurück und die Schleife kann verlassen werden, da sowieso nur ein Item generiert werden soll. Konnte das Item nicht generiert werden, da es beispielsweise auf einer Schlange gespawnt wäre, wird es wieder entfern, um keinen Platz für nicht vorhandene Items zu verschwenden. Da alle weiteren Items auch an dieser Position erstellt werden würden, kann die Schleife auch in diesem Fall abgebrochen werden. Anschließend wird mittels Zufall bestimmt, in wie vielen Ticks das nächste Item genertiert werden soll. Dafür wird der Standardwert mit einem Wert zwischen 0,1 und 1,99 multipliziert.
```C++
byte randX = random(0, 16);
byte randY = random(0, 16);
for(int i = 0; i < MAX_ITEMS; i++) {
  if(items[i].exists()) continue;

  boolean allowedGeneration = items[i].generateItem(randX, randY, 0);

  if(!allowedGeneration) {
    items[i].remove();
  }
  break;
}
nextItemTickCount = DEFAULT_ITEM_SPAWN_TICKS * random(10, 200) / 100.0;
```

Anschließend wird im Fall eines laufenden Spiels der Spieler (bzw. seine Schlange) nach vorne bewegt. Abhängig von der Geschwindigkeit wird die Schlange nicht in jedem Tick weiter bewegt, genaueres dazu in der Klasse Snake. Die Funktion gibt zurück, ob die Schlange die Bewegung überlebt hat und speichert dies in die Variable *survived*. Anschließend wird die der Score des Spielers an die Statistik Klasse übergeben. Hat der Spieler nicht überlebt, wird dieser getötet (Hinweis: es wäre eventuell sinnvoller, dies automatisch in der Snake Klasse zu machen) und in der Statistik ebenfalls hinterlegt, dass der Spieler gestorben ist. Anschließend wird der Spielstatus auf 1 gesezt, da diese Runde mit dem Tod eines Spielers beendet ist. Anschließend wird die Statistik auf dem LCD ausgegeben.

```C++
if(gameState == 2) {
  bool survived = player.moveForward();

  stats.setPlayerScore(0, player.getScore());
  if(!survived) {
    player.die();
    stats.playerDead(0);
    gameState = 1;
  }
  stats.printPlayers();
}
```

Ist der Spieler gestorben, wird abschließend eine Blinkanimation abgespielt. Da diese mit der eigentlichen Tickrate viel zu schnell wäre, wird sie nur alle in *BLINK_TICKS* festgelegten Ticks ausgeführt. Dafür wird ebenfalls wieder gezählz, wie viele Ticks seit der letzten Animation vergangen sind bzw. die Tick Anzahl wird mit jedem Animationsschritt zurückgesetzt.
Die Methode *blink()* der Klasse Snake sorgt automatisch dafür, dass die richtigen Farben angezeigt werden.

```C++
if(player.dead() && ticksSinceLastAnimation > BLINK_TICKS) {
  ticksSinceLastAnimation = 0;
  player.blink();
}
ticksSinceLastAnimation++;
```
Anschließend werden alle in diesem Tick ausgeführten Spielveränderungen auf der LED MAtrix angezeigt.
```C++
m.show();
```
