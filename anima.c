// programa para animar o elevador (ver www.inf.ufsm.br/~benhur/so2015b)

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

// o elevador

struct {
  int andar;    // onde ta
  bool bi[5];   // botoes internos
  bool bs[5];   // botoes ext - subir
  bool bd[5];   // botoes ext - descer
  bool aberta;  // (a porta)
  int npessoas; // carga
  int id[3];    // quem esta sendo carregado
} el;

// as pessoas

typedef enum { dentro, espera, fora } local_t;
typedef enum { zumbi, bloqueado, ativo } estado_t;
struct {
  int andar;        // onde ta
  local_t local;    // em que local do andar ele está
  estado_t estado;  // da thread (bloqueada ou nao)
} p[99];

// calculo da posicao (coord x, y) de uma pessoa
// id é a pessoa, andar é o andar, local é onde, em relacao ao elev,
//   lugar é qual dos 3 lugares se estiver dentro do elev.
float posx(int id, int andar, local_t local, int lugar)
{
  float x;
  id--;
  if (local == dentro) {
    x = lugar * 2.5 + 3.5;
  } else {
    x = 15 + (id % 10) * 2 + ((id/10+id/50)%2) + ((id/50)%2);
    if (local == fora) {
      x += 22;
    }
  }
  return x;
}

float posy(int id, int andar, local_t local, int lugar)
{
  float y;
  id--;
  y = 29.5 - andar * 6;
  if (local == espera || local == fora) {
    y -= ((id / 10) % 5);
  }
  return y;
}

// o tempo da animacao

float t_atual;

// animacoes
//   escreve animacoes no arquivo svg, correspondentes a cada evento no arquivo

