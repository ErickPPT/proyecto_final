#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#define DATA "Datos30dias.txt"

#define LIMIT_PM25 15.0
#define LIMIT_CO2 1000.0
#define LIMIT_SO2 40.0
#define LIMIT_NO2 25.0

struct Contaminacion{
    float co2;
    float so2;
    float no2;
    float pm25;
}contaminacion;

// Estructura para Zona con ID y nombre
struct Zona {
    int id;
    char nombre[100];
}zona;

struct ClimaZona {
    int id;
    float temperatura;
    float viento;
    float humedad;
    int registrado;
};
struct ClimaZona climas[100];
int total_climas = 0;

void IngresarZona();
int LeerNumeroEnteroEntre(char *mensaje,int min,int max);
void GuardarPalabra(char *mensaje, char destino[], int longitud);
float LeerNumeroFlotanteEntre(char *mensaje,int min,int max);
void obtenerClimaZona(int id, float* temperatura, float* viento, float* humedad);
void prediccion();
void promediosHistoricos();
void exportarDatosZona();
void generacionRecomendaciones(float co2, float so2, float no2, float pm25);
void recomendacionesPorZona();

int main() {
    int continuar = 1;
    int opcion = 0;
    do {
        printf("\nSistema Integral de Gestión y Predicción de Contaminación del Aire\n");
        printf("1. Agregar datos de contaminacion\n");
        printf("2. Prediccion de niveles futuros\n");
        printf("3. Calculo de promedios historicos\n");
        printf("4. Generacion de recomendaciones\n");
        printf("5. Exportacion de datos\n");
        printf("6. Salir\n");
        opcion = LeerNumeroEnteroEntre("Ingrese una opcion (1-6): ", 1, 6);
        switch (opcion) {
            case 1:
                IngresarZona();
                break;
            case 2:
                prediccion();
                break;
            case 3:
                printf("Funcion para calculo de promedios historicos\n");
                promediosHistoricos();
                break;
            case 4:
                recomendacionesPorZona();
                break;
            case 5:
                printf("Funcion para exportacion de datos\n");
                exportarDatosZona();
                break;
            case 6:
                continuar = 0;
                printf("Saliendo.....\n");
                break;
            default:
                printf("Opcion no valida. Intente de nuevo.\n");
                break;
        }
    } while (continuar);
    return 0;
}
int LeerNumeroEnteroEntre(char *mensaje,int min,int max){
    int valor = 0;
    printf("%s",mensaje);
    while(scanf("%d",&valor) != 1 || valor > max || valor < min) {
        printf("Dato mal ingresado\n");
        printf("%s",mensaje);
        while ((getchar()) != '\n');
    }
    while ((getchar()) != '\n');
    return valor;
}

float LeerNumeroFlotanteEntre(char *mensaje,int min,int max){
    float valor = 0;
    printf("%s",mensaje);
    while(scanf("%f",&valor) != 1 || valor > max || valor < min) {
        printf("Dato mal ingresado\n");
        printf("%s",mensaje);
        while ((getchar()) != '\n');
    }
    while ((getchar()) != '\n');
    return valor;
}

void GuardarPalabra(char *mensaje, char destino[], int longitud) {
    int esValido = 0;
    while (esValido == 0){
        printf("%s", mensaje);
        if (fgets(destino, longitud, stdin) != NULL) {
            size_t len = strlen(destino);
            if (len > 0 && destino[len - 1] == '\n') {
                destino[len - 1] = '\0';
            }
            esValido = 1;
            for (size_t i = 0; i < strlen(destino); i++) {
                if (isdigit(destino[i])) {
                    printf("Error: No se permiten numeros. Intente de nuevo.\n");
                    esValido = 0;
                    break;
                }
            }
        } else {
            printf("Error al leer la entrada.\n");
        }
    }
    for (int i = 1; destino[i] != '\0'; i++) {
        destino[i] = tolower(destino[i]);
    }
}

