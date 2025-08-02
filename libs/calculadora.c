#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "Vector.h"
#include "calculadora.h"
#include <string.h>
#include "Archivos.h"
#include "calculadora_bmp.h"


#define TAM_ECUACION 512
#define MAX_ECUAC 10

typedef enum{
    ECUACION_OK,
    SYNTAX_ERROR,
    DOBLE_OPERADOR_ERROR,
    POTENCIA_PARENTESIS_ERROR,
    PARENTESIS_NO_CERRADO,
    DIVISION_ENTRE_CERO,
    NO_ES_ECUACION,
    ERROR_IGUAL,
    FALTA_OPERANDO,
    ERROR_IGUAL_OPERADOR,
    PARENTESIS_ERROR
}EstadoEcuacion;

void limpiarPantalla() {
    #ifdef _WIN32
        system("cls");     // Windows
    #else
        system("clear");   // Linux / macOS
    #endif
}

void EnterParaSalir() {
    printf("\n\nPress enter to continue...");
    getchar();
}

void borrarecuacion(ecuacion *ecu , size_t *ce){
    destruirecuaciones(ecu,*ce);
    *ce=0;
}

void destruirecuaciones(ecuacion *ecu, size_t n) {
    if (ecu == NULL) return;

    ecuacion *p_ecu = ecu;
    for (size_t i = 0; i < n; ++i) {
        if (p_ecu->arbol_ecu != NULL) {
            EliminarArbol(p_ecu->arbol_ecu);
            p_ecu->arbol_ecu = NULL;
        }
        p_ecu++;
    }
}

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

char pedir_opcion(const char *validas, const char *mensaje) {
    char op;
    printf("%s", mensaje);
    do {
        do {
            op = getchar();
        } while (op == '\n' || op == ' ');  // ignora enter o espacios

        limpiarBuffer();  // limpia el resto de la linea
        op = Amayus(op);

        if (!strchr(validas, op)) {
            printf("Enter a valid option: ");
        }
    } while (!strchr(validas, op));

    return op;
}

char menu(){
    char letra;

    limpiarPantalla();
    printf("\n--- MAIN MENU ---\n");
    printf("[A] - Enter Equations\n");
    printf("[B] - View Current Session Equations\n");
    printf("[C] - Save Equations and Reset Session\n");
    printf("[D] - Load Saved Equations\n");
    printf("[E] - Delete Saved Equations\n");
    printf("[F] - Solve Equations\n");
    printf("[G] - Graph Equations\n");
    printf("[H] - Help\n");
    printf("[X] - Exit\n");
    printf("Select an option: ");
    do {
        letra = getchar();            // Lee un solo caracter
        limpiarBuffer();             // Descarta el resto de la linea
        letra = Amayus(letra);       // Convertir a mayuscula si hace falta

        if (letra != 'X' && (letra < 'A' || letra > 'H')) {
            printf("Enter a valid equation: ");
        }
    } while (letra != 'X' && (letra < 'A' || letra > 'H'));

    return letra;
}

void quitarespacios(char* ecu) {
    char* origen = ecu;
    char* destino = ecu;

    while (*origen) {
        if (!isspace(*origen)) {
            *destino = *origen;
            destino++;
        }
        origen++;
    }
    *destino = '\0';
}

