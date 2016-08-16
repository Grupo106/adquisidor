[![GitHub tag](https://img.shields.io/github/tag/Grupo106/adquisidor.svg?maxAge=2592000?style=plastic)](https://github.com/Grupo106/adquisidor/releases)
[![Build Status](https://travis-ci.org/Grupo106/adquisidor.svg?branch=master)](https://travis-ci.org/Grupo106/adquisidor)
[![codecov](https://codecov.io/gh/Grupo106/adquisidor/branch/master/graph/badge.svg)](https://codecov.io/gh/Grupo106/adquisidor)

adquisidor
======================================================
> Adquisidor de datos. Captura, procesa y guarda datos de paquetes de red en la base de datos


Funcionalidades
------------------------------------------------------
* Captura, procesa y guarda datos de paquetes en la base de datos
* Conexion a la base de datos parametrizable en tiempo de compilación
* Genera mensajes de log a través del syslog del sistema


Dependencias
-------------------------------------------------------

### Fedora

```sh
sudo dnf install libpcap-devel postgresql-devel
```

### Debian

```sh
apt-get install libpcap0.8-dev libecpg-dev
```

Compilación e instalación
-------------------------------------------------------
### Desarrollo
```sh
make debug
```

### Produccion
```sh
make
sudo make install
```

Uso
-------------------------------------------------------
```
Uso: adquisidor -h | -v | --config filename.ini
     adquisidor interfaz [saliente|entrante]

Este programa captura los paquetes ENTRANTES de la interfaz pasada por parámetro.
Si ninguna interfaz es establecida, se capturará en la primer interfaz disponible.

-h, --help             Muestra esta ayuda.
-v, --version          Muestra numero de version.
--config filename.ini  Lee configuración desde archivo INI.
interfaz               Nombre de la interfaz en la que se capturarán los paquetes entrantes.
saliente               Asume que los paquetes son desde la LAN hacia internet. 
                       Solo es util para el analizador
entrante               Asume que los paquetes son desde Internet hacia la LAN. 
                       Solo es útl para el analizador.
(c) Netcop 2016 - Universidad Nacional de la Matanza
```

Ver logs
-------------------------------------------------------
Para ver logs generados por la aplicación se puede utilizar el journalctl
provisto por systemd
```
journalctl /usr/local/sbin/adquisidor -f
```