void IngresarZona() {
    FILE *datos = fopen(DATA, "a+"); // Abrir o crear archivo, permite lectura y escritura al final
    if (datos == NULL) {
        printf("No se pudo abrir el archivo\n");
        return;
    }

    printf("\n--- Ingreso de Zona ---\n");
    int idIngresado = LeerNumeroEnteroEntre("Ingrese el ID de la zona: ", 1, 9999);
    int idTemporal;
    char nombreTemporal[50], fechaTemporal[20];
    int zonaExistente = 0;
    int datosHoyExistentes = 0;

    // Obtener la fecha actual en formato yyyy-mm-dd
    char fechaHoy[11];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(fechaHoy, sizeof(fechaHoy), "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    // Buscar si ya existe la zona y si ya hay datos de hoy
    rewind(datos);
    while (fscanf(datos, "%d,%49[^,],%19[^,]", &idTemporal, nombreTemporal, fechaTemporal) == 3) {
        int c;
        while ((c = fgetc(datos)) != '\n' && c != EOF); // Saltar al final de la línea

            if (idTemporal == idIngresado) {
                zonaExistente = 1;
                strncpy(zona.nombre, nombreTemporal, 50);
                zona.nombre[49] = '\0';

            if (strcmp(fechaTemporal, fechaHoy) == 0) {
                datosHoyExistentes = 1;
                break;
            }
        }
    }

    zona.id = idIngresado;

    if (datosHoyExistentes) {
        printf("Ya se ingresaron los datos de hoy (%s) para esta zona.\n", fechaHoy);
        fclose(datos);
        return;
    }

    if (!zonaExistente) {
        GuardarPalabra("Ingrese el nombre de la zona (max 50 caracteres): ", zona.nombre, 50);
    } else {
        printf("Nombre de zona detectado automáticamente: %s\n", zona.nombre);
    }

    // Agregar nuevos datos
    printf("Ingrese los datos de contaminación para la zona %s (ID: %d)\n", zona.nombre, zona.id);
    contaminacion.co2 = LeerNumeroFlotanteEntre("Ingrese la cantidad de CO2: ", 0, 10000);
    contaminacion.so2 = LeerNumeroFlotanteEntre("Ingrese la cantidad de SO2: ", 0, 10000);
    contaminacion.no2 = LeerNumeroFlotanteEntre("Ingrese la cantidad de NO2: ", 0, 10000);
    contaminacion.pm25 = LeerNumeroFlotanteEntre("Ingrese la cantidad de PM2.5: ", 0, 10000);

    fprintf(datos, "%d,%s,%s,%.2f,%.2f,%.2f,%.2f\n",
            zona.id, zona.nombre, fechaHoy,
            contaminacion.co2, contaminacion.so2, contaminacion.no2, contaminacion.pm25);

    printf("Zona ingresada correctamente. Fecha: %s\n", fechaHoy);
    fclose(datos);
}

void obtenerClimaZona(int id, float* temperatura, float* viento, float* humedad) {
    for (int i = 0; i < total_climas; i++) {
        if (climas[i].id == id) {
            int opcion;
            opcion=LeerNumeroEnteroEntre("Ingrese 1 para usar datos anteriores o 0 para ingresar nuevos datos: ", 0, 1);
            if (opcion == 1) {
                *temperatura = climas[i].temperatura;
                *viento = climas[i].viento;
                *humedad = climas[i].humedad;
                return;
            } else {
                break;
            }
        }
    }

    *temperatura=LeerNumeroFlotanteEntre("Ingrese la temperatura actual (°C): ", -50, 50);
    *viento = LeerNumeroFlotanteEntre("Ingrese la velocidad del viento actual (km/h): ", 0, 200);
   *humedad = LeerNumeroFlotanteEntre("Ingrese el nivel de humedad actual (%%): ", 0, 100);

    int actualizado = 0;
    for (int i = 0; i < total_climas; i++) {
        if (climas[i].id == id) {
            climas[i].temperatura = *temperatura;
            climas[i].viento = *viento;
            climas[i].humedad = *humedad;
            climas[i].registrado = 1;
            actualizado = 1;
            break;
        }
    }
    if (!actualizado) {
        climas[total_climas].id = id;
        climas[total_climas].temperatura = *temperatura;
        climas[total_climas].viento = *viento;
        climas[total_climas].humedad = *humedad;
        climas[total_climas].registrado = 1;
        total_climas++;
    }
}

void prediccion() {
    printf("\n--- Predicción de niveles futuros (promedio 7 días + clima actual) ---\n");
    int id_zona = LeerNumeroEnteroEntre("Ingrese el ID de la zona para predecir: ", 1, 10000);

    FILE *archivo = fopen(DATA, "r");
    if (!archivo) {
        printf("No se pudo abrir el archivo de datos.\n");
        return;
    }

    float co2[7], so2[7], no2[7], pm25[7];
    char fechas[7][20];
    int count = 0;
    char nombre[50], fecha[20];
    int id;
    float vco2, vso2, vno2, vpm25;

    while (fscanf(archivo, "%d,%49[^,],%19[^,],%f,%f,%f,%f",
                  &id, nombre, fecha, &vco2, &vso2, &vno2, &vpm25) == 7) {
        if (id == id_zona && count < 7) {
            co2[count] = vco2;
            so2[count] = vso2;
            no2[count] = vno2;
            pm25[count] = vpm25;
            strncpy(fechas[count], fecha, 20);
            fechas[count][19] = '\0';
            count++;
        }
    }
    fclose(archivo);

    if (count == 0) {
        printf("No hay datos para la zona seleccionada.\n");
        return;
    }

    float suma_co2 = 0, suma_so2 = 0, suma_no2 = 0, suma_pm25 = 0;
    for (int i = 0; i < count; i++) {
        suma_co2 += co2[i];
        suma_so2 += so2[i];
        suma_no2 += no2[i];
        suma_pm25 += pm25[i];
    }

    printf("\nDatos usados para la predicción (últimos %d días):\n", count);
    for (int i = 0; i < count; i++) {
        printf("%s: CO2=%.2f, SO2=%.2f, NO2=%.2f, PM25=%.2f\n",
               fechas[i], co2[i], so2[i], no2[i], pm25[i]);
    }

    // Calcular fecha de mañana
    time_t t_actual = time(NULL);
    struct tm tm_maniana = *localtime(&t_actual);
    tm_maniana.tm_mday += 1;
    mktime(&tm_maniana); // Normalizar
    char fecha_maniana[20];
    strftime(fecha_maniana, sizeof(fecha_maniana), "%Y-%m-%d", &tm_maniana);

    // Promedios
    float promedio_co2 = suma_co2 / count;
    float promedio_so2 = suma_so2 / count;
    float promedio_no2 = suma_no2 / count;
    float promedio_pm25 = suma_pm25 / count;

    // Clima actual
    float temperatura, viento, humedad;
    obtenerClimaZona(id_zona, &temperatura, &viento, &humedad);

    // Predicción con fórmula
    float pred_co2 = 0.7 * promedio_co2 + 0.1 * temperatura - 0.1 * viento + 0.1 * humedad;
    float pred_so2 = 0.5 * promedio_so2 + 0.2 * temperatura - 0.2 * viento + 0.2 * humedad;
    float pred_no2 = 0.6 * promedio_no2 + 0.2 * temperatura - 0.1 * viento + 0.1 * humedad;
    float pred_pm25 = 0.6 * promedio_pm25 + 0.2 * temperatura - 0.1 * viento + 0.1 * humedad;

    // Resultados
    printf("\nPredicción para el día %s en la zona ID %d:\n", fecha_maniana, id_zona);
    printf("CO2: %.2f ppm\n", pred_co2);
    printf("SO2: %.2f ug/m3\n", pred_so2);
    printf("NO2: %.2f ug/m3\n", pred_no2);
    printf("PM2.5: %.2f ug/m3\n", pred_pm25);

    generacionRecomendaciones(pred_co2, pred_so2, pred_no2, pred_pm25);
}

void recomendacionesPorZona() {
    printf("\n--- Generación de recomendaciones por zona (niveles actuales) ---\n");

    int id_zona = LeerNumeroEnteroEntre("Ingrese el ID de la zona para recomendaciones: ", 1, 10000);

    FILE *archivo = fopen(DATA, "r");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo de datos.\n");
        return;
    }

    float co2 = 0, so2 = 0, no2 = 0, pm25 = 0;
    int id;
    char nombre[50], fecha[20];
    float vco2, vso2, vno2, vpm25;
    int encontrado = 0;
    char fecha_ultimo[20] = "";

    // Buscar el último registro de la zona seleccionada
    while (fscanf(archivo, "%d,%49[^,],%19[^,],%f,%f,%f,%f",
                  &id, nombre, fecha, &vco2, &vso2, &vno2, &vpm25) == 7) {
        if (id == id_zona) {
            co2 = vco2;
            so2 = vso2;
            no2 = vno2;
            pm25 = vpm25;
            strncpy(fecha_ultimo, fecha, sizeof(fecha_ultimo));
            fecha_ultimo[sizeof(fecha_ultimo) - 1] = '\0';
            encontrado = 1;
        }
    }
    fclose(archivo);

    if (encontrado) {
        printf("\nÚltimos valores registrados para la zona %d (fecha %s):\n", id_zona, fecha_ultimo);
        printf("CO2: %.2f ppm\n", co2);
        printf("SO2: %.2f ug/m3\n", so2);
        printf("NO2: %.2f ug/m3\n", no2);
        printf("PM2.5: %.2f ug/m3\n", pm25);

        printf("\nRecomendaciones\n");
        generacionRecomendaciones(co2, so2, no2, pm25);
    } else {
        printf("No se encontraron registros para la zona seleccionada.\n");
    }
}