int verificarecuacion(char* ecu){
    char *p_ecu=ecu; // -2^(1/2) || 1/(x-2) cuando x=2
    int band_error=ECUACION_OK, cont_par_abi=0, cont_par_cerr=0, cont_igual=0,cont_x=0,cont_y=0;
    quitarespacios(ecu);
    while(*p_ecu && !band_error && (esoperador(*p_ecu) || esnumero(*p_ecu) || esvariable(*p_ecu) || esparentesis(*p_ecu))){


        if(p_ecu > ecu && *(p_ecu-1)=='=' && !(*p_ecu=='+' ||*p_ecu=='-'  || *p_ecu=='(' || esnumero(*p_ecu) || esvariable(*p_ecu))){
            band_error=ERROR_IGUAL_OPERADOR;
        }
        if(*ecu=='='){
            band_error=ERROR_IGUAL;
        }
        if(*p_ecu=='='){
            cont_igual++;
        }
        if(EsOperacion(*p_ecu) && EsOperacion(*(p_ecu+1)))
            band_error=DOBLE_OPERADOR_ERROR;

        if(EsOperacion(*p_ecu) && !(esnumero(*(p_ecu+1)) || esvariable(*(p_ecu+1)) || esparentesis(*(p_ecu+1))))
            band_error=FALTA_OPERANDO;

        if(*p_ecu=='(' && !(esnumero(*(p_ecu+1)) || esvariable(*(p_ecu+1)) || esparentesis(*(p_ecu+1)) || *(p_ecu+1)=='-'))
            band_error=PARENTESIS_ERROR;

        if(*p_ecu == '^' && *(p_ecu+1)!= '('){ //
            band_error=POTENCIA_PARENTESIS_ERROR;
        }

        if(*p_ecu == '('){
            cont_par_abi++;
        }else if(*p_ecu==')')
            cont_par_cerr++;

        if(*p_ecu == '/' && *(p_ecu+1)== '0'){ // x/(0)
            band_error=DIVISION_ENTRE_CERO;
        }

        if(esvariable(*p_ecu)){
            if(*p_ecu=='x')
                cont_x++;
            else if(*p_ecu=='y')
                cont_y++;
        }
        p_ecu++;
    }

    if(!*p_ecu=='\0' && !esoperador(*p_ecu) && !esnumero(*p_ecu) && !esvariable(*p_ecu) && !esparentesis(*p_ecu)){
        band_error=SYNTAX_ERROR;
        return band_error;
    }
    if(p_ecu > ecu && (*(p_ecu-1)=='=' || cont_igual!=1)){
        band_error=ERROR_IGUAL;
    }

    if(cont_x ==0 || cont_y==0){
        band_error=NO_ES_ECUACION;
    }


    if(cont_par_abi != cont_par_cerr)
        band_error=PARENTESIS_NO_CERRADO;

    if(band_error){
        return band_error;
    }

return ECUACION_OK;
}
void Amin(char* ecu) {
    char *p_ecu = ecu;
    while (*p_ecu) {
        *p_ecu = tolower(*p_ecu);
        p_ecu++;
    }
}

void introducirecuacion(char* ecu) {
    EstadoEcuacion estado;
    char confirmacion;
    do {
        estado=1;
        printf("Enter Equation: ");
        fgets(ecu, TAM_ECU, stdin);
        quitarenter(ecu);
        Amin(ecu);
        printf("\nEquation: %s",ecu);
        printf("\nDo you want to introduce this equation? S/N: ");
        do{
            scanf("%c",&confirmacion);
            limpiarBuffer();
            confirmacion=Amayus(confirmacion);
            if(confirmacion!='S' && confirmacion!='N')
                printf("\Choose a correct option: ");
        }while(confirmacion!='S' && confirmacion!='N');

        if(confirmacion=='S'){
            estado = verificarecuacion(ecu);

            if (estado != ECUACION_OK) {
                ayuda_ecuacion(); // Muestra ayuda si hay error
                printf("Invalid equation. Try again.\n");
            }
        }

    } while (estado != ECUACION_OK);
}

void mostrarecuaciones(ecuacion *ecuac, size_t ce, int print){
    if(print){
        limpiarPantalla();
        printf("\n--- Loaded equations ---\n");
    }
    if(ce==0){
        printf("\nThere are no loaded equations.\n");
        return;
    }
    ecuacion *p_ecuac=ecuac;
    for(int i=0; i<ce; i++){
        printf("\nEquation %d: %s",i+1, p_ecuac->ecu);
        p_ecuac++;
    }
}

