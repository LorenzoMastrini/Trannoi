#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gamelib.h"



extern int impostazioni;   //variabile per controllare che prima di giocare sia stato impostato il gioco (0 = no; 1 = si)

struct Giocatore* giocatori; //puntatore all'array di struct Giocatore
struct Stanza* stanza_inizio;   //puntatore al primo elemento della lista
struct Stanza* pLast;   //puntatore all'ultimo elemento della lista
struct Stanza* lista_stanze;
static short quest_da_finire;
static int n_giocatori;   //numero di giocatori; serve visibile nella imposta_gioco(), nella stampa_giocatori() e nella gioca()
static int contatore_impostori = 0;   //globale perché usata anche in gioca()
static void stampa_giocatori(struct Giocatore* giocatori, struct Stanza* stanza_inizio);
static void inizia_gioco(void);
static void avanza(struct Giocatore* pGiocatore);
static unsigned short esegui_quest(struct Giocatore* astronauta);
static void chiamata_emergenza(struct Giocatore* giocatori, struct Stanza* stanza);
static int uccidi_astronauta(struct Giocatore* impostore, struct Giocatore* giocatori);
static void usa_botola(struct Giocatore* impostore);
static void sabotaggio(struct Giocatore* impostore);



struct Giocatore* imposta_gioco() {
  time_t t;
  srand ((unsigned) time(&t)); //per la generazione di numeri casuali
  contatore_impostori = 0;   //nel caso si vogliano giocare più partite di seguito riinizializzo i contatori

  printf("\n\nHai selezionato \"imposta gioco\", dove potrai scegliere i parametri per la partita.\n\n\n");

  do {
    printf("Selezionare il numero di giocatori (da 4 a 10): ");
    scanf("%d", &n_giocatori);
    if (n_giocatori < 4 || n_giocatori > 10) {   //controlla la validità del valore inserito
      printf("Inserisci un valore valido.\n");
      while (getchar() != '\n');
    }

    puts("");
  } while (n_giocatori < 4 || n_giocatori > 10); //continua a richiedere valori validi

  struct Giocatore* giocatori = (struct Giocatore*) malloc(n_giocatori * sizeof(struct Giocatore)); //alloca memoria dinamicamente in base al numero di giocatori

  int giocatori_usciti[n_giocatori];   //memorizza i nomi mano a mano, così da averli tutti diversi
  for (int i = 0; i < n_giocatori; i++) {
    int assegna_nome = rand() % 10;
    int count = 0;

    while (count < n_giocatori) {
      if (assegna_nome == giocatori_usciti[count]) {   //controlla che i nomi siano tutti diversi tra loro
        assegna_nome = rand() % 10;
        count = 0;
      }
      else
        count++;
    }
    giocatori_usciti[i] = assegna_nome;
    giocatori[i].Nome_giocatore = assegna_nome;
  }

  if (n_giocatori <= 5) {   //se i giocatori sono 4 o 5, fa sì che ci sia almeno un impostore
    while (contatore_impostori < 1 || contatore_impostori > 3) {
      contatore_impostori = 0;

      for (int i = 0; i < n_giocatori; i++) {
        int assegna_impostore = 1 + rand() % 100;
        if (assegna_impostore <= 10) {   //10% di probabilità che il giocatore i-esimo sia impostore, altrimenti astronauta
          giocatori[i].Stato_giocatore = impostore;
          contatore_impostori++;
        }
        else
          giocatori[i].Stato_giocatore = astronauta;
      }
    }
  }

  if (n_giocatori >= 6 && n_giocatori <= 8) {   //se i giocatori sono da 6 a 8, fa sì che ci sia almeno un impostore e al massimo 3
    contatore_impostori = 0;
    while (contatore_impostori < 1 || contatore_impostori > 3) {
      contatore_impostori = 0;

      for (int i = 0; i < n_giocatori; i++) {
        int assegna_impostore = 1 + rand() % 100;
        if (assegna_impostore <= 25) {   //25% di probabilità che il giocatore i-esimo sia impostore, altrimenti astronauta
          giocatori[i].Stato_giocatore = impostore;
          contatore_impostori++;
        }
        else
          giocatori[i].Stato_giocatore = astronauta;
      }
    }
  }

  if (n_giocatori >= 9) {   //se i giocatori sono 9 o 10, fa sì che ci siano almeno un impostore e al massimo 3
    contatore_impostori = 0;
    while (contatore_impostori < 1 || contatore_impostori > 3) {
      contatore_impostori = 0;

      for (int i = 0; i < n_giocatori; i++) {
        int assegna_impostore = 1 + rand() % 100;
        if (assegna_impostore <= 40) {   //40% di probabilità che il giocatore i-esimo sia impostore, altrimenti astronauta
          giocatori[i].Stato_giocatore = impostore;
          contatore_impostori++;
        }
        else
          giocatori[i].Stato_giocatore = astronauta;
      }
    }
  }

  do {
    printf("Inserisci il numero di quest da raggiungere perché gli astronauti vincano la partità: ");
    scanf("%hd", &quest_da_finire);
    if (quest_da_finire < 0 || quest_da_finire > 65535) {
      printf("Valore non valido.\n\n");
      while (getchar() != '\n');
    }
  } while (quest_da_finire < 0 || quest_da_finire > 65535);   //controlla la validità del valore inserito

  struct Stanza* stanza_inizio = (struct Stanza*) malloc(sizeof(struct Stanza));   //alloca memoria per la stanza iniziale

  stanza_inizio -> avanti = NULL;
  stanza_inizio -> sinistra = NULL;
  stanza_inizio -> destra = NULL;
  stanza_inizio -> stanza_precedente = NULL;
  int assegna_tipo_stanza = 1 + rand() % 100;
  if (assegna_tipo_stanza <= 25)
    stanza_inizio -> Tipo_stanza = botola;
  if (assegna_tipo_stanza > 25 && assegna_tipo_stanza <= 40)
    stanza_inizio -> Tipo_stanza = quest_doppia;
  if (assegna_tipo_stanza > 40 && assegna_tipo_stanza <= 70)
    stanza_inizio -> Tipo_stanza = quest_semplice;
  if (assegna_tipo_stanza > 70 && assegna_tipo_stanza <= 100)
    stanza_inizio -> Tipo_stanza = vuota;
  stanza_inizio -> emergenza_chiamata = 0;   //se 0 la chiamata di emergenza non è stata effettuata

  lista_stanze = stanza_inizio;   //imposta come primo elemento della lista la stanza iniziale
  pLast = stanza_inizio;   //imposta come ultmio elemento della lista la stanza iniziale

  for (int i = 0; i < n_giocatori; i++) {   //fa partire tutti i giocatori nella stanza iniziale
    giocatori[i].posizione = stanza_inizio;
  }

