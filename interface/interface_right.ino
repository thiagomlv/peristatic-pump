/* 
    AUTOR: Thiago Oliveira
    REFERENCIA 1: BrincandoComIdeias (https://www.youtube.com/brincandocomideias)
    REFERENCIA 2: Canal WR Kits (https://www.youtube.com/canalwrkits)
*/

// --- Bibliotecas Auxiliares ---k
#include <LiquidCrystal.h>

// --- Definicao de macros ---
#define     PASSOS_POR_REVOLUCAO           200         // quantidade de passos necessarios para completar uma volta

// --- Mapeamento de Hardware ---
#define pinBottons A0

#define pinRs 8
#define pinEn 9
#define pinD4 4
#define pinD5 5
#define pinD6 6
#define pinD7 7
#define pinBackLight 10


// --- Mapeamento de Logica ---
int adc_value  = 0x00;                          // armazena o valor digital do conversor AD
unsigned long ultimoTempoLeitura = 0;
const int debounceDelay = 50;                   // Tempo de debounce em milissegundos

boolean right  = 0x00,   right_flag  = 0x00,
        up     = 0x00,   up_flag     = 0x00,
        down   = 0x00,   down_flag   = 0x00,
        left   = 0x00,   left_flag   = 0x00,
        select = 0x00,   select_flag = 0x00;
    
boolean calibracao_flag = 0x00;                 // flag para a funcao menuSelecionarFluxo, se 0x01 a unidade e sempre passos/segundo
String minutos_calibracao = "";                 // flag para a funcao menuSelecionarFluxo, se 0x01 a unidade e sempre passos/segundo
int minutos_calibracao_int = 0;                 // flag para a funcao menuSelecionarFluxo, se 0x01 a unidade e sempre passos/segundo

String calibracao_selecionada;                  // armazena a calibracao selecionada pelo usuario
float fator_calibracao_seringa_A = 0.0;         // armazena o resultado da calibracao da bomba A
float fator_calibracao_seringa_B = 0.0;         // armazena o resultado da calibracao da bomba A

int pos_seta_selecao = 1;                       // armazena em qual posicao do menu o cursor esta
int pos_seta_selecao_anterior = 1;              // armazena em qual posicao do menu o cursor esta
int pos_seta_selecao_ante_anterior = 1;         // armazena em qual posicao do menu o cursor esta
int menu = 1;                                   // armazena qual menu deve ser exibido  

char bomba_selecionada;                         // armazena qual bomba sera acionada                     
int algarismo = 0;                              // algarismo exibido no menu de selecao de fluxo                 
int posicao_algarismo = 0;                      // indica a posicao do algarismo no numero                 

String fluxo_selecionado;                       // armazena o fluxo selecionado no menu                
String fluxo_A = "0000";                        // armazena o fluxo atual da bomba A
String fluxo_B = "0000";                        // armazena o fluxo atual da bomba B

int volume_esvaziado_calibracao_A = 0;          // armazena o fluxo de calibracao da bomba A
int volume_esvaziado_calibracao_B = 0;          // armazena o fluxo de calibracao da bomba B
int velocidade_calibracao_A = 0;
int velocidade_calibracao_B = 0;

String seringa_selecionada;                     // armazena o tamanho da seringa selecionado no menu 
String seringa_atual;                           // armazena o tamanho da seringa que esta atualmente na bomba selecionada   
String seringa_A = "000";                       // armazena o tamanho da seringa A
String seringa_B = "000";                       // armazena o tamanho da seringa B

int unidade;                                    // armazena a unidade da bomba selecionada
int unidade_A = 1;                              // armazena a unidade das bombas
int unidade_B = 1;                              // unidade = 1: mL/min | unidade = 2: passos/s | unidade = 3: rpm                  


// --- Prototipo das funcoes auxiliares ---
void lerTeclado(boolean usa_right, boolean usa_up, boolean usa_down, boolean usa_left, boolean usa_select);
void menuSelecionarBomba();
void menuSelecionarOpcoesIniciais();
void menuSelecionarFluxo();
void menuSelecionarAjuste();
void menuSelecionarUnidade();
void calibrarBomba();
void menuSelecionarSeringa();
void menuSelecionarMinutosCalibracao();
void menuSelecionarCalibracao();
void atualizarAlgarismoUp(int max);
void atualizarAlgarismoDown(int max);
void atualizarPosicaoSetaUp(int max);
void atualizarPosicaoSetaDown(int max);
float converterRPMParaPassos(int fluxo);
float converterMLParaPassos(char bomba, int fluxo);
float obterFatorCalibracao(char bomba);
void enviarFluxo();

// --- Inicializacao da biblioteca auxiliar ---
LiquidCrystal lcd(pinRs, pinEn, pinD4, pinD5, pinD6, pinD7);

/*
##################################################################################################
Configuracoes iniciais
##################################################################################################
*/

void setup() 
{
    pinMode(pinBackLight, OUTPUT);              // Define o pino da luz de fundo como OUTUP   
    digitalWrite(pinBackLight, HIGH);           // Acende a luz de fundo
    lcd.begin(16, 2);                           // Inicializa o display de 16 caracteres e 2 linhas
    Serial.begin(9600);                         // Inicializa a comunicacao serial
}


