
enum Stato_giocatore {astronauta, impostore, assassinato, defenestrato};

enum Nome_giocatore {rosso, giallo, blu, arancione, verde, viola, nero, marrone, bianco, rosa};

enum Tipo_stanza {vuota, quest_semplice, quest_doppia, botola};

struct Stanza {
  struct Stanza* avanti;
  struct Stanza* sinistra;
  struct Stanza* destra;
  struct Stanza* stanza_precedente;
  enum Tipo_stanza Tipo_stanza;
  int emergenza_chiamata;
  struct Stanza* pNext;
};

struct Giocatore {
  enum Nome_giocatore Nome_giocatore;
  enum Stato_giocatore Stato_giocatore;
  struct Stanza* posizione;
};

struct Giocatore* imposta_gioco();
void gioca ();
int termina_gioco();