  int scelta = 0;
  while (scelta != 2) {
    do {
      printf("\nSelezionare un'azione:\n");
      printf("1) stampa le informazioni di tutti i giocatori\n");
      printf("2) esce dall'impostazione del gioco e torna al menu principale per giocare (o chiudere il programma)\n\n");
      printf("Selezionare l'azione desiderata: ");
      scanf("%d", &scelta);
      impostazioni = 1;   //significa che ora posso giocare

      switch (scelta) {
        case 1:
          scelta = 0;   //evita un ciclo infinito durante l'esecuzione se inserisco un valore non accettabile
          stampa_giocatori(giocatori, stanza_inizio);   //stampa le informazioni di tutti i giocatori
          break;
        case 2:
          inizia_gioco();   //torna al menu principale
          break;
        default:
          printf("Inserisci un valore valido.\n");
          while (getchar() != '\n');
          break;
      }
    } while (scelta != 1 && scelta != 2);
  }
  return giocatori;
}   //fine imposta_gioco



void stampa_giocatori(struct Giocatore* giocatori, struct Stanza* stanza) {
  int count = 1;

  for (int i = 0; i < n_giocatori; i++) {
    printf("Giocatore %d:\n", count);

    switch (giocatori[i].Nome_giocatore) {  //stampa i nomi dei giocatori
      case rosso: printf("Nome:\t\t\033[31mrosso\033[0m\n");
        break;
      case giallo: printf("Nome:\t\t\033[93;1mgiallo\033[0m\n");
        break;
      case blu: printf("Nome:\t\t\033[34mblu\033[0m\n");
        break;
      case arancione: printf("Nome:\t\t\033[38;2;255;102;1marancione\033[0m\n");
        break;
      case verde: printf("Nome:\t\t\033[32mverde\033[0m\n");
        break;
      case viola: printf("Nome:\t\t\033[35mviola\033[0m\n");
        break;
      case nero: printf("Nome:\t\t\033[30;1mnero\033[0m\n");
        break;
      case marrone: printf("Nome:\t\t\033[38;2;139;69;19mmarrone\033[0m\n");
        break;
      case bianco: printf("Nome:\t\t\033[0;37mbianco\033[0m\n");
        break;
      case rosa: printf("Nome:\t\t\033[95;1mrosa\033[0m\n");
        break;
    };

    switch (giocatori[i].Stato_giocatore) {  //stampa gli stati dei giocatori
      case astronauta: printf("Stato:\t\tastronauta\n\n");
        break;
      case impostore: printf("Stato:\t\timpostore\n\n");
        break;
      case assassinato: printf("Stato:\t\tassassinato\n\n");
        break;
      case defenestrato: printf("Stato:\t\tdefenestrato\n\n");
        break;
    }
    count++;
  }
  printf("Tutti i giocatori partono dalla stanza iniziale di indirizzo %p e di tipo ", stanza);
  switch (stanza -> Tipo_stanza) {   //stampa il tipo della stanza
    case 0:   printf("vuota\n\n");
    break;
    case 1:   printf("quest semplice\n\n");
    break;
    case 2:   printf("quest doppia\n\n");
    break;
    case 3:   printf("botola\n\n");
    break;
  }
}   //fine stampa giocatori



void inizia_gioco() {
  return;   //non torna nulla e quindi esce dallo switch di imposta_gioco, concludendo la funzione e tornando al menu principale
}   //fine inizia gioco