/*
##################################################################################################
Cotrole do sistema
##################################################################################################
*/

void loop()
{
    if      (menu == 1) menuSelecionarBomba();
    else if (menu == 2) menuSelecionarOpcoesIniciais();
    else if (menu == 3) menuSelecionarFluxo();
    else if (menu == 4) menuSelecionarAjuste();
    else if (menu == 5) menuSelecionarUnidade();
    else if (menu == 6) menuSelecionarSeringa();
    else if (menu == 7) menuSelecionarMinutosCalibracao();
    else if (menu == 8) calibrarBomba();
    else if (menu == 9) menuSelecionarCalibracao();
}

/*
##################################################################################################
Exibicao do menu inicial (Selecao da bomba A ou B)
##################################################################################################
*/

void menuSelecionarBomba() // menu 1
{
    lerTeclado(0x01, 0x01, 0x01, 0x00, 0x00);           // Le qual botao foi apertado

    // --- Avalia em qual posicao a seta de selecao deve ser exibida ---
    if (pos_seta_selecao == 1)
    {
        lcd.setCursor(0,0); lcd.print(" > Bomba A      ");
        lcd.setCursor(0,1); lcd.print("   Bomba B      ");
    } else 
    if (pos_seta_selecao == 2)
    {
        lcd.setCursor(0,0); lcd.print("   Bomba A      ");
        lcd.setCursor(0,1); lcd.print(" > Bomba B      ");
    }

    // --- Acoes para cada botao ---
    if(up == 0x01)                                         // tecla up pressionada?
    {                                                      // sim...
        up = 0x00;                                         // limpa flag da tecla
        atualizarPosicaoSetaUp(2);                         // a posicao da seta de selecaoo
    } //end if up
    
    if(down == 0x01)                                       // tecla down pressionada?
    {                                                      // sim...
        down = 0x00;                                       // limpa flag da tecla
        atualizarPosicaoSetaDown(2);                       // a posicao da seta de selecao 
    } //end if down
    
    if(right == 0x01)                                     // tecla select pressionada?
    {                                                      // sim...
        right = 0x00;                                     // limpa flag da tecla

        // Define a bomba que foi selecionada pelo usuario
        if      (pos_seta_selecao == 1) bomba_selecionada = 'A';
        else if (pos_seta_selecao == 2) bomba_selecionada = 'B';

        pos_seta_selecao = 1;                              // reinicia a seta de selecao na primeira posicao
        pos_seta_selecao_anterior = 1;                     // reinicia a posicao anterior para a posicao inicial
        menu = 2;                                          // atualiza a variavel que muda o menu no proximo loop
    } //end if left
}

/*
##################################################################################################
Exibicao do menu inicial da bomba selecionada
##################################################################################################
*/

