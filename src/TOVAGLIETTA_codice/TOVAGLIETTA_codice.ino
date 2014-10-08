/*-------------------------------------------TOVAGLIETTA_codice.ino----#

                         SNIP

 
Copyright (c) 2014 Maddalena Sommadossi
Copyright (c) 2014 Matteo Perini

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the

GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc.,59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
-----------------------------------------------------------------------
*/

// Codice per la creazione di una tovaglietta interattiva che facilita la comunicazione in soggetti autistici.

/* 
Valori soglia dei sensori capacitivi:
- sensori A4, A3, A0, 5 = 2500
- sensori A1, A2, 8, 9 = 4000
*/

/* 
Riferimenti numeri pin - simbolo:
A0 = Ancora
A1 = Buono
A2 = Per piacere
A3 = Casa
A4 = Bagno
A5 = Aiutami
5 = Non buono
8 = Basta
9 = Grazie
*/

// Carico la libreria che mi permetterà permetterà di calibrare ed utilizzare i sensori capacitivi
#include <SPI.h>
// Carico le librerie che mi serviranno per far funzionare la shield mp3 "Adafruit Music Maker"
#include <Adafruit_VS1053.h>
#include <SD.h>

// Definisco alcuni valori che utilizzerò per la calibrazione dei sensori capacitivi
#define resolution 8
#define mains 50 // 60: north america, japan; 50: most other places

#define refresh 2 * 1000000 / mains

// Definisco i pin utilizzati dalla shield mp3
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)
#define CARDCS 4     // Card chip select pin
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

// Definisco il numero di sensori
#define TRIGGER_COUNT 9

// Definisco un valore zero per ogni sensore, il quale verrà utilizzato per la calibrazione dei sensori
long zeroA = 0;
long zeroB = 0;
long zeroC = 0;
long zeroD = 0;
long zeroE = 0;
long zeroF = 0;
long zeroG = 0;
long zeroH = 0;
long zeroI = 0;

// Inizializzo per ogni sensore una variabile che mi servirà come variabile temporanea durante la calibrazione
long tempValueA;
long tempValueB;
long tempValueC;
long tempValueD;
long tempValueE;
long tempValueF;
long tempValueG;
long tempValueH;
long tempValueI;

// Inizializzo per ogni sensore una variabile che rappresenta il valore del tempo di carica, necessario per leggere la differenza di capacità del sensore quando viene toccato
long normalizedA;
long normalizedB;
long normalizedC;
long normalizedD;
long normalizedE;
long normalizedF;
long normalizedG;
long normalizedH;
long normalizedI;

// Definisco i pin dei vari sensori
#define SENSOR_A A0
#define SENSOR_B A1
#define SENSOR_C A2
#define SENSOR_D A3
#define SENSOR_E A4
#define SENSOR_F A5
#define SENSOR_G 5
#define SENSOR_H 8
#define SENSOR_I 9

// Creo un oggetto rappresentante il "lettore musicale"
Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

// Definisco un array contentente i file audio presenti sulla scheda micro-SD
char* audioNames[] = {"track001.mp3", "track002.mp3", "track003.mp3", "track004.mp3", "track005.mp3", "track006.mp3", "track007.mp3", "track008.mp3", "track009.mp3"};
// Definisco un array contente i nomi dei file presenti sulla scheda micro-SD, da utilizzare per debug (decommentare la prossima riga in caso di debug)
//char* audio[] = {"Ancora", "Mi piace", "Per piacere", "Casa", "Bagno", "Aiutami", "Non mi piace", "Basta", "Grazie"};  

// Definisco una variabile che mi servirà per avvisare quando è finita la calibrazione
char* beepSound = "track010.mp3";

