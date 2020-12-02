#include <avr/interrupt.h>
#include <math.h>

#define PONTOS_SENO 50

double seno_duty_cycle[PONTOS_SENO];
static int conta_interrupcoes = 0;
static int semiciclo = 0;

ISR(TIMER1_COMPA_vect) {
	// A interrupção vai ser chamada a cada 65,1 us (15KHz)
	// A razão entre a frequência do PWM e do sinal de 60Hz é 256
	// Como considero o seno com 50 pontos, preciso representar cada ponto com 256/50 = ~5 interrupções
	conta_interrupcoes++;
	
	int posicao_duty_cycle = (conta_interrupcoes - 1) / 5; // Indica a qual duty cycle essa interrupção pertence
	double duty_cycle = seno_duty_cycle[posicao_duty_cycle]; // Pega o valor do duty cycle na tabela
	
	if (semiciclo == 0) {
		// Está no semiciclo positivo
		OCR0A = 0; // Zera o Duty Cycle (PWM) das chaves do semiciclo negativo
		OCR0B = 255 * duty_cycle; // OCR1A varia de 0 a 255, onde 0 = 0% e 255 = 100%
	} else {
		// Está no semiciclo negativo
		OCR0B = 0; // Zera o Duty Cycle (PWM) das chaves do semiciclo positivo
		OCR0A = 255 * duty_cycle; // OCR1A varia de 0 a 255, onde 0 = 0% e 255 = 100%
	}
	
	// Zera conta_interrupções e muda o semiciclo.
	if (conta_interrupcoes >= 250) {
		PORTB ^= 0b00010000;
		conta_interrupcoes = 0;
		semiciclo = !semiciclo;
	}
}

int main(){
	DDRD = 0xFF; // Setando PD5 e PD6 na saída
	DDRB = 0xFF; // Setando PB4 como saída
	
	for(int i = 0; i < PONTOS_SENO; i++){
		double duty_cycle = sin((i*M_PI)/PONTOS_SENO);
		seno_duty_cycle[i] = duty_cycle; // Varia de 0 a 1, onde 0 = 0% e 1 = 100%
	}
 
	TCCR1A = 0b00000010; // Configuração do timer para gerar uma interrupção por software
	TCCR1B = 0b00000001; // Usa o clock sem prescaler. Então o PWM terá 64.25KHz
	TIMSK1 = 0b00000010; // Habilita a comparação no estouro do contador
	OCR1A = 255;
	
	TCCR0A = 0b10100011; // Habilita o modo PWM rápido, no modo invertido para OCR0A
	TCCR0B = 0b00000001; // Usa o clock sem prescaler. Então o PWM terá 64.25KHz
	
	OCR0A = 200;
	OCR0B = 200;
	sei(); // Habilita interrupções globais
	
	while(1) {
		
	}
}


