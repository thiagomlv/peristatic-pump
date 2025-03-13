/* 
    AUTOR: Thiago Oliveira

    OBSERVACOES: VERSAO DE TESTE
    ==> No arduino Mega do projeto, a interface serial usada eh a Serial1, que tem o RX no pino 19
*/

// --- Bibliotecas Auxiliares ---k
//#include <Accelbomba_A.h>

// --- Definicao de Macros ---
#define     TAMANHO_MENSAGEM_POR_BOMBA     11          // A0000-0-000: A mensagem de cada bomba tem 11 caracteres
#define     PASSOS_POR_REVOLUCAO           200         // quantidade de passos necessarios para completar uma volta

// --- Mapeamento de Hardware ---
#define     A_STEP_PIN       60
#define     A_DIR_PIN        61
#define     A_ENABLE_PIN     56                        // nao usado no codigo: por padrao em pull down

#define     B_STEP_PIN       46
#define     B_DIR_PIN        48
#define     B_ENABLE_PIN     62                        // nao usado no codigo: por padrao em pull down


// --- Mapeamento de Logica ---
String mensagem,                                      // armazena a mensagem recebida pela interface serial
       mensagem_A,                                    // armazena a parte da mensagem para a bomba A
       mensagem_B;                                    // armazena a parte da mensagem para a bomba B
                                           
String fluxo,                                         // armazena o fluxo que o sistema acabou de receber
       unidade,                                       // armazena a unidade que o sistema acabou de receber
       seringa;                                       // armazena o tamanho da seringa que o sistema acabou de receber 

int indice_separador;                                 // armazena o indice do separador "|" na mensagem recebida 

int fluxo_A = 0,                                      // armazena o fluxo atual da bomba A
    fluxo_B = 0,                                      // armazena o fluxo atual da bomba B
    fluxo_int = 0;                                    // armazena o fluxo no formato inteiro temporariamente

float fluxo_A_pps,                                    // armazena o fluxo A orecebido nas unidades 1 ou 3 convertido para rpm temporariamente
      fluxo_B_pps;                                    // armazena o fluxo B orecebido nas unidades 1 ou 3 convertido para rpm temporariamente

int unidade_A = 1,                                    // armazena a unidade da bomba selecionada
    unidade_B = 1;                                    // armazena a unidade das bombas
    unidade_int = 0;                                  // armazena o fluxo no formato inteiro temporariamente

int seringa_A = 0,                                    // armazena o tamanho da seringa A
    seringa_B = 0;                                    // armazena o tamanho da seringa B
    seringa_int = 0;                                  // armazena o fluxo no formato inteiro temporariamente

//***************************** TESTE *****************************
#define          A_REINICIAR_BOMBA_LED_AMARELO             5
#define          A_PARAR_FLUXO_ATUAL_LED_VERMELHO          6
#define          A_FLUXO_VALIDO_LED_VERDE                  7

#define          B_REINICIAR_BOMBA_LED_AMARELO             8
#define          B_PARAR_FLUXO_ATUAL_LED_VERMELHO          9
#define          B_FLUXO_VALIDO_LED_VERDE                  10
//***************************** TESTE *****************************


// --- Prototipo das funcoes auxiliares ---
void processarParametros(String mensagem, char bomba);
void acionar_bomba(char bomba);
void parar_bomba(char bomba);
void voltar_posicao_inicial(char bomba);
float converterRPMParaPassos(float rpm);
float converterMLParaPassos(float rpm);

// --- Inicializacao da biblioteca auxiliar ---
// Accelbomba_A bomba_A(::DRIVER, A_STEP_PIN, A_DIR_PIN);
// Accelbomba_A bomba_B(::DRIVER, B_STEP_PIN, B_DIR_PIN);


/*
##################################################################################################
Configuracoes iniciais
##################################################################################################
*/