void menuSelecionarOpcoesIniciais()  // menu 2
{
    lerTeclado(0x01, 0x01, 0x01, 0x00, 0x00);           // Le qual botao foi apertado

    // --- Avalia em qual posicao a seta de selecao deve ser exibida ---
    if (pos_seta_selecao == 1 && (pos_seta_selecao_anterior == 1 || pos_seta_selecao_anterior == 2))
    {
        lcd.setCursor(0,0); lcd.print(" > Iniciar    " + String(bomba_selecionada) + " ");
        lcd.setCursor(0,1); lcd.print("   Parar        ");
    } else 
    if (pos_seta_selecao == 2 && pos_seta_selecao_anterior == 1)
    {
        lcd.setCursor(0,0); lcd.print("   Iniciar    " + String(bomba_selecionada) + " ");
        lcd.setCursor(0,1); lcd.print(" > Parar        ");
    } else 
    if (pos_seta_selecao == 2 && pos_seta_selecao_anterior == 3)
    {
        lcd.setCursor(0,0); lcd.print(" > Parar      " + String(bomba_selecionada) + " ");
        lcd.setCursor(0,1); lcd.print("   Reiniciar    ");
    } else 
    if (pos_seta_selecao == 3 && pos_seta_selecao_anterior == 2)
    {
        lcd.setCursor(0,0); lcd.print("   Parar      "  + String(bomba_selecionada) + " ");
        lcd.setCursor(0,1); lcd.print(" > Reiniciar    ");
    } else 
    if (pos_seta_selecao == 3 && pos_seta_selecao_anterior == 4)
    {
        lcd.setCursor(0,0); lcd.print(" > Reiniciar  "  + String(bomba_selecionada) + " ");
        lcd.setCursor(0,1); lcd.print("   Ajustes      ");
    } else 
    if (pos_seta_selecao == 4 && pos_seta_selecao_anterior == 3)
    {
        lcd.setCursor(0,0); lcd.print("   Reiniciar  "  + String(bomba_selecionada) + " ");
        lcd.setCursor(0,1); lcd.print(" > Ajustes      ");
    } else
    if (pos_seta_selecao == 4 && pos_seta_selecao_anterior == 5)
    {
        lcd.setCursor(0,0); lcd.print(" > Ajustes    "  + String(bomba_selecionada) + " ");
        lcd.setCursor(0,1); lcd.print("   Voltar       ");
    }else
    if (pos_seta_selecao == 5 && pos_seta_selecao_anterior == 4)
    {
        lcd.setCursor(0,0); lcd.print("   Ajustes    "  + String(bomba_selecionada) + " ");
        lcd.setCursor(0,1); lcd.print(" > Voltar       ");
    } else
    if (pos_seta_selecao == 1 && pos_seta_selecao_anterior == 5)
    {
        lcd.setCursor(0,0); lcd.print("   Voltar     "  + String(bomba_selecionada) + " ");
        lcd.setCursor(0,1); lcd.print(" > Iniciar      ");
    } else
    if (pos_seta_selecao == 5 && pos_seta_selecao_anterior == 1)
    {
        lcd.setCursor(0,0); lcd.print(" > Voltar     "  + String(bomba_selecionada) + " ");
        lcd.setCursor(0,1); lcd.print("   Iniciar      ");
    }

    // --- Acoes para cada botao --- 
    if(up == 0x01)                                         // tecla up pressionada?
    {                                                      // sim...
        up = 0x00;                                         // limpa flag da tecla
        atualizarPosicaoSetaUp(5);                         // a posicao da seta de selecao     
    } //end if up
    
    if(down == 0x01)                                       // tecla down pressionada?
    {                                                      // sim...
        down = 0x00;                                       // limpa flag da tecla
        atualizarPosicaoSetaDown(5);                       // a posicao da seta de selecao
    
    } //end if down
    
    if(right == 0x01)                                       // tecla select pressionada?
    {                                                      // sim...
        right = 0x00;                                       // limpa flag da tecla

        // atualiza a variavel que muda o menu no proximo loop
        if      (pos_seta_selecao == 1) menu = 3;         // iniciar
        else if (pos_seta_selecao == 2)                   // parar
        {       
            // Atualiza o fluxo da bomba selecionada
            if      (bomba_selecionada == 'A') fluxo_A = "0000";
            else if (bomba_selecionada == 'B') fluxo_B = "0000"; 

            enviarFluxo();                                  // envia o fluxo para o arduino que controla as bombas

            fluxo_selecionado = "";                         // reinicia o fluxo selecionado para 0
            menu = 2;                                       // atualiza a variavel que muda o menu no proximo loop
        }
        else if (pos_seta_selecao == 3)           // reiniciar
        {       
            // Atualiza o fluxo da bomba selecionada com o codigo de reset
            if      (bomba_selecionada == 'A') fluxo_A = "XXXX";
            else if (bomba_selecionada == 'B') fluxo_B = "XXXX"; 

            enviarFluxo();                                  // envia o fluxo para o arduino que controla as bombas

            fluxo_selecionado = "";                         // reinicia o fluxo selecionado para 0
            menu = 2;                                       // atualiza a variavel que muda o menu no proximo loop
        }
        else if (pos_seta_selecao == 4) menu = 4; // ajustes
        else if (pos_seta_selecao == 5) menu = 1; // voltar
        
        if (!(pos_seta_selecao == 2 || pos_seta_selecao == 3)) // se a opcao selecionada for parar ou reiniciar, mantem a seta de selecao na posicao
        {
            pos_seta_selecao = 1;                              // reinicia a seta de selecao na primeira posicao
            pos_seta_selecao_anterior = 1;                     // reinicia a posicao anterior para a posicao inicial
        }
    } //end if left
}

/*
##################################################################################################
Selecionar fluxo conforme a unidade
##################################################################################################
*/