#define SOGLIA 4000
#define SOGLIA_MIN 2000
String stringOne="Normalized ";
// Setup del sistema
void setup() {
   
  // Carico la libreria Serial per debug... decommentare la prossima riga in caso di utilizzo
  Serial.begin(9600);  
  Serial.println("Serial begin");
  
  // Definisco la modalità di utilizzo dei sensori
  pinMode(SENSOR_A, INPUT);
  pinMode(SENSOR_B, INPUT);
  pinMode(SENSOR_C, INPUT);
  pinMode(SENSOR_D, INPUT);
  pinMode(SENSOR_E, INPUT);
  pinMode(SENSOR_F, INPUT);
  pinMode(SENSOR_G, INPUT);
  pinMode(SENSOR_H, INPUT);
  pinMode(SENSOR_I, INPUT);

  // Avvio il timer
  startTimer();
  
  // Calibrazione dei sensori: misuro il tempo di carica cambiando rapidamente lo stato del pin, leggo 100 valori per ottenere lo zero
  for(int i=0;i<100;i++)
  {
    tempValueA=getDistance(time(SENSOR_A, B00000001));
    if (tempValueA>zeroA) { zeroA=tempValueA;}
    Serial.println("ZeroA");
    
    tempValueB=getDistance(time(SENSOR_B, B00000010));
    if (tempValueB>zeroB) { zeroB=tempValueB;}
        Serial.println("ZeroB");

    
    tempValueC=getDistance(time(SENSOR_C, B00000100));
    if (tempValueC>zeroC) { zeroC=tempValueC;}
        Serial.println("ZeroC");

    
    tempValueD=getDistance(time(SENSOR_D, B00001000));
    if (tempValueD>zeroD) { zeroD=tempValueD;}
        Serial.println("ZeroD");

    
    tempValueE=getDistance(time(SENSOR_E, B00010000));
    if (tempValueE>zeroE) { zeroE=tempValueE;}
        Serial.println("ZeroE");

    
    tempValueF=getDistance(time(SENSOR_F, B00100000));
    if (tempValueF>zeroF) { zeroF=tempValueF;}
        Serial.println("ZeroF");

    
    tempValueG=getDistance(time(SENSOR_G, B00100000));
    if (tempValueG>zeroG) { zeroG=tempValueG;}
        Serial.println("ZeroG");

    
    tempValueH=getDistance(time(SENSOR_H, B00000001));
    if (tempValueH>zeroH) { zeroH=tempValueH;}
        Serial.println("ZeroH");

    
    tempValueI=getDistance(time(SENSOR_I, B00000010));
    if (tempValueI>zeroI) { zeroI=tempValueI;}
        Serial.println("ZeroI");

  }
    
  // Inizializzo la shield mp3
  if (! musicPlayer.begin()) {
     // Decommentare la prossima riga in caso di debug
     Serial.println("Non trovo la shield VS1053, prova a modificare il pin 'SHIELD_CS' ");
     while (1);
  }
  // Decommentare la prossima riga in caso di debug
  Serial.println("VS1053 trovata");
  
  // Inizializzo la micro-SD
  if (! SD.begin(CARDCS)){
    // Decommentare la prossima riga in caso di debug
    Serial.println("Non trovo la micro-SD, prova a modificare il pin 'CARDCS' definendolo a 8 oppure a 10");
  }
  // Decommentare la prossima riga in caso di debug
  Serial.println("SD trovata");
  
  // Imposto il volume sinistro e destro dello speaker. Più è basso il valore, più è alto il volume
  musicPlayer.setVolume(10,10);
  
  // Faccio riprodurre un suono per avvisare che è finito il setup
  musicPlayer.playFullFile(beepSound);
}