void setup() 
{
    pinMode(DIR_PIN_A,  OUTPUT);                // define o pino de direcao do motor A como OUTUP 
    pinMode(STEP_PIN_A, OUTPUT);                // define o pino de passos do motor A como OUTUP 
    pinMode(DIR_PIN_B,  OUTPUT);                // define o pino de direcao do motor B como OUTUP  
    pinMode(STEP_PIN_B, OUTPUT);                // define o pino de passos do motor A como OUTUP 
    
    Serial.begin(9600);                         // inicializa a interface de comunicacao serial 1

    bomba_A.setMaxSpeed(1000);                  // Velocidade máxima (passos/segundo)
    bomba_A.setAcceleration(500);               // Aceleração (passos/segundo²)

    bomba_B.setMaxSpeed(1000);                  // Velocidade máxima (passos/segundo)
    bomba_B.setAcceleration(500);               // Aceleração (passos/segundo²)

    //***************************** TESTE *****************************
    pinMode(A_REINICIAR_BOMBA_LED_AMARELO, OUTPUP);
    pinMode(A_PARAR_FLUXO_ATUAL_LED_VERMELHO, OUTPUP);
    pinMode(A_FLUXO_VALIDO_LED_VERDE, OUTPUP);

    pinMode(B_REINICIAR_BOMBA_LED_AMARELO, OUTPUP);
    pinMode(B_PARAR_FLUXO_ATUAL_LED_VERMELHO, OUTPUP);
    pinMode(B_FLUXO_VALIDO_LED_VERDE, OUTPUP);
    //***************************** TESTE *****************************
}

/*
##################################################################################################
Cotrole do sistema
##################################################################################################
*/

void loop()
{
    if (Serial.available() > 0) 
    {
        mensagem = Serial.readStringUntil('\n');       // le a mensagem ate o caractere de nova linha
        mensagem.trim();                               // remove espaços em branco e caracteres de nova linha

        // verifica se a mensagem contem os delimitadores esperados
        if (mensagem.indexOf('|') != -1) 
        {
            // divide a mensagem em duas partes: A e B
            indice_separador = mensagem.indexOf('|');
            mensagem_A = mensagem.substring(0, indice_separador);
            mensagem_B = mensagem.substring(indice_separador + 1);

            // processa a parte A
            if (mensagem_A.length() == TAMANHO_MENSAGEM_POR_BOMBA) processarParametros(mensagem_A, "A");

            // processa a parte B
            if (mensagem_B.length() == TAMANHO_MENSAGEM_POR_BOMBA) processarParametros(mensagem_B, "B");;
        }
    }
}

/*
##################################################################################################
Extrair parametros: fluxo, unidade e seringa
##################################################################################################
*/

/*
* @brief extrai o fluxo, a unidade e o tamanho da seringa da mensagem apenas da bomba selecionada
* @param menssagem: mensagem apenas da bomba selecionada (ex.: A0000-0-000)
* @param bomba: bomba selecionada
*/
void processarParametros(String mensagem, char bomba)
{   
    if (bomba == 'A')
    {
        fluxo = mensagem.substring(1, 5);                      // obtem o fluxo
        if (fluxo != "XXXX")                                   // o fluxo eh o codigo de reiniciar "XXXX"?
        {                                                      // nao...
            fluxo_int = fluxo.toInt();                         // converte o fluxo para inteiro
            if (fluxo_A != fluxo_int)                          // verifica se o fluxo recebido eh igual ao que ja estava
            {                                                  // nao...
                fluxo_A = fluxo_int                            // atualiza o fluxo da bomba com o novo fluxo (localmente)
            }

            unidade_int = mensagem.substring(6, 7).toInt();    // obtem o codigo da unidade ja em inteiro
            if (unidade_A != unidade_int)                      // verifica se o fluxo recebido eh igual ao que ja estava
            {                                                  // nao...
                unidade_A = unidade_int                        // atualiza o fluxo da bomba com o novo fluxo (localmente)
            }

            seringa_int = mensagem.substring(8).toInt();       // obtem o tamanho da seringa ja em inteiro
            if (seringa_A != seringa_int)                      // verifica se o fluxo recebido eh igual ao que ja estava
            {                                                  // nao...
                seringa_A = seringa_int                        // atualiza o fluxo da bomba com o novo fluxo (localmente)
            }

            if (fluxo_A == 0) parar_bomba('A')                 // para a bomba
            else acionar_bomba('A')                            // aciona a bomba com os parametros devidos
        }
        else
        {
            fluxo_A = 0;                                       // define o fluxo atual como zero
            voltar_posicao_inicial('A');                     // volta a bomba para a posicao inicial
        }
    }

    if (bomba == 'B')
    {
        fluxo = mensagem.substring(1, 5);                      // obtem o fluxo
        if (fluxo != "XXXX")                                   // o fluxo eh o codigo de reiniciar "XXXX"?
        {                                                      // nao...
            fluxo_int = fluxo.toInt();                         // converte o fluxo para inteiro
            if (fluxo_B != fluxo_int)                          // verifica se o fluxo recebido eh igual ao que ja estava
            {                                                  // nao...
                fluxo_B = fluxo_int                            // atualiza o fluxo da bomba com o novo fluxo (localmente)
            }

            unidade_int = mensagem.substring(6, 7).toInt();    // obtem o codigo da unidade ja em inteiro
            if (unidade_B != unidade_int)                      // verifica se o fluxo recebido eh igual ao que ja estava
            {                                                  // nao...
                unidade_B = unidade_int                        // atualiza o fluxo da bomba com o novo fluxo (localmente)
            }

            seringa_int = mensagem.substring(8).toInt();       // obtem o tamanho da seringa ja em inteiro
            if (seringa_B != seringa_int)                      // verifica se o fluxo recebido eh igual ao que ja estava
            {                                                  // nao...
                seringa_B = seringa_int                        // atualiza o fluxo da bomba com o novo fluxo (localmente)
            }

            if (fluxo_B == 0) parar_bomba('B')                 // para a bomba
            else acionar_bomba('B')                            // aciona a bomba com os parametros devidos
        }
        else
        {
            fluxo_B = 0;                                       // define o fluxo atual como zero
            voltar_posicao_inicial('B');                     // volta a bomba para a posicao inicial
        }
    }
}