void menuSelecionarFluxo() // menu 3
{
    lerTeclado(0x01, 0x01, 0x01, 0x00, 0x00);           // Le qual botao foi apertado

    // define o valor da unidade da bomba selecionada
    if      (bomba_selecionada == 'A') unidade = unidade_A;
    else if (bomba_selecionada == 'B') unidade = unidade_B;

    // --- Avalia em qual algarismo esta sendo selecionado no fluxo ---
    if (posicao_algarismo == 0) 
    {
        if      (unidade == 1) {lcd.setCursor(0,0); lcd.print(" " + String(algarismo) + "       mL/min ");}
        else if (unidade == 2) {lcd.setCursor(0,0); lcd.print(" " + String(algarismo) + "     passos/s ");}
        else if (unidade == 3) {lcd.setCursor(0,0); lcd.print(" " + String(algarismo) + "          rpm ");}

        lcd.setCursor(0,1); lcd.print(" -              ");
    } else 
    if (posicao_algarismo == 1) 
    {
        if      (unidade == 1) {lcd.setCursor(0,0); lcd.print(" " + fluxo_selecionado + String(algarismo) + "      mL/min ");}
        else if (unidade == 2) {lcd.setCursor(0,0); lcd.print(" " + fluxo_selecionado + String(algarismo) + "    passos/s ");}
        else if (unidade == 3) {lcd.setCursor(0,0); lcd.print(" " + fluxo_selecionado + String(algarismo) + "         rpm ");}

        lcd.setCursor(0,1); lcd.print("  -             ");
    } else 
    if (posicao_algarismo == 2) 
    {
        if      (unidade == 1) {lcd.setCursor(0,0); lcd.print(" " + fluxo_selecionado + String(algarismo) + "     mL/min ");}
        else if (unidade == 2) {lcd.setCursor(0,0); lcd.print(" " + fluxo_selecionado + String(algarismo) + "   passos/s ");}
        else if (unidade == 3) {lcd.setCursor(0,0); lcd.print(" " + fluxo_selecionado + String(algarismo) + "        rpm ");}

        lcd.setCursor(0,1); lcd.print("   -            ");
    } else 
    if (posicao_algarismo == 3) 
    {
        if      (unidade == 1) {lcd.setCursor(0,0); lcd.print(" " + fluxo_selecionado + String(algarismo) + "    mL/min ");}
        else if (unidade == 2) {lcd.setCursor(0,0); lcd.print(" " + fluxo_selecionado + String(algarismo) + "  passos/s ");}
        else if (unidade == 3) {lcd.setCursor(0,0); lcd.print(" " + fluxo_selecionado + String(algarismo) + "       rpm ");}

        lcd.setCursor(0,1); lcd.print("    -           ");
    } else 
    if (posicao_algarismo == 4) 
    {
        if      (unidade == 1) {lcd.setCursor(0,0); lcd.print(" " + fluxo_selecionado + "    mL/min ");}
        else if (unidade == 2) {lcd.setCursor(0,0); lcd.print(" " + fluxo_selecionado + "  passos/s ");}
        else if (unidade == 3) {lcd.setCursor(0,0); lcd.print(" " + fluxo_selecionado + "       rpm ");}

        lcd.setCursor(0,1); lcd.print(" ----           ");
    }
  
    // --- Acoes para cada botao --- 
    if(up == 0x01)                                         // tecla up pressionada?
    {                                                      // sim...
        up = 0x00;                                         // limpa flag da tecla
        atualizarAlgarismoUp(9);                           // atualiza o algarismo
    } //end if up
    
    if(down == 0x01)                                       // tecla down pressionada?
    {                                                      // sim...
        down = 0x00;                                       // limpa flag da tecla
        atualizarAlgarismoDown(9);                         // atualiza o algarismo
    } //end if down
    
    if(right == 0x01)                                      // tecla select pressionada?
    {                                                       // sim...
        right = 0x00;                                      // limpa flag da tecla

        if (posicao_algarismo == 0) {
            fluxo_selecionado += String(algarismo);
            algarismo = 0;
            posicao_algarismo = 1;
        } else 
        if (posicao_algarismo == 1) {
            fluxo_selecionado += String(algarismo);
            algarismo = 0;
            posicao_algarismo = 2;
        } else
        if (posicao_algarismo == 2) {
            fluxo_selecionado += String(algarismo);
            algarismo = 0;
            posicao_algarismo = 3;
        } else 
        if (posicao_algarismo == 3) {
            fluxo_selecionado += String(algarismo);
            algarismo = 0;
            posicao_algarismo = 4;
        } else 
        if (posicao_algarismo == 4) {
            // Atualiza o fluxo da bomba selecionada
            if      (bomba_selecionada == 'A') fluxo_A = fluxo_selecionado;
            else if (bomba_selecionada == 'B') fluxo_B = fluxo_selecionado; 

            if (!calibracao_flag) enviarFluxo();            // envia o fluxo se nao for para calibracao

            fluxo_selecionado = "";                         // reinicia o fluxo selecionado para 0
            algarismo = 0;                                  // reinicia o algarismo para 0
            posicao_algarismo = 0;                          // reinicia a posicao do algarismo para 0
            pos_seta_selecao = 1;                           // reinicia a seta de selecao na primeira posicao
            pos_seta_selecao_anterior = 1;                  // reinicia a posicao anterior para a posicao inicial
            
            if (calibracao_flag) menu = 7;                  // proximo menu: menuSelecionarMinutosCalibracao()
            else menu = 2;                                  // proximo menu: menuSelecionarOpcoesIniciais()

            calibracao_flag = 0x00;                         // limpa flag de calibracao
        }      
    } //end if left
}

/*
##################################################################################################
Selecionar os parametros ajustaveis
##################################################################################################
*/

void menuSelecionarAjuste()
{
    lerTeclado(0x01, 0x01, 0x01, 0x00, 0x00);           // Le qual botao foi apertado

    // obtem o tamanho da seringa atual
    if      (bomba_selecionada == 'A') seringa_atual = seringa_A;
    else if (bomba_selecionada == 'B') seringa_atual = seringa_B;

    // --- Avalia em qual posicao a seta de selecao deve ser exibida ---
    if (pos_seta_selecao == 1)
    {
        lcd.setCursor(0,0); lcd.print(" > Unidade      ");
        lcd.setCursor(0,1); lcd.print("   Calibrar " + seringa_atual + " ");
    } else 
    if (pos_seta_selecao == 2 && pos_seta_selecao_anterior == 1)
    {
        lcd.setCursor(0,0); lcd.print("   Unidade      ");
        lcd.setCursor(0,1); lcd.print(" > Calibrar " + seringa_atual + " ");
    }

    // --- Acoes para cada botao ---
    if(up == 0x01)                                         // tecla up pressionada?
    {                                                      // sim...
        up = 0x00;                                         // limpa flag da tecla
        atualizarPosicaoSetaUp(2);                         // Atualiza a posicao da seta de selecao
    } //end if up
    
    if(down == 0x01)                                       // tecla down pressionada?
    {                                                      // sim...
        down = 0x00;                                       // limpa flag da tecla
        atualizarPosicaoSetaDown(2);                       // Atualiza a posicao da seta de selecao 
    } //end if down
    
    if(right == 0x01)                                     // tecla select pressionada?
    {                                                      // sim...
        right = 0x00;                                     // limpa flag da tecla

        // atualiza a variavel que muda o menu no proximo loop
        menu = pos_seta_selecao + 4;

        pos_seta_selecao = 1;                              // reinicia a seta de selecao na primeira posicao
        pos_seta_selecao_anterior = 1;                     // reinicia a posicao anterior para a posicao inicial
    } //end if left
}

