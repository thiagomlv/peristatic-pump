/* 
    AUTOR: Thiago Oliveira
    REFERENCIA 1: BrincandoComIdeias (https://www.youtube.com/brincandocomideias)
    REFERENCIA 2: Canal WR Kits (https://www.youtube.com/canalwrkits)
*/

// --- Bibliotecas Auxiliares ---
#include <LiquidCrystal.h>


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

boolean right  = 0x00,   right_flag  = 0x00,
        up     = 0x00,   up_flag     = 0x00,
        down   = 0x00,   down_flag   = 0x00,
        left   = 0x00,   left_flag   = 0x00,
        select = 0x00,   select_flag = 0x00;

int menu_arrow = 1;                             // armazena em qual posicao do menu o cursor esta
int menu = 1;                                   // armazena qual menu deve ser exibido  

char bomba;                                     // armazena qual bomba sera acionada                     
String fluxo;                                   // armazena o fluxo                    
int unidade = 1;                                // unidade = 1: mL/min | unidade = 2: passos/s                    
int algarismo = 0;                              // algarismo exibido no menu de selecao de fluxo                 
int posicao_algarismo = 0;                      // indica a posicao do algarismo no numero                 


// --- Prototipo das funcoes auxiliares ---
void lerTeclado();
void menuSelecionarBomba();
void menuBombaSelecionada();
void atualizarAlgarismoUp();
void atualizarAlgarismoDown();


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
    else if (menu == 2) menuBombaSelecionada();
    else if (menu == 3) menuSelecionarFluxo();
}

/*
##################################################################################################
Exibicao do menu inicial (Selecao da bomba A ou B)
##################################################################################################
*/

void menuSelecionarBomba()
{
    lerTeclado();                                  // Le qual botao foi apertado

    // --- Avalia em qual posicao a seta de selecao deve ser exibida ---
    if (menu_arrow == 1)
    {
        lcd.setCursor(0,0); lcd.print(" > Bomba A      ");
        lcd.setCursor(0,1); lcd.print("   Bomba B      ");
    } else 
    if (menu_arrow == 2)
    {
        lcd.setCursor(0,0); lcd.print("   Bomba A      ");
        lcd.setCursor(0,1); lcd.print(" > Bomba B      ");
    }
  
    // --- Acoes para cada botao --- 
    if(right == 0x01)                                   // tecla right pressionada?
    {                                                   // sim...
        right = 0x00;                                   // limpa flag da tecla
        // Botao sem funcao nesse menu
    } //end if right

    if(up == 0x01)                                      // tecla up pressionada?
    {                                                   // sim...
        up = 0x00;                                      // limpa flag da tecla
        
        if      (menu_arrow == 1) menu_arrow = 2;       // se o cursor estiver na op 1, seta o cursor para a op 2
        else if (menu_arrow == 2) menu_arrow = 1;       // se o cursor estiver na op 2, seta o cursor para a op 1
    } //end if up
    
    if(down == 0x01)                                    // tecla down pressionada?
    {                                                   // sim...
        down = 0x00;                                    // limpa flag da tecla

        if (menu_arrow == 1) menu_arrow = 2;            // se o cursor estiver na op 1, seta o cursor para a op 2
        else if (menu_arrow == 2) menu_arrow = 1;       // se o cursor estiver na op 2, seta o cursor para a op 1
    
    } //end if down
    
    if(left == 0x01)                                    // tecla left pressionada?
    {                                                   // sim...
        left = 0x00;                                    // limpa flag da tecla
        // Botao sem funcao nesse menu
    } //end if left
    
    if(select == 0x01)                                  // tecla select pressionada?
    {                                                   // sim...
        select = 0x00;                                  // limpa flag da tecla

        // Define a bomba que foi selecionada pelo usuario
        if      (menu_arrow == 1) bomba = 'A';
        else if (menu_arrow == 2) bomba = 'B';

        // Chama o proximo menu, resetando a posicao da seta de selecao
        menu_arrow = 1;
        menu = 2;
    } //end if left
}

/*
##################################################################################################
Exibicao do menu inicial da bomba selecionada
##################################################################################################
*/