void gioca (struct Giocatore* giocatori) {
  unsigned short quest_completate = 0;
  int impostori_defenestrati = 0;
  int n_astronauti = n_giocatori - contatore_impostori;

  printf("\n\n\nINIZIA IL GIOCO!\n\n\n");

  do {
    int turni[n_giocatori];
    int turni_usciti[n_giocatori];   //memorizza i turni mano a mano, così da averli tutti diversi

    for (int i = 0; i < n_giocatori; i++) {   //inizializza entrambi gli arrary con valori non validi così che quando scelgo i turni non ci sono problemi
      turni[i] = -1;
      turni_usciti[i] = -1;
    }

    for (int i = 0; i < n_giocatori; i++) {   //come per la scelta dei nomi dei giocatori; i turni sono casuali ogni volta che finisce il primo giro di turni ma sono anche sempre diversi così che giochino tutti
      int assegna_turno = rand() % n_giocatori;
      int count = 0;

      while (count < n_giocatori) {
        if (assegna_turno == turni_usciti[count]) {   //controlla che i turni siano tutti diversi tra loro
          assegna_turno = rand() % n_giocatori;
          count = 0;
        }
        else
          count++;
      }
      turni_usciti[i] = assegna_turno;
      turni[i] = assegna_turno;
    }

    for (int i = 0; i < n_giocatori; i++) {
      printf("\n\n----------------------------------------------------------------------------------\n");
      printf("È il turno del giocatore ");
      switch (giocatori[turni[i]].Nome_giocatore) {  //stampa il nome del giocatore che deve giocare
        case rosso: printf("\033[31mrosso\033[0m\n\n");
          break;
        case giallo: printf("\033[93;1mgiallo\033[0m\n\n");
          break;
        case blu: printf("\033[34mblu\033[0m\n\n");
          break;
        case arancione: printf("\033[38;2;255;102;1marancione\033[0m\n\n");
          break;
        case verde: printf("\033[32mverde\033[0m\n\n");
          break;
        case viola: printf("\033[35mviola\033[0m\n\n");
          break;
        case nero: printf("\033[30;1mnero\033[0m\n\n");
          break;
        case marrone: printf("\033[38;2;139;69;19mmarrone\033[0m\n\n");
          break;
        case bianco: printf("\033[0;37mbianco\033[0m\n\n");
          break;
        case rosa: printf("\033[95;1mrosa\033[0m\n\n");
          break;
      };

      if (giocatori[turni[i]].Stato_giocatore != assassinato && giocatori[turni[i]].Stato_giocatore != defenestrato) {   //stampa le informazioni necessarie solo se il giocatore può giocare

        printf("Il tipo della stanza attuale è: ");
        switch (giocatori[turni[i]].posizione -> Tipo_stanza) {   //stampa il tipo della stanza in cui si trova il giocatore
          case vuota: printf("vuota\n");
            break;
          case quest_semplice: printf("quest_semplice\n");
            break;
          case quest_doppia: printf("quest_doppia\n");
            break;
          case botola: printf("botola\n");
            break;
        }

        int flag_altri_giocatori = 0;
        for (int count = 0; count < n_giocatori; count++) {   //controllo in più solo per sistemare l'output
          if (giocatori[turni[i]].posizione == giocatori[count].posizione && giocatori[count].Stato_giocatore != assassinato && giocatori[count].Stato_giocatore != defenestrato && giocatori[turni[i]].Nome_giocatore != giocatori[count].Nome_giocatore) {
            flag_altri_giocatori = 1;
            break;
          }
        }
        if (flag_altri_giocatori == 0)
          printf("Non ci sono altri giocatori nella stanza\n");
        else
          printf("I giocatori presenti in questa stanza sono:\n");

        int contatore_assassinati = 0;
        for (int count = 0; count < n_giocatori; count++) {
          if (giocatori[turni[i]].posizione == giocatori[count].posizione && giocatori[count].Stato_giocatore != assassinato && giocatori[count].Stato_giocatore != defenestrato && giocatori[turni[i]].Nome_giocatore != giocatori[count].Nome_giocatore) {   //stampa i giocatori ancora in gioco nella stessa stanza, escuso il giocatore che sta giocando il turno
            switch (giocatori[count].Nome_giocatore) {
              case rosso: printf("\033[31mrosso\033[0m\n");
                break;
              case giallo: printf("\033[93;1mgiallo\033[0m\n");
                break;
              case blu: printf("\033[34mblu\033[0m\n");
                break;
              case arancione: printf("\033[38;2;255;102;1marancione\033[0m\n");
                break;
              case verde: printf("\033[32mverde\033[0m\n");
                break;
              case viola: printf("\033[35mviola\033[0m\n");
                break;
              case nero: printf("\033[30;1mnero\033[0m\n");
                break;
              case marrone: printf("\033[38;2;139;69;19mmarrone\033[0m\n");
                break;
              case bianco: printf("\033[0;37mbianco\033[0m\n");
                break;
              case rosa: printf("\033[95;1mrosa\033[0m\n");
                break;
            };
          }
          else if (giocatori[turni[i]].posizione == giocatori[count].posizione && giocatori[count].Stato_giocatore == assassinato)
            contatore_assassinati++;
        }
        if (contatore_assassinati == 1)
          printf("\nC'è un cadavere nella stanza\n");
        else if (contatore_assassinati == 2)
          printf("\nCi sono due cadaveri nella stanza\n");
        else if (contatore_assassinati > 2)
          printf("\nCi sono più cadaveri nella stanza\n");

        if (giocatori[turni[i]].posizione -> emergenza_chiamata == 1)
          printf("\nLa chiamata di emergenza non può essere effettuata perché è stata già usata in questa stanza\n");
      }

      if (giocatori[turni[i]].Stato_giocatore == astronauta) {
        int scelta = 0;
        printf("\nLe azioni possibili per gli astronauti sono:\n1) avanza\n2) esegui quest\n3) chiamata di emergenza\n\n");

        do {
          scelta = 0;
          struct Giocatore* pGiocatore = &giocatori[turni[i]];   //puntatore al giocatore attuale da passare alle funzioni
          int flag_chiamata_emergenza = 0;
          printf("Seleziona un'azione: ");
          scanf("%d", &scelta);

          switch(scelta) {
            case 1:
              avanza(pGiocatore);   //non ci sono condizioni particolari per avanzare nella mappa
              break;
            case 2:
              if (giocatori[turni[i]].posizione -> Tipo_stanza != quest_semplice && giocatori[turni[i]].posizione -> Tipo_stanza != quest_doppia) {   //La stanza deve essere di tipo quest_semplice o quest_doppia per poter usare la esegui_quest
                printf("\nL'azione non può essere eseguita perché la stanza non ha quest eseguibili\n");
                scelta = 0;
                continue;   //torna a inizio ciclo do...while per far compiere un'altra scelta al giocatore
              }
              else {
                quest_completate += esegui_quest(pGiocatore);
                printf("Le quest completate sono %d su %d da finire\n", quest_completate, quest_da_finire);
                break;
              }
              break;
            case 3:
              for (int count = 0; count < n_giocatori; count++) {   //scorre i giocatori per controllarne la posizione e lo stato
                if (giocatori[count].posizione == giocatori[turni[i]].posizione) {
                  if (giocatori[count].Stato_giocatore == assassinato && giocatori[turni[i]].posizione -> emergenza_chiamata == 0) {
                    chiamata_emergenza(giocatori, giocatori[turni[i]].posizione);
                    flag_chiamata_emergenza = 1;
                    break;   //una volta chiamata la funzione chiamata_emergenza esce dallo switch
                  }
                }
              }
              if (flag_chiamata_emergenza == 0) {   //se il contatore non raggiunnge il valore (n_giocatori - 1), significa che la funzione emergenza_chiamata non è stata chiamata e quindi non può essere eseguita
                printf("\nL'azione non può essere eseguita perché non ci sono astronauti assassinati in questa stanza o la chiamata di emergenza è già stata effettuata\n");
                scelta = 0;
                continue;
              }
              break;
            default:
              printf("Inserisci un valore valido\n");
              while (getchar() != '\n');
              break;
          };
        } while (scelta != 1 && scelta != 2 && scelta != 3);

        while (getchar() != '\n');
        printf("\n\nPremere invio per continuare");
        getchar();
      }   //fine if astronauta

      if (quest_completate >= quest_da_finire)   //se una delle condizioni di vittoria è state raggiunta non fa continuare il ciclo di turni
        break;   //esce dal for dell'i-esimo giocatore

      if (giocatori[turni[i]].Stato_giocatore == impostore) {
        int scelta = 0;
        printf("\nLe azioni possibili per gli impostori sono:\n1) avanza\n2) chiamata di emergenza\n3) uccidi astronauta\n4) usa botola\n5) sabotaggio\n\n");

        do {
          scelta = 0;
          struct Giocatore* pGiocatore = &giocatori[turni[i]];   //puntatore al giocatore attuale da passare alle funzioni
          int flag_chiamata_emergenza = 0;
          int flag_uccidi_astronauta = 0;
          printf("Seleziona un'azione: ");
          scanf("%d", &scelta);

          switch (scelta) {
            case 1:
              avanza(pGiocatore);   //non ci sono condizioni particolari per avanzare nella mappa
              break;
            case 2:
              for (int count = 0; count < n_giocatori; count++) {   //scorre i giocatori per controllarne la posizione e lo stato
                if (giocatori[count].posizione == giocatori[turni[i]].posizione) {
                  if (giocatori[count].Stato_giocatore == assassinato && giocatori[turni[i]].posizione -> emergenza_chiamata == 0) {
                    chiamata_emergenza(giocatori, giocatori[turni[i]].posizione);
                    flag_chiamata_emergenza = 1;
                    break;   //una volta chiamata la funzione chiamata_emergenza esce dallo switch e prosegue
                  }
                }
              }
              if (flag_chiamata_emergenza == 0) {   //se il contatore non raggiunge il valore (n_giocatori - 1), significa che la funzione emergenza_chiamata non è stata chiamata e quindi non può essere eseguita
                printf("\nL'azione non può essere eseguita perché non ci sono astronauti assassinati in questa stanza\n");
                scelta = 0;
                continue;
              }
              break;
            case 3:
              for (int count = 0; count < n_giocatori; count++) {
                if (giocatori[turni[i]].posizione == giocatori[count].posizione) {
                  if (giocatori[count].Stato_giocatore == astronauta) {   //la funzione è chiamata se l'impostore ha almeno un astronauta nella sua stessa stanza
                  impostori_defenestrati += uccidi_astronauta(pGiocatore, giocatori);
                  n_astronauti--;
                  printf("Numero di astronauti: %d\n", n_astronauti);
                  printf("Impostori defenestrati: %d su %d\n", impostori_defenestrati, contatore_impostori);
                  flag_uccidi_astronauta = 1;
                  break;   //una volta chiamata la funzione uccidi_astronauta esce dallo switch e prosegue
                  }
                }
              }
              if (flag_uccidi_astronauta == 0) {   //se il contatore non raggiunge il valore (n_giocatori - 1), significa che la funzione uccidi_astronauta non è stata chiamata e quindi non può essere eseguita
                printf("\nL'azione non può essere eseguita perché non ci sono altri astronauti nella stanza\n");
                scelta = 0;
                continue;
              }
              break;
            case 4:
                if (giocatori[turni[i]].posizione -> Tipo_stanza == botola)
                  usa_botola(pGiocatore);
                else {
                  printf("L'azione non può essere eseguita perché la stanza attuale non è di tipo botola\n");
                  scelta = 0;
                  continue;   //torna a inizio ciclo do...while per far compiere un'altra scelta al giocatore
                }
                break;
            case 5:
              if (giocatori[turni[i]].posizione -> Tipo_stanza != quest_semplice && giocatori[turni[i]].posizione -> Tipo_stanza != quest_doppia) {   //La stanza deve essere di tipo quest_semplice o quest_doppia per poter usare la esegui_quest
                printf("\nL'azione non può essere eseguita perché la stanza non ha quest eseguibili\n");
                scelta = 0;
                continue;   //torna a inizio ciclo do...while per far compiere un'altra scelta al giocatore
              }
              else
                sabotaggio(pGiocatore);
              break;
            default:
              printf("Inserisci un valore valido\n");
              while (getchar() != '\n');
              break;
          };


        } while (scelta != 1 && scelta != 2 && scelta != 3 && scelta != 4 && scelta != 5);

        while (getchar() != '\n');
        printf("\n\nPremere invio per continuare");
        getchar();
      }   //fine if impostore

      if (giocatori[turni[i]].Stato_giocatore == assassinato)
        printf("Il giocatore è stato assassinato e quindi non può più giocare\n");

      if (giocatori[turni[i]].Stato_giocatore == defenestrato)
        printf("Il giocatore è stato defenestrato e quindi non può più giocare\n");


      if (impostori_defenestrati >= contatore_impostori)   //se una delle condizioni di vittoria è state raggiunta non fa continuare il ciclo di turni
        break;   //esce dal do...while e valuta le espressioni di controllo, quindi conclude la partita
      if (n_astronauti <= 0)   //se una delle condizioni di vittoria è state raggiunta non fa continuare il ciclo di turni
        break;   //esce dal do...while e valuta le espressioni di controllo, quindi conclude la partita

    }   //fine turno i-esimo giocatore (fine for)


    if (quest_completate >= quest_da_finire)   //se una delle condizioni di vittoria è state raggiunta non fa continuare il ciclo di turni
      break;   //esce dal do...while e valuta le espressioni di controllo, quindi conclude la partita
    if (impostori_defenestrati >= contatore_impostori)   //se una delle condizioni di vittoria è state raggiunta non fa continuare il ciclo di turni
      break;   //esce dal do...while e valuta le espressioni di controllo, quindi conclude la partita
    if (n_astronauti <= 0)   //se una delle condizioni di vittoria è state raggiunta non fa continuare il ciclo di turni
      break;   //esce dal do...while e valuta le espressioni di controllo, quindi conclude la partita

  } while (quest_completate < quest_da_finire && impostori_defenestrati < contatore_impostori && n_astronauti != 0);   //ripete fino a che una delle tre condizioni di vittoria non è soddisfatta


  if (quest_completate >= quest_da_finire)
    printf("\n\n\nLA PARTITA SI È CONCLUSA CON LA VITTORIA DEGLI ASTRONAUTI (NUMERO DI QUEST DA FINIRE RAGGIUNTO)\n\n\n");

  if (impostori_defenestrati >= contatore_impostori)
    printf("\n\n\nLA PARTITA SI È CONCLUSA CON LA VITTORIA DEGLI ASTRONAUTI (IMPOSTORI DEFENESTRATI)\n\n\n");

  if (n_astronauti <= 0)
    printf("\n\n\nLA PARTITA SI È CONCLUSA CON LA VITTORIA DEGLI IMPOSTORI (TUTTI GLI ASTRONAUTI UCCISI)\n\n\n");


  return;
} //fine gioca