/*
##################################################################################################
Selecionar unidade
##################################################################################################
*/

void menuSelecionarUnidade() // menu 5
{
    lerTeclado(0x01, 0x01, 0x01, 0x00, 0x00);           // Le qual botao foi apertado

    // --- Avalia em qual posicao a seta de selecao deve ser exibida ---
    if (pos_seta_selecao == 1 && (pos_seta_selecao_anterior == 1 || pos_seta_selecao_anterior == 2))
    {
        lcd.setCursor(0,0); lcd.print(" > mL/min       ");
        lcd.setCursor(0,1); lcd.print("   passos/s     ");
    } else 
    if (pos_seta_selecao == 2 && pos_seta_selecao_anterior == 1)
    {
        lcd.setCursor(0,0); lcd.print("   mL/min       ");
        lcd.setCursor(0,1); lcd.print(" > passos/s     ");
    } else 
    if (pos_seta_selecao == 2 && pos_seta_selecao_anterior == 3) ///editar
    {
        lcd.setCursor(0,0); lcd.print(" > passos/s     ");
        lcd.setCursor(0,1); lcd.print("   rpm          ");
    } else 
    if (pos_seta_selecao == 3 && pos_seta_selecao_anterior == 2)
    {
        lcd.setCursor(0,0); lcd.print("   passos/s     ");
        lcd.setCursor(0,1); lcd.print(" > rpm          ");
    } else 
    if (pos_seta_selecao == 3 && pos_seta_selecao_anterior == 1)  // editar
    {
        lcd.setCursor(0,0); lcd.print(" > rpm          ");
        lcd.setCursor(0,1); lcd.print("   mL/min       ");
    } else 
    if (pos_seta_selecao == 1 && pos_seta_selecao_anterior == 3)
    {
        lcd.setCursor(0,0); lcd.print("   rpm          ");
        lcd.setCursor(0,1); lcd.print(" > mL/min       ");
    }

    // --- Acoes para cada botao ---
    if(up == 0x01)                                         // tecla up pressionada?
    {                                                      // sim...
        up = 0x00;                                         // limpa flag da tecla
        atualizarPosicaoSetaUp(3);                         // Atualiza a posicao da seta de selecao
    } //end if up
    
    if(down == 0x01)                                       // tecla down pressionada?
    {                                                      // sim...
        down = 0x00;                                       // limpa flag da tecla
        atualizarPosicaoSetaDown(3);                       // Atualiza a posicao da seta de selecao 
    } //end if down
    
    if(right == 0x01)                                     // tecla select pressionada?
    {                                                      // sim...
        right = 0x00;                                     // limpa flag da tecla

        // atualiza a unidade
        if      (bomba_selecionada == 'A') unidade_A = pos_seta_selecao;
        else if (bomba_selecionada == 'B') unidade_B = pos_seta_selecao;

        pos_seta_selecao = 1;                              // reinicia a seta de selecao na primeira posicao
        pos_seta_selecao_anterior = 1;                     // reinicia a posicao anterior para a posicao inicial
        menu = 2;                                          // atualiza a variavel que muda o menu no proximo loop
    } //end if left
}

/*
##################################################################################################
Selecionar tamanho da seringa
##################################################################################################
*/