void menuBombaSelecionada()
{
    lerTeclado();                                       // Le qual botao foi apertado

    // --- Avalia em qual posicao a seta de selecao deve ser exibida ---
    if (menu_arrow == 1)
    {
        lcd.setCursor(0,0); lcd.print(" > Iniciar    " + String(bomba) + " ");
        lcd.setCursor(0,1); lcd.print("   Ajustes      ");
    } else 
    if (menu_arrow == 2)
    {
        lcd.setCursor(0,0); lcd.print("   Iniciar    " + String(bomba) + " ");
        lcd.setCursor(0,1); lcd.print(" > Ajustes      ");
    }
  
    // --- Acoes para cada botao --- 
    if(right == 0x01)                                   // tecla right pressionada?
    {                                                   // sim...
        right = 0x00;                                   // limpa flag da tecla
        // Botao sem funcao nesse menu
    } //end if right

    if(up == 0x01)                                      // tecla up pressionada?
    {                                                   // sim...
        up = 0x00;                                      // limpa flag da tecla
        
        if      (menu_arrow == 1) menu_arrow = 2;       // se o cursor estiver na op 1, seta o cursor para a op 2
        else if (menu_arrow == 2) menu_arrow = 1;       // se o cursor estiver na op 2, seta o cursor para a op 1
    } //end if up
    
    if(down == 0x01)                                    // tecla down pressionada?
    {                                                   // sim...
        down = 0x00;                                    // limpa flag da tecla

        if (menu_arrow == 1) menu_arrow = 2;            // se o cursor estiver na op 1, seta o cursor para a op 2
        else if (menu_arrow == 2) menu_arrow = 1;       // se o cursor estiver na op 2, seta o cursor para a op 1
    
    } //end if down
    
    if(left == 0x01)                                    // tecla left pressionada?
    {                                                   // sim...
        left = 0x00;                                    // limpa flag da tecla
        // Botao sem funcao nesse menu
    } //end if left
    
    if(select == 0x01)                                  // tecla select pressionada?
    {                                                   // sim...
        select = 0x00;                                  // limpa flag da tecla
        menu = 3;                                       // chama o menu de selecao de fluxo atualizando a variavel menu
    } //end if left
}

/*
##################################################################################################
Selecionar fluxo conforme a unidade
##################################################################################################
*/

void menuSelecionarFluxo()
{
    lerTeclado();                                       // Le qual botao foi apertado

    // --- Avalia em qual algarismo esta sendo selecionado no fluxo ---
    if (posicao_algarismo == 0) 
    {
        if      (unidade == 1) {lcd.setCursor(0,0); lcd.print(" " + String(algarismo) + "    mL/min    ");}
        else if (unidade == 2) {lcd.setCursor(0,0); lcd.print(" " + String(algarismo) + "    passos/s  ");}

        lcd.setCursor(0,1); lcd.print(" -              ");
    } else 
    if (posicao_algarismo == 1) 
    {
        if      (unidade == 1) {lcd.setCursor(0,0); lcd.print("  " + String(algarismo) + "   mL/min    ");}
        else if (unidade == 2) {lcd.setCursor(0,0); lcd.print("  " + String(algarismo) + "   passos/s  ");}

        lcd.setCursor(0,1); lcd.print("  -             ");
    } else 
    if (posicao_algarismo == 2) 
    {
        if      (unidade == 1) {lcd.setCursor(0,0); lcd.print("   " + String(algarismo) + "  mL/min    ");}
        else if (unidade == 2) {lcd.setCursor(0,0); lcd.print("   " + String(algarismo) + "  passos/s  ");}

        lcd.setCursor(0,1); lcd.print("   -            ");
    } else 
    if (posicao_algarismo == 3) 
    {
        if      (unidade == 1) {lcd.setCursor(0,0); lcd.print("    " + String(algarismo) + " mL/min    ");}
        else if (unidade == 2) {lcd.setCursor(0,0); lcd.print("    " + String(algarismo) + " passos/s  ");}

        lcd.setCursor(0,1); lcd.print("    -           ");
    } else 
    if (posicao_algarismo == 4) 
    {
        if      (unidade == 1) {lcd.setCursor(0,0); lcd.print(" " + fluxo + " mL/min    ");}
        else if (unidade == 2) {lcd.setCursor(0,0); lcd.print(" " + fluxo + " passos/s  ");}

        lcd.setCursor(0,1); lcd.print(" ----           ");
    }
  
    // --- Acoes para cada botao --- 
    if(right == 0x01)                                   //tecla right pressionada?
    {                                                   //sim...
        right = 0x00;                                   //limpa flag da tecla
        // Botao sem funcao nesse menu
    } //end if right

    if(up == 0x01)                                      //tecla up pressionada?
    {                                                   //sim...
        up = 0x00;                                      //limpa flag da tecla
        atualizarAlgarismoUp();                          // Atualiza o algarismo
    } //end if up
    
    if(down == 0x01)                                    //tecla down pressionada?
    {                                                   //sim...
        down = 0x00;                                    //limpa flag da tecla
        atualizarAlgarismoDown();                          // Atualiza o algarismo
    } //end if down
    
    if(left == 0x01)                                    //tecla left pressionada?
    {                                                   //sim...
        left = 0x00;                                    //limpa flag da tecla
        // Botao sem funcao nesse menu
    } //end if left
    
    if(select == 0x01)                                  //tecla select pressionada?
    {                                                   //sim...
        select = 0x00;                                  //limpa flag da tecla

        if (posicao_algarismo == 0) {
            fluxo += String(algarismo);
            algarismo = 0;
            posicao_algarismo = 1;
        } else 
        if (posicao_algarismo == 1) {
            fluxo += String(algarismo);
            algarismo = 0;
            posicao_algarismo = 2;
        } else
        if (posicao_algarismo == 2) {
            fluxo += String(algarismo);
            algarismo = 0;
            posicao_algarismo = 3;
        } else 
        if (posicao_algarismo == 3) {
            fluxo += String(algarismo);
            algarismo = 0;
            posicao_algarismo = 4;
        } else 
        if (posicao_algarismo == 4) {
            menu = 2;                     // quando o ultimo algarismo for selecionado, volta pro menu bomba selecionada
        }      
    } //end if left
}

