/*
 * Lab4Completo.c
 *
 *  Created on: 13 dic 2023
 *      Author: Utente
 */

#include <stdio.h>
#include "xstatus.h"
#include "xtmrctr_l.h"
#include "xil_printf.h"
#include "xparameters.h"

#ifndef SDT
#define TMRCTR_BASEADDR		XPAR_TMRCTR_0_BASEADDR
#else
#define TMRCTR_BASEADDR		XPAR_XTMRCTR_0_BASEADDR
#endif

#define TIMER_COUNTER_0	 0

//Creo un vettore e delle variabile globali 
int vettore[8],y=0,flag=0;

void update_vector_up(int *vector);
void myISR(void)__attribute__((interrupt_handler));
void write_digit(u8 digit, u8 dotted);
int TimerCounter(UINTPTR TmrCtrBaseAddress, u8 TimerCounter);

int TimerCounter(UINTPTR TmrCtrBaseAddress, u8 TmrCtrNumber){
	u32 ControlStatus;

	//Pulisco il valore del Control Status Register
	XTmrCtr_SetControlStatusReg(TmrCtrBaseAddress, TmrCtrNumber, 0x0);
	/*Accediamo a load register e ci carichiamo dentro il volore al quale vogliamo far
	 *arrivare il conteggio, in questo caso vogliamo che conti fino a 10000 in modo da
	 * vedere sui diversi gruppi di segmenti dei valori differenti contemporanemante.
	 */
	XTmrCtr_SetLoadReg(TmrCtrBaseAddress, TmrCtrNumber, 10000);
	// Trasferiamo il contenuto del Load register nel counter settando a 1 il bit 5
	//del TCSR.
	XTmrCtr_LoadTimerCounterReg(TmrCtrBaseAddress, TmrCtrNumber);

	//Rileggiamo il valore contenuto nel TCST per settare i diversi bit che mi abilitino
	//il conteggio

	//a 0 il bit del Load register
	//(bit 5) così da abilitare il conteggio
	ControlStatus = XTmrCtr_GetControlStatusReg(TmrCtrBaseAddress,
			TmrCtrNumber);

	/*Dopo aver letto il contenuto del TCSR setto a  0 il bit del Load register
	*(bit 5) così da abilitare il conteggio e successivamente con l'OR  setto a 1
	*i bit 1,4 e 6 rispettivamente per abilitare il down counter, l'autoreload e
	*l'interrupt
	*/
	XTmrCtr_SetControlStatusReg(TmrCtrBaseAddress, TmrCtrNumber,
				    (ControlStatus & (~XTC_CSR_LOAD_MASK))|(0x52));

   /*A questo punto inizia il conteggio andando a decrementare il valore caricato,
    * quando il conteggio arriva a 0 il timer genererà un interrupt e il conteggio
    * ripartirà dal valore caricato grazie all'autoreload
    */
	//faccio partire il conteggio
	XTmrCtr_Enable(TmrCtrBaseAddress, TmrCtrNumber);//il bit 7 viene settatol a 1 e inizia il conteggio

	return 1;
}


void write_digit(u8 digit, u8 dotted){

	if(dotted==0){//punto spento
		if (digit=='0' | digit==0)//Gestisco sia i valori che arrivano da UART come caratteri ASCII sia quelli gestisti come interi
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0xC0);
		else if (digit=='1' | digit==1)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0xF9);
		else if (digit=='2' | digit==2)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0xA4);
		else if (digit=='3' | digit==3)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0xB0);
		else if (digit=='4' | digit==4)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x99);
		else if (digit=='5' | digit==5)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x92);
		else if (digit=='6' | digit==6)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x82);
		else if (digit=='7' | digit==7)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0xF8);
		else if (digit=='8' | digit==8)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x80);
		else if (digit=='9' | digit==9)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x90);
	}

	else{//punto acceso
		if (digit=='0' | digit==0)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x40);
		else if (digit=='1' | digit==1)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x79);
		else if (digit=='2' | digit==2)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x24);
		else if (digit=='3' | digit==3)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x30);
		else if (digit=='4' | digit==4)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x19);
		else if (digit=='5' | digit==5)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x12);
		else if (digit=='6' | digit==6)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x02);
		else if (digit=='7' | digit==7)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x78);
		else if (digit=='8' | digit==8)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x00);
		else if (digit=='9' | digit==9)
			Xil_Out32(XPAR_AXI_7SEGS_GPIO_BASEADDR, 0x10);
	}
}

