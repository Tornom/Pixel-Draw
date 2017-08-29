#include <iostream>
#include <math.h>
#include <allegro5/allegro.h> // include principal da allegro;
#include <allegro5/allegro_font.h> //adiciona o uso de fontes;
#include <allegro5/allegro_ttf.h> //adiciona o uso de fontes;
#include <allegro5/allegro_primitives.h>//adiciona o uso de primitivas;
#include <allegro5/allegro_image.h> //adiciona o uso de imagens;
#include <allegro5/allegro_audio.h>//adiciona o uso audio;
#define FPS 100.0 // importante colocar o ".0" no final;

enum {quadrado,retangulo,circulo,elipse};

typedef struct{
int r;
int g;
int b;
}cor;

typedef struct{
ALLEGRO_BITMAP *plano_de_fundo,*config,*tumb,*toolbox,*flecha,*corretivo,*opt[4],*change,*retback,*varia,*outimage,*outimage2,*pointer;
bool repinta;

}imagem;

typedef struct{//estrutura do usuário;
float x;//coordenadas do mouse;
float y;
cor cortraz;//cor de fundo;
cor qua;//cor do quadrado;
cor ret;//cor do retangulo;
cor cir;//cor do circulo;
cor eli;//cor da elipse;
short forma;//qual ferramenta está em uso;
short quant;//quantos passos(variação de valores) são dados por clique;
short alteravalores;//qual botão para alterar as propriedades das ferramentas;
bool apertou;//mantem o controle de quando o usuário apertou o mouse;
bool travou;// fixa a tela de ferramentas para que esta não altere o seu estado;
bool atualizar;//fica verdadeira caso seja pressionado qualquer botão de alteração de propriedades;
bool temarrow;//verdadeira caso uma seta de seleção esteja presente na tela;
bool nobotao;//verdadeira quando o mouse esta sobre um botão de ferramenta;
bool apertouarrow[5];//mantem controle de qual botao de ferramneta foi pressionado;
bool alteravalor[2];//[0] verdadeiro reduz a propriedade indicada,[1] verdadeiro aumenta a propriedade indicada;
bool qualbot[5];// qual botão do menu está em uso;
bool umavez;// somente executa a função uma vez;
bool qualconfig[5];//controla se o mouse está dentro dos limites para pressionar os botoes de propriedades;
bool crianovo;//verdadeira caso for necessário criar uma nova tela de pintura;
bool ativatipo[5];//guarda o ultimo valor de "qualconfig";
float tamanhoq;//tamanho da ferramenta quadrada;
float tamanhoc;//tamanho da ferramenta circular;
float tamanhoex;//tamanho da ferramenta da elípse em x;
float tamanhoey;//tamanho da ferramenta da elípse em y;
float tamanhorx;//tamanho da ferramenta retangular em x;
float tamanhory;//tamanho da ferramenta retangular em y;

int telax,telay;//limites da tela de pintura;

ALLEGRO_DISPLAY *pintura[10],*configura;//Telas do usuário;

}cursor;

using namespace std;

imagem img;

cursor pinta;

void novapintura ();//principal função, função de desenho;

void toolbox(int tipo);//função que ativa flags para determinar qual ferramenta esta sendo utilizada;

void alteraconf(int tipo,int qualconf, bool altera, short quant); //função que altera os parâmetros das ferramentas; tipo -> qual ferramenta;
// qualconf -> qual parâmetro a ser alterado; altera -> aumenta para verdadeiro, reduz para falso; quant -> quanto varia a cada operação;

float funcaocoli(int y,int a,int correcao);//função para reconhecer colizão em objetos não retangulares;

void botaoconfi();//reconhece qual botão está em uso;

int retornavalor(int atual);//função que altera o valor "Passo";

