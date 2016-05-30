[![GitHub tag](https://img.shields.io/github/tag/Grupo106/adquisidor.svg?maxAge=2592000?style=plastic)](https://github.com/Grupo106/adquisidor/releases)

adquisidor
======================================================
Adquisidor de datos. Captura, procesa y guarda datos de paquetes de red en la base de datos


Dependencias
-------------------------------------------------------

### Fedora

```sh
sudo dnf install libpcap-devel postgresql-devel
```

### Debian

```sh
apt-get install libpcap-dev posgresql-dev
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

**Atención**: Para la ejecución necesita privilegios de administrador (root)

Ver logs
-------------------------------------------------------
Para ver logs generados por la aplicación se puede utilizar el journalctl
provisto por systemd
```
journalctl -t adquisidor -f
```