void promediosHistoricos() {
    FILE *archivo = fopen(DATA, "r");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo de datos.\n");
        return;
    }

    int id_zona = LeerNumeroEnteroEntre("Ingrese el ID de la zona para calcular promedios históricos: ", 1, 10000);

    int id;
    char nombre[50], fecha[20];
    float co2, so2, no2, pm25;
    float suma_co2 = 0, suma_so2 = 0, suma_no2 = 0, suma_pm25 = 0;
    int contador = 0;

    time_t t_actual = time(NULL);
    struct tm tm_actual = *localtime(&t_actual);

    while (fscanf(archivo, "%d,%49[^,],%19[^,],%f,%f,%f,%f",
                  &id, nombre, fecha, &co2, &so2, &no2, &pm25) == 7) {
        if (id == id_zona) {
            int anio, mes, dia;
            if (sscanf(fecha, "%d-%d-%d", &anio, &mes, &dia) == 3) {
                struct tm tm_registro = {0};
                tm_registro.tm_year = anio - 1900;
                tm_registro.tm_mon = mes - 1;
                tm_registro.tm_mday = dia;
                time_t t_registro = mktime(&tm_registro);
                double diferencia_dias = difftime(t_actual, t_registro) / (60 * 60 * 24);
                if (diferencia_dias >= 0 && diferencia_dias <= 30) {
                    suma_co2 += co2;
                    suma_so2 += so2;
                    suma_no2 += no2;
                    suma_pm25 += pm25;
                    contador++;
                }
            }
        }
    }

    fclose(archivo);

    if (contador == 0) {
        printf("No hay datos suficientes de los últimos 30 días para esta zona.\n");
        return;
    }

    printf("\n--- Promedios históricos para zona ID %d (últimos %d días) ---\n", id_zona, contador);
    printf("CO2: %.2f ppm\n", suma_co2 / contador);
    printf("SO2: %.2f ug/m3\n", suma_so2 / contador);
    printf("NO2: %.2f ug/m3\n", suma_no2 / contador);
    printf("PM2.5: %.2f ug/m3\n", suma_pm25 / contador);
}

