/*************************************************
   PROJETO DE CARRO AUTÔNOMO - OFICINAS 4.0 -
   CARRO QUE SE MOVE E TOMA DECISÕES SOZINHO.
   POR: SAULO.
 **************************************************/
/*============================================================================*/
//Defines do projeto.
//Defines do sensor ultrassônico
#define ECHO 13
#define TRIG 12

//define dos leds de sinalização
#define LED_E 9 //Led esquerda
#define LED_D 8 //Led direita
#define LED_R 7 //Led ré
#define LED_SIN 4 //LED de sinalização

//definir pino de interrupção
#define BOT 2 //Interrupção 0.

//BUZZER DE SINALIZAÇÃO.

/*============================================================================*/

//Classe para controlar o motor.
class DCmotor
{
    //Definindo variáveis internas da classe
    int spd = 255, pin1, pin2;

    //Métodos e variáveis que são utilizados
  public:
    void Pinout(int in1, int in2) {//Método para inicializar as entradas para o motor.
      pin1 = in1;
      pin2 = in2;
      pinMode(pin1, OUTPUT);
      pinMode(pin2, OUTPUT);
    }
    void Speed(int in1) //Método para configurar a velocidade
    {
      spd = in1;
    }
    void Forward() //Método para girar no sentido direto
    {
      analogWrite(pin1, spd);
      digitalWrite(pin2, LOW);
    }
    void Backward() //Método para girar no sentido inverso
    {
      digitalWrite(pin1, LOW);
      analogWrite(pin2, spd);
    }
    void Stop() //Método para parar.
    {
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, LOW);
    }
};
//Criando os objetos motor1 e motor2
DCmotor Motor1, Motor2;

/*============================================================================*/
//Criando as funções
void frente(int t);
void parado(int t);
void para(int t);
void esquerda(int t);
void direita(int t);

//Criando função de medição
long detectar();

//Variável transforma em dist.
float cmDet();

//função callback
void chaveOnoff();

void rotina_motor();

/*============================================================================*/

//Definindo variável para interrupção.
volatile int estado = LOW;
//OBS: INICIA NO ESTADO LOW(Desligado).

/*============================================================================*/
//Iniciando o Setup
void setup() {
  //Configurando os pinos do motor
  Motor1.Pinout(11, 10);
  Motor2.Pinout(6, 5);

  //Gerando interrupção no pino 2.

  attachInterrupt(0, chaveOnOff, RISING);

  /**************************************************
     0 -> Interrupção no pino 2;
     ChaveOnOff: Função de callback da interrupção
     Rising: Altera de low para high => Interrupção.
   ***************************************************/

  //Controle de pinos do sensor de distância
  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);

  //Configurando pinos dos leds de sinalização.
  pinMode(LED_E, OUTPUT);
  pinMode(LED_D, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_SIN, OUTPUT);

  //Iniciando a Serial.
  Serial.begin(9600);

  //Deixar velocidade fixa
  velocidade(255);

  //Gera semente para variável aleatória
  randomSeed(A0);
}

/*============================================================================*/

//Iniciando o loop infinito.
void loop() {
  bool op;

  //Pega valor em cm da distância.
  float cm = cmDet();

  //Se o estado for ligado
  if (estado)
  {
    //analisa a distância
    if (cm > 25 && cm <1400)
    {
      digitalWrite(LED_SIN, LOW);
      frente(75);
      Serial.print("DIRECAO: FRENTE");
      Serial.print("   DIST:");
      Serial.println(cm);
    }
    //Caso esteja na distância de risco
    else
    {
      //Liga o buzzer e o led de sinalização.
      digitalWrite(LED_SIN, HIGH);

      //Efetua operação random
      op = random(2);

      //Vê a direção de desvio.
      if (op)
      {
        para(75);
        re(100);
        direita(175);
        Serial.print("DIRECAO: RE => DIREITA");
        Serial.print("   DIST:");
        Serial.println(cm);
      }
      else
      {
        para(100);
        re(100);
        esquerda(175);
        Serial.print("DIRECAO: RE => ESQUERDA");
        Serial.print("   DIST:");
        Serial.println(cm);
      }
    }
  }
  //Se o estado for desligado. Os motores param.
  else
  {
    Motor1.Stop();
    Motor2.Stop();
    digitalWrite(LED_SIN, LOW);
  }

}
/*============================================================================*/
//Definindo funções utilizadas pelo robô.

void frente(int t)
{
  // Comando para o motor ir para frente
  Motor1.Forward();
  Motor2.Forward();
  //Controle de leds
  digitalWrite(LED_D, LOW);
  digitalWrite(LED_E, LOW);
  digitalWrite(LED_R, LOW);
  delay(t);
}

void re(int t)
{
  // Comando para o motor ir para trás
  Motor1.Backward();
  Motor2.Backward();
  //Controle de leds
  digitalWrite(LED_D, LOW);
  digitalWrite(LED_E, LOW);
  digitalWrite(LED_R, HIGH);
  delay(t);
}

void esquerda(int t)
{
  //Comando para o motor parar
  Motor1.Stop(); //Comando para o motor para
  Motor2.Forward();
  //Controle de leds
  digitalWrite(LED_D, LOW);
  digitalWrite(LED_E, HIGH);
  digitalWrite(LED_R, LOW);
  delay(t);
}

void direita(int t)
{


  Motor1.Forward();
  Motor2.Stop();
  //Controle de leds.
  digitalWrite(LED_D, HIGH);
  digitalWrite(LED_E, LOW);
  digitalWrite(LED_R, LOW);
  delay(t);
}

void para(int t)
{
  Motor1.Stop(); // Comando para o motor parar
  Motor2.Stop();
  //Controle de leds
  digitalWrite(LED_D, LOW);
  digitalWrite(LED_E, LOW);
  digitalWrite(LED_R, LOW);
  delay(t);
}

void velocidade(int num)
{
  //Transforma o número no intervalo de 0 a 255
  constrain(num, 0, 255);
  //Define a velocidade do carrinho
  Motor1.Speed(num);
  Motor2.Speed(num);
  Serial.println("Velocidade dos motores configurada para:");
  Serial.println(num);
}

//Função de detectar do sensor.
long detectar()
{
  digitalWrite(TRIG, LOW);
  delay(3);
  digitalWrite(TRIG, HIGH);
  delay(10);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH);
}


//Função de conversão de valor.
float  cmDet()
{
  long tempo = detectar();
  tempo = tempo / 2;
  float distancia = tempo * 0.034; // cm

  return distancia;
}

//Função de callback do carro.

void chaveOnOff()
{
  static unsigned long lastMillis = 0;
  unsigned long newMillis = millis();

  if (newMillis - lastMillis < 50)
  {

  }
  else
  {
    estado = !estado;
    Serial.println("Estado mudou.");
    if (estado)
    {
      Serial.println("Ligado!");
    }
    else
    {
      Serial.println("Desligado!");
    }
    lastMillis = newMillis;
  }

}

void rotina_motor()
{
  frente(3000);
  Serial.println("Frente");
  esquerda(2000);
  Serial.println("Esquerad");
  direita(2000);
  Serial.println("Direita");
  re(2000);
  Serial.println("Re");
  para(4000);
}