void avanza (struct Giocatore* giocatore) {
  int scelta = 0;
  struct Stanza* Nuova_stanza;

  printf("\nSi può avanzare nella mappa:\n1) avanti\n2) sinistra\n3) destra\n4) rimani fermo\n");

  do {
    printf("Selezionare un'azione: ");
    scanf ("%d", &scelta);

    switch (scelta) {
      case 1:
        if (giocatore -> posizione -> avanti == NULL) {   //se il puntatore è a NULL, la stanza deve essere creata

          Nuova_stanza = (struct Stanza*) malloc (sizeof(struct Stanza));   //alloca memoria per la nuova stanza se non era già stata creata
          Nuova_stanza -> avanti = NULL;
          Nuova_stanza -> sinistra = NULL;
          Nuova_stanza -> destra = NULL;
          Nuova_stanza -> stanza_precedente = giocatore -> posizione;   //collega le stanze tramite i puntatori
          int assegna_tipo_stanza = 1 + rand() % 100;   //assegna il tipo della nuova stanza con le solite probabilità
          if (assegna_tipo_stanza <= 25)
            Nuova_stanza -> Tipo_stanza = botola;
          if (assegna_tipo_stanza > 25 && assegna_tipo_stanza <= 40)
            Nuova_stanza -> Tipo_stanza = quest_doppia;
          if (assegna_tipo_stanza > 40 && assegna_tipo_stanza <= 70)
            Nuova_stanza -> Tipo_stanza = quest_semplice;
          if (assegna_tipo_stanza > 70 && assegna_tipo_stanza <= 100)
            Nuova_stanza -> Tipo_stanza = vuota;
          Nuova_stanza -> emergenza_chiamata = 0;   //flag booleano a 0, cioè la chiamata di emergenza non è stata effettuata nella nuova stanza
          Nuova_stanza -> pNext = NULL;

          //senza controllo che la lista sia vuota perché una stanza (stanza_inizio) è stata già creata quando si arriva a questo punto del programma

          pLast -> pNext = Nuova_stanza;   //quello che era l'ultimo elemento ora è il penultimo
          pLast = Nuova_stanza;   //il nuovo elemento ora è l'ultimo della lista

          giocatore -> posizione -> avanti = Nuova_stanza;   //il puntatore alla stanza avanti ora punta alla nuova stanza
          giocatore -> posizione = Nuova_stanza;   //aggiorna la posizione del giocatore attuale alla nuova stanza
        }
        else
          giocatore -> posizione = giocatore -> posizione -> avanti;   //se la stanza esiste già aggiorna solo la posizione del giocatore
        break;
      case 2:
        if (giocatore -> posizione -> sinistra == NULL) {   //se il puntatore è a NULL, la stanza deve essere creata

          Nuova_stanza = (struct Stanza*) malloc (sizeof(struct Stanza));   //alloca memoria per la nuova stanza se non era già stata creata
          Nuova_stanza -> avanti = NULL;
          Nuova_stanza -> sinistra = NULL;
          Nuova_stanza -> destra = NULL;
          Nuova_stanza -> stanza_precedente = giocatore -> posizione;   //collega le stanze tramite i puntatori
          int assegna_tipo_stanza = 1 + rand() % 100;   //assegna il tipo della nuova stanza con le solite probabilità
          if (assegna_tipo_stanza <= 25)
            Nuova_stanza -> Tipo_stanza = botola;
          if (assegna_tipo_stanza > 25 && assegna_tipo_stanza <= 40)
            Nuova_stanza -> Tipo_stanza = quest_doppia;
          if (assegna_tipo_stanza > 40 && assegna_tipo_stanza <= 70)
            Nuova_stanza -> Tipo_stanza = quest_semplice;
          if (assegna_tipo_stanza > 70 && assegna_tipo_stanza <= 100)
            Nuova_stanza -> Tipo_stanza = vuota;
          Nuova_stanza -> emergenza_chiamata = 0;   //flag booleano a 0, cioè la chiamata di emergenza non è stata effettuata nella nuova stanza
          Nuova_stanza -> pNext = NULL;

          pLast -> pNext = Nuova_stanza;   //quello che era l'ultimo elemento ora è il penultimo
          pLast = Nuova_stanza;   //il nuovo elemento ora è l'ultimo della lista

          giocatore -> posizione -> sinistra = Nuova_stanza;   //il puntatore alla stanza a sinistra ora punta alla nuova stanza
          giocatore -> posizione = Nuova_stanza;   //aggiorna la posizione del giocatore attuale alla nuova stanza
        }
        else
          giocatore -> posizione = giocatore -> posizione -> sinistra;   //se la stanza esiste già aggiorna solo la posizione del giocatore
        break;
      case 3:
        if (giocatore -> posizione -> destra == NULL) {   //se il puntatore è a NULL, la stanza deve essere creata

          Nuova_stanza = (struct Stanza*) malloc (sizeof(struct Stanza));   //alloca memoria per la nuova stanza se non era già stata creata
          Nuova_stanza -> avanti = NULL;
          Nuova_stanza -> sinistra = NULL;
          Nuova_stanza -> destra = NULL;
          Nuova_stanza -> stanza_precedente = giocatore -> posizione;   //collega le stanze tramite i puntatori
          int assegna_tipo_stanza = 1 + rand() % 100;   //assegna il tipo della nuova stanza con le solite probabilità
          if (assegna_tipo_stanza <= 25)
            Nuova_stanza -> Tipo_stanza = botola;
          if (assegna_tipo_stanza > 25 && assegna_tipo_stanza <= 40)
            Nuova_stanza -> Tipo_stanza = quest_doppia;
          if (assegna_tipo_stanza > 40 && assegna_tipo_stanza <= 70)
            Nuova_stanza -> Tipo_stanza = quest_semplice;
          if (assegna_tipo_stanza > 70 && assegna_tipo_stanza <= 100)
            Nuova_stanza -> Tipo_stanza = vuota;
          Nuova_stanza -> emergenza_chiamata = 0;   //flag booleano a 0, cioè la chiamata di emergenza non è stata effettuata nella nuova stanza
          Nuova_stanza -> pNext = NULL;

          pLast -> pNext = Nuova_stanza;   //quello che era l'ultimo elemento ora è il penultimo
          pLast = Nuova_stanza;   //il nuovo elemento ora è l'ultimo della lista

          giocatore -> posizione -> destra = Nuova_stanza;   //il puntatore alla stanza a sinistra ora punta alla nuova stanza
          giocatore -> posizione = Nuova_stanza;   //aggiorna la posizione del giocatore attuale alla nuova stanza
        }
        else
          giocatore -> posizione = giocatore -> posizione -> destra;   //se la stanza esiste già aggiorna solo la posizione del giocatore
        break;
      case 4:
        giocatore -> posizione = giocatore -> posizione;   //il giocatore rimane nella stanza attuale
        break;
      default:
        while (getchar() != '\n');
        printf("Valore non valido\n");
        break;

    }
  } while (scelta != 1 && scelta != 2 && scelta != 3 && scelta != 4);

  return;
}   //fine avanza



