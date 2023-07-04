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