int main()
{
    //variaveis allegro;
    ALLEGRO_DISPLAY *tela = NULL;
    ALLEGRO_TIMER *tempo;
    ALLEGRO_EVENT_QUEUE *fila = NULL;
    ALLEGRO_MOUSE_STATE rato;
    ALLEGRO_FONT *fonte;

    fonte  = al_load_font(".\\Assets\\Fonte\\arial.ttf",20,0);

    //inicialização;
    al_init();
    tela = al_create_display(600,400);
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_init_primitives_addon();
    al_install_keyboard();
    al_install_mouse();

    //eventos e temporizador;
    fila = al_create_event_queue();
    al_register_event_source(fila,al_get_keyboard_event_source());
    al_register_event_source(fila,al_get_mouse_event_source());
    al_register_event_source(fila,al_get_display_event_source(tela));
    tempo = al_create_timer(1.0/FPS);
    al_register_event_source(fila,al_get_timer_event_source(tempo));

    //flag para repintar a tela;
    img.repinta = true;
    //tamanho ferramenta;
    pinta.tamanhoq = 4;
    pinta.tamanhoc = 3;
    pinta.tamanhorx = 5;
    pinta.tamanhory = 10;
    pinta.tamanhoex = 10;
    pinta.tamanhoey = 5;
    //criar nova tela;
    pinta.crianovo = false;
    //Passos;
    pinta.quant = 2;
    //forma ferramenta;
    pinta.forma = quadrado;
    //tamanho tela pintura padrao;
    pinta.telax = 600;
    pinta.telay = 400;
    //cor quadrado;
    pinta.qua.r = 255;
    pinta.qua.g = 255;
    pinta.qua.b = 255;
    //cor retangulo;
    pinta.ret.r = 255;
    pinta.ret.g = 255;
    pinta.ret.b = 255;
    //cor circulo;
    pinta.cir.r = 255;
    pinta.cir.g = 255;
    pinta.cir.b = 255;
    //cor elipse;
    pinta.eli.r = 255;
    pinta.eli.g = 255;
    pinta.eli.b = 255;
    pinta.atualizar = false;
    //plano de fundo do menu inicial;
    img.plano_de_fundo = al_load_bitmap(".\\Assets\\Imagem\\back.png");
    //imagens;
    img.tumb = al_load_bitmap(".\\Assets\\Imagem\\tumb.png");
    img.varia = al_load_bitmap(".\\Assets\\Imagem\\varia.png");
    img.pointer = al_load_bitmap(".\\Assets\\Imagem\\pointer.png");
    al_draw_bitmap(img.plano_de_fundo,0,0,0);
    //animação do menu inicial;
    int m = 0;
    int n = 0;
    int x,y;
    bool vaivem0 = false;
    bool vaivem1 = false;
    bool umaturn = true;
    srand((unsigned)time(NULL));

    x = ((int)rand%20);
    y = ((int)rand%10);
    //altera o icone da tela;
    al_set_display_icon(tela,img.tumb);

    al_start_timer(tempo);
    while(1)//loop do main;
    {
        ALLEGRO_EVENT evento;

        al_wait_for_event(fila, &evento); //função que trava o programa até que apareça algum evento na fila;

        if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            break;//encerra caso o "X" do display for pressionado;
        }

        if(evento.type == ALLEGRO_EVENT_TIMER)
        {
            //animação inicial;
            if(!pinta.nobotao)al_draw_bitmap(img.plano_de_fundo,0,0,0);
            if((n*y)/8.0 > 2124 - 400) vaivem0 = !vaivem0;   //inverte sentido de deslocamento em y;
            if((m*x)/12.0 > 1920 - 198 ) vaivem1 = !vaivem1; //inverte sentido de deslocamento em x;
            al_draw_tinted_scaled_rotated_bitmap_region(img.varia,(m*x)/12.0,(n*y)/8.0,596,854,al_map_rgb(255,255,255),0,0,198,-2,2,2,0,0); //imprime parte da imagem de fundo;
            img.repinta = true;
            if(++n > 10000 && (!vaivem0 || umaturn))//ao atingir a borda da tela uma nova velocidade em x e em y é atrubuida;
            {
                x = ((int)rand%5);
                y = ((int)rand%10);
                vaivem0 = true;
                umaturn = false;
            }
            else if(vaivem0)
            {
                x = ((int)rand%5);
                y = ((int)rand%10);
                if((n = n - 2) < 0 ) vaivem0 = false;
            }

            if((m = m + 2) > 10000 && !vaivem1)//ao atingir a borda da tela uma nova velocidade em x e em y é atrubuida;
            {
                x = ((int)rand%5);
                y = ((int)rand%10);
                vaivem1 = true;
            }

            else if(vaivem1)
            {
                x = ((int)rand%5);
                y = ((int)rand%10);
                if((m = m -4) < 0) vaivem1 = false;
            }
        }//fim timer;

        if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            if(evento.mouse.button & 1)//qual botão foi pressionado;
            {
                if(pinta.qualbot[0])
                {
                    pinta.crianovo = true;//cria nova pintura;
                }

                if(pinta.qualbot[1])
                {
                    pinta.qualbot[1] = false;
                }

                if(pinta.qualbot[3]) break;//termina o programa ao clicar em "Sair";
            }

            if(evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                break;//termina o programa ao pressionar "ESC";
            }
        }

        if(evento.type == ALLEGRO_EVENT_MOUSE_AXES)
        {
            /*
                função que retorna valores próprios para o mouse,
                como coordenadas,qual display, qual botão foi pressionado, etc;
            */
            al_get_mouse_state(&rato);

            //mantem controle de qual botão esta sob o mouse;
            if(rato.x > 15 && rato.x < 187)
            {
                if(rato.y > 115 && rato.y < 136)
                {
                    al_draw_line(15,138,187,138,al_map_rgb(255,255,255),2);
                    img.repinta = true;
                    pinta.qualbot[0] = true;
                }

                else pinta.qualbot[0] = false;

            }

            else pinta.qualbot[0] = false;

            if(rato.x > 15 && rato.x < 187)
            {
                if(rato.y > 184 && rato.y < 209)
                {
                    al_draw_line(15,210,187,210,al_map_rgb(255,255,255),2);
                    img.repinta = true;
                    pinta.qualbot[1] = true;
                }
                else pinta.qualbot[1] = false;
            }

            else pinta.qualbot[1] = false;

            if(rato.x > 20 && rato.x < 190)
            {
                if(rato.y > 257 && rato.y < 279)
                {
                    al_draw_line(15,281,187,281,al_map_rgb(255,255,255),2);
                    img.repinta = true;
                    pinta.qualbot[2] = true;
                }
                else pinta.qualbot[2] = false;
            }

            else pinta.qualbot[2] = false;

            if(rato.x > 75 && rato.x < 118)
            {
                if(rato.y > 331 && rato.y < 348)
                {
                    al_draw_line(75,350,118,350,al_map_rgb(255,255,255),2);
                    img.repinta = true;
                    pinta.qualbot[3] = true;
                }
                else pinta.qualbot[3] = false;
            }
            else pinta.qualbot[3] = false;

            int conta = 0;
            for(int i = 0; i < 5;i++)
            {
                if(pinta.qualbot[i])
                {
                    conta++;
                }
            }

            if(conta > 0) pinta.nobotao = true;
            else pinta.nobotao = false;
        }

        if(al_is_event_queue_empty(fila))
        {
            if(pinta.crianovo)          //caso o botao novo desenho tenha sido pressionado;
            {
                al_stop_timer(tempo);   //para o timer de eventos;
                pinta.crianovo = false;
                pinta.qualbot[0] = false;
                novapintura();          //função para lidar com a pintura;
                /*
                    seleciona o backbuffer do menu como alvo;(backbuffer) = elemento onde primeiramente
                    são desenhados os bitmaps,tela "invisivel", torna-se visível com o comado
                    al_flip_display();
                */
                al_set_target_backbuffer(tela);
                m = 0;
                n = 0;
                al_start_timer(tempo);//reinicia o timer;
            }

            if(img.repinta)//caso a tela atual possui elementos diferentes da ultima tela, o display será refeito;
            {
                img.repinta = false;
                /*
                    Comando para tornar o backbuffer do display atual visivel;
                    O display atual pode ser alterado com o comando al_set_target_backbuffer(ALLEGRO_DISPLAY*);
                */
                al_flip_display();
            }
        }
    }

    /*
        funções de destruição, podem ser comparadas com a função free(); de alocação dinâmica;
        tais elemntos do tipo ALLEGRO* necessitam de um al_destroy para liberarem espaço de memória
    */

    al_destroy_bitmap(img.plano_de_fundo);
    al_destroy_bitmap(img.tumb);
    al_destroy_bitmap(img.pointer);
    al_destroy_bitmap(img.varia);
    al_destroy_display(tela);
    al_destroy_event_queue(fila);
    al_destroy_font(fonte);
    al_destroy_timer(tempo);

    return 0;
}