unsigned short esegui_quest(struct Giocatore* astronauta) {
  if ((astronauta -> posizione) -> Tipo_stanza == quest_semplice) {
    (astronauta -> posizione) -> Tipo_stanza = vuota;   //dopo aver eseguito la quest, la stanza diventa di tipo vuota
    return 1;   //se la quest è semplice incrementa il contatore delle quest completate di 1
  }
  else if ((astronauta -> posizione) -> Tipo_stanza == quest_doppia) {
    (astronauta -> posizione) -> Tipo_stanza = vuota;   //dopo aver eseguito la quest, la stanza diventa di tipo vuota
    return 2;   //se la quest è doppia incrementa il contatore delle quest completate di 2
  }

  return 0;   //se torna 0 significa che nessuno dei due if è stato eseguito e quindi il contatore quest_completate non viene incrementato
}   //fine esegui_quest



void chiamata_emergenza(struct Giocatore* giocatori, struct Stanza* stanza) {
  int probabilita_impostore = 0;
  int probabilita_astronauta = 0;
  int probabilita_defenestraggio = 0;
  int flag_defenestrato = 0;   //se 0 non è ancora stato defenestrato nessuno, altrimenti si

  stanza -> emergenza_chiamata = 1;

  for (int i = 0; i < n_giocatori; i++) {
    probabilita_impostore = 0;
    probabilita_astronauta = 0;

    if (giocatori[i].posizione == stanza && giocatori[i].Stato_giocatore == impostore) {   //controlla che il giocatore i sia impostore e che si trovi nella stanza in cui si è chiamata la funzione
      probabilita_impostore = 30;   //probabilita_impostore impostato inizialmente a 30%
      for (int count = 0; count < n_giocatori; count++) {
        if (giocatori[count].posizione == stanza && giocatori[count].Stato_giocatore == astronauta)   //più 20% per ogni astronauta nella stanza
          probabilita_impostore += 20;
        else if (giocatori[count].posizione == stanza && giocatori[count].Stato_giocatore == impostore)   //meno 30% per ogni impostore nella stanza
          probabilita_impostore -= 30;
      }

      probabilita_defenestraggio = 1 + rand() % 100;
      if (probabilita_defenestraggio <= probabilita_impostore) {
        giocatori[i].Stato_giocatore = defenestrato;
        flag_defenestrato = 1;

        printf("L'impostore ");
        switch (giocatori[i].Nome_giocatore) {
          case rosso: printf("\033[31mrosso\033[0m ");
            break;
          case giallo: printf("\033[93;1mgiallo\033[0m ");
            break;
          case blu: printf("\033[34mblu\033[0m ");
            break;
          case arancione: printf("\033[38;2;255;102;1marancione\033[0m ");
            break;
          case verde: printf("\033[32mverde\033[0m ");
            break;
          case viola: printf("\033[35mviola\033[0m ");
            break;
          case nero: printf("\033[30;1mnero\033[0m ");
            break;
          case marrone: printf("\033[38;2;139;69;19mmarrone\033[0m ");
            break;
          case bianco: printf("\033[0;37mbianco\033[0m ");
            break;
          case rosa: printf("\033[95;1mrosa\033[0m ");
            break;
        };
        printf("è stato defenestrato\n");
        break;   //una volta defenestrato un giocatore esce dal for concludendo anche la funzione, così che solo un giocatore venga defenestrato
      }
    }   //fine if impostore


    if (giocatori[i].posizione == stanza && giocatori[i].Stato_giocatore == astronauta) {   //controlla che il giocatore i sia astronauta e che si trovi nella stanza in cui si è chiamata la funzione
      probabilita_astronauta = 30;   //probabilita_astronauta impostato inizialmente a 30%
      for (int count = 0; count < n_giocatori; count++) {
        if (giocatori[count].posizione == stanza && giocatori[count].Stato_giocatore == impostore)   //più 20% per ogni impostore nella stanza
          probabilita_astronauta += 20;
        else if (giocatori[count].posizione == stanza && giocatori[count].Stato_giocatore == astronauta)   //meno 30% per ogni astronauta nella stanza
          probabilita_astronauta -= 30;
      }

      probabilita_defenestraggio = 1 + rand() % 100;
      if (probabilita_defenestraggio <= probabilita_astronauta) {
        giocatori[i].Stato_giocatore = defenestrato;
        flag_defenestrato = 1;

        printf("L'astronauta ");
        switch (giocatori[i].Nome_giocatore) {
          case rosso: printf("\033[31mrosso\033[0m ");
            break;
          case giallo: printf("\033[93;1mgiallo\033[0m ");
            break;
          case blu: printf("\033[34mblu\033[0m ");
            break;
          case arancione: printf("\033[38;2;255;102;1marancione\033[0m ");
            break;
          case verde: printf("\033[32mverde\033[0m ");
            break;
          case viola: printf("\033[35mviola\033[0m ");
            break;
          case nero: printf("\033[30;1mnero\033[0m ");
            break;
          case marrone: printf("\033[38;2;139;69;19mmarrone\033[0m ");
            break;
          case bianco: printf("\033[0;37mbianco\033[0m ");
            break;
          case rosa: printf("\033[95;1mrosa\033[0m ");
            break;
        };
        printf("è stato defenestrato\n");
        break;   //una volta defenestrato un giocatore esce dal for concludendo anche la funzione, così che solo un giocatore venga defenestrato
      }
    }   //fine if astronauta

  }   //fine i-esimo giocatore

  if (flag_defenestrato == 0)
    printf("Non è stato defenestrato nessun giocatore\n");

  return;
}   //fine chiamata_emergenza