void menuSelecionarSeringa()  // menu 6
{
    if      (bomba_selecionada == 'A') unidade_A = 2;           // define a unidade da bomba selecionada para passos/segundo
    else if (bomba_selecionada == 'B') unidade_B = 2;           // define a unidade da bomba selecionada para passos/segundo
    calibracao_flag = 0x01;                                     // sinaliza que o menu de selecao de fluxo deve ter unidade 2
    
    lerTeclado(0x01, 0x01, 0x01, 0x00, 0x00);                   // Le qual botao foi apertado

    // --- avalia em qual algarismo esta sendo selecionado no fluxo ---
    if (posicao_algarismo == 0) 
    {
        lcd.setCursor(0,0); lcd.print(" " + String(algarismo) + "           mL ");
        lcd.setCursor(0,1); lcd.print(" -              ");
    } else 
    if (posicao_algarismo == 1) 
    {
        lcd.setCursor(0,0); lcd.print(" " + seringa_selecionada + String(algarismo) + "          mL ");
        lcd.setCursor(0,1); lcd.print("  -             ");
    } else 
    if (posicao_algarismo == 2) 
    {
        lcd.setCursor(0,0); lcd.print(" " + seringa_selecionada + String(algarismo) + "         mL ");
        lcd.setCursor(0,1); lcd.print("   -            ");
    } else 
    if (posicao_algarismo == 3) 
    {
        lcd.setCursor(0,0); lcd.print(" " + seringa_selecionada + "         mL ");
        lcd.setCursor(0,1); lcd.print(" ---            ");
    }
  
    // --- Acoes para cada botao --- 
    if(up == 0x01)                                         // tecla up pressionada?
    {                                                      // sim...
        up = 0x00;                                         // limpa flag da tecla
        atualizarAlgarismoUp(9);                           // atualiza o algarismo
    } //end if up
    
    if(down == 0x01)                                       // tecla down pressionada?
    {                                                      // sim...
        down = 0x00;                                       // limpa flag da tecla
        atualizarAlgarismoDown(9);                         // Atualiza o algarismo
    } //end if down
    
    if(right == 0x01)                                      // tecla select pressionada?
    {                                                       // sim...
        right = 0x00;                                      // limpa flag da tecla

        if (posicao_algarismo == 0) {
            seringa_selecionada += String(algarismo);
            algarismo = 0;
            posicao_algarismo = 1;
        } else 
        if (posicao_algarismo == 1) {
            seringa_selecionada += String(algarismo);
            algarismo = 0;
            posicao_algarismo = 2;
        } else
        if (posicao_algarismo == 2) {
            seringa_selecionada += String(algarismo);
            algarismo = 0;
            posicao_algarismo = 3;
        } else 
        if (posicao_algarismo == 3) {

            // Atualiza o tamanho das seringas das bombas selecionada
            if      (bomba_selecionada == 'A') seringa_A = seringa_selecionada;
            else if (bomba_selecionada == 'B') seringa_B = seringa_selecionada; 

            seringa_selecionada = "";                       // reinicia a seringa selecioanada para 0
            algarismo = 0;                                  // reinicia o algarismo para 0
            posicao_algarismo = 0;                          // reinicia a posicao do algarismo para 0
            pos_seta_selecao = 1;                           // reinicia a seta de selecao na primeira posicao
            pos_seta_selecao_anterior = 1;                  // reinicia a posicao anterior para a posicao inicial
            menu = 3;                                       // proximo menu: selecionar o fluxo
        }      
    } //end if left
}

/*
##################################################################################################
Selecionar por quantos minutos a calibracao vai ocorrer
##################################################################################################
*/

void menuSelecionarMinutosCalibracao() // menu 7
{
    lerTeclado(0x01, 0x01, 0x01, 0x00, 0x00);           // Le qual botao foi apertado

    // --- avalia em qual algarismo esta sendo selecionado no fluxo ---
    if (posicao_algarismo == 0) 
    {
        lcd.setCursor(0,0); lcd.print(" " + String(algarismo) + "      minutos ");
        lcd.setCursor(0,1); lcd.print(" -              ");
    } else 
    if (posicao_algarismo == 1) 
    {
        lcd.setCursor(0,0); lcd.print(" " + minutos_calibracao + String(algarismo) + "     minutos ");
        lcd.setCursor(0,1); lcd.print("  -             ");
    } else 
    if (posicao_algarismo == 2) 
    {
        lcd.setCursor(0,0); lcd.print(" " + minutos_calibracao + "     minutos ");
        lcd.setCursor(0,1); lcd.print(" --             ");
    }
  
    // --- Acoes para cada botao --- 
    if(up == 0x01)                                         // tecla up pressionada?
    {                                                      // sim...
        up = 0x00;                                         // limpa flag da tecla
        atualizarAlgarismoUp(9);                           // atualiza o algarismo
    } //end if up
    
    if(down == 0x01)                                       // tecla down pressionada?
    {                                                      // sim...
        down = 0x00;                                       // limpa flag da tecla
        atualizarAlgarismoDown(9);                         // Atualiza o algarismo
    } //end if down
    
    if(right == 0x01)                                      // tecla select pressionada?
    {                                                       // sim...
        right = 0x00;                                      // limpa flag da tecla

        if (posicao_algarismo == 0) {
            minutos_calibracao += String(algarismo);
            algarismo = 0;
            posicao_algarismo = 1;
        } else 
        if (posicao_algarismo == 1) {
            minutos_calibracao += String(algarismo);
            algarismo = 0;
            posicao_algarismo = 2;
        } else 
        if (posicao_algarismo == 2) {

            minutos_calibracao_int = minutos_calibracao.toInt();
            minutos_calibracao = "";
            algarismo = 0;                                  // reinicia o algarismo para 0
            posicao_algarismo = 0;                          // reinicia a posicao do algarismo para 0
            pos_seta_selecao = 1;                           // reinicia a seta de selecao na primeira posicao
            pos_seta_selecao_anterior = 1;                  // reinicia a posicao anterior para a posicao inicial
            menu = 8;                                       // proximo menu: calibrarBomba
        }      
    } //end if left
}

/*
##################################################################################################
Calibrar bomba
##################################################################################################
*/