void novapintura ()
{
    bool terminou = false;   //verdadeira caso o programa terminou;
    pinta.travou = false;

    //variaveis allegro;
    ALLEGRO_EVENT_QUEUE *fila2 = NULL;
    ALLEGRO_TIMER *tempo2;
    ALLEGRO_MOUSE_STATE rato;
    ALLEGRO_FONT *fonte;

    int troca = 0; //qual ferramenta está em uso no momento;

    fonte  = al_load_font(".\\Assets\\Fonte\\arial.ttf",10,0);

    /*
        Cria tela com tamanho extra em X
        para acomodar a barra de ferramentas;
    */
    pinta.pintura[0] = al_create_display(pinta.telax + 152,pinta.telay);

    //Definições padrão;
    img.toolbox = al_load_bitmap(".\\Assets\\Imagem\\toolbox.png");
    img.flecha = al_load_bitmap(".\\Assets\\Imagem\\flecha.png");
    img.corretivo = al_load_bitmap(".\\Assets\\Imagem\\corretivo.png");
    img.opt[0] = al_load_bitmap(".\\Assets\\Imagem\\qua.png");
    img.opt[1] = al_load_bitmap(".\\Assets\\Imagem\\retangulo.png");
    img.opt[2] = al_load_bitmap(".\\Assets\\Imagem\\circulo.png");
    img.opt[3] = al_load_bitmap(".\\Assets\\Imagem\\elip.png");
    img.change = al_load_bitmap(".\\Assets\\Imagem\\retchange.png");
    img.retback = al_load_bitmap(".\\Assets\\Imagem\\retback.png");
    pinta.alteravalor[0] = false;
    pinta.alteravalor[1] = false;
    pinta.alteravalores = 0;
    //cor padrão para fundo;
    pinta.cortraz.r = 0;
    pinta.cortraz.g = 0;
    pinta.cortraz.b = 0;

    al_set_target_backbuffer(pinta.pintura[0]); //Direciona o programa para o backbuffer da tela de pintura;

    //inicializações da fila/eventos;
    fila2 = al_create_event_queue();
    al_register_event_source(fila2,al_get_keyboard_event_source());
    al_register_event_source(fila2,al_get_mouse_event_source());
    al_register_event_source(fila2,al_get_display_event_source(pinta.pintura[0]));
    tempo2 = al_create_timer(1.0/FPS);
    al_register_event_source(fila2,al_get_timer_event_source(tempo2));

    al_set_display_icon(pinta.pintura[0],img.tumb);//muda o ícone da tela para "img.tumb";

    al_clear_to_color(al_map_rgb(pinta.cortraz.r,pinta.cortraz.g,pinta.cortraz.b));//limpa a tela para a cor predefinida;
    al_draw_bitmap(img.toolbox,0,0,0);//imprime a caixa de ferramentas;
    al_draw_bitmap(img.opt[0],7,196,0);//junto com as preferências;
    al_flip_display();

    al_start_timer(tempo2);
    while(1)
    {
        ALLEGRO_EVENT evento;

        al_wait_for_event(fila2,&evento);

        if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            terminou = true;
        }

        if(evento.type == ALLEGRO_EVENT_MOUSE_AXES)
        {
            al_get_mouse_state(&rato);
            pinta.x = rato.x;
            pinta.y = rato.y;
            if(pinta.x > 152)al_hide_mouse_cursor(pinta.pintura[0]);
            else al_show_mouse_cursor(pinta.pintura[0]);

            if(pinta.x > 106 && pinta.x < 122)
            {
                if(pinta.y > 106 && pinta.y < 123)
                {
                    pinta.qualbot[quadrado] = true;
                    img.repinta = true;
                    if(!pinta.temarrow)al_draw_bitmap(img.flecha,96,108,0);
                    pinta.temarrow = true;
                }

                else pinta.qualbot[quadrado] = false;
            }

            else pinta.qualbot[quadrado] = false;

            if(pinta.x > 95 && pinta.x < 122)
            {
                if(pinta.y > 126 && pinta.y < 139)
                {
                    pinta.qualbot[retangulo] = true;
                    img.repinta = true;
                    if(!pinta.temarrow)al_draw_bitmap(img.flecha,85,129,0);
                    pinta.temarrow = true;
                }
                else pinta.qualbot[retangulo] = false;
            }

            else pinta.qualbot[retangulo] = false;

            if(pinta.x > 106 && pinta.x < 122)
            {
                if(pinta.y > 146 && pinta.y < 162)
                {
                    pinta.qualbot[circulo] = true;
                    img.repinta = true;
                    if(!pinta.temarrow)al_draw_bitmap(img.flecha,96,150,0);
                    pinta.temarrow = true;
                }

                else pinta.qualbot[circulo] = false;
            }

            else pinta.qualbot[circulo] = false;

            if(pinta.x > 104 && pinta.x < 126)
            {
                if(pinta.y > 170 && pinta.y < 182)
                {
                    pinta.qualbot[elipse] = true;
                    img.repinta = true;
                    if(!pinta.temarrow)al_draw_bitmap(img.flecha,94,173,0);
                    pinta.temarrow = true;
                }

                else pinta.qualbot[elipse] = false;
            }

            else pinta.qualbot[elipse] = false;

            //qual configuração;

            if(pinta.x > 78 && pinta.x < 125)
            {
                if(pinta.y > 245 && pinta.y < 257)
                {
                    pinta.qualconfig[0] = true;
                }

                else pinta.qualconfig[0] = false;
            }
            else pinta.qualconfig[0] = false;

            if(pinta.x > 78 && pinta.x < 125)
            {
                if(pinta.y > 263 && pinta.y < 275)
                {
                    pinta.qualconfig[1] = true;
                }
                else pinta.qualconfig[1] = false;
            }
            else pinta.qualconfig[1] = false;

            if(pinta.x > 78 && pinta.x < 125)
            {
                if(pinta.y > 283 && pinta.y < 295)
                {
                    pinta.qualconfig[2] = true;
                }
                else pinta.qualconfig[2] = false;
            }
            else pinta.qualconfig[2] = false;

            if(pinta.x > 78 && pinta.x < 125)
            {
                if(pinta.y > 302 && pinta.y < 314)
                {
                    pinta.qualconfig[3] = true;
                }
                else pinta.qualconfig[3] = false;
            }
            else pinta.qualconfig[3] = false;

            if(pinta.x > 78 && pinta.x < 125)
            {
                if(pinta.y > 321 && pinta.y < 333)
                {
                    pinta.qualconfig[4] = true;
                }
                else pinta.qualconfig[4] = false;
            }
            else pinta.qualconfig[4] = false;

            botaoconfi();//botao de configuração - hitbox;

            if(pinta.x > 86 && pinta.x < 119)
            {
                if(pinta.y > 356 && pinta.y < 368)
                {
                    pinta.atualizar = true;
                }
                 else pinta.atualizar = false;
            }
            else pinta.atualizar = false;

        }//fim  mouse axes;

        if(evento.type == ALLEGRO_EVENT_TIMER)
        {
            if(pinta.apertou)
            {
                if(pinta.forma == circulo && pinta.x > 152 + pinta.tamanhoc)al_draw_filled_circle(pinta.x,pinta.y,pinta.tamanhoc,al_map_rgb(pinta.cir.r,pinta.cir.g,pinta.cir.b));
                else if(pinta.forma == quadrado && pinta.x > 152) al_draw_filled_rectangle(pinta.x,pinta.y,pinta.x + pinta.tamanhoq,pinta.y + pinta.tamanhoq, al_map_rgb(pinta.qua.r,pinta.qua.g,pinta.qua.b));
                else if(pinta.forma == retangulo && pinta.x > 152) al_draw_filled_rectangle(pinta.x,pinta.y,pinta.x + pinta.tamanhorx,pinta.y + pinta.tamanhory, al_map_rgb(pinta.ret.r,pinta.ret.g,pinta.ret.b));
                else if(pinta.x > 152 + pinta.tamanhoex)al_draw_filled_ellipse(pinta.x,pinta.y,pinta.tamanhoex,pinta.tamanhoey,al_map_rgb(pinta.eli.r,pinta.eli.g,pinta.eli.b));
                int conta2 = 0;

                for (int j = 0; j < 5; j++)
                {
                    if(pinta.qualbot[j])    //confere qual botão foi selecionado;
                    {
                        pinta.apertouarrow[j] = true;
                        conta2++;
                        troca = j;          //salva o valor do botão pressionado;
                    }
                    else pinta.apertouarrow[j] = false;
                }

                toolbox(troca);             //ativa flags para determinar qual ferramenta esta sendo utilizada;

                if(pinta.travou && !pinta.temarrow)
                switch (troca)              //seleciona qual ferramenta / ativa configuração para ferramenta;
                {
                case(retangulo):
                    {
                        pinta.forma = retangulo;
                        al_draw_bitmap(img.flecha,85,129,0);
                        pinta.temarrow = true;
                        break;
                    }

                    case(circulo):
                    {
                        pinta.forma = circulo;
                        al_draw_bitmap(img.flecha,96,150,0);
                        pinta.temarrow = true;
                        break;
                    }

                    case(elipse):
                    {
                        pinta.forma = elipse;
                        al_draw_bitmap(img.flecha,94,173,0);
                        pinta.temarrow = true;
                        break;
                    }

                    case(quadrado):
                    {
                        pinta.forma = quadrado;
                        al_draw_bitmap(img.flecha,96,108,0);
                        pinta.temarrow = true;
                        break;
                    }
                }

                int parametro = 0;
                for(int m = 0; m < 5; m++)          //procura por qual parametro está para ser modificado;
                {
                    if(pinta.ativatipo[m]) parametro = m;
                }

                if(pinta.alteravalor[1] && pinta.umavez)alteraconf(troca,parametro,1,pinta.quant);

                if(pinta.alteravalor[0] && pinta.umavez)alteraconf(troca,parametro,0,pinta.quant);

                if(pinta.atualizar)
                {

                    pinta.quant = retornavalor(pinta.quant);
                }

                if(conta2 > 0)
                {
                    pinta.travou = true;
                }
                else pinta.travou = false;

                if(pinta.travou)            //travar a imagem da caixa de ferramentas após alguma ser selecionada;
                {
                    al_draw_bitmap(img.toolbox,0,0,0);
                    al_draw_bitmap(img.opt[troca],7,196,0);
                    pinta.temarrow = false;
                }

                img.repinta = true;
                al_get_mouse_state(&rato);
                pinta.x = rato.x;
                pinta.y = rato.y;
            }//fim if apertou;

            else pinta.umavez = true;

            if(pinta.ativatipo[0]) //atualiza a caixa de ferramentas informando que esta foi selecionada;
            {
                al_draw_bitmap(img.change,85,245,0);
                if(troca == 0)
                {
                    al_draw_textf(fonte,al_map_rgb(0,0,0),102,245,ALLEGRO_ALIGN_CENTER,"%.0f",pinta.tamanhoq); // função utilizada para imprimir texto na tela, formatação parecida com printf();
                }
                else if(troca == 1)
                {
                    al_draw_textf(fonte,al_map_rgb(0,0,0),102,245,ALLEGRO_ALIGN_CENTER,"%.0f",pinta.tamanhorx);
                }
                else if(troca == 2)
                {
                    al_draw_textf(fonte,al_map_rgb(0,0,0),102,245,ALLEGRO_ALIGN_CENTER,"%.0f",pinta.tamanhoc);
                }
                else if( troca == 3)
                {
                    al_draw_textf(fonte,al_map_rgb(0,0,0),102,245,ALLEGRO_ALIGN_CENTER,"%.0f",pinta.tamanhoex);
                }
            }
            else if(!pinta.qualconfig[0]) al_draw_bitmap(img.retback,85,245,0);

            if(pinta.ativatipo[1])
            {
                al_draw_bitmap(img.change,85,263,0);
                if(troca == 0 )
                {
                    al_draw_textf(fonte,al_map_rgb(255,0,0),102,263,ALLEGRO_ALIGN_CENTER,"%i",pinta.qua.r);
                }
                else if(troca == 1)
                {
                    al_draw_textf(fonte,al_map_rgb(0,0,0),102,263,ALLEGRO_ALIGN_CENTER,"%.0f",pinta.tamanhory);
                }
                else if(troca == 2)
                {
                    al_draw_textf(fonte,al_map_rgb(255,0,0),102,263,ALLEGRO_ALIGN_CENTER,"%i",pinta.cir.r);
                }
                else if(troca == 3)
                {
                    al_draw_textf(fonte,al_map_rgb(0,0,0),102,263,ALLEGRO_ALIGN_CENTER,"%.0f",pinta.tamanhoey);
                }
            }
            else if(!pinta.qualconfig[1]) al_draw_bitmap(img.retback,85,263,0);

            if(pinta.ativatipo[2])
            {
                al_draw_bitmap(img.change,85,283,0);

                if(troca == 0)
                {
                    al_draw_textf(fonte,al_map_rgb(0,255,0),102,283,ALLEGRO_ALIGN_CENTER,"%i",pinta.qua.g);
                }
                else if(troca == 1)
                {
                    al_draw_textf(fonte,al_map_rgb(255,0,0),102,283,ALLEGRO_ALIGN_CENTER,"%i",pinta.ret.r);
                }

                else if(troca == 2)
                {
                    al_draw_textf(fonte,al_map_rgb(0,255,0),102,283,ALLEGRO_ALIGN_CENTER,"%i",pinta.cir.g);
                }
                else if(troca == 3) al_draw_textf(fonte,al_map_rgb(255,0,0),102,283,ALLEGRO_ALIGN_CENTER,"%i",pinta.eli.r);
            }
            else if(!pinta.qualconfig[2]) al_draw_bitmap(img.retback,85,283,0);

            if(pinta.ativatipo[3])
            {
                al_draw_bitmap(img.change,85,302,0);

                if(troca == 0)
                {
                    al_draw_textf(fonte,al_map_rgb(0,0,255),102,302,ALLEGRO_ALIGN_CENTER,"%i",pinta.qua.b);
                }
                else if(troca == 1)
                {
                    al_draw_textf(fonte,al_map_rgb(0,255,0),102,302,ALLEGRO_ALIGN_CENTER,"%i",pinta.ret.g);
                }

                else if(troca == 2)
                {
                    al_draw_textf(fonte,al_map_rgb(0,0,255),102,302,ALLEGRO_ALIGN_CENTER,"%i",pinta.cir.b);
                }

                else if(troca == 3) al_draw_textf(fonte,al_map_rgb(0,255,0),102,302,ALLEGRO_ALIGN_CENTER,"%i",pinta.eli.g);
            }
            else if(!pinta.qualconfig[3]) al_draw_bitmap(img.retback,85,302,0);

            if(pinta.ativatipo[4])
            {
                al_draw_bitmap(img.change,85,321,0);
                if(troca == 1)al_draw_textf(fonte,al_map_rgb(0,0,255),102,321,ALLEGRO_ALIGN_CENTER,"%i",pinta.ret.b);
                else if(troca == 3) al_draw_textf(fonte,al_map_rgb(0,0,255),102,321,ALLEGRO_ALIGN_CENTER,"%i",pinta.eli.b);
            }
            else if(!pinta.qualconfig[4] && (troca == 1 || troca == 3)) al_draw_bitmap(img.retback,85,321,0);

            //quadrado para informar Valor da função "Passos";
            al_draw_bitmap(img.retback,85,356,0);
            al_draw_textf(fonte,al_map_rgb(0,0,0),102,356,ALLEGRO_ALIGN_CENTER,"%i",pinta.quant);

            switch(troca)//atualiza a caixa de ferramentas para novos valores de parametros;
            {
                case(quadrado):
                {
                    al_draw_textf(fonte,al_map_rgb(0,0,0),102,245,ALLEGRO_ALIGN_CENTER,"%.0f",pinta.tamanhoq);
                    al_draw_textf(fonte,al_map_rgb(255,0,0),102,263,ALLEGRO_ALIGN_CENTER,"%i",pinta.qua.r);
                    al_draw_textf(fonte,al_map_rgb(0,255,0),102,283,ALLEGRO_ALIGN_CENTER,"%i",pinta.qua.g);
                    al_draw_textf(fonte,al_map_rgb(0,0,255),102,302,ALLEGRO_ALIGN_CENTER,"%i",pinta.qua.b);
                    break;
                }

                case(circulo):
                {
                    al_draw_textf(fonte,al_map_rgb(0,0,0),102,245,ALLEGRO_ALIGN_CENTER,"%.0f",pinta.tamanhoc);
                    al_draw_textf(fonte,al_map_rgb(255,0,0),102,263,ALLEGRO_ALIGN_CENTER,"%i",pinta.cir.r);
                    al_draw_textf(fonte,al_map_rgb(0,255,0),102,283,ALLEGRO_ALIGN_CENTER,"%i",pinta.cir.g);
                    al_draw_textf(fonte,al_map_rgb(0,0,255),102,302,ALLEGRO_ALIGN_CENTER,"%i",pinta.cir.b);
                    break;
                }

                case(retangulo):
                {
                    al_draw_textf(fonte,al_map_rgb(0,0,0),102,245,ALLEGRO_ALIGN_CENTER,"%.0f",pinta.tamanhorx);
                    al_draw_textf(fonte,al_map_rgb(0,0,0),102,263,ALLEGRO_ALIGN_CENTER,"%.0f",pinta.tamanhory);
                    al_draw_textf(fonte,al_map_rgb(255,0,0),102,283,ALLEGRO_ALIGN_CENTER,"%i",pinta.ret.r);
                    al_draw_textf(fonte,al_map_rgb(0,255,0),102,302,ALLEGRO_ALIGN_CENTER,"%i",pinta.ret.g);
                    al_draw_textf(fonte,al_map_rgb(0,0,255),102,321,ALLEGRO_ALIGN_CENTER,"%i",pinta.ret.b);
                    break;
                }

                case(elipse):
                {

                    al_draw_textf(fonte,al_map_rgb(0,0,0),102,245,ALLEGRO_ALIGN_CENTER,"%.0f",pinta.tamanhoex);
                    al_draw_textf(fonte,al_map_rgb(0,0,0),102,263,ALLEGRO_ALIGN_CENTER,"%.0f",pinta.tamanhoey);
                    al_draw_textf(fonte,al_map_rgb(255,0,0),102,283,ALLEGRO_ALIGN_CENTER,"%i",pinta.eli.r);
                    al_draw_textf(fonte,al_map_rgb(0,255,0),102,302,ALLEGRO_ALIGN_CENTER,"%i",pinta.eli.g);
                    al_draw_textf(fonte,al_map_rgb(0,0,255),102,321,ALLEGRO_ALIGN_CENTER,"%i",pinta.eli.b);
                    break;
                }
            }
        }//Fim timer;

        if(evento.type == ALLEGRO_EVENT_KEY_DOWN)                           //evento recebido quando alguma tecla é pressionada pela primeira vez;
        {
            if(evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                terminou = true;                                            //encerra o modo pintura;
            }

            else if(evento.keyboard.keycode == ALLEGRO_KEY_O)               //guarda a imagem na memória do computador;
            {
                img.outimage = al_create_bitmap(pinta.telax,pinta.telay);   //Cria bitmaps no tamanho da tela de pintura;
                img.outimage2 = al_create_bitmap(pinta.telax,pinta.telay);
                al_set_target_bitmap(img.outimage);                         //altera o alvo de impressão para um bitmap;
                img.outimage = al_get_backbuffer(pinta.pintura[0]);         //função que transporta todo o conteúdo do backbuffer para um bitmap;
                pinta.pintura[4] = al_create_display(pinta.telax,pinta.telay); //criação de um display para realizar um corte na imagem;
                al_set_target_backbuffer(pinta.pintura[4]);                 //altera o alvo de impressão para o novo display;
                /*
                    função de desenho de região, o corte da caixa de ferramentas da imagem é realizado;
                */
                al_draw_bitmap_region(img.outimage,152,0,pinta.telax,pinta.telay,0,0,0);
                img.outimage2 = al_get_backbuffer(pinta.pintura[4]);        //a nova versão cortada da imagem é recuperada;
                /*
                    por fim esta imagem é salva na memória do computado com o nome "imagem.png"
                    (existe a possibilidade de implantar um sistema para nomear o arquivo de saída);
                */
                al_save_bitmap(".\\Saida\\imagem.png", img.outimage2);
                //por fim o espaço de memória é liberado e a saída alvo retorna para pinta.pintura[0] - a pintura original;
                al_destroy_display(pinta.pintura[4]);
                al_destroy_bitmap(img.outimage);
                al_set_target_bitmap(al_get_backbuffer(pinta.pintura[0]));
            }

            else if(evento.keyboard.keycode == ALLEGRO_KEY_R)
            {
                /*
                    Ao pressionar o botão "R" a tela de pintura é completamente apagada, e em seu lugar somente a cor
                    de fundo preencherá a pintura;
                */
                al_draw_tinted_scaled_bitmap(img.corretivo,al_map_rgb(pinta.cortraz.r,pinta.cortraz.g,pinta.cortraz.b),1,1,200,200,152,0,pinta.telax + pinta.telax*0.05,pinta.telay + pinta.telay*0.05,0);
                img.repinta = true;
            }
        }//Fim key char;

        if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) // evento para registrar o primeiro toque do pressionar do mouse;
        {
            if(evento.mouse.button & 1) // se o botão do mouse pressionado for o Nº 1 (botão esquerdo);
            {
                img.repinta = true;
                pinta.apertou = true;
                al_get_mouse_state(&rato);
                pinta.x = rato.x;
                pinta.y = rato.y;
            }
        }//Fim button down;

        if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) // evento para registrar o soltar do botão do mouse;
        {
            if(evento.mouse.button & 1)
            {
                pinta.apertou = false;
            }
        }//Fim button up;

        if(al_is_event_queue_empty(fila2))              //evento que espera a fila estar limpa para ativar;
        {
            int conta = 0;
            for(int i = 0; i < 5;i++)
            {
                if(pinta.qualbot[i]) conta++;           //conta quantos botões estão ativos;
            }

            if(conta > 0) pinta.nobotao = true;         //caso este valor seja maior que 0, o flag pinta.nobotão notifica para outras funções que o usuário está sobre algum botão;
            else pinta.nobotao = false;

            if(img.repinta)
            {
                img.repinta = false;
                al_flip_display();
            }

            else if(terminou)                           //Encerrar a função de pintura, libera os espaços de memória utilizados;
            {
                al_show_mouse_cursor(pinta.pintura[0]);
                al_destroy_display(pinta.pintura[0]);
                al_destroy_event_queue(fila2);
                al_destroy_bitmap(img.toolbox);
                al_destroy_bitmap(img.opt[0]);
                al_destroy_bitmap(img.opt[1]);
                al_destroy_bitmap(img.opt[2]);
                al_destroy_bitmap(img.opt[3]);
                al_destroy_bitmap(img.corretivo);
                return;
            }

        }
    }
}