int uccidi_astronauta(struct Giocatore* impostore, struct Giocatore* giocatori) {
  int scelta = 0;
  int flag_rosso = 0;
  int flag_giallo = 0;
  int flag_blu = 0;
  int flag_arancione = 0;
  int flag_verde = 0;
  int flag_viola = 0;
  int flag_nero = 0;
  int flag_marrone = 0;
  int flag_bianco = 0;
  int flag_rosa = 0;

  printf("Gli astronauti (vivi) presenti nella stanza sono:\n");
  for (int i = 0; i < n_giocatori; i++) {
    if (giocatori[i].posizione == impostore -> posizione && giocatori[i].Stato_giocatore == astronauta) {
      switch (giocatori[i].Nome_giocatore) {  //stampa i nomi degli astronauti
        case rosso:
          printf("1) \033[31mrosso\033[0m\n");
          flag_rosso = 1;
          break;
        case giallo:
          printf("2) \033[93;1mgiallo\033[0m\n");
          flag_giallo = 1;
          break;
        case blu:
          printf("3) \033[34mblu\033[0m\n");
          flag_blu = 1;
          break;
        case arancione:
          printf("4) \033[38;2;255;102;1marancione\033[0m\n");
          flag_arancione = 1;
          break;
        case verde:
          printf("5) \033[32mverde\033[0m\n");
          flag_verde = 1;
          break;
        case viola:
          printf("6) \033[35mviola\033[0m\n");
          flag_viola = 1;
          break;
        case nero:
          printf("7) \033[30;1mnero\033[0m\n");
          flag_nero = 1;
          break;
        case marrone:
          printf("8) \033[38;2;139;69;19mmarrone\033[0m\n");
          flag_marrone = 1;
          break;
        case bianco:
          printf("9) \033[0;37mbianco\033[0m\n");
          flag_bianco = 1;
          break;
        case rosa:
          printf("10) \033[95;1mrosa\033[0m\n");
          flag_rosa = 1;
          break;
      };
    }
  }

  do {
    printf("\nScegli l'astronauta da uccidere selezionando il numero accanto al suo nome: ");
    scanf("%d", &scelta);

    switch (scelta) {
      case 1:
        if (flag_rosso == 1) {   //indica se il giocatore è stato stampato, cioè se è possibile assassinarlo
          for (int i = 0; i < n_giocatori; i++) {
            if (giocatori[i].Nome_giocatore == rosso) {
              giocatori[i].Stato_giocatore = assassinato;
              printf("Il giocatore rosso è stato assassinato\n");
            }
          }
        }
        else {
          printf("Il giocatore non può essere assassinato perché non è un astronauta o non è in questa stanza\n");
          scelta = 0;
          continue;   //permette di compiere una nuova scelta
        }
        break;
      case 2:
        if (flag_giallo == 1) {   //indica se il giocatore è stato stampato, cioè se è possibile assassinarlo
          for (int i = 0; i < n_giocatori; i++) {
            if (giocatori[i].Nome_giocatore == giallo) {
              giocatori[i].Stato_giocatore = assassinato;
              printf("Il giocatore giallo è stato assassinato\n");
            }
          }
        }
        else {
          printf("Il giocatore non può essere assassinato perché non è un astronauta o non è in questa stanza\n");
          scelta = 0;
          continue;   //permette di compiere una nuova scelta
        }
        break;
      case 3:
        if (flag_blu == 1) {   //indica se il giocatore è stato stampato, cioè se è possibile assassinarlo
          for (int i = 0; i < n_giocatori; i++) {
            if (giocatori[i].Nome_giocatore == blu) {
              giocatori[i].Stato_giocatore = assassinato;
              printf("Il giocatore blu è stato assassinato\n");
            }
          }
        }
        else {
          printf("Il giocatore non può essere assassinato perché non è un astronauta o non è in questa stanza\n");
          scelta = 0;
          continue;   //permette di compiere una nuova scelta
        }
        break;
      case 4:
        if (flag_arancione == 1) {   //indica se il giocatore è stato stampato, cioè se è possibile assassinarlo
          for (int i = 0; i < n_giocatori; i++) {
            if (giocatori[i].Nome_giocatore == arancione) {
              giocatori[i].Stato_giocatore = assassinato;
              printf("Il giocatore arancione è stato assassinato\n");
            }
          }
        }
        else {
          printf("Il giocatore non può essere assassinato perché non è un astronauta o non è in questa stanza\n");
          scelta = 0;
          continue;   //permette di compiere una nuova scelta
        }
        break;
      case 5:
        if (flag_verde == 1) {   //indica se il giocatore è stato stampato, cioè se è possibile assassinarlo
          for (int i = 0; i < n_giocatori; i++) {
            if (giocatori[i].Nome_giocatore == verde) {
              giocatori[i].Stato_giocatore = assassinato;
              printf("Il giocatore verde è stato assassinato\n");
            }
          }
        }
        else {
          printf("Il giocatore non può essere assassinato perché non è un astronauta o non è in questa stanza\n");
          scelta = 0;
          continue;   //permette di compiere una nuova scelta
        }
        break;
      case 6:
        if (flag_viola == 1) {   //indica se il giocatore è stato stampato, cioè se è possibile assassinarlo
          for (int i = 0; i < n_giocatori; i++) {
            if (giocatori[i].Nome_giocatore == viola) {
              giocatori[i].Stato_giocatore = assassinato;
              printf("Il giocatore viola è stato assassinato\n");
            }
          }
        }
        else {
          printf("Il giocatore non può essere assassinato perché non è un astronauta o non è in questa stanza\n");
          scelta = 0;
          continue;   //permette di compiere una nuova scelta
        }
        break;
      case 7:
        if (flag_nero == 1) {   //indica se il giocatore è stato stampato, cioè se è possibile assassinarlo
          for (int i = 0; i < n_giocatori; i++) {
            if (giocatori[i].Nome_giocatore == nero) {
              giocatori[i].Stato_giocatore = assassinato;
              printf("Il giocatore nero è stato assassinato\n");
            }
          }
        }
        else {
          printf("Il giocatore non può essere assassinato perché non è un astronauta o non è in questa stanza\n");
          scelta = 0;
          continue;   //permette di compiere una nuova scelta
        }
        break;
      case 8:
        if (flag_marrone == 1) {   //indica se il giocatore è stato stampato, cioè se è possibile assassinarlo
          for (int i = 0; i < n_giocatori; i++) {
            if (giocatori[i].Nome_giocatore == marrone) {
              giocatori[i].Stato_giocatore = assassinato;
              printf("Il giocatore marrone è stato assassinato\n");
            }
          }
        }
        else {
          printf("Il giocatore non può essere assassinato perché non è un astronauta o non è in questa stanza\n");
          scelta = 0;
          continue;   //permette di compiere una nuova scelta
        }
        break;
      case 9:
        if (flag_bianco == 1) {   //indica se il giocatore è stato stampato, cioè se è possibile assassinarlo
          for (int i = 0; i < n_giocatori; i++) {
            if (giocatori[i].Nome_giocatore == bianco) {
              giocatori[i].Stato_giocatore = assassinato;
              printf("Il giocatore bianco è stato assassinato\n");
            }
          }
        }
        else {
          printf("Il giocatore non può essere assassinato perché non è un astronauta o non è in questa stanza\n");
          scelta = 0;
          continue;   //permette di compiere una nuova scelta
        }
          break;
        case 10:
          if (flag_rosa == 1) {   //indica se il giocatore è stato stampato, cioè se è possibile assassinarlo
            for (int i = 0; i < n_giocatori; i++) {
              if (giocatori[i].Nome_giocatore == rosa) {
                giocatori[i].Stato_giocatore = assassinato;
                printf("Il giocatore rosa è stato assassinato\n");
              }
            }
          }
          else {
            printf("Il giocatore non può essere assassinato perché non è un astronauta o non è in questa stanza\n");
            scelta = 0;
            continue;   //permette di compiere una nuova scelta
          }
          break;
        default:
          printf("Inserisci un valore valido\n");
          while (getchar() != '\n');
          break;
    }
  } while (scelta < 1 || scelta > 10);


  unsigned int probabilita_defenestraggio = 0;

  for (int i = 0; i < n_giocatori; i++) {
    if (giocatori[i].Stato_giocatore == astronauta && impostore -> posizione == giocatori[i].posizione)
      probabilita_defenestraggio += 50;

    else if (giocatori[i].Stato_giocatore == astronauta && giocatori[i].posizione == (impostore -> posizione) -> stanza_precedente)
      probabilita_defenestraggio += 20;
  }

  switch (probabilita_defenestraggio) {
    case 0:
      printf("Non sei stato scoperto.....ancora\n\n");
      break;
    case 20:
      if (1 + rand() % 100 <= 20) {
        printf("Un astronauta ti ha visto uccidere il compagno e ora sei stato defenestrato!\n\n");
        impostore -> Stato_giocatore = defenestrato;
        return 1;   //torna 1, cioè è stato defenestrato l'impostore (in gioca incrementerà il contatore di impostori defenestrati)
      }
      else
        printf("Non sei stato scoperto.....ancora\n\n");
      break;
    case 40:
      if (1 + rand() % 100 <= 40) {
        printf("Un astronauta ti ha visto uccidere il compagno e ora sei stato defenestrato!\n\n");
        impostore -> Stato_giocatore = defenestrato;
        return 1;   //torna 1, cioè è stato defenestrato l'impostore (in gioca incrementerà il contatore di impostori defenestrati)
      }
      else
        printf("Non sei stato scoperto.....ancora\n\n");
      break;
    case 50:
      if (1 + rand() % 100 <= 50) {
        printf("Un astronauta ti ha visto uccidere il compagno e ora sei stato defenestrato!\n\n");
        impostore -> Stato_giocatore = defenestrato;
        return 1;   //torna 1, cioè è stato defenestrato l'impostore (in gioca incrementerà il contatore di impostori defenestrati)
      }
      else
        printf("Non sei stato scoperto.....ancora\n\n");
      break;
    case 60:
      if (1 + rand() % 100 <= 60) {
        printf("Un astronauta ti ha visto uccidere il compagno e ora sei stato defenestrato!\n\n");
        impostore -> Stato_giocatore = defenestrato;
        return 1;   //torna 1, cioè è stato defenestrato l'impostore (in gioca incrementerà il contatore di impostori defenestrati)
      }
      else
        printf("Non sei stato scoperto.....ancora\n\n");
      break;
    case 70:
      if (1 + rand() % 100 <= 70) {
        printf("Un astronauta ti ha visto uccidere il compagno e ora sei stato defenestrato!\n\n");
        impostore -> Stato_giocatore = defenestrato;
        return 1;   //torna 1, cioè è stato defenestrato l'impostore (in gioca incrementerà il contatore di impostori defenestrati)
      }
      else
        printf("Non sei stato scoperto.....ancora\n\n");
      break;
    case 80:
      if (1 + rand() % 100 <= 80) {
        printf("Un astronauta ti ha visto uccidere il compagno e ora sei stato defenestrato!\n\n");
        impostore -> Stato_giocatore = defenestrato;
        return 1;   //torna 1, cioè è stato defenestrato l'impostore (in gioca incrementerà il contatore di impostori defenestrati)
      }
      else
        printf("Non sei stato scoperto.....ancora\n\n");
      break;
    case 90:
      if (1 + rand() % 100 <= 90) {
        printf("Un astronauta ti ha visto uccidere il compagno e ora sei stato defenestrato!\n\n");
        impostore -> Stato_giocatore = defenestrato;
        return 1;   //torna 1, cioè è stato defenestrato l'impostore (in gioca incrementerà il contatore di impostori defenestrati)
      }
      else
        printf("Non sei stato scoperto.....ancora\n\n");
      break;
    case 100:
      printf("Un astronauta ti ha visto uccidere il compagno e ora sei stato defenestrato!\n\n");
      impostore -> Stato_giocatore = defenestrato;
      return 1;   //torna 1, cioè è stato defenestrato l'impostore (in gioca incrementerà il contatore di impostori defenestrati)
      break;
    default:   //essendo probabilita_defenestraggio unsigned int il caso di default si ha quando questa assume valori superiori a 100
      printf("Un astronauta ti ha visto uccidere il compagno e ora sei stato defenestrato!\n\n");
      impostore -> Stato_giocatore = defenestrato;
      return 1;   //torna 1, cioè è stato defenestrato l'impostore (in gioca incrementerà il contatore di impostori defenestrati)
      break;
  }

  return 0;   //l'impostore non è stato defenestrato e quindi in gioca non viene incrementato il contatore degli impostori defenestrati
}   //fine uccidi_astronauta



