#include "Archivos.h"
#include "calculadora.h"
#include <string.h>


#define NOM_ARCH 50

void quitarenter(char*p){
    while (*p && *p != '\n')
        p++;

    if (*p == '\n')
        *p = '\0';
}

int contarArchivos(int *band){
    FILE* pf;
    char buffer[NOM_ARCH];
    pf=fopen("NombresArchivos.txt","rt");
    if(!pf){
        return 1;
    }
    while(fgets(buffer,NOM_ARCH,pf)){
        (*band)++;
    }
    return 0;
}

int GuardaArchivos(ecuacion* ecu, size_t ce, int* band){
    char nomArch[NOM_ARCH], nom[NOM_ARCH], *p_nomarch;
    int i=0, nombandera=1;
    FILE* pf1, *pf;
    limpiarPantalla();
    printf("\n--- Save File ---\n\n");

    if(ce==0){
        printf("There are no saved equations\n");
        return 0;
    }

    printf("Choose the name you are gonna save the file with (recomendation: without the .txt extension): ");
    do{
        fgets(nomArch,NOM_ARCH, stdin);
        quitarenter(nomArch);
        p_nomarch=nomArch;
        if(*p_nomarch=='\0')
            printf("Enter a valid name for the file(recomendation: without the .txt extension): ");
    }while(*p_nomarch=='\0');

    strcpy(nomArch,strcat(nomArch,".txt"));

    pf = fopen(nomArch, "wt");

    if(!pf){
        printf("File couldn't be created.\n");
        return 1;
    }

    ecuacion *p_ecu = ecu;
    for(i=0 ; i <ce ; i++){
        fprintf(pf,p_ecu->ecu, p_ecu);
        fprintf(pf,"\n");
        p_ecu++;
    }

    pf1= fopen("NombresArchivos.txt", "a+t");
    if(!pf1){
        printf("Names file couldn't be created/read.\n");
        return 1;
    }
    rewind(pf1);

    while(nombandera && fgets(nom, NOM_ARCH, pf1)){
        quitarenter(nom);

        if((strcmp(nom,nomArch))==0){
            nombandera=0;
        }
    }

    if(nombandera){
        (*band)++;
        fprintf(pf1,"%s",nomArch);
        fprintf(pf1,"\n");
    }

    fclose(pf);
    fclose(pf1);

    printf("File saved correctly.\n");
return 0;
}

int CargarArchivo(ecuacion* ecu, size_t *cecu) {
    FILE *pf;
    int i = 0, band_arch = 1;
    char lineaarch[TAM_ECU], lineausuario[TAM_ECU];

    limpiarPantalla();
    printf("\n--- Load File ---\n\n");

    pf = fopen("NombresArchivos.txt", "rt");
    if (!pf) {
        printf("No saved files found.\n");
        printf("\n\nPress enter to continue...");
        getchar();
        return 1;
    }

    while (fgets(lineaarch, TAM_ECU, pf)) {
        printf("File %d: %s\n", i+1, lineaarch);
        i++;
    }
    rewind(pf);

    printf("¿Which file do you want to load to the program? \n(name of the file without the .txt extension): ");
    do {
        fgets(lineausuario, TAM_ECU, stdin);
        quitarenter(lineausuario);
        strcpy(lineausuario, strcat(lineausuario, ".txt"));

        band_arch = 1;
        rewind(pf);
        while (band_arch && fgets(lineaarch, TAM_ECU, pf)) {
            quitarenter(lineaarch);
            if (!strcmp(lineausuario, lineaarch)) {
                band_arch = 0;
            }
        }
        if (band_arch) {
            printf("No file with that name was found, please try again: ");
        }
    } while (band_arch);

    fclose(pf);

    pf = fopen(lineausuario, "rt");
    borrarecuacion(ecu, cecu);

    while (fgets(lineaarch, TAM_ECU, pf))
        ingresarecu(ecu, cecu, 0, lineaarch);

    puts("File loaded correctly.");
    fclose(pf);

    return 0;
}

int borrarArchivos(){
    FILE *pf_arch;
    char linea[TAM_ECU];
    pf_arch=fopen("NombresArchivos.txt","rt");
    limpiarPantalla();
    printf("\n--- Delete files ---\n\n");
    if(!pf_arch){
        printf("No created files found, saved equations were deleted.\n");
        return 1;
    }
    printf("The next files will be deleted: \n");
    while(fgets(linea,TAM_ECU,pf_arch)){
        printf("%s", linea);
        quitarenter(linea);
        remove(linea);
    }
    fclose(pf_arch);
    remove("NombresArchivos.txt");

    return 0;
}