/*
    ativa flags para determinar qual ferramenta esta sendo utilizada,
    guarda os valores em flags que não muda com o movimento do mouse,mas sim com o passar do tempo;
*/
void toolbox(int tipo)
{
    if(pinta.qualconfig[0])
    {
        pinta.ativatipo[0] = true;
    }

    else pinta.ativatipo[0] = false;

    if(pinta.qualconfig[1])
    {
        pinta.ativatipo[1] = true;
    }

    else pinta.ativatipo[1] = false;

    if(pinta.qualconfig[2])
    {
        pinta.ativatipo[2] = true;
    }

    else pinta.ativatipo[2] = false;

    if(pinta.qualconfig[3])
    {
        pinta.ativatipo[3] = true;
    }

    else pinta.ativatipo[3] = false;

    if(pinta.qualconfig[4] && (tipo == 1 || tipo == 3))
    {
        pinta.ativatipo[4] = true;
    }

    else pinta.ativatipo[4] = false;

}

void alteraconf(int tipo, int qualconf, bool altera, short quant) // função designada para fazer controle dos parametros das ferramentas;
{
    switch (tipo)
    {
        case(quadrado):
        {
            switch(qualconf)
            {
                case(0):                                          //tamanho do lado do quadrado;
                {
                    if(altera)                                    //altera = true -> aumento / altera = false -> redução;
                    {
                        pinta.tamanhoq += quant;                  //aumenta/diminui o valor do parametro na proporção de quant("Passo");
                        if(pinta.tamanhoq > 1000) pinta.tamanhoq = 1000;//remover/alterar limite superior do tamanho da ferramenta;
                    }
                    else if(!altera)
                    {
                        pinta.tamanhoq -= quant;
                        if(pinta.tamanhoq < 1) pinta.tamanhoq = 1;//limite inferior da ferramenta;
                    }
                    break;
                }

                case(1):                                            //cor vermelha do quadrado;
                {
                    if(altera)
                    {
                        pinta.qua.r += quant;
                        if(pinta.qua.r > 255) pinta.qua.r = 255;    //limite superior da ferramenta;
                    }
                    else if(!altera)
                    {
                        pinta.qua.r -= quant;
                        if(pinta.qua.r < 0) pinta.qua.r = 0;        //limite inferior da ferramenta;
                    }
                    break;
                }

                case(2):                                            //cor verde do quadrado;

                {
                    if(altera)
                    {
                        pinta.qua.g += quant;
                        if(pinta.qua.g > 255) pinta.qua.g = 255;    //limite superior da ferramenta;
                    }
                    else if(!altera)
                    {
                        pinta.qua.g -= quant;
                        if(pinta.qua.g < 0) pinta.qua.g = 0;        //limite inferior da ferramenta;
                    }
                    break;
                }

                case(3):                                            //cor azul do quadrado;

                {
                    if(altera)
                    {
                        pinta.qua.b += quant;
                        if(pinta.qua.b > 255) pinta.qua.b = 255;    //limite superior da ferramenta;
                    }
                    else if(!altera)
                    {
                        pinta.qua.b -= quant;
                        if(pinta.qua.b < 0) pinta.qua.b = 0;        //limite inferior da ferramenta;
                    }
                    break;
                }

            }
            break;
        }

        case(retangulo):
        {
            switch(qualconf)
            {
                case(0):                                            //tamanho de X do retangulo;
                {
                    if(altera)
                    {
                        pinta.tamanhorx += quant;
                        if(pinta.tamanhorx > 1000) pinta.tamanhorx = 1000;//remover/alterar limite superior do tamanho da ferramenta;
                    }
                    else if(!altera)
                    {
                        pinta.tamanhorx -= quant;
                        if(pinta.tamanhorx < 1) pinta.tamanhorx = 1;//limite inferior da ferramenta;
                    }
                    break;
                }

                case(1):                                            //tamanho de Y do retangulo;
                {
                    if(altera)
                    {
                        pinta.tamanhory += quant;
                        if(pinta.tamanhory > 1000) pinta.tamanhory = 1000;//limite superior da ferramenta;
                    }
                    else if(!altera)
                    {
                        pinta.tamanhory -= quant;
                        if(pinta.tamanhory < 1) pinta.tamanhory = 1;//limite inferior da ferramenta;
                    }
                    break;
                }

                case(2):                                            //cor vermelha do retangulo;
                {
                    if(altera)
                    {
                        pinta.ret.r += quant;
                        if(pinta.ret.r > 255) pinta.ret.r = 255;    //limite superior da ferramenta;
                    }
                    else if(!altera)
                    {
                        pinta.ret.r -= quant;
                        if(pinta.ret.r < 0) pinta.ret.r = 0;        //limite inferior da ferramenta;
                    }
                    break;
                }

                case(3):                                            //cor verde do retangulo;
                {
                    if(altera)
                    {
                        pinta.ret.g += quant;
                        if(pinta.ret.g > 255) pinta.ret.g = 255;    //limite superior da ferramenta;
                    }
                    else if(!altera)
                    {
                        pinta.ret.g -= quant;
                        if(pinta.ret.g < 0) pinta.ret.g = 0;        //limite inferior da ferramenta;
                    }
                    break;
                }

                case(4):                                            //cor azul do retangulo;
                {
                    if(altera)
                    {
                        pinta.ret.b += quant;
                        if(pinta.ret.b > 255) pinta.ret.b = 255;    //limite superior da ferramenta;
                    }
                    else if(!altera)
                    {
                        pinta.ret.b -= quant;
                        if(pinta.ret.b < 0) pinta.ret.b = 0;        //limite inferior da ferramenta;
                    }
                    break;
                }

            }
            break;
        }

        case(circulo):
        {
            switch(qualconf)
            {
                case(0):                                                //tamanho do circulo;
                {
                    if(altera)
                    {
                        pinta.tamanhoc += quant;
                        if(pinta.tamanhoc> 1000) pinta.tamanhoc = 1000; //remover/alterar limite superior do tamanho da ferramenta;
                    }
                    else if(!altera)
                    {
                        pinta.tamanhoc -= quant;
                        if(pinta.tamanhoc < 1) pinta.tamanhoc = 1;      //limite inferior da ferramenta;
                    }
                    break;
                }

                case(1):                                                //cor vermelha do circulo;
                {
                    if(altera)
                    {
                        pinta.cir.r += quant;
                        if(pinta.cir.r > 255) pinta.cir.r = 255;        //limite superior da ferramenta;
                    }
                    else if(!altera)
                    {
                        pinta.cir.r -= quant;
                        if(pinta.cir.r < 0) pinta.cir.r = 0;            //limite inferior da ferramenta;
                    }
                    break;
                }

                case(2):                                                //cor verde do circulo;
                {
                    if(altera)
                    {
                        pinta.cir.g += quant;
                        if(pinta.cir.g > 255) pinta.cir.g = 255;        //limite superior da ferramenta;
                    }
                    else if(!altera)
                    {
                        pinta.cir.g -= quant;
                        if(pinta.cir.g < 0) pinta.cir.g = 0;            //limite inferior da ferramenta;
                    }
                    break;
                }

                case(3):                                                //cor azul do circulo;
                {
                    if(altera)
                    {
                        pinta.cir.b += quant;
                        if(pinta.cir.b > 255) pinta.cir.b = 255;        //limite superior da ferramenta;
                    }
                    else if(!altera)
                    {
                        pinta.cir.b -= quant;
                        if(pinta.cir.b < 0) pinta.cir.b = 0;            //limite inferior da ferramenta;
                    }
                    break;
                }

            }
            break;
        }

    case(elipse):
        {
            switch(qualconf)
            {
                case(0):                                                //tamanho X da elipse;
                {
                    if(altera)
                    {
                        pinta.tamanhoex += quant;
                        if(pinta.tamanhoex > 1000) pinta.tamanhoex = 1000;//remover/alterar limite superior do tamanho da ferramenta;
                    }
                    else if(!altera)
                    {
                        pinta.tamanhoex -= quant;
                        if(pinta.tamanhoex < 1) pinta.tamanhoex = 1;    //limite inferior da ferramenta;
                    }
                    break;
                }

                case(1):                                                //tamanho Y da elipse;
                {
                    if(altera)
                    {
                        pinta.tamanhoey += quant;
                        if(pinta.tamanhoey > 1000) pinta.tamanhoey = 1000;//limite superior da ferramenta;
                    }
                    else if(!altera)
                    {
                        pinta.tamanhoey -= quant;
                        if(pinta.tamanhoey < 1) pinta.tamanhoey = 1;    //limite inferior da ferramenta;
                    }
                    break;
                }

                case(2):                                                //cor vermelha da elipse;
                {
                    if(altera)
                    {
                        pinta.eli.r += quant;
                        if(pinta.eli.r > 255) pinta.eli.r = 255;        //limite superior da ferramenta;
                    }
                    else if(!altera)
                    {
                        pinta.eli.r -= quant;
                        if(pinta.eli.r < 0) pinta.eli.r = 0;            //limite inferior da ferramenta;
                    }
                    break;
                }

                case(3):                                                //cor verde da elipse;
                {
                    if(altera)
                    {
                        pinta.eli.g += quant;
                        if(pinta.eli.g > 255) pinta.eli.g = 255;        //limite superior da ferramenta;
                    }
                    else if(!altera)
                    {
                        pinta.eli.g -= quant;
                        if(pinta.eli.g < 0) pinta.eli.g = 0;            //limite inferior da ferramenta;
                    }
                    break;
                }

                case(4):                                                //cor azul da elipse;
                {
                    if(altera)
                    {
                        pinta.eli.b += quant;
                        if(pinta.eli.b > 255) pinta.eli.b = 255;        //limite superior da ferramenta;
                    }
                    else if(!altera)
                    {
                        pinta.eli.b -= quant;
                        if(pinta.eli.b < 0) pinta.eli.b = 0;            //limite inferior da ferramenta;
                    }
                    break;
                }

            }
        break;
        }
    }
    pinta.umavez = false;
}