void calibrarBomba() // menu 8
{
    enviarFluxo();
    
    if      (bomba_selecionada == 'A') velocidade_calibracao_A = fluxo_A.toInt();
    else if (bomba_selecionada == 'B') velocidade_calibracao_B = fluxo_B.toInt();

    fluxo_selecionado = "";                         // reinicia o fluxo selecionado para 0

    lcd.setCursor(0,0); lcd.print(" Calibrando... ");
    lcd.setCursor(0,1); lcd.print(" Bomba " + String(bomba_selecionada) + "        ");

    delay((unsigned long) minutos_calibracao_int*60*1000);

    if      (bomba_selecionada == 'A') fluxo_A = "0000";
    else if (bomba_selecionada == 'B') fluxo_B = "0000";

    enviarFluxo();
    fluxo_selecionado = "";                         // reinicia o fluxo selecionado para 0

    menu = 9;
    algarismo = 0;
    pos_seta_selecao = 1;                              // reinicia a seta de selecao na primeira posicao
    pos_seta_selecao_anterior = 1;                     // reinicia a posicao anterior para a posicao inicial
}

/*
##################################################################################################
Selecionar calibracao observada
##################################################################################################
*/

void menuSelecionarCalibracao()  // menu 9
{
    lerTeclado(0x01, 0x01, 0x01, 0x00, 0x00);           // Le qual botao foi apertado

    // --- avalia em qual algarismo esta sendo selecionado no fluxo ---
    if (posicao_algarismo == 0) 
    {
        lcd.setCursor(0,0); lcd.print(" " + String(algarismo) + "           mL ");
        lcd.setCursor(0,1); lcd.print(" -              ");
    } else 
    if (posicao_algarismo == 1) 
    {
        lcd.setCursor(0,0); lcd.print(" " + calibracao_selecionada + String(algarismo) + "          mL ");
        lcd.setCursor(0,1); lcd.print("  -             ");
    } else 
    if (posicao_algarismo == 2) 
    {
        lcd.setCursor(0,0); lcd.print(" " + calibracao_selecionada + String(algarismo) + "         mL ");
        lcd.setCursor(0,1); lcd.print("   -            ");
    } else 
    if (posicao_algarismo == 3) 
    {
        lcd.setCursor(0,0); lcd.print(" " + calibracao_selecionada + "         mL ");
        lcd.setCursor(0,1); lcd.print(" ---            ");
    }
  
    // --- Acoes para cada botao --- 
    if(up == 0x01)                                         // tecla up pressionada?
    {                                                      // sim...
        up = 0x00;                                         // limpa flag da tecla
        atualizarAlgarismoUp(9);                           // atualiza o algarismo
    } //end if up
    
    if(down == 0x01)                                       // tecla down pressionada?
    {                                                      // sim...
        down = 0x00;                                       // limpa flag da tecla
        atualizarAlgarismoDown(9);                         // atualiza o algarismo
    } //end if down
    
    if(right == 0x01)                                      // tecla select pressionada?
    {                                                       // sim...
        right = 0x00;                                      // limpa flag da tecla
        calibracao_flag = 0x00;                             // limpa flag de calibracao

        if (posicao_algarismo == 0) {
            calibracao_selecionada += String(algarismo);
            algarismo = 0;
            posicao_algarismo = 1;
        } else 
        if (posicao_algarismo == 1) {
            calibracao_selecionada += String(algarismo);
            algarismo = 0;
            posicao_algarismo = 2;
        } else
        if (posicao_algarismo == 2) {
            calibracao_selecionada += String(algarismo);
            algarismo = 0;
            posicao_algarismo = 3;
        } else  
        if (posicao_algarismo == 3) {

            // Atualiza o fluxo da bomba selecionada
            if      (bomba_selecionada == 'A') 
            {
                volume_esvaziado_calibracao_A = calibracao_selecionada.toInt();
                if (fator_calibracao_seringa_A != obterFatorCalibracao('A')) 
                    fator_calibracao_seringa_A = obterFatorCalibracao('A');
            }
            else if (bomba_selecionada == 'B')
            {
                volume_esvaziado_calibracao_B = calibracao_selecionada.toInt();
                if (fator_calibracao_seringa_B != obterFatorCalibracao('B')) 
                    fator_calibracao_seringa_B = obterFatorCalibracao('B');
            }  

            enviarFluxo();                                  // envia o fluxo para o arduino que controla as bombas

            calibracao_selecionada = "";                    // reinicia o fluxo selecionado para 0
            algarismo = 0;                                  // reinicia o algarismo para 0
            posicao_algarismo = 0;                          // reinicia a posicao do algarismo para 0
            pos_seta_selecao = 1;                           // reinicia a seta de selecao na primeira posicao
            pos_seta_selecao_anterior = 1;                  // reinicia a posicao anterior para a posicao inicial
            
            menu = 2;                                       // proximo menu: menuSelecionarOpcoesIniciais()
        }      
    } //end if left
}


/*
##################################################################################################
Le o botao que foi apertado e seta sua respectiva flag de apertado
##################################################################################################
*/