/////////////////////////////////////////////////////////////////////////////////INI: PRINTEO

void mostrar_char(char* p) {
    printf("%c ", *p);
}

void mostrar_double(double* p) {
    printf("%.2f ", *p);
}

void inorder(nodo* root, void (*mostrar1)(char*), void (*mostrar2)(double*)) {
    if (!root)
        return;

    if (root->tipo == OPERADOR) {
        inorder(root->op.izq, mostrar1, mostrar2);
        mostrar1(&root->op.operador);
        inorder(root->op.der, mostrar1, mostrar2);
    }
    else if (root->tipo == VARIABLE) {
        mostrar1(&root->variable);
    }
    else if (root->tipo == NUMERO) {
        mostrar2(&root->numero);
    }
}
/////////////////////////////////////////////////////////////////////////////////FIN: PRINTEO

void introducirecuacionarchivo(char* ecuU, void* ecuarch){
    char *p_ecuarch=(char*)ecuarch, *p_ecuU=ecuU;
    while(*p_ecuarch){
        if(*p_ecuarch!='\n')
            *p_ecuU=*p_ecuarch;
        p_ecuU++;
        p_ecuarch++;
    }
    p_ecuU++;
    p_ecuU='\0';
}

int ingresarecu(ecuacion *ecuac, size_t *cecu, int ingreso, void*ecuarch){
    //INGRESO ==1 :CARGAR POR USUARIO
    //INGRESO ==0 : CARGAR POR ARCHIVO
    //INGRESO ==2 : CAMBIAR ECUACION
    int num=-1;
    ecuacion aux_ecuacion={0}, *p_ecuac;
    char ecuA[TAM_ECUACION]={0};

    if(ingreso==1 || ingreso==2){
        if(ingreso==2)
            printf("\nReplace equation\n");
        introducirecuacion(ecuA);
    }
    else if(ingreso==0){
        introducirecuacionarchivo(ecuA,ecuarch);
    }

    aux_ecuacion.arbol_ecu=NULL;
    p_ecuac=ecuac;

    if(ingreso!=2 && *cecu<MAX_ECUAC){
        p_ecuac+=*cecu;
        (*cecu)++;
    }
    else{
        if(ingreso!=2)
            printf("Full memory. Which equation do you want to replace?: ");
        else
            printf("Which equation do you want to replace?: ");

        printf("\nEquation 0: %s","CANCEL");
        mostrarecuaciones(ecuac,*cecu,0);
        do{
            if(num<0 || num>10)
                printf("\nEnter a valid number:");
            scanf("%d", &num);
            limpiarBuffer();
        }while(num<0 || num>10);

        aux_ecuacion.arbol_ecu=NULL;
        p_ecuac=ecuac;
        p_ecuac+=(num-1);
    }

    if(num==0)
        return 0;

    strcpy(aux_ecuacion.ecu, ecuA);
    if(num>0)
        EliminarArbol((p_ecuac->arbol_ecu)); //eliminar arbol anterior en caso de reemplazarlo
    *p_ecuac=aux_ecuacion;


    Vector ecuac_token;
    tokenizar(&ecuac_token,ecuA); //TOKENIZAMOS ecuA (la ecuacion actual)

    insertarmultiplicaciones(&ecuac_token); //Ingresamos las multiplicaciones implicitas

    postfija(&ecuac_token); //PASAMOS EL TOKEN A POSTFIJA
    CrearArbol(&ecuac_token,&(p_ecuac->arbol_ecu)); //CREAMOS EL ARBOL CON LOS DATOS DE LA ECUACION

    vectorEliminar(&ecuac_token);

return 0;
}