float funcaocoli(int x,int a,int correcao)  //função para colizão não retangular;
{
    int y;
    x = x - correcao;
    y = a*x;
    return y;
}

void botaoconfi()                           //função que seleciona qual botão de configuração está em uso;
{
    int valor2 = 0;
    int valor1 = 0;
    //botoes de configuração;
    //confi 0;
    if(pinta.x > 120 && pinta.x < 125)
    {
        if(pinta.y > 246 + funcaocoli(pinta.x,1,120) && pinta.y < 255 + funcaocoli(pinta.x,-1,120))
        {
            pinta.alteravalor[1] = true;
        }
        else valor1++;
    }

    else valor1++;

    if(pinta.x > 78 && pinta.x < 83)
    {
        if(pinta.y > 250 + funcaocoli(pinta.x,-1,78) && pinta.y < 250 + funcaocoli(pinta.x,1,78))
        {
            pinta.alteravalor[0] = true;
        }
        else valor2++;
    }
    else valor2++;

    //confi 1;

    if(pinta.x > 120 && pinta.x < 125)
    {
        if(pinta.y > 264 + funcaocoli(pinta.x,1,120) && pinta.y < 272 + funcaocoli(pinta.x,-1,120))
        {
            pinta.alteravalor[1] = true;
        }
        else valor1++;
    }

    else valor1++;

    if(pinta.x > 78 && pinta.x < 83)
    {
        if(pinta.y > 268 + funcaocoli(pinta.x,-1,78) && pinta.y < 268 + funcaocoli(pinta.x,1,78))
        {
            pinta.alteravalor[0] = true;
        }
        else valor2++;
    }
    else valor2++;

    //confi 2;

    if(pinta.x > 120 && pinta.x < 125)
    {
        if(pinta.y > 284 + funcaocoli(pinta.x,1,120) && pinta.y < 292 + funcaocoli(pinta.x,-1,120))
        {
            pinta.alteravalor[1] = true;
        }
        else valor1++;
    }

    else valor1++;

    if(pinta.x > 78 && pinta.x < 83)
    {
        if(pinta.y > 288 + funcaocoli(pinta.x,-1,78) && pinta.y < 288 + funcaocoli(pinta.x,1,78))
        {
            pinta.alteravalor[0] = true;
        }
        else valor2++;
    }
    else valor2++;

    //confi 3;

    if(pinta.x > 120 && pinta.x < 125)
    {
        if(pinta.y > 303 + funcaocoli(pinta.x,1,120) && pinta.y < 311 + funcaocoli(pinta.x,-1,120))
        {
            pinta.alteravalor[1] = true;
        }
        else valor1++;
    }

    else valor1++;

    if(pinta.x > 78 && pinta.x < 83)
    {
        if(pinta.y > 307 + funcaocoli(pinta.x,-1,78) && pinta.y < 307 + funcaocoli(pinta.x,1,78))
        {
            pinta.alteravalor[0] = true;
        }
        else valor2++;
    }
    else valor2++;

    //confi 4;

    if(pinta.x > 120 && pinta.x < 125)
    {
        if(pinta.y > 323 + funcaocoli(pinta.x,1,120) && pinta.y < 331 + funcaocoli(pinta.x,-1,120))
        {
            pinta.alteravalor[1] = true;
        }
        else valor1++;
    }

    else valor1++;

    if(pinta.x > 78 && pinta.x < 83)
    {
        if(pinta.y > 327 + funcaocoli(pinta.x,-1,78) && pinta.y < 327 + funcaocoli(pinta.x,1,78))
        {
            pinta.alteravalor[0] = true;
        }
        else valor2++;
    }
    else valor2++;

    if(valor1 == 5) pinta.alteravalor[1] = false;
    if(valor2 == 5) pinta.alteravalor[0] = false;

}