void an_inicio(void)
{
printf("%s", 
"<svg viewBox='0 0 88 40' xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink'>\n"
"  <g id='elevador'>\n"
"    <rect id='caixa'\n"
"        x='1' y='25' width='10' height='6' fill='white' stroke='black' />\n"
"    <rect id='porta'\n"
"        x='11' y='29' width='1' height='0' stroke='white' fill='white' />\n"
"    <circle id='b0' cx='2' cy='30' r='.45' fill='gray' stroke='transparent' />\n"
"    <circle id='b1' cx='2' cy='29' r='.45' fill='gray' stroke='transparent' />\n"
"    <circle id='b2' cx='2' cy='28' r='.45' fill='gray' stroke='transparent' />\n"
"    <circle id='b3' cx='2' cy='27' r='.45' fill='gray' stroke='transparent' />\n"
"    <circle id='b4' cx='2' cy='26' r='.45' fill='gray' stroke='transparent' />\n"
"  </g>\n"
"  <circle id='b4s' cx='12' cy='3' r='.45' fill='gray' stroke='transparent' />\n"
"  <circle id='b4d' cx='12' cy='4' r='.45' fill='gray' stroke='transparent' />\n"
"  <circle id='b3s' cx='12' cy='9' r='.45' fill='gray' stroke='transparent' />\n"
"  <circle id='b3d' cx='12' cy='10' r='.45' fill='gray' stroke='transparent' />\n"
"  <circle id='b2s' cx='12' cy='15' r='.45' fill='gray' stroke='transparent' />\n"
"  <circle id='b2d' cx='12' cy='16' r='.45' fill='gray' stroke='transparent' />\n"
"  <circle id='b1s' cx='12' cy='21' r='.45' fill='gray' stroke='transparent' />\n"
"  <circle id='b1d' cx='12' cy='22' r='.45' fill='gray' stroke='transparent' />\n"
"  <circle id='b0s' cx='12' cy='27' r='.45' fill='gray' stroke='transparent' />\n"
"  <circle id='b0d' cx='12' cy='28' r='.45' fill='gray' stroke='transparent' />\n");
}
void an_fim(void)
{
  printf("%s", "</svg>\n");
}
void an_move_el(int andar1, int andar2)
{
  int deltay = (andar1 - andar2) * 6;
  float duracao = abs(deltay) * 0.1;
  float t0 = t_atual;
  int i;

  printf("<animateTransform "
    "xlink:href='#elevador' "
    "attributeName='transform' "
    "type='translate' "
    "from='0 %d' "
    "to='0 %d' "
    "dur='%.1fs' "
    "begin='%.1fs' "
    "fill='freeze'/>\n", -andar1*6, -andar2*6, duracao, t0);

  for (i=0; i<3; i++) {
    if (el.id[i] != 0) {
      printf("<animateTransform "
        "xlink:href='#t%d' "
        "attributeName='transform' "
        "type='translate' "
        "from='%.1f %.1f' "
        "to='%.1f %.1f' "
        "dur='%.1fs' "
        "begin='%.1fs' "
        "fill='freeze'/>\n", el.id[i],
          posx(el.id[i], andar1, dentro, i), posy(el.id[i], andar1, dentro, i),
          posx(el.id[i], andar2, dentro, i), posy(el.id[i], andar2, dentro, i),
          duracao, t0);
    }
  }

  t_atual += duracao;
}
void an_abre(void)
{
  float duracao = 0.3;
  printf("<animate "
    "xlink:href='#porta' "
    "attributeName='height' "
    "from='0' "
    "to='1' "
    "dur='%.1fs' "
    "begin='%.1fs' "
    "fill='freeze'/>\n", duracao, t_atual);
  t_atual += duracao;
}
void an_fecha(void)
{
  float duracao = 0.3;
  printf("<animate "
    "xlink:href='#porta' "
    "attributeName='height' "
    "from='1' "
    "to='0' "
    "dur='%.1fs' "
    "begin='%.1fs' "
    "fill='freeze'/>\n", duracao, t_atual);
  t_atual += duracao;
}
void an_bloqueou(int id)
{
  float duracao = 0.3;
  printf("<animate "
    "xlink:href='#t%dc' "
    "attributeName='fill' "
    "from='green' "
    "to='gray' "
    "dur='%.1fs' "
    "begin='%.1fs' "
    "fill='freeze'/>\n", id, duracao, t_atual);
  t_atual += duracao;
}
void an_acordou(int id)
{
  float duracao = 0.3;
  printf("<animate "
    "xlink:href='#t%dc' "
    "attributeName='fill' "
    "from='gray' "
    "to='green' "
    "dur='%.1fs' "
    "begin='%.1fs' "
    "fill='freeze'/>\n", id, duracao, t_atual);
  t_atual += duracao;
}
void an_morreu(int id)
{
  float duracao = 0.3;
  printf("<animate "
    "xlink:href='#t%dc' "
    "attributeName='fill' "
    "from='green' "
    "to='transparent' "
    "dur='%.1fs' "
    "begin='%.1fs' "
    "fill='freeze'/>\n", id, duracao, t_atual);
  t_atual += duracao;
}
void an_aperta_s(int id, int andar)
{
  float duracao = 0.4;
  printf("<set xlink:href='#b%ds' attributeName='fill' to='yellow' "
         "begin='%.1fs' dur='1s'/>\n", andar, t_atual);
  printf("<set xlink:href='#t%dc' attributeName='fill' to='yellow' "
         "begin='%.1fs' dur='1s'/>\n", id, t_atual+.1);
  printf("<set xlink:href='#b%ds' attributeName='fill' to='yellow' "
         "begin='%.1fs' dur='1s'/>\n", andar, t_atual+.2);
  printf("<set xlink:href='#b%ds' attributeName='fill' to='red' "
         "begin='%.1fs'/>\n", andar, t_atual+.3);
  t_atual += duracao;
}
void an_aperta_d(int id, int andar)
{
  float duracao = 0.4;
  printf("<set xlink:href='#b%dd' attributeName='fill' to='yellow' "
         "begin='%.1fs' dur='.1s'/>\n", andar, t_atual);
  printf("<set xlink:href='#t%dc' attributeName='fill' to='yellow' "
         "begin='%.1fs' dur='.1s'/>\n", id, t_atual+0.1);
  printf("<set xlink:href='#b%dd' attributeName='fill' to='yellow' "
         "begin='%.1fs' dur='.1s'/>\n", andar, t_atual+0.2);
  printf("<set xlink:href='#b%dd' attributeName='fill' to='red' "
         "begin='%.1fs' fill='freeze'/>\n", andar, t_atual+0.3);
  t_atual += duracao;
}
void an_aperta_i(int id, int andar)
{
  float duracao = 0.4;
  printf("<set xlink:href='#b%d' attributeName='fill' to='yellow' "
         "begin='%.1fs' dur='.1s'/>\n", andar, t_atual);
  printf("<set xlink:href='#t%dc' attributeName='fill' to='yellow' "
         "begin='%.1fs' dur='.1s'/>\n", id, t_atual+0.1);
  printf("<set xlink:href='#b%d' attributeName='fill' to='yellow' "
         "begin='%.1fs' dur='.1s'/>\n", andar, t_atual+0.2);
  printf("<set xlink:href='#b%d' attributeName='fill' to='red' "
         "begin='%.1fs' fill='freeze'/>\n", andar, t_atual+0.3);
  t_atual += duracao;
}
void an_apaga_s(int andar)
{
  float duracao = 0.1;
  printf("<set xlink:href='#b%ds' attributeName='fill' to='gray' "
         "begin='%.1fs' fill='freeze'/>\n", andar, t_atual);
  t_atual += duracao;
}
void an_apaga_d(int andar)
{
  float duracao = 0.1;
  printf("<set xlink:href='#b%dd' attributeName='fill' to='gray' "
         "begin='%.1fs' fill='freeze'/>\n", andar, t_atual);
  t_atual += duracao;
}
void an_apaga_i(int andar)
{
  float duracao = 0.1;
  printf("<set xlink:href='#b%d' attributeName='fill' to='gray' "
         "begin='%.1fs' fill='freeze'/>\n", andar, t_atual);
  t_atual += duracao;
}
void an_move_espera(int id)
{
  float duracao = 1;
  float t0 = t_atual;
  printf("<animateTransform "
    "xlink:href='#t%d' "
    "attributeName='transform' "
    "type='translate' "
    "from='%.1f %.1f' "
    "to='%.1f %.1f' "
    "dur='%.1fs' "
    "begin='%.1fs' "
    "fill='freeze'/>\n", id, 
      posx(id, p[id].andar, fora, 0), posy(id, p[id].andar, fora, 0),
      posx(id, p[id].andar, espera, 0), posy(id, p[id].andar, espera, 0),
      duracao, t0);
  t_atual += duracao;
}
void an_entra(int id, int pos)
{
  float duracao = 1;
  float t0 = t_atual;
  printf("<animateTransform "
    "xlink:href='#t%d' "
    "attributeName='transform' "
    "type='translate' "
    "from='%.1f %.1f' "
    "to='%.1f %.1f' "
    "dur='%.1fs' "
    "begin='%.1fs' "
    "fill='freeze'/>\n", id, 
      posx(id, p[id].andar, espera, 0), posy(id, p[id].andar, espera, 0),
      posx(id, p[id].andar, dentro, pos), posy(id, p[id].andar, dentro, pos),
      duracao, t0);
  t_atual += duracao;
}
void an_sai(int id, int pos)
{
  float duracao = 1;
  float t0 = t_atual;
  printf("<animateTransform "
    "xlink:href='#t%d' "
    "attributeName='transform' "
    "type='translate' "
    "from='%.1f %.1f' "
    "to='%.1f %.1f' "
    "dur='%.1fs' "
    "begin='%.1fs' "
    "fill='freeze'/>\n", id,
      posx(id, p[id].andar, dentro, pos), posy(id, p[id].andar, dentro, pos),
      posx(id, p[id].andar, fora, 0), posy(id, p[id].andar, fora, 0),
      duracao, t0);
  t_atual += duracao;
}
void an_cria(int id)
{
  float duracao = 0.3;
  float t0 = t_atual;
  float x = 34 + (id-1)/3*0.5;
  float y = 29.5-0.5*((id-1)%3);
  printf("<g id='t%d'> "
            "<circle id='t%dc' "
              "cx='0' cy='0' r='1' fill='gray' stroke='transparent' /> "
            "<text x='0' y='.3' font-family='arial' text-anchor='middle' "
              "font-size='1' stroke-width='.02' fill='yellow' "
              "stroke='yellow'>%d</text> "
         "</g>\n", id, id, id);
  p[id].local = fora;
  p[id].estado = ativo;
  p[id].andar = 0;
  printf("<animateTransform "
    "xlink:href='#t%d' "
    "attributeName='transform' "
    "type='translate' "
    "from='0 0' "
    "to='%.1f %.1f' "
    "dur='%.1fs' "
    "begin='%.1fs' "
    "fill='freeze'/>\n", id,
      posx(id, 0, fora, 0), posy(id, 0, fora, 0),
      duracao, t0);
  t_atual += duracao;
}