void generacionRecomendaciones(float co2, float so2, float no2, float pm25) {
    printf("\nRecomendaciones\n");
    int alerta = 0;

    if (pm25 > LIMIT_PM25) {
        printf("PM2.5 alto (%.2f ug/m3):\n", pm25);
        printf("- Limitar actividades al aire libre y uso de vehículos particulares.\n");
        printf("- Promover el uso de mascarillas en exteriores.\n");
        alerta = 1;
    }
    if (co2 > LIMIT_CO2) {
        printf("CO2 alto (%.2f ppm):\n", co2);
        printf("- Fomentar el uso de transporte público y bicicletas.\n");
        printf("- Mejorar la eficiencia energética en edificios e industrias.\n");
        alerta = 1;
    }
    if (so2 > LIMIT_SO2) {
        printf("SO2 alto (%.2f ug/m3):\n", so2);
        printf("- Controlar emisiones industriales y uso de combustibles fósiles.\n");
        printf("- Promover el uso de energías limpias.\n");
        alerta = 1;
    }
    if (no2 > LIMIT_NO2) {
        printf("NO2 alto (%.2f ug/m3):\n", no2);
        printf("- Restringir el tráfico vehicular en horas pico.\n");
        printf("- Revisar y mantener vehículos para reducir emisiones.\n");
        alerta = 1;
    }

    if (!alerta) {
        printf("Todos los niveles están dentro de los límites recomendados.\n");
    }
}

void exportarDatosZona() {
    printf("\n--- Exportación de datos por zona ---\n");

    FILE *entrada = fopen(DATA, "r");
    if (entrada == NULL) {
        printf("No se pudo abrir el archivo de datos.\n");
        return;
    }

    int id_zona = LeerNumeroEnteroEntre("Ingrese el ID de la zona que desea exportar: ", 1, 10000);

    FILE *salida = fopen("reporte_contaminacion.txt", "w"); // modo "w" sobrescribe
    if (salida == NULL) {
        printf("No se pudo crear el archivo de reporte.\n");
        fclose(entrada);
        return;
    }

    int id;
    char nombre[50], fecha[20];
    float co2, so2, no2, pm25;
    int exportado = 0;

    while (fscanf(entrada, "%d,%49[^,],%19[^,],%f,%f,%f,%f",
                  &id, nombre, fecha, &co2, &so2, &no2, &pm25) == 7) {
        if (id == id_zona) {
            fprintf(salida, "Zona: %s (ID %d)\n", nombre, id);
            fprintf(salida, "Fecha: %s\n", fecha);
            fprintf(salida, "CO2: %.2f ppm\n", co2);
            fprintf(salida, "SO2: %.2f ug/m3\n", so2);
            fprintf(salida, "NO2: %.2f ug/m3\n", no2);
            fprintf(salida, "PM2.5: %.2f ug/m3\n", pm25);
            fprintf(salida, "----------------------------------------\n");
            exportado = 1;
        }
    }

    fclose(entrada);
    fclose(salida);

    if (exportado) {
        printf("Datos exportados exitosamente a 'reporte_contaminacion.txt'.\n");
    } else {
        printf("No se encontraron datos para la zona seleccionada.\n");
        remove("reporte_contaminacion.txt"); // elimina archivo vacío
    }
}