void update_vector_up(int *vector){
	//Questa funzione conta i secondi
	   int riporto = 1, dimensione=8;

	    for (int i = 0; i < dimensione && riporto > 0; i++) {
	    	//Aggiungo il valore del riporto al contenuto del vettore in posizione i
	        vettore[i] += riporto;
	        //Mi ricalcolo il riporto dividendo il contenuto del vettore in posizione i per 10
	        riporto = vettore[i] / 10;
	        //Ottiengo il resto della divisione per 10 e assegno il risultato al contenuto del vettore in posizione i
	        vettore[i] %= 10;
	    }
}

void myISR(){
	u32 control_status;
	u8 status;

	//Ogni volta che arriva l'interrupt dal timer flag si incrementa di uno
	flag++;
	if(flag==10000){ //Quando flag arriva a 10000 vuol dire che è passato un secondo
		update_vector_up(vettore); //Aggiorniamo il contatore
		flag=0; //Riportiamo flag a 0 per continuare a contare i secondi
	}
	//Leggo il contenuto del registro degli anodi
	status=*((u8*)XPAR_AXI_7SEGSAN_GPIO_BASEADDR);
	/*Se sto utilizzando l'ultimo gruppo di segmenti o sono tutti spenti, shifto di una
	*posizione il contenuto del registro in modo che nella posizione meno significativa
	*del registro mi venga aggiunto uno zero accendendomi quindi il primo gruppo
	*del di segmenti */
	if(status==0x7F | status==0xFF){
		status=status<<1;
		y=0;
	}
	/*In tutti gli altri casi, shifto di uno tutto il registro in modo da accendendermi
	 *  il gruppo di segmenti successivo.Lo shift introduce però uno zero nella
	 *  posizione meno significativa che mi accenderebbe anche il primo gruppo di segmenti,
	 *  per evitare questo aggiungo uno.
	 */

	else{
		status=status<<1;
		status=status+1;
		y++;//Incremento y per leggere il valore dell'array successivo
	}
	//Attivo il gruppo di segmenti accendendo quello identificato da status
	Xil_Out32(XPAR_AXI_7SEGSAN_GPIO_BASEADDR, status);

	//Ad ogni interrupt generato dal timer, scrivo in un gruppo di segmenti un valore letto
	//dal vettore.
	write_digit(vettore[y],0);//Lo zero sta per disattivare il punto

	//Leggo lo stato del registro TCSR
	control_status=*((unsigned int*)XPAR_AXI_TIMER_0_BASEADDR);
	//modifico solo il bit 8
	*((unsigned int*)XPAR_AXI_TIMER_0_BASEADDR)=control_status | 0x0100;
	/*ack INTC- base_address+0x0C (IAR)
   * Questo registro serve per pulire le richieste dell'interrup dopo che la periferica
   * è stata servita settando a 1 il canale dal quale è arrivato l'interrupt.
   */
	*((unsigned int*)(XPAR_AXI_INTC_0_BASEADDR+0xC))=0x4;
}

int main(void)
{
	init_platform();

	//enable INTC
	/* enable MER, base_addres+0x1C
   *E' usato per abilitare le richieste di interrupts verso il processore, i due bit meno
   *significativi vengono settati a 1 per abilitare le richieste delle le periferiche.
   */
	*((unsigned int*)(XPAR_AXI_INTC_0_BASEADDR+0x1C))=0x3;
	/*
   * enable  IER, base_addres+0x08
   * E' usato per abilitare la selezione degli interruputs.
   * Setto 1 il 3 bit meno significativo e gli altri a 0 perchè l'interrupt del timer arrriva
   * dal canale 3.
   */
	*((unsigned int*)(XPAR_AXI_INTC_0_BASEADDR+0x08))=0x4;

	//Parto con una condizione dove tutti i gruppi di segmenti sono spenti
	Xil_Out32(XPAR_AXI_7SEGSAN_GPIO_BASEADDR, 0xFF);

	//Parto da una condizione dove tutti i gruppi di segmenti sono a 0
	for(int i=0;i<8;i++)
		vettore[i]=0;

	microblaze_enable_interrupts ();
	TimerCounter(TMRCTR_BASEADDR, TIMER_COUNTER_0);
	cleanup_platform();
}