int retornavalor(int atual)                         //função que altera o valor digitado pelo usuário para o parametro "Passo";
{
    int valor = atual;
    float multi;
    int patual = 0;
    int vetor[3] = {0,0,0};
    bool fora = false;

    //variaveis ALLEGRO;
    ALLEGRO_EVENT_QUEUE *fila5 = NULL;
    ALLEGRO_MOUSE_STATE rato;
    ALLEGRO_FONT *fonte;

    fonte = al_load_font(".\\Assets\\Fonte\\arial.ttf",10,0);
    //inicialização de fila / eventos;
    fila5 = al_create_event_queue();

    al_register_event_source(fila5,al_get_keyboard_event_source());
    al_register_event_source(fila5,al_get_mouse_event_source());
    al_register_event_source(fila5,al_get_display_event_source(pinta.pintura[0]));

    while(1)
    {
        ALLEGRO_EVENT evento;

        al_wait_for_event(fila5,&evento);

        if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            if(fora)
            {
                pinta.atualizar = false;        //caso o mouse for pressionado fora da caixa do Passo, sair da função;
                return atual;
            }
        }

        if(evento.type == ALLEGRO_EVENT_MOUSE_AXES)
        {
            al_get_mouse_state(&rato);
            pinta.x = rato.x;
            pinta.y = rato.y;


            //mantem controle se o mouse está na caixa de "Passo";
            if(pinta.x > 86 && pinta.x < 119)
            {
                if(pinta.y > 356 && pinta.y < 368)
                {
                    fora = false;
                }
                 else fora = true;
            }
            else fora = true;
        }

        if(evento.type == ALLEGRO_EVENT_KEY_DOWN)   //le quais valores estão atribuidos a nova quantia de "Passo"
        {
            if(patual <= 2)                         //limita o valor para 3 digitos;
            {

                if(evento.keyboard.keycode == ALLEGRO_KEY_1)
                {
                    vetor[patual] = 1;
                    patual++;
                }

                if(evento.keyboard.keycode == ALLEGRO_KEY_2)
                {
                    vetor[patual] = 2;
                    patual++;
                }

                if(evento.keyboard.keycode == ALLEGRO_KEY_3)
                {
                    vetor[patual] = 3;
                    patual++;
                }

                if(evento.keyboard.keycode == ALLEGRO_KEY_4)
                {
                    vetor[patual] = 4;
                    patual++;
                }

                if(evento.keyboard.keycode == ALLEGRO_KEY_5)
                {
                    vetor[patual] = 5;
                    patual++;
                }

                if(evento.keyboard.keycode == ALLEGRO_KEY_6)
                {
                    vetor[patual] = 6;
                    patual++;
                }

                if(evento.keyboard.keycode == ALLEGRO_KEY_7)
                {
                    vetor[patual] = 7;
                    patual++;
                }

                if(evento.keyboard.keycode == ALLEGRO_KEY_8)
                {
                    vetor[patual] = 8;
                    patual++;
                }

                if(evento.keyboard.keycode == ALLEGRO_KEY_9)
                {
                    vetor[patual] = 9;
                    patual++;
                }

                if(evento.keyboard.keycode == ALLEGRO_KEY_0)
                {
                    vetor[patual] = 0;
                    patual++;
                }
            }

            if(evento.keyboard.keycode == ALLEGRO_KEY_ENTER)
            {
                pinta.atualizar = false;
                return valor;
            }

            if(evento.keyboard.keycode == ALLEGRO_KEY_BACKSPACE)
            {
                //limpa todos os valores digitados;
                for(int i = 0; i < 3;i++)
                vetor[i] = 0;
                patual = 0;
            }
        }

        if(al_event_queue_is_empty(fila5))
        {
            al_draw_bitmap(img.change,85,356,0);
            /*
                função que calcula os pesos de cada posição em funçao de
                quantos digitos foram guardados; caso o peso total for 0, este então torna-se 1;
            */
            if((multi = (pow(10.0,(patual-1)))) <= 0) multi = 1;
            //converte os valores salvos individualmente no vetor, para um valor inteiro;
            valor = vetor[2]+vetor[1]*(multi/10.0)+vetor[0]*multi;
            if(valor > 999) valor = 999;    //limita superiormente o valor de "Passo";
            else if(valor < 1) valor = 1;   //limita inferiormente o valor de "Passo";
            al_draw_textf(fonte,al_map_rgb(0,0,0),102,356,ALLEGRO_ALIGN_CENTER,"%i",valor);
            al_flip_display();
        }
    }
pinta.atualizar = false;
return valor;
}

//fim;