/*
##################################################################################################
Acionar a bomba com os devidos parametros definidos pelo usuario
##################################################################################################
*/

void acionar_bomba(char bomba)
{
    if (bomba == 'A')
    {
        if (unidade_A == 1)   // unidade: mL/min
        {
            // publicar na bomba usando accel bomba_A
        } else
        if (unidade_A == 2)   // unidade: passos/s
        {
            bomba_A.setSpeed(fluxo_A);                       // define a velocidade deseja em passos/segundo
            bomba_A.runSpeed();                              // aciona essa velocidade, sem considerar aceleracao suave
        } else
        if (unidade_A == 3)   // unidade: rpm
        {
            fluxo_A_pps = converterRpmParaPassosPorSegundo(fluxo_A); 
            bomba_A.setSpeed(fluxo_A_pps);                   // define a velocidade deseja em passos/segundo
            bomba_A.runSpeed();                              // aciona essa velocidade, sem considerar aceleracao suave
        }
    } else
    if (bomba == 'B')
    {
        if (unidade_B == 1)
        {
            // publicar na bomba usando accel bomba_A
        } else
        if (unidade_B == 2)
        {
            bomba_B.setSpeed(fluxo_B);                       // define a velocidade deseja em passos/segundo
            bomba_B.runSpeed();                              // aciona essa velocidade, sem considerar aceleracao suave
        } else
        if (unidade_B == 3)
        {
            fluxo_B_pps = converterRpmParaPassosPorSegundo(fluxo_B); 
            bomba_B.setSpeed(fluxo_B_pps);                   // define a velocidade deseja em passos/segundo
            bomba_B.runSpeed();                              // aciona essa velocidade, sem considerar aceleracao suave
        }
    }
}

/*
##################################################################################################
Para o movimento da bomba
##################################################################################################
*/

void parar_bomba(char bomba)
{
    if (bomba == 'A')
    {
        bomba_A.stop();                           // para o movimento da bomba A
    } else
    if (bomba == 'B')
    {
        bomba_B.stop();                           // para o movimento da bomba B
    }
}

/*
##################################################################################################
Volta a bomba a posicao inicial
##################################################################################################
*/

void voltar_posicao_inicial(char bomba)
{
    if (bomba == 'A')
    {
        bomba_A.stop();        // para o motor
        bomba_A.moveTo(0);     // move para a posicao 0
        bomba_A.run();         // atualiza o motor para alcançar a posicao 0

        // aguarda ate que o motor alcance a posicao 0
        while (bomba_A.distanceToGo() != 0) 
        {
            bomba_A.run();
        }
    } else
    if (bomba == 'B')
    {
        bomba_B.stop();        // para o motor
        bomba_B.moveTo(0);     // move para a posicao 0
        bomba_B.run();         // atualiza o motor para alcançar a posicao 0

        // aguarda ate que o motor alcance a posicao 0
        while (bomba_B.distanceToGo() != 0) 
        {
            bomba_B.run();
        }
    }
}

/*
##################################################################################################
Converter rpm para passos/segundo
##################################################################################################
*/

float converterRPMParaPassos(float rpm) 
{
    return (rpm * PASSOS_POR_REVOLUCAO) / 60;
}

/*
##################################################################################################
Converter mL/min para passos/segundo
##################################################################################################
*/

float converterMLParaPassos(float ml_por_min) 
{
    return //formula;
}