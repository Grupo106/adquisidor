#ifndef ADQUISIDOR_H
#define ADQUISIDOR_H

/**
* captura_inicio()
* --------------------------------------------------------------------------
* Captura, procesa y guarda información de los paquetes que atraviesan una
* interfaz en la base de datos.
*/
void captura_inicio();

/**
* captura_fin()
* --------------------------------------------------------------------------
* Termina captura de paquetes y libera recursos
*/
void captura_fin();

#endif /* ADQUISIDOR_H */