double evaluarArbol(struct nodo* arbol, double x, double y) {
    if (arbol == NULL) return 0; // seguridad

    switch (arbol->tipo) {
        case NUMERO:
            return arbol->numero;

        case VARIABLE:
            return (arbol->variable == 'x') ? x : y;

        case OPERADOR: {
            double izq = evaluarArbol(arbol->op.izq, x, y);
            double der = evaluarArbol(arbol->op.der, x, y);

            switch (arbol->op.operador) {
                case '+': return izq + der;
                case '-': return izq - der;
                case '*': return izq * der;
                case '/':
                    if (fabs(der) < 1e-9) // Si el denominador es muy cercano a cero
                        return izq / 1e-9;
                    else
                        return izq / der;
                case '^': return pow(izq, der);
                default: return 0; // operador desconocido
            }
        }
    }

    return 0; // caso no reconocido
}

void graficar(ecuacion* ecuaciones, size_t cecu, int* indice){
    double x_min, x_max, y_min, y_max;
    limpiarPantalla();
    printf("\n--- GRAPH EQUATIONS ---\n");
    mostrarecuaciones(ecuaciones, cecu,0);
    printf("\nEnter the number of equation to be graphed: ");
    scanf("%d", indice);
    limpiarBuffer();
    if(*indice < 1 || *indice > cecu) {
        puts("Invalid index.");
        return;
    }

    printf("Enter the lower limit of X: ");
    scanf("%lf", &x_min);
    printf("Enter the higher limit of X: ");
    do{
        scanf("%lf", &x_max);
        if(x_max <=x_min) {
            printf("The higher limit should be higher than the lower limit. Try again: ");
        }
    }while(x_max <= x_min);

    printf("Enter the lower limit of Y: ");
    scanf("%lf", &y_min);
    printf("Enter the higher limit of Y: ");
    do{
        scanf("%lf", &y_max);
        if(y_max <= y_min) {
            printf("The higher limit should be higher than the lower limit. Try again: ");
        }
    }while(y_max <= y_min);
    limpiarBuffer();

    graficar_ecuacion(ecuaciones, *indice-1, x_min, x_max, y_min, y_max);
    graficar_ecuacion_consola(ecuaciones, *indice-1, x_min, x_max, y_min, y_max);
    puts("Graph saved as 'grafico.bmp'.");
    EnterParaSalir();
}

double buscar_raiz(nodo* arbol, double fijo, double min, double max, double tolerancia, int max_iter, int buscar_y) {
    double a = min, b = max, fa, fb, fm, m;
    int iter = 0;

    fa = buscar_y ? evaluarArbol(arbol, fijo, a) : evaluarArbol(arbol, a, fijo);
    fb = buscar_y ? evaluarArbol(arbol, fijo, b) : evaluarArbol(arbol, b, fijo);

    if (fa * fb > 0) return NAN;

    while ((b - a) > tolerancia && iter < max_iter) {
        m = (a + b) / 2.0;
        fm = buscar_y ? evaluarArbol(arbol, fijo, m) : evaluarArbol(arbol, m, fijo);

        if (fa * fm < 0) {
            b = m; fb = fm;
        } else {
            a = m; fa = fm;
        }
        iter++;
    }
    return (a + b) / 2.0;
}

double buscar_raiz_auto(nodo* arbol, double fijo, double min, double max, double tolerancia, int max_iter, int buscar_y) {
    double paso = (max - min) / 100.0;
    double a = min, b = a + paso;
    double fa = buscar_y ? evaluarArbol(arbol, fijo, a) : evaluarArbol(arbol, a, fijo);
    for (; b <= max; a += paso, b += paso) {
        double fb = buscar_y ? evaluarArbol(arbol, fijo, b) : evaluarArbol(arbol, b, fijo);
        if (fa * fb <= 0) {
            // Hay cambio de signo, aplicar bisección aquí
            return buscar_raiz(arbol, fijo, a, b, tolerancia, max_iter, buscar_y);
        }
        fa = fb;
    }
    return NAN;
}