// funcoes auxiliares

// retorna true se id estiver em uma das tres posicoes do elevador
bool ta_dentro(int id)
{
  return el.id[0] == id || el.id[1] == id || el.id[2] == id;
}

// processa cada tipo de evento
//   para cada evento, verifica se ele é possível pelo estado do sistema,
//   atualiza o estado e chama a animação correspondente

void abre(int id, int andar)
{
  assert(id == 0);
  assert(!el.aberta);
  if (el.andar != andar) {
    an_move_el(el.andar, andar);
    el.andar = andar;
  }
  an_abre();
  el.aberta = true;
}

void fecha(int id, int andar)
{
  assert(id == 0);
  assert(el.aberta);
  assert(el.andar == andar);
  an_fecha();
  el.aberta = false;
}

void bloqueou(int id, int andar)
{
  assert(p[id].estado == ativo);
  an_bloqueou(id);
  p[id].estado = bloqueado;
}

void acordou(int id, int andar)
{
  assert(p[id].estado != ativo);
  if (p[id].estado == zumbi) {
    assert(andar == 0);
    an_cria(id);
  }
  an_acordou(id);
  p[id].estado = ativo;
}

void morreu(int id, int andar)
{
  assert(p[id].estado == ativo);
  an_morreu(id);
  p[id].estado = zumbi;
}

