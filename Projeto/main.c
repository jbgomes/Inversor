#include <avr/interrupt.h>
#include <math.h>

#define PONTOS_SENO 50

double seno_duty_cycle[PONTOS_SENO];
static int conta_interrupcoes = 0;
static int semiciclo = 0;

ISR(TIMER1_COMPA_vect) {
	// A interrup��o vai ser chamada a cada 65,1 us (15KHz)
	// A raz�o entre a frequ�ncia do PWM e do sinal de 60Hz � 256
	// Como considero o seno com 50 pontos, preciso representar cada ponto com 256/50 = ~5 interrup��es
	conta_interrupcoes++;
	
	int posicao_duty_cycle = (conta_interrupcoes - 1) / 5; // Indica a qual duty cycle essa interrup��o pertence
	double duty_cycle = seno_duty_cycle[posicao_duty_cycle]; // Pega o valor do duty cycle na tabela
	
	if (semiciclo == 0) {
		// Est� no semiciclo positivo
		OCR0A = 0; // Zera o Duty Cycle (PWM) das chaves do semiciclo negativo
		OCR0B = 255 * duty_cycle; // OCR1A varia de 0 a 255, onde 0 = 0% e 255 = 100%
	} else {
		// Est� no semiciclo negativo
		OCR0B = 0; // Zera o Duty Cycle (PWM) das chaves do semiciclo positivo
		OCR0A = 255 * duty_cycle; // OCR1A varia de 0 a 255, onde 0 = 0% e 255 = 100%
	}
	
	// Zera conta_interrup��es e muda o semiciclo.
	if (conta_interrupcoes >= 250) {
		PORTB ^= 0b00010000;
		conta_interrupcoes = 0;
		semiciclo = !semiciclo;
	}
}

int main(){
	DDRD = 0xFF; // Setando PD5 e PD6 na sa�da
	DDRB = 0xFF; // Setando PB4 como sa�da
	
	for(int i = 0; i < PONTOS_SENO; i++){
		double duty_cycle = sin((i*M_PI)/PONTOS_SENO);
		seno_duty_cycle[i] = duty_cycle; // Varia de 0 a 1, onde 0 = 0% e 1 = 100%
	}
 
	TCCR1A = 0b00000010; // Configura��o do timer para gerar uma interrup��o por software
	TCCR1B = 0b00000001; // Usa o clock sem prescaler. Ent�o o PWM ter� 64.25KHz
	TIMSK1 = 0b00000010; // Habilita a compara��o no estouro do contador
	OCR1A = 255;
	
	TCCR0A = 0b10100011; // Habilita o modo PWM r�pido, no modo invertido para OCR0A
	TCCR0B = 0b00000001; // Usa o clock sem prescaler. Ent�o o PWM ter� 64.25KHz
	
	OCR0A = 200;
	OCR0B = 200;
	sei(); // Habilita interrup��es globais
	
	while(1) {
		
	}
}