void usa_botola(struct Giocatore* impostore) {
  int contatore_botole = 0;
  int contatore_tot = 0;
  struct Stanza* prima_stanza = lista_stanze;   //evito di passare il puntatore stanza_inizio
  struct Stanza* posizione_prima = impostore -> posizione;
  lista_stanze = prima_stanza;   //reimposto la lista come prima per farla scorrere una seconda volta

  do {
    if (lista_stanze -> Tipo_stanza == botola)
      contatore_botole++;

    contatore_tot++;

    lista_stanze = lista_stanze -> pNext;
  } while (lista_stanze != NULL);   //scorre la lista solo per contare le stanze di tipo botola
  lista_stanze = prima_stanza;   //reimposto la lista come prima per farla scorrere una seconda volta

  int scelta_stanza = 0;   //variabile per scegliere a caso una stanza di tipo botola dalla lista delle stanze
  if (contatore_tot == 1) {
    //non fa nulla dato che la prima è l'unica stanza creata
  }
  else if (contatore_botole == 1) {   //se non ci sono altre botole esclusa quella usata, il giocatore finisce a caso su una delle stanze create
    scelta_stanza = 1 + rand() % contatore_tot;
    do {
      if (1 + rand() % contatore_tot == scelta_stanza) {
        impostore -> posizione = lista_stanze;
        if (impostore -> posizione == posizione_prima) {
          lista_stanze = prima_stanza;
          continue;
        }
      }

      lista_stanze = lista_stanze -> pNext;
    } while (lista_stanze != NULL);
  }


  if (contatore_botole == 2) {   //se ci sono esattamente due botole, l'impostore "rimbalza" tra queste due
    do {
      if (lista_stanze != posizione_prima) {
        if (lista_stanze -> Tipo_stanza == botola)
          impostore -> posizione = lista_stanze;
      }

      lista_stanze = lista_stanze -> pNext;
    } while (lista_stanze != NULL);
  }


  if (contatore_botole > 2) {   //se ci sono più di tre botole c'è un 30% di probabilità per ogni stanza di tipo botola, che l'impostore finisca in una di queste
    do {
      if (lista_stanze -> Tipo_stanza == botola) {
        if (1 + rand() % 100 <= 30) {
          impostore -> posizione = lista_stanze;

          if (impostore -> posizione == posizione_prima) {   //se la sua posizione non è cambiata allora ricomincia il ciclo per assegnargni una posizione diversa
            lista_stanze = prima_stanza;
            continue;
          }
        }
      }

      lista_stanze = lista_stanze -> pNext;
    } while (lista_stanze != NULL);
  }
  lista_stanze = prima_stanza;   //reimpostato per nuovi possibili usi come in termina_gioco

  return;
}   //fine usa_botola



void sabotaggio(struct Giocatore* impostore) {
  (impostore -> posizione) -> Tipo_stanza = vuota;
  printf("\nÈ stato manomesso il circuito ed ora la stanza è di tipo vuota\n");
  return;
}   //fine sabotaggio



int termina_gioco(struct Giocatore* puntatore_giocatori) {
  char continuare = 0;

  if (puntatore_giocatori == NULL)
    printf("\nNon è stato creato nessun giocatore.\n");
  else
    free(puntatore_giocatori);

  if (lista_stanze == NULL)
    printf("Non è stata creata nessuna stanza.\n\n");   //quindi non c'è nulla da deallocare
  else {
    struct Stanza* temp = NULL;
    do {
      temp = lista_stanze;
      lista_stanze = lista_stanze -> pNext;
      free(temp);

    } while (lista_stanze != NULL);
    printf("Tutta la memoria è stata liberata con successo.\n");
  }

  while(getchar() != '\n');
  printf("Continuare? y/n ");
  scanf("%c", &continuare);
  if (continuare == 'y' || continuare == 'Y')
    return 1;
  else if (continuare == 'n' || continuare == 'N') {
    printf("\nAlla prossima!\n\n");
    return 0;
  }
  return 0;
}   //fine termina_gioco