/*
##################################################################################################
Le o botao que foi apertado e seta sua respectiva flag de apertado
##################################################################################################
*/

void lerTeclado()
{   
    //Variável para leitura recebe valor AD de A0
    adc_value = analogRead(A0);

    // --- Testa se os botões foram pressionados ---
    // Se foi pressionado, seta a respectiva flag
    if      (adc_value < 60)                      right_flag  =  0x01;
    else if (adc_value > 60  && adc_value < 200)  up_flag     =  0x01;
    else if (adc_value > 200 && adc_value < 400)  down_flag   =  0x01;
    else if (adc_value > 400 && adc_value < 600)  left_flag   =  0x01;
    else if (adc_value > 600 && adc_value < 800)  select_flag =  0x01;
     
    // --- Testa se os botões foram liberados ---
    //
    if (adc_value > 60 && right_flag)             //Botão right solto e flag right_flag setada?
    {                                             //Sim...
        right_flag = 0x00;                        //Limpa flag right_flag
        right      = 0x01;                        //Seta flag right  
    }

    if (adc_value > 200 && up_flag)               //Botão up solto e flag up_flag setada?
    {                                             //Sim...
        up_flag = 0x00;                           //Limpa flag up_flag
        up      = 0x01;                           //Seta flag up 
    }

    if (adc_value > 400 && down_flag)             //Botão down solto e flag down_flag setada?
    {                                             //Sim...
        down_flag = 0x00;                         //Limpa flag down_flag
        down      = 0x01;                         //Seta flag down  
    }

    if (adc_value > 600 && left_flag)             //Botão left solto e flag left_flag setada?
    {                                             //Sim...
        left_flag = 0x00;                         //Limpa flag left_flag
        left      = 0x01;                         //Seta flag left  
    }

    if (adc_value > 800 && select_flag)           //Botão left solto e flag left_flag setada?
    {                                             //Sim...
        select_flag = 0x00;                       //Limpa flag left_flag
        select   = 0x01;                          //Seta flag left    
    }  
}

/*
##################################################################################################
Atualiza o algarismo
##################################################################################################
*/

void atualizarAlgarismoUp() 
{
    if (algarismo == 9) algarismo = 0;
    else algarismo++; 
}

void atualizarAlgarismoDown() 
{
    if (algarismo == 0) algarismo = 9;
    else algarismo--; 
}