int buscar_raiz_multi(nodo* arbol, double fijo, double min, double max, double tolerancia, int max_iter, double* soluciones, int max_sol, int buscar_y) {
    double paso = (max - min) / 1000.0;
    int n_sol = 0;
    double a = min, b = a + paso;
    double fa = buscar_y ? evaluarArbol(arbol, fijo, a) : evaluarArbol(arbol, a, fijo);
    double *p_sol = soluciones;

    while (b <= max && n_sol < max_sol) {
        double fb = buscar_y ? evaluarArbol(arbol, fijo, b) : evaluarArbol(arbol, b, fijo);

        // Raiz exacta en a
        if (fabs(fa) < tolerancia) {
            int duplicado = 0;
            for (double *p_check = soluciones; p_check < p_sol; p_check++) {
                if (fabs(*p_check - a) < tolerancia * 2)
                    duplicado = 1;
            }
            if (!duplicado) {
                *p_sol = a;
                p_sol++;
                n_sol++;
            }
        }
        // Raiz exacta en b
        if (fabs(fb) < tolerancia) {
            int duplicado = 0;
            for (double *p_check = soluciones; p_check < p_sol; p_check++) {
                if (fabs(*p_check - b) < tolerancia * 2)
                    duplicado = 1;
            }
            if (!duplicado) {
                *p_sol = b;
                p_sol++;
                n_sol++;
            }
        }
        // Cambio de signo
        if (fa * fb < 0) {
            double raiz = buscar_raiz(arbol, fijo, a, b, tolerancia, max_iter, buscar_y);
            int duplicado = 0;
            for (double *p_check = soluciones; p_check < p_sol; p_check++) {
                if (fabs(*p_check - raiz) < tolerancia * 2)
                    duplicado = 1;
            }
            if (!duplicado) {
                *p_sol = raiz;
                p_sol++;
                n_sol++;
            }
        }
        a += paso;
        b += paso;
        fa = fb;
    }
    return n_sol;
}

double leer_fraccion_o_decimal(const char* prompt) {
    char buffer[64];
    double resultado = 0.0;
    int valido = 0;
    while (!valido) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
        char* barra = strchr(buffer, '/');
        if (barra) {
            // Es una fracción
            *barra = '\0';
            double num = atof(buffer);
            double den = atof(barra + 1);
            if (den != 0) {
                resultado = num / den;
                valido = 1;
            } else {
                printf("Denominator cannot be zero.\n");
            }
        } else {
            // Es decimal
            resultado = atof(buffer);
            valido = 1;
        }
    }
    return resultado;
}
void TablaAlrededorDeValor(ecuacion *ecu, size_t cecu) {
    int i = 0;
    double valor, paso = 1.0;
    int variable;
    ecuacion *p_ecu = ecu;

    limpiarPantalla();
    printf("\n--- TABLE OF VALUES ---\n");

    mostrarecuaciones(ecu, cecu,0);
    printf("\nEnter the number of equation to evaluate: ");
    do {
        scanf("%d", &i);
        limpiarBuffer();
        if (i < 1 || i > cecu)
            printf("\nEnter a valid equation: ");
    } while (i < 1 || i > cecu);

    p_ecu += (i - 1);
    printf("\nYou choose: %s\n", p_ecu->ecu);

    variable = pedir_opcion("XY", "What variable do you want to generate the table on? (X/Y): ");

    valor = leer_fraccion_o_decimal("Enter the central value for the variable: ");
    paso = leer_fraccion_o_decimal("Enter the step: ");
    if (paso <= 0) paso = 1;

    limpiarPantalla();
    printf("\n--- TABLE OF VALUES ---\n");
    if (variable == 'X') {
        printf("\nTable of values for X of the function %s\nin the range [%.2f, %.2f]:\n",p_ecu->ecu, valor - 5 * paso, valor + 5 * paso);
        printf("X\tY (that makes f(x,y)=0)\n");
        int j;
        for (j = -5; j <= 5; j++) {
            double x = valor + j * paso;
            double soluciones[10];
            double *p_sol = soluciones;
            int n_sol = buscar_raiz_multi(p_ecu->arbol_ecu, x, -100, 100, 1e-6, 100, soluciones, 10, 1);
            printf("%.6f\t", x);
            if (n_sol == 0)
                printf("No solution\n");
            else {
                double *p_fin = soluciones + n_sol;
                while (p_sol < p_fin) {
                    printf("%.6f", *p_sol);
                    if (p_sol < p_fin - 1)
                        printf(" | ");
                    p_sol++;
                }
                printf("\n");
            }
        }
    } else {
        printf("\nTable of values for Y in the range [%.2f, %.2f]:\n", valor - 5 * paso, valor + 5 * paso);
        printf("Y\tX (that makes f(x,y)=0)\n");
        int j;
        for (j = -5; j <= 5; j++) {
            double y = valor + j * paso;
            double soluciones[10];
            double *p_sol = soluciones;
            int n_sol = buscar_raiz_multi(p_ecu->arbol_ecu, y, -100, 100, 1e-6, 100, soluciones, 10, 0);
            printf("%.6f\t", y);
            if (n_sol == 0)
                printf("No solution\n");
            else {
                double *p_fin = soluciones + n_sol;
                while (p_sol < p_fin) {
                    printf("%.6f", *p_sol);
                    if (p_sol < p_fin - 1)
                        printf(" | ");
                    p_sol++;
                }
                printf("\n");
            }
        }
    }
}

