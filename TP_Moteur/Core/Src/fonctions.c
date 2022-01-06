/*
 * fonctions.c
 *
 *  Created on: 20 oct. 2021
 *      Author: pensi
 */
int V=50;

#include "fonctions.h"

// Ecriture du début de ligne
void Ligne_Init(UART_HandleTypeDef *huart){
	HAL_UART_Transmit(huart, (uint8_t *)&LIGNE, strlen(LIGNE), 0xFFFF);
}

// Gestion des commandes entrées dans la console
void Commande_exec(int i){
	int commande_existe=0;
	//Commande pinout
	if ((strncmp(Commande,PINOUT,i)==0) && (i==strlen(PINOUT))){
		printf("\r\nLes broches connectées sont:\r\n");
		printf("    - PA7 au pin 31 du hacheur\r\n");
		printf("    - PC0 au pin 13 du hacheur\r\n");
		printf("    - PC1 au pin 12 du hacheur\r\n");
		printf("    - PB0 au pin 30 du hacheur\r\n");
		printf("    - PA4 au pin 33 du hacheur\r\n");
		printf("    - PA0 à la phase 1 du codeur\r\n");
		printf("    - PA1 à la phase 2 du codeur\r\n");
		commande_existe=1;
	}
	//Commande speed
	if (strncmp(Commande,SPEED,6)==0){
		Set_SPEED(i);
		commande_existe=1;
	}
	//Commande start
	if ((strncmp(Commande,START,i)==0) && (i==strlen(START))){
		printf("\r\nPower ON\r\n");
		Start();
		commande_existe=1;
	}
	//Commande stop
	if ((strncmp(Commande,STOP,i)==0) && (i==strlen(STOP))){
		printf("\r\nPower OFF\r\n");
		Stop();
		commande_existe=1;
	}
	//Commande help
	if ((strncmp(Commande,HELP,i)==0) && (i==strlen(HELP))){
		printf("\r\nLes commandes disponibles sont:\r\n");
		printf("    - help\r\n");
		printf("    - pinout\r\n");
		printf("    - start\r\n");
		printf("    - stop\r\n");
		commande_existe=1;
	}
	//Commande get_speed
	if ((strncmp(Commande,GET_SPEED,i)==0) && (i==strlen(GET_SPEED))){
		Get_Speed();
		commande_existe=1;
	}
	if(commande_existe==0){
		printf("\r\nCette commande n'existe pas\r\n");
	}
}

void Get_Speed(){
	printf("\r\nLa vitesse est %d pulse/sec\r\n", vitesse);
}

//Set_speed prend une valeur entre 1 et 99. 50 correspond à la vitesse nulle et à un rapport cyclique de 1/2
void Set_SPEED(int i){
	int length = i - strlen(SPEED);
	char valeur[length];
	//Isolation e la valeur de la vitesse dans la commmande
	for (int j=0; j<length; j++){
		valeur[j]=Commande[6+j];
	}
	//Transtypage de cette valeur en int
	int a = atoi(valeur);
	//Valeur cohérente ?
	if(a>0 || a<100){
		V=a;
		//Modification des CCR et donc des rapports ciclyques
		TIM1->CCR1 = (int) ((TIM1->ARR)*a)/100;
		TIM1->CCR2 = (TIM1->ARR) - TIM1->CCR1;
	}else{
		printf("La valeur de vitesse n'est pas valable. Elle doit être comprise entre 1 et 99.=%d\r\n", a);
	}
}

//Lancement de la PWM et démmarage du hacheur
void Start(){
	TIM1->CCR1 = (int) ((TIM1->ARR)*V)/100;
	TIM1->CCR2 = (TIM1->ARR) - TIM1->CCR1;
	printf("\r\nARR=%d\r\n",(int)TIM1->ARR);
	printf("\r\nCCR1=%d\r\n",(int)TIM1->CCR1);
	printf("\r\nCCR2=%d\r\n",(int)TIM1->CCR2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	ON=1;
}

//Stop de la PWM
void Stop(){
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
	ON=0;
}
