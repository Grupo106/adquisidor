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
```sh
make release
make install
```

**Atención**: Para la ejecución necesita privilegios de administrador (root)