void BuscarRaices(ecuacion *ecu, size_t cecu) {
    int i = 0;
    int variable;
    ecuacion *p_ecu = ecu;
    double soluciones[20];
    double *p_sol, *p_fin;
    int n_sol;
    limpiarPantalla();
    printf("\n--- SEARCH FOR ROOTS ---\n");

    mostrarecuaciones(ecu, cecu, 0);
    printf("\nEnter the number of equation you want to analize: ");
    do {
        scanf("%d", &i);
        if (i < 1 || i > cecu)
            printf("\nEnter a valid equation: ");
    } while (i < 1 || i > cecu);

    p_ecu += (i - 1);
    printf("\n You choose: %s\n", p_ecu->ecu);

    variable = pedir_opcion("XY", "What variable do you want to generate the table on? (X/Y): ");

    if (variable == 'X') {
        n_sol = buscar_raiz_multi(p_ecu->arbol_ecu, 0, -100, 100, 1e-6, 100, soluciones, 20, 0);
        printf("\nRoots for X (f(x,0)=0):\n");
        if (n_sol == 0)
            printf("There are no real roots in that range.\n");
        else {
            p_sol = soluciones;
            p_fin = soluciones + n_sol;
            while (p_sol < p_fin) {
                printf("x = %.6f\n", *p_sol);
                p_sol++;
            }
        }
    } else {
        n_sol = buscar_raiz_multi(p_ecu->arbol_ecu, 0, -100, 100, 1e-6, 100, soluciones, 20, 1);
        printf("\nRoots for Y (f(0,y)=0):\n");
        if (n_sol == 0)
            printf("There are no real roots in that range.\n");
        else {
            p_sol = soluciones;
            p_fin = soluciones + n_sol;
            while (p_sol < p_fin) {
                printf("y = %.6f\n", *p_sol);
                p_sol++;
            }
        }
    }
}
void EvaluarListaDeValores(ecuacion *ecu, size_t cecu) {
    int i = 0;
    int variable;
    ecuacion *p_ecu = ecu;
    double valores[100];
    double *p_val = valores;
    char buffer[64], *p_buffer=buffer;

    limpiarPantalla();
    printf("\n--- TABLE OF VALUES (PERSONALIZED LIST) ---\n");

    mostrarecuaciones(ecu, cecu, 0);
    printf("\nEnter the number of equation you want to evaluate: ");
    do {
        scanf("%d", &i);
        limpiarBuffer();
        if (i < 1 || i > cecu)
            printf("\nEnter a valid equation: ");
    } while (i < 1 || i > cecu);

    p_ecu += (i - 1);
    printf("\n You choose: %s\n", p_ecu->ecu);

    variable = pedir_opcion("XY", "What variable do you want to generate the table on? (X/Y): ");

    printf("Enter the values for %c (one for each line, you can use fractions, end with X):\n", variable);
    while (p_val < valores + 100) {
        printf("%c[%ld]: ", variable, (long)(p_val - valores) + 1);
        if (!fgets(buffer, sizeof(buffer), stdin)) break;
        if (*p_buffer == 'X' || *p_buffer == 'x') break;
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(buffer) == 0) continue;
        char* barra = strchr(buffer, '/');
        double valor;
        if (barra) {
            *barra = '\0';
            double num = atof(buffer);
            double den = atof(barra + 1);
            if (den == 0) {
                printf("Denominator cannot be zero.\n");
                continue;
            }
            valor = num / den;
        } else {
            valor = atof(buffer);
        }
        *p_val = valor;
        p_val++;
    }

    limpiarPantalla();
    printf("\n--- TABLE OF VALUES ---\n");
    if (variable == 'X') {
        printf("\nTable of values for X in the function %s (f(x,y)=0):\n", p_ecu->ecu);
        printf("X\tY (that makes f(x,y)=0)\n");
        double *p_iter = valores;
        while (p_iter < p_val) {
            double x = *p_iter;
            double soluciones[10];
            double *p_sol = soluciones;
            int n_sol = buscar_raiz_multi(p_ecu->arbol_ecu, x, -100, 100, 1e-6, 100, soluciones, 10, 1);
            printf("%.6f\t", x);
            int printed = 0;
            double *p_fin_sol = soluciones + n_sol;
            while (p_sol < p_fin_sol) {
                double y = *p_sol;
                double fxy = evaluarArbol(p_ecu->arbol_ecu, x, y);
                if (!isnan(fxy) && !isinf(fxy) && fabs(fxy) < 1e-4) {
                    if (printed)
                        printf(" | ");
                    printf("%.6f", y);
                    printed = 1;
                }
                p_sol++;
            }
            if (!printed)
                printf("No solution");
            printf("\n");
            p_iter++;
        }
    } else {
        printf("\nTable of values for Y of the function %s (f(x,y)=0):\n", p_ecu->ecu);
        printf("Y\tX (that makes f(x,y)=0)\n");
        double *p_iter = valores;
        while (p_iter < p_val) {
            double y = *p_iter;
            double soluciones[10];
            double *p_sol = soluciones;
            int n_sol = buscar_raiz_multi(p_ecu->arbol_ecu, y, -100, 100, 1e-6, 100, soluciones, 10, 0);
            printf("%.6f\t", y);
            int printed = 0;
            double *p_fin_sol = soluciones + n_sol;
            while (p_sol < p_fin_sol) {
                double x = *p_sol;
                double fxy = evaluarArbol(p_ecu->arbol_ecu, x, y);
                if (!isnan(fxy) && !isinf(fxy) && fabs(fxy) < 1e-4) {
                    if (printed)
                        printf(" | ");
                    printf("%.6f", x);
                    printed = 1;
                }
                p_sol++;
            }
            if (!printed)
                printf("No solution");
            printf("\n");
            p_iter++;
        }
    }
    EnterParaSalir();
}

