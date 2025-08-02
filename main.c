#include <stdio.h>
#include <stdlib.h>
#include "libs/calculadora.h"
#include "libs/Archivos.h"
#include "libs/calculadora_bmp.h"
int main()
{
    ecuacion ecuaciones[10];
    size_t cecu=0;
    char op, subop;
    int arch_guardados=0, salir=1;
    contarArchivos(&arch_guardados);
    int indice;

    while(salir){
        op=menu();
        switch(op){
            case 'A':
                if(cecu!=0){
                    limpiarPantalla();
                    subop = pedir_opcion("ABX",
                    "--- ENTER EQUATION ---\n"
                    "[A] - Add new equation.\n"
                    "[B] - Change existing equation.\n"
                    "[X] - Return.\n"
                    "Enter an option: "
                    );

                    if(subop=='B'){
                        ingresarecu(ecuaciones, &cecu,2,0);
                        printf("\nEquation modified correctly.\n");
                        EnterParaSalir();
                        break;
                    }
                    if(subop=='X'){
                        break;
                    }
                }
                if(ingresarecu(ecuaciones, &cecu,1,0))
                    puts("Equation couldn't be added");
                printf("\nEquation added correctly.\n");
                EnterParaSalir();
                break;
            case 'B':
                mostrarecuaciones(ecuaciones, cecu,1);
                EnterParaSalir();
                break;
            case 'C':
                if(arch_guardados<10){
                    if(!GuardaArchivos(ecuaciones, cecu, &arch_guardados)){
                        borrarecuacion(ecuaciones,&cecu);
                    }
                }
                else
                    puts("You cant save more equations. Use the option [E] to delete the saved equations");

                EnterParaSalir();
                break;
            case 'D':
                CargarArchivo(ecuaciones,&cecu);
                break;
            case 'E':
                borrarArchivos();
                borrarecuacion(ecuaciones,&cecu);
                arch_guardados=0;
                EnterParaSalir();
                break;
            case 'F':
                if(cecu != 0){
                    limpiarPantalla();
                    printf("--- EVALUATE EQUATION ---\n\n");
                    subop = pedir_opcion("ABCX",
                        "[A] - Enter a table of values.\n"
                        "[B] - Enter a single value for the variable.\n"
                        "[C] - Search for roots.\n"
                        "[X] - Return.\n"
                        "Enter an option: "
                    );
                    if(subop=='A'){
                        EvaluarListaDeValores(ecuaciones, cecu);
                    }
                    else if(subop=='B'){
                        TablaAlrededorDeValor(ecuaciones,cecu);
                        EnterParaSalir();
                    }
                    else if(subop=='C'){
                        BuscarRaices(ecuaciones, cecu);
                        EnterParaSalir();
                    }
                }
                else {
                    puts("There are no saved equations to evaluate.");
                    EnterParaSalir();
                }
                break;
            case 'G':
                if(cecu == 0) {
                    puts("There are no saved equations to graph.");
                    EnterParaSalir();
                    break;
                }
                graficar(ecuaciones,cecu,&indice);
                break;
            case 'H':
                menu_ayuda_interactivo();
                EnterParaSalir();
                break;
            case 'X':
                subop = pedir_opcion("SN",
                    "Are you sure you want to exit? S/N: "
                );
                if(subop=='S'){
                    printf("\nClosing the program...\n");
                    salir=0;
                }
                break;
        }
    }

    borrarecuacion(ecuaciones,&cecu);
    return 0;
}