void aperta_s(int id, int andar)
{
  assert(id != 0);
  assert(!ta_dentro(id));
  assert(p[id].andar == andar);
  assert(p[id].estado == ativo);
  assert(p[id].local != dentro);
  if (p[id].local == fora) {
    an_move_espera(id);
    p[id].local = espera;
  }
  an_aperta_s(id, andar);
  el.bs[andar] = true;
}

void aperta_d(int id, int andar)
{
  assert(id != 0);
  assert(!ta_dentro(id));
  assert(p[id].andar == andar);
  assert(p[id].estado == ativo);
  assert(p[id].local != dentro);
  if (p[id].local == fora) {
    an_move_espera(id);
    p[id].local = espera;
  }
  an_aperta_d(id, andar);
  el.bd[andar] = true;
}

void aperta_i(int id, int andar)
{
  assert(id != 0);
  assert(ta_dentro(id));
  assert(p[id].estado == ativo);
  an_aperta_i(id, andar);
  el.bi[andar] = true;
}

void apaga_s(int id, int andar)
{
  assert(id == 0);
  an_apaga_s(andar);
  el.bs[andar] = false;
}

void apaga_d(int id, int andar)
{
  assert(id == 0);
  an_apaga_d(andar);
  el.bd[andar] = false;
}

void apaga_i(int id, int andar)
{
  assert(id == 0);
  an_apaga_i(andar);
  el.bi[andar] = false;
}

void entra(int id, int andar)
{
  assert(id != 0);
  assert(el.aberta);
  assert(el.npessoas < 3);
  assert(!ta_dentro(id));
  assert(andar == el.andar);
  assert(andar == p[id].andar);
  assert(p[id].local == espera);
  assert(p[id].estado == ativo);
  int i;
  for (i=0; i<3; i++) {
    if (el.id[i] == 0) {
      el.id[i] = id;
      el.npessoas++;
      p[id].local = dentro;
      an_entra(id, i);
      break;
    }
  }
  assert(ta_dentro(id));
}

void sai(int id, int andar)
{
  int i;
  assert(id != 0);
  assert(el.aberta);
  assert(andar == el.andar);
  assert(ta_dentro(id));
  assert(p[id].estado == ativo);
  for (i=0; i<3; i++) {
    if (el.id[i] == id) {
      el.id[i] = 0;
      el.npessoas--;
      p[id].andar = el.andar;
      p[id].local = fora;
      an_sai(id, i);
      break;
    }
  }
  assert(!ta_dentro(id));
}

// cada linha do arquivo de entrada contém um evento
typedef struct {
  int id;
  long data;
  char tipo;
  int andar;
} evento;

evento le_evento(FILE *arq)
{
  evento e = {0};
  char s[50];
  if (fgets(s, 50, arq) != NULL) {
    sscanf(s, "%d%ld %c%d", &e.id, &e.data, &e.tipo, &e.andar);
  }
  return e;
}

void processa_arquivo(FILE *arq)
{
  evento e;
  for (;;) {
    e = le_evento(arq);
    switch (e.tipo) {
      case 'A': abre(e.id, e.andar); break;
      case 'F': fecha(e.id, e.andar); break;
      case 'B': bloqueou(e.id, e.andar); break;
      case 'E': acordou(e.id, e.andar); break;
      case 'M': morreu(e.id, e.andar); break;
      case 'S': aperta_s(e.id, e.andar); break;
      case 'D': aperta_d(e.id, e.andar); break;
      case 'I': aperta_i(e.id, e.andar); break;
      case 's': apaga_s(e.id, e.andar); break;
      case 'd': apaga_d(e.id, e.andar); break;
      case 'i': apaga_i(e.id, e.andar); break;
      case 'N': entra(e.id, e.andar); break;
      case 'V': sai(e.id, e.andar); break;
      default: return;
    }
  }
}

int main()
{
  an_inicio();
  processa_arquivo(stdin);
  an_fim();
  return 0;
}