void lerTeclado(boolean usa_right, boolean usa_up, boolean usa_down, boolean usa_left, boolean usa_select)
{   
    /* 
    As variavies "usa" que estiverem com valor de 0x01 estão sendo usadas.adc_value
    As que estao como 0x00 nao exercem funcao no menu que foram chamadas, logo sua flag deve ser limpa
    */

    // Variavel para leitura recebe valor AD de A0
    adc_value = analogRead(A0);

    // --- Testa se os botoes foram pressionados ---
    // Se foi pressionado, seta a respectiva flag
    if      (adc_value < 60  && usa_right)                      right_flag  =  0x01;
    else if (adc_value > 60  && adc_value < 200 && usa_up)      up_flag     =  0x01;
    else if (adc_value > 200 && adc_value < 400 && usa_down)    down_flag   =  0x01;
    else if (adc_value > 400 && adc_value < 600 && usa_left)    left_flag   =  0x01;
    else if (adc_value > 600 && adc_value < 800 && usa_select)  select_flag =  0x01;
     
    // --- Testa se os botoes foram liberados ---
    //
    if (adc_value > 60 && right_flag)             // botao right solto e flag right_flag setada?
    {                                             // sim...
        right_flag = 0x00;                        // limpa flag right_flag
        right      = 0x01;                        // seta flag right 
    } // end right

    if (adc_value > 200 && up_flag)               // botao up solto e flag up_flag setada?
    {                                             // sim...
        up_flag = 0x00;                           // limpa flag up_flag
        up      = 0x01;                           // seta flag up 
    } // end up

    if (adc_value > 400 && down_flag)             // botao down solto e flag down_flag setada?
    {                                             // sim...
        down_flag = 0x00;                         // limpa flag down_flag
        down      = 0x01;                         // seta flag down  
    } // end down

    if (adc_value > 600 && left_flag)             // botao left solto e flag left_flag setada?
    {                                             // sim...
        left_flag = 0x00;                         // limpa flag left_flag
        left      = 0x01;                         // seta flag left 
    } // end left

    if (adc_value > 800 && select_flag)           //Botão left solto e flag left_flag setada?
    {                                             //Sim...
        select_flag = 0x00;                       //Limpa flag left_flag
        select   = 0x01;                          //Seta flag left   
    } // end select 
}

/*
##################################################################################################
Atualiza o algarismo
##################################################################################################
*/

void atualizarAlgarismoUp(int max) 
{
    if (algarismo == max) algarismo = 0;
    else algarismo++; 
}

void atualizarAlgarismoDown(int max) 
{
    if (algarismo == 0) algarismo = max;
    else algarismo--; 
}

/*
##################################################################################################
Atualiza a posicao da seta de selecao
##################################################################################################
*/

void atualizarPosicaoSetaUp(int max) 
{
    pos_seta_selecao_anterior = pos_seta_selecao;               // Atualiza a posicao anterior
    if (pos_seta_selecao == 1) pos_seta_selecao = max;
    else pos_seta_selecao--; 
}

void atualizarPosicaoSetaDown(int max) 
{
    pos_seta_selecao_anterior = pos_seta_selecao;              // Atualiza a posicao anterior
    if (pos_seta_selecao == max) pos_seta_selecao = 1;
    else pos_seta_selecao++; 
}

/*
##################################################################################################
Converter rpm para passos/segundo
##################################################################################################
*/

float converterRPMParaPassos(int fluxo) 
{
    return round( (((fluxo*PASSOS_POR_REVOLUCAO)/60)*100.0)/100.0 ) * 1.00;
}

/*
##################################################################################################
Converter mL/min para passos/segundo
##################################################################################################
*/

float converterMLParaPassos(char bomba, int fluxo) 
{ 
    if (bomba == 'A')
    {
        return round( (fator_calibracao_seringa_A*fluxo) / 60.00);
    }
    else if (bomba == 'B')
    { 
       return round( (fator_calibracao_seringa_B*fluxo) / 60.00);
    }
}

float obterFatorCalibracao(char bomba)
{
    float total_de_passos;
    if (bomba_selecionada == 'A')
    {
        total_de_passos = minutos_calibracao_int * 60.00 * velocidade_calibracao_A;
        return total_de_passos / volume_esvaziado_calibracao_A;
    } else
    if (bomba_selecionada == 'B')
    {
        total_de_passos = minutos_calibracao_int * 60.00 * velocidade_calibracao_B;
        return total_de_passos / volume_esvaziado_calibracao_B;
    }
}


/*
##################################################################################################
Enviar o fluxo para o arduino que controla os motores
##################################################################################################
*/

// --- Deve ser chamada toda vez que um dos fluxos for atualizado ---
void enviarFluxo()
{
    // formata o fluxo A no formado que deve ser enviado
    float pps_enviado_A;
    if      (unidade_A == 1) pps_enviado_A = converterMLParaPassos('A', fluxo_A.toInt());
    else if (unidade_A == 2) pps_enviado_A = fluxo_A.toInt();
    else if (unidade_A == 3) pps_enviado_A = converterRPMParaPassos(fluxo_A.toInt());

    float pps_enviado_B;
    if      (unidade_B == 1) pps_enviado_B = converterMLParaPassos('B', fluxo_B.toInt());
    else if (unidade_B == 2) pps_enviado_B = fluxo_B.toFloat();
    else if (unidade_B == 3) pps_enviado_B = converterRPMParaPassos(fluxo_B.toInt());          
    
    // junta em uma unica mensagem
    String fluxo_enviado = String(pps_enviado_A * 1.00) + "|" + String(pps_enviado_B * 1.00) + "\n";

    // envia a mensagem
    Serial.print(fluxo_enviado);
}