// Funzione principale di Arduino: viene ripetuta finchè non si interrompe l'alimentazione
void loop() { 
    
    // Definisco il valore delle variabili inizializzate ad inizio codice
    normalizedA = 0;
    normalizedB = 0;
    normalizedC = 0;
    normalizedD = 0;
    normalizedE = 0;
    normalizedF = 0;
    normalizedG = 0;
    normalizedH = 0;
    normalizedI = 0;
  
    // Misuro il tempo di carica di ogni sensore e lo normalizzo come valore di distanza. Le misurazioni sono a gruppi di 3 e poi viene calcolato il valore medio. Così facendo il valore risulta molto più stabile.
    for (int i=0;i<3;i++) 
    {
       normalizedA+= normalize(zeroA,getDistance(time(SENSOR_A  , B00000001)),1000);
       normalizedB+= normalize(zeroB,getDistance(time(SENSOR_B  , B00000010)),500);
       normalizedC+= normalize(zeroC,getDistance(time(SENSOR_C  , B00000100)),500);
       normalizedD+= normalize(zeroD,getDistance(time(SENSOR_D  , B00001000)),500);
       normalizedE+= normalize(zeroE,getDistance(time(SENSOR_E  , B00010000)),500);
       normalizedF+= normalize(zeroF,getDistance(time(SENSOR_F  , B00100000)),500);
       normalizedG+= normalize(zeroG,getDistance(time(SENSOR_G  , B00100000)),500);
       normalizedH+= normalize(zeroH,getDistance(time(SENSOR_H  , B00000001)),500);
       normalizedI+= normalize(zeroI,getDistance(time(SENSOR_I  , B00000010)),500);
    }
    normalizedA=normalizedA/3;
    normalizedB=normalizedB/3;
    normalizedC=normalizedC/3;
    normalizedD=normalizedD/3;
    normalizedE=normalizedE/3;
    normalizedF=normalizedF/3;
    normalizedG=normalizedG/3;
    normalizedH=normalizedH/3;
    normalizedI=normalizedI/3;
    
    // Decommentare le prossime righe in caso di debug
    Serial.print("\nNormalized A = ");
    Serial.println(normalizedA);
    Serial.print("\nNormalized B = ");
    Serial.println(normalizedB);
    Serial.print("\nNormalized C = ");
    Serial.println(normalizedC);
    Serial.print("\nNormalized D = ");
    Serial.println(normalizedD);
    Serial.print("\nNormalized E = ");
    Serial.println(normalizedE);
    Serial.print("\nNormalized F = ");
    Serial.println(normalizedF);
    Serial.print("\nNormalized G = ");
    Serial.println(normalizedG);
    Serial.print("\nNormalized H = ");
    Serial.println(normalizedH);
    Serial.print("\nNormalized I = ");
    Serial.println(normalizedI);

    // Verifico il valore letto da ogni sensore e qualora uno di essi superi il valore soglia faccio partire l'audio relativo
    if (normalizedA>SOGLIA_MIN)
    {
      musicPlayer.playFullFile(audioNames[0]);
    }
    else if (normalizedB>SOGLIA)
    {
      musicPlayer.playFullFile(audioNames[1]);
    }
    else if (normalizedC>SOGLIA)
    {
      musicPlayer.playFullFile(audioNames[2]);
    }
    else if (normalizedD>SOGLIA)
    {
      musicPlayer.playFullFile(audioNames[3]);
    }
    else if (normalizedE>SOGLIA_MIN)
    {
      musicPlayer.playFullFile(audioNames[4]);
    }
    else if (normalizedF>SOGLIA_MIN)
    {
      musicPlayer.playFullFile(audioNames[5]);
    }
    else if (normalizedG>SOGLIA)
    {
      musicPlayer.playFullFile(audioNames[6]);
    }
    else if (normalizedH>SOGLIA)
    {
      musicPlayer.playFullFile(audioNames[7]);
    }
    else if (normalizedI>SOGLIA)
    {
      musicPlayer.playFullFile(audioNames[8]);
    }
} 

// Funzione time: eseguo cicli di carica e scarica del pin per misurare il tempo impiegato dal condensatore per caricarsi a 5v
long time(int pin, byte mask) { 
  
  unsigned long count = 0, total = 0;
  while(checkTimer() < refresh) {
    // pinMode is about 6 times slower than assigning
    // DDRB directly, but that pause is important
    pinMode(pin, OUTPUT);
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    pinMode(pin, INPUT);
    if (pin<=7 )
    {
    while((PIND & mask) == 0)
      count++;
    }
    else if( pin > 7 && pin <= 13)
    {
      while((PINB & mask) == 0)
      count++;
    }
    else
    {
      while((PINC & mask) == 0)
      count++;

    }
    total++;
  }
  startTimer();
  return (count << resolution) / total;
}

extern volatile unsigned long timer0_overflow_count;

void startTimer() {
  timer0_overflow_count = 0;
  TCNT0 = 0;
}

unsigned long checkTimer() {
  return ((timer0_overflow_count << 8) + TCNT0) << 2;
}

// Funzione getDistance: calcola la distanza tramite il valore del tempo (passato come argomento della funzione)
long getDistance(long value)
{
 return  (long) ((double)1)/sqrt((double)(value/2))*1000000000;
}

// Funzione normalize: normalizza il valore ricevuto dai sensori (espresso in distanza) in base al valore zero misurato durante il setup
long normalize(long zeroValue, long distanceValue, long scaleValue)
{
  long normalized=0;
  normalized=zeroValue - distanceValue ;
  if (normalized<0)  { normalized=0; };
  normalized=normalized/scaleValue;
  return normalized;
}
