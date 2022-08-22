#include <stdio.h>
#include "gamelib.h"


int impostazioni = 0;   //se vale 0, la funzione imposta_gioco non è stata chiamata, e quindi non posso giocare; si altrimenti



int main(void) {
  if (impostazioni == 0) {

    printf(" __________________    __________    ____________    ____     __    ____     __    ____________    __                    \n");
    printf("|_______    _______|  |   _____  |  |   ______   |  |    \\   |  |  |    \\   |  |  |   ______   |  |  |                 \n");
    printf("        |  |          |  |     | |  |  |      |  |  |     \\  |  |  |     \\  |  |  |  |      |  |  |  |                 \n");
    printf("        |  |          |  |_____| |  |  |______|  |  |  |\\  \\ |  |  |  |\\  \\ |  |  |  |      |  |  |  |               \n");
    printf("        |  |          |   _   ___|  |   ______   |  |  | \\  \\|  |  |  | \\  \\|  |  |  |      |  |  |  |               \n");
    printf("        |  |          |  | \\  \\     |  |      |  |  |  |  \\     |  |  |  \\     |  |  |      |  |  |  |               \n");
    printf("        |  |          |  |  \\  \\    |  |      |  |  |  |   \\    |  |  |   \\    |  |  |      |  |  |  |               \n");
    printf("        |  |          |  |   \\  \\   |  |      |  |  |  |    \\   \033[31m|\033[0m  |  |    \\   |  |  |______|  |  |  |\n");
    printf("        |  |          |  |    \\  \\  |  |      |  |  |  |     \\  |  |  |     \\  |  |            |  |  |               \n");
    printf("         ‾‾            ‾‾      ‾‾    ‾‾        ‾‾    ‾‾       ‾‾    ‾‾       ‾‾    ‾‾‾‾‾‾‾‾‾‾‾‾    ‾‾                \n");

    printf("\nTRANNOI\n\n");
    printf("STORIA: Un gruppo di astronauti si trova in viaggio sull’astronave Skelt, e il loro obiettivo è riuscire\n");
    printf("a completare tutte le attività previste (le quest) per il mantenimento della nave, arrivando così a destinazione.\n");
    printf("Tra di loro si celano però anche degli impostori, il cui scopo è eliminare di nascosto gli astronauti senza farsi\n");
    printf("scoprire da essi. Riusciranno ad arrivare a destinazione prima di essere decimati?\n\n");
  }

  static int scelta = 0;
  struct Giocatore* puntatore_giocatori;
  int continuare = 0;

  do {
    printf("\nMENU PRINCIPALE\nSeleziona un'impostazione:\n");
    printf("1) Imposta gioco\n");
    printf("2) Gioca\n");
    printf("3) Termina gioco\n\n");

    printf("Selezionare l'impostazione desiderata: ");
    scanf("%d", &scelta);

    switch (scelta) {
      case 1:
        puntatore_giocatori = imposta_gioco();   //creo il puntatore per poter passare giocatori alla funzione gioca()
        scelta = 0;   //permette di ristampare il menu e selezionare un'azione
        continue;
        break;
      case 2:
        if (impostazioni == 0) {   //se non si è prima impostata la partita non si può giocare
          printf("Per giocare si deve prima impostare il gioco.\n");
          scelta = 0;
          continue;
        }
        gioca(puntatore_giocatori);
        scelta = 0;
        break;
      case 3:
        continuare = termina_gioco(puntatore_giocatori);
        if (continuare == 0)   //se si vuole uscire esce dallo switch e chiude il programma
          break;
        else {
          scelta = 0;
          continue;   //altrimenti ricomincia il ciclo
        }
        break;
      default:
        printf("Inserisci un valore valido.\n");
        while (getchar() != '\n');
        break;
    }
  } while (scelta != 1 && scelta != 2 && scelta != 3);
}
