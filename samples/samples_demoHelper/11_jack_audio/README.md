# Dépendances

Ce programme nécéssite d'installer *fftw* et *jack* pour fonctionner

## Installer fftw

- Arch linux: `pacman -S fftw`
- Debian: `apt-get install fftw2 fftw-dev`

## Installer jack

Jack est un serveur audio permettant la création de "clients" audio avec des entrées et des sorties, et de les connecter entre elles.
Ici, jack est utilisé pour créer un client avec une entrée, utilisée pour lire un signal sonore en temps réel.


En plus de jack, on conseille l'utilisation d'un client graphique pour pouvoir configurer jack mais surtout connecter/déconnecter simplement les entrées/sorties que l'on veut connecter. Il existe plusieurs clients graphique, nous conseillons ici **qjackctl** qui est simple et performant.

### Sans pipewire

- Arch linux: `pacman -S jack2 qjackctl`
- Debian: `apt-get install libjack-jackd2-dev jackd2 qjackctl`

### Avec pipewire

Si vous utilisez **pipewire**, remplacez le paquet jack2 par le sous-paquet jack lié à pipewire (en général nommé **pipewire-jack**).

Si vous ne savez pas si votre ordinateur utilise pipewire, vous pouvez vérifier si le service est actif sur votre machine: `systemctl --user status pipewire.service`

- Arch linux: `pacman -S pipewire-jack qjackctl`
- Debian: `apt-get install libjack-jackd2-dev pipewire-jack qjackctl`