void ayuda_ecuacion(){
    puts("  Limitations and rules for equations:");
    puts("    - Only variables X and Y are accepted (uppercase or lowercase).");
    puts("    - Allowed operators: +, -, *, /, ^, =.");
    puts("    - There must be at least one X and one Y.");
    puts("    - Division by zero is not allowed.");
    puts("    - Exponents must be written with '^' and parentheses, e.g., x^(2).");
    puts("    - Roots are written as fractional exponents, e.g., y^(1/2).");
    puts("");
    puts("Valid example: x^2 + y^2 = 1");
}

void menu_ayuda_interactivo() {
    char op;
    do {
        limpiarPantalla();
        printf("\n--- HELP MENU ---\n");
        printf("[A] - Enter equation\n");
        printf("[B] - View equations\n");
        printf("[C] - Save and reset\n");
        printf("[D] - Load equations\n");
        printf("[E] - Delete saved equations\n");
        printf("[F] - Solve equation\n");
        printf("[G] - Graph equation\n");
        printf("[X] - Return to main menu\n");
        printf("Select an option to view help: ");

        op = getchar();
        limpiarBuffer();
        op = Amayus(op);

        limpiarPantalla();
        switch(op) {
            case 'A':
                puts("Enter equation:");
                puts("  Allows you to enter a new equation or modify an existing one.");
                ayuda_ecuacion();
                break;
            case 'B':
                puts("View equations:");
                puts("  Displays all equations currently loaded in memory.");
                break;
            case 'C':
                puts("Save and reset:");
                puts("  Saves the current equations to a file and clears the memory.");
                puts("  Limit: only up to 10 different files can be saved.");
                break;
            case 'D':
                puts("Load equations:");
                puts("  Allows you to load previously saved equations from a file.");
                break;
            case 'E':
                puts("Delete saved equations:");
                puts("  Deletes all saved files and equations.");
                break;
            case 'F': {
                char subop;
                do {
                    limpiarPantalla();
                    puts("Solve equation:");
                    puts("  Allows working with a selected equation from those loaded in memory.");
                    puts("  Functions are bounded between [-100;100], so results outside this interval might be undefined.");
                    puts("  Available options:");
                    puts("    [A] Evaluate the equation for given values of X and Y");
                    puts("    [B] Generate a value table");
                    puts("    [C] Find roots with respect to a variable");
                    puts("    [X] Return to help menu");
                    printf("Select an option to see more details: ");
                    subop = getchar();
                    limpiarBuffer();
                    subop = Amayus(subop);

                    limpiarPantalla();
                    switch(subop) {
                        case 'A':
                            puts("Evaluate the equation for given values of X and Y:");
                            puts("  - The program will ask you to enter numeric values for X and Y.");
                            puts("  - Then it will compute the result of the equation with those values.");
                            puts("  - If the equation is of the form 'y = ...', you can leave Y blank and the program will calculate it.");
                            puts("  - If the equation is of the form 'x = ...', you can leave X blank and the program will calculate it.");
                            puts("  - If the equation is not explicit for a variable, you must enter both values.");
                            break;
                        case 'B':
                            puts("Generate a value table:");
                            puts("  - The program will ask you to choose the independent variable (X or Y).");
                            puts("  - Then you must enter the range (minimum and maximum) and the step for that variable.");
                            puts("  - The program will compute and display a table with the corresponding values.");
                            puts("  - Useful to see how the equation behaves over an interval.");
                            break;
                        case 'C':
                            puts("Find roots with respect to a variable:");
                            puts("  - The program will ask you to choose the variable to search for the root (X or Y).");
                            puts("  - You must enter the search range.");
                            puts("  - The program will attempt to find a value of the selected variable that satisfies the equation (root).");
                            puts("  - A numerical method is used (bisection or similar).");
                            break;
                        case 'X':
                            puts("Returning to help menu...");
                            break;
                        default:
                            puts("Invalid option. Please try again.");
                    }
                    if(subop != 'X') {
                        EnterParaSalir();
                    }
                } while(subop != 'X');
                break;
            }

            case 'G':
                puts("Graph equation:");
                puts("  Allows graphing the selected equation within a given range for X and Y.");
                puts("  The graph is saved as 'grafico.bmp' and also displayed in the console.");
                break;
            case 'X':
                puts("Returning to main menu...");
                break;
            default:
                puts("Invalid option. Please try again.");
        }
        if(op != 'X') {
            EnterParaSalir();
        }
    } while(op != 'X');